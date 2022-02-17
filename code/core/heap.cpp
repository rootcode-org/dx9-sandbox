/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include <mutex>
#include "core/heap.h"

// Configuration
#define REGION_PAGE_SIZE        (1<<20)					// Size of pages used by region allocator
#define POOL_PAGE_SIZE			(1<<17)					// Size of pages used by pool allocator
#define REGION_MIN_FREE			(64)					// Minimum size in bytes of a free element in a RegionAllocator
#define MEMORY_SENTINEL			(0x6F6F6F6F6F6F6F6F)	// Test for buffer overrun/underrun
#define FILL_VALUE				(0xE1)					// Value used for fill on free

#define ptradd(p,offset) ((intptr_t) (p) + (intptr_t) (offset))	
#define ptrsub(p,offset) ((intptr_t) (p) - (intptr_t) (offset))	

#define list_insert(head, e) {e->prev = nullptr; e->next = head; head = e; if (e->next) e->next->prev = e; }
#define list_remove(head, e) {if (e->next) e->next->prev = e->prev; if (e->prev) e->prev->next = e->next; else head = e->next; }

// base class for allocators
class Allocator
{
public:
	virtual ~Allocator() = 0;
	virtual void* Resize(void* address, size_t new_size, bool fill) { (address); (new_size); (fill); throw("virtual method"); };
	virtual void* Relocate(void* address, bool fill) { (address); (fill);  throw("virtual method"); };
	virtual void  Free(void* address, bool fill) { (address); (fill); throw("virtual method"); };
protected:
	unsigned int num_allocations = 0;
	size_t total_allocated = 0;
	size_t peak_allocated = 0;

	void Count(size_t size)   { num_allocations++; total_allocated += size; if (total_allocated > peak_allocated) peak_allocated = total_allocated; }
	void Uncount(size_t size) { num_allocations--; total_allocated -= size; }
};
Allocator::~Allocator() {}

// base struct for memory pages
struct Page
{
	Allocator* allocator;		// allocator that owns the page
};

// marker placed at end of allocated memory to detect buffer overrun
struct Sentinel
{
	unsigned __int64 value;
};

//#################################################################################################################################
// System Memory Allocator
//#################################################################################################################################

struct SystemElement
{
	size_t size : 63;				// size in bytes of allocated memory
	size_t track : 1;				// 1 = track as potential leak, 0 = don't track
	SystemElement* next;			// pointers for list of allocated elements
	SystemElement* prev;			// "
	Page* page;						// always nullptr for a SystemElement; MUST be last field in struct
};

class SystemAllocator : Allocator
{
public:
	void* Allocate(size_t size, bool track_leaks);
	void* Resize(void* address, size_t new_size, bool fill);
	void* Relocate(void* address, bool fill) { (fill); return address; }		// intentionally a null operation
	void  Free(void* address, bool fill);
	void  VerifyIntegrity(void);
	void  ReportLeaks(void);
private:
	std::mutex mtx;
	SystemElement* elements = nullptr;		// list of allocated elements
};

void* SystemAllocator::Allocate(size_t size, bool track_leaks)
{
	size = (size + 7) & -8;
	size_t allocation_size = size + sizeof(SystemElement) + sizeof(Sentinel);
	SystemElement* element = (SystemElement*)HeapAlloc(GetProcessHeap(), 0, allocation_size);
	element->size = size;
	element->track = track_leaks;
	element->page = nullptr;
	list_insert(elements, element);
	Sentinel* sentinel = (Sentinel*)ptradd(element, sizeof(SystemElement) + element->size);
	sentinel->value = MEMORY_SENTINEL;
	Count(size);
	return (void*)ptradd(element, sizeof(SystemElement));
}

void* SystemAllocator::Resize(void* address, size_t new_size, bool fill)
{
	SystemElement* element = (SystemElement*)ptrsub(address, sizeof(SystemElement));
	Sentinel* sentinel = (Sentinel*)ptradd(address, element->size);
	if (sentinel->value != MEMORY_SENTINEL) throw("system allocation buffer overrun");
	list_remove(elements, element);
	Uncount(element->size);
	bool original_track = element->track;

	new_size = (new_size + 7) & -8;
	size_t allocation_size = new_size + sizeof(SystemElement) + sizeof(Sentinel);
	element = (SystemElement*)HeapReAlloc(GetProcessHeap(), fill ? HEAP_ZERO_MEMORY : 0, (void*)element, allocation_size);
	element->size = new_size;
	element->track = original_track;
	element->page = nullptr;
	list_insert(elements, element);
	sentinel = (Sentinel*)ptradd(element, sizeof(SystemElement) + element->size);
	sentinel->value = MEMORY_SENTINEL;
	Count(element->size);
	return (void*)ptradd(element, sizeof(SystemElement));
}

void SystemAllocator::Free(void* address, bool fill)
{
	SystemElement* element = (SystemElement*)ptrsub(address, sizeof(SystemElement));
	Sentinel* sentinel = (Sentinel*)ptradd(address, element->size);
	if (sentinel->value != MEMORY_SENTINEL) throw("system allocation buffer overrun");
	list_remove(elements, element);
	Uncount(element->size);
	if (fill) memset(element, 0, sizeof(SystemElement) + element->size + sizeof(Sentinel));
	HeapFree(GetProcessHeap(), 0, (void*)element);
}

void SystemAllocator::VerifyIntegrity()
{
	for (SystemElement* element = elements; element != nullptr; element = element->next)
	{
		Sentinel* sentinel = (Sentinel*)ptradd(element, sizeof(SystemElement) + element->size);
		if (sentinel->value != MEMORY_SENTINEL) throw("system allocation buffer overrun");
	}
}

void SystemAllocator::ReportLeaks()
{
	for (SystemElement* element = elements; element != nullptr; element = element->next)
	{
		// If this is a tracked allocation then report it as a leak
		if (element->track)
		{
			printf("Memory Leak");
		}
	}
}

//#################################################################################################################################
// Region Allocator
//#################################################################################################################################

// a page of memory from which allocations are made
struct RegionPage : Page
{
	RegionPage* next;
	RegionPage* prev;
	unsigned int num_allocations;
	unsigned int reserved;
};

// describes an individual free or allocated element of memory
struct RegionElement
{
	size_t size : 61;				// size in bytes of allocated memory
	size_t is_allocated : 1;		// 1 = allocated element, 0 = free element
	size_t has_sentinel : 1;		// 1 = has a sentinel, 0 = no sentinel
	size_t track : 1;				// 1 = track as potential leak, 0 = do not track
	RegionElement* prev_element;	// Pointer to element that precedes this one in the page
	RegionPage* page;				// page that owns this element; MUST be last real field in the struct

	// Virtual fields; a shortened struct length is used such that these pointers are placed into the element memory
	RegionElement* next;			// free list pointers
	RegionElement* prev;
};

// Use shortened struct length for RegionElement, allowing the free pointers to extend into element memory
// This works because the pointers are only accessed when the element is not allocated
#define REGION_ELEMENT_SIZE (sizeof(RegionElement) - (sizeof(RegionElement*) * 2))

class RegionAllocator : public Allocator
{
public:
	RegionAllocator(size_t page_size);

	void* Allocate(size_t size, bool track_leaks, bool append_sentinel);
	void* Resize(void* address, size_t new_size, bool fill);
	void* Relocate(void* address, bool fill);
	void  Free(void* address, bool fill);
	void  VerifyIntegrity(void);
	void  ReportLeaks(void);

private:
	size_t page_size;
	RegionPage* pages = nullptr;		// list of pages owned by the allocator
	RegionElement* free_lists[64];		// free list head pointers
	std::mutex mtx;

	void AddFreeElement(RegionElement* e);
	void RemoveFreeElement(RegionElement* e);
};

RegionAllocator::RegionAllocator(size_t page_size)
{
	this->page_size = page_size;
	memset(free_lists, 0, sizeof(free_lists));
}

void* RegionAllocator::Allocate(size_t size, bool track_leaks, bool append_sentinel)
{
	// Apply rounding and minimum size requirements
	size = (size + 7) & -8;
	if (append_sentinel) size += sizeof(Sentinel);								// add room for sentinel if requested
	if (size < (sizeof(RegionElement*) * 2)) size = sizeof(RegionElement*) * 2;	// allocation must be big enough for free pointers

	// An allocation cannot exceed the available memory in a page
	int page_overhead = sizeof(RegionPage) + (REGION_ELEMENT_SIZE * 2);
	if (size > page_size - page_overhead) return nullptr;

	// Synchronize thread access to this code block
	{
		std::lock_guard<std::mutex> lock(mtx);

		// Find the smallest free element that meets the size requirement
		RegionElement* e = nullptr;
		unsigned int search_size = (unsigned int)size + REGION_ELEMENT_SIZE;
		for (int index = (int)log2(search_size); index <= (int)log2(page_size); index++)
		{
			e = free_lists[index];
			while (e && (search_size > e->size)) e = e->next;
			if (e) break;
		}

		// If we can't satisfy the allocation request then add a page of memory
		if (!e)
		{
			RegionPage* page = (RegionPage*)HeapAlloc(GetProcessHeap(), 0, page_size);
			page->allocator = this;
			page->num_allocations = 0;
			list_insert(pages, page);

			// Initialize the free element
			e = (RegionElement*)ptradd(page, sizeof(RegionPage));
			e->size = page_size - sizeof(RegionPage) - (REGION_ELEMENT_SIZE * 2);
			e->is_allocated = false;
			e->page = page;
			e->prev_element = nullptr;

			// Create a dummy element with size 0 at the end of the page.
			RegionElement* f = (RegionElement*)ptradd(e, REGION_ELEMENT_SIZE + e->size);
			f->size = 0;
			f->is_allocated = true;
			f->page = page;
			f->prev_element = e;
		}
		else
		{
			RemoveFreeElement(e);
		}

		// Allocate from the start of the free element
		size_t remainder = e->size - size - REGION_ELEMENT_SIZE;
		if (remainder >= REGION_MIN_FREE)
		{
			// resize the allocated memory
			e->size = size;				// includes sentinel size

			// Create a new free element after the allocated element
			RegionElement* f = (RegionElement*)ptradd(e, REGION_ELEMENT_SIZE + e->size);
			f->size = remainder;
			f->is_allocated = false;
			f->page = e->page;
			f->prev_element = e;
			AddFreeElement(f);

			// Find the next element and point it back to the new free element
			RegionElement* g = (RegionElement*)ptradd(f, REGION_ELEMENT_SIZE + f->size);
			g->prev_element = f;
		}

		// Update and return the allocated element
		e->is_allocated = true;
		e->has_sentinel = append_sentinel;
		e->track = track_leaks;
		e->page->num_allocations++;
		if (append_sentinel) {
			Sentinel* sentinel = (Sentinel*)ptradd(e, REGION_ELEMENT_SIZE + e->size - sizeof(Sentinel));
			sentinel->value = MEMORY_SENTINEL;
		}
		Count(size);
		return (void*)ptradd(e, REGION_ELEMENT_SIZE);
	}
}

void* RegionAllocator::Resize(void* address, size_t new_size, bool fill)
{
	RegionElement* e = (RegionElement*)ptrsub(address, REGION_ELEMENT_SIZE);
	void* new_memory = Allocate(new_size, e->track, e->has_sentinel);
	size_t copy_size = e->size < new_size ? e->size : new_size;
	memcpy(new_memory, address, copy_size);
	Free(address, fill);
	return new_memory;
}

void* RegionAllocator::Relocate(void* address, bool fill)
{
	// Resizing to the current size will relocate to the most efficient location
	RegionElement* b = ((RegionElement*)address) - 1;
	return Resize(address, b->size, fill);
}

void RegionAllocator::Free(void* address, bool fill)
{
	RegionElement* e = (RegionElement*)ptrsub(address, REGION_ELEMENT_SIZE);
	if (e->has_sentinel) {
		Sentinel* sentinel = (Sentinel*)ptradd(address, e->size - sizeof(Sentinel));
		if (sentinel->value != MEMORY_SENTINEL) throw ("region allocator buffer overrun");
	}
	if (fill) {
		memset(address, FILL_VALUE, e->size);
	}

	// Synchronize thread access to this code block
	{
		std::lock_guard<std::mutex> lock(mtx);

		// Test for a double-free
		if (!e->is_allocated) throw("Region allocator double free");

		// Mark as free
		e->is_allocated = false;
		e->page->num_allocations--;
		Uncount(e->size);

		// If the previous element in memory is free then merge it with this one
		RegionElement* prev = e->prev_element;
		if (prev && !prev->is_allocated)
		{
			// Remove from the free list
			RemoveFreeElement(prev);

			// Combine the current element with the previous one
			prev->size += REGION_ELEMENT_SIZE + e->size;
			RegionElement* next = (RegionElement*)ptradd(e, REGION_ELEMENT_SIZE + e->size);
			next->prev_element = prev;
			if (fill) {
				memset(e, FILL_VALUE, REGION_ELEMENT_SIZE);
			}

			// We now consider the previous element as the current element
			e = prev;
		}

		// If the next element in memory is free then merge it with this one
		RegionElement* next = (RegionElement*)ptradd(e, REGION_ELEMENT_SIZE + e->size);
		if (!next->is_allocated)
		{
			// Remove from the free list
			RemoveFreeElement(next);

			// Combine the next element with the current one
			e->size += REGION_ELEMENT_SIZE + next->size;
			RegionElement* f = (RegionElement*)ptradd(e, REGION_ELEMENT_SIZE + e->size);
			f->prev_element = e;
			if (fill) {
				memset(next, FILL_VALUE, REGION_ELEMENT_SIZE);
			}
		}

		// If the page is empty then discard it
		RegionPage* page = e->page;
		if (page->num_allocations == 0)
		{
			list_remove(pages, page);
			HeapFree(GetProcessHeap(), 0, page);
		}
		else
		{
			// The page is not empty so just add the free element back to the free list
			AddFreeElement(e);
		}
	}
}

void RegionAllocator::AddFreeElement(RegionElement* e)
{
	// Sort into free bucket
	int index = (int)log2(e->size);
	RegionElement* prev = nullptr;
	RegionElement* next = free_lists[index];
	while (next)
	{
		if (e->size <= next->size) break;
		prev = next;
		next = next->next;
	}
	e->next = next;
	e->prev = prev;
	if (next) next->prev = e;
	if (prev) prev->next = e;
	else free_lists[index] = e;
}

void RegionAllocator::RemoveFreeElement(RegionElement* e)
{
	int index = (int)log2(e->size);
	list_remove(free_lists[index], e);
}

void RegionAllocator::VerifyIntegrity()
{
	for (RegionPage* page = pages; page != nullptr; page = page->next)
	{
		void* end = (void*)ptradd(page, page_size);
		RegionElement* e = (RegionElement*)ptradd(page, sizeof(RegionPage));
		while (e < end)
		{
			if (e->is_allocated && e->has_sentinel) {
				Sentinel* sentinel = (Sentinel*)ptradd(e, REGION_ELEMENT_SIZE + e->size - sizeof(Sentinel));
				if (sentinel->value != MEMORY_SENTINEL) throw("region allocator buffer overrun");
			}
			e = (RegionElement*)ptradd(e, REGION_ELEMENT_SIZE + e->size);
		}
	}
}

void RegionAllocator::ReportLeaks()
{
	for (RegionPage* page = pages; page != nullptr; page = page->next)
	{
		void* end = (void*)ptradd(page, page_size);
		RegionElement* e = (RegionElement*)ptradd(page, sizeof(RegionPage));
		while (e < end)
		{
			if (e->is_allocated && e->track) {
				printf("region allocator leak");
			}
			e = (RegionElement*)ptradd(e, REGION_ELEMENT_SIZE + e->size);
		}
	}
}

//#################################################################################################################################
// Pool Allocator
//#################################################################################################################################

struct PoolElement {
	struct PoolPage* page;		// page that owns this element; MUST be last real field in the struct

	// Virtual fields; a shortened struct length is used such that these pointers are placed into the element memory
	PoolElement* next;			// free list pointers
	PoolElement* prev;
};

// Use shortened struct length for PoolElement, allowing the free pointers to extend into element memory
// This works because the pointers are only accessed when the element is not allocated
#define POOL_ELEMENT_SIZE (sizeof(PoolElement) - (sizeof(PoolElement*) * 2))

struct PoolPage : Page
{
	PoolPage* next;						// pointers for page list
	PoolPage* prev;
	PoolElement* free_list = nullptr;	// head of free element list
	int num_allocations = 0;
};

class PoolAllocator : public Allocator
{
public:
	PoolAllocator(size_t page_size, size_t element_size, bool append_sentinel);

	void* Allocate();
	void* Resize(void* address, size_t new_size) { (new_size); (address); throw("cannot resize a pool element"); }
	void* Relocate(void* address, bool fill);
	void  Free(void* address, bool fill);
	void  VerifyIntegrity(void);
	void  ReportLeaks(void);

private:
	bool       append_sentinel;
	size_t     page_size;
	size_t     element_size;
	size_t     element_overhead;
	PoolPage*  pages = nullptr;
	std::mutex mtx;
};


PoolAllocator::PoolAllocator(size_t page_size, size_t element_size, bool append_sentinel)
{
	this->page_size = page_size;
	this->append_sentinel = append_sentinel;

	// round size to 64-bit boundary and ensure element is big enough to hold free pointers
	element_size = (element_size + 7) & -8;
	if (element_size < (sizeof(PoolElement*) * 2)) element_size = sizeof(PoolElement*) * 2;
	this->element_size = element_size;
	this->element_overhead = append_sentinel ? POOL_ELEMENT_SIZE + sizeof(Sentinel) : POOL_ELEMENT_SIZE;
}

void* PoolAllocator::Allocate()
{
	std::lock_guard<std::mutex> lock(mtx);

	// Find first page with a free element
	PoolPage* page = nullptr;
	for (page = pages; page != nullptr; page = page->next)
	{
		if (page->free_list) break;
	}

	// If we didn't find a page with a free element then add a new page
	if (!page)
	{
		page = (PoolPage*)HeapAlloc(GetProcessHeap(), 0, page_size);
		list_insert(pages, page);
		page->free_list = nullptr;
		page->allocator = this;

		// Split page into a list of free elements
		PoolElement* element = (PoolElement*)ptradd(page, sizeof(PoolPage));
		size_t num_elements = (page_size - sizeof(PoolPage)) / (element_size + element_overhead);
		for (size_t i = 0; i < num_elements; i++)
		{
			element->page = page;
			list_insert(page->free_list, element);
			if (append_sentinel) {
				Sentinel* sentinel = (Sentinel*)ptradd(element, POOL_ELEMENT_SIZE + element_size);
				sentinel->value = MEMORY_SENTINEL;
			}
			element = (PoolElement*)ptradd(element, element_size + element_overhead);
		}
	}

	// Allocate free element from page
	PoolElement* element = page->free_list;
	if (element)
	{
		list_remove(page->free_list, element);
		page->num_allocations++;
		Count(element_size);
		return (void*)ptradd(element, POOL_ELEMENT_SIZE);
	}

	return nullptr;
}

void* PoolAllocator::Relocate(void* address, bool fill)
{
	void* new_address = Allocate();
	memcpy(new_address, address, element_size);
	Free(address, fill);
	return new_address;
}

void  PoolAllocator::Free(void* address, bool fill)
{
	PoolElement* element = (PoolElement*)ptrsub(address, POOL_ELEMENT_SIZE);

	if (append_sentinel) {
		Sentinel* sentinel = (Sentinel*)ptradd(address, element_size);
		if (sentinel->value != MEMORY_SENTINEL) throw("Pool element buffer overrun");
	}

	if (fill) {
		// skip over free pointers and fill the remainder of the element
		size_t fill_size = element_size - (sizeof(PoolElement*) * 2);
		if (fill_size > 0) memset((void*)ptradd(address, (sizeof(PoolElement*) * 2)), FILL_VALUE, fill_size);
	}

	// Synchronize thread access for this code block
	{
		std::lock_guard<std::mutex> lock(mtx);

		// Return the element to its page
		PoolPage* page = element->page;
		list_insert(page->free_list, element);
		page->num_allocations--;
		Uncount(element_size);

		// Remove the page if empty
		if (page->num_allocations == 0)
		{
			list_remove(pages, page);
			HeapFree(GetProcessHeap(), 0, page);
		}
		else
		{
			// page is not empty; move it to the head of the list to make next allocation faster
			if (page->prev != nullptr) {	// if not already at the head of the list
				list_remove(pages, page);
				list_insert(pages, page);
			}
		}
	}
}

void PoolAllocator::VerifyIntegrity(void)
{
	if (append_sentinel)
	{
		for (PoolPage* page = pages; page != nullptr; page = page->next)
		{
			void* end = (void*)ptradd(page, page_size - element_size - element_overhead);
			PoolElement* e = (PoolElement*)ptradd(page, sizeof(PoolPage));
			while (e <= end)
			{
				Sentinel* sentinel = (Sentinel*)ptradd(e, POOL_ELEMENT_SIZE + element_size);
				if (sentinel->value != MEMORY_SENTINEL) throw("pool allocator buffer overrun");
				e = (PoolElement*)ptradd(e, element_size + element_overhead);
			}
		}
	}
}

void PoolAllocator::ReportLeaks(void)
{
	for (PoolPage* p = pages; p != nullptr; p = p->next)
	{
		if (p->num_allocations > 0) printf ("Pool leak");
	}
}


//#################################################################################################################################
// Global Heap
//#################################################################################################################################

// Global heap instance
static Heap global_heap;
Heap* Heap::GetInstance(void) {return &global_heap;}

void Heap::VerifyIntegrity(void)
{
	if (system_allocator) system_allocator->VerifyIntegrity();
	if (default_allocator) default_allocator->VerifyIntegrity();
	if (transient_allocator) transient_allocator->VerifyIntegrity();
	if (permanent_allocator) permanent_allocator->VerifyIntegrity();
	for (int i = 0; i < LARGE_ALLOCATION_SIZE; i++)
	{
		PoolAllocator* p = pools[i];
		if (p != nullptr) p->VerifyIntegrity();
	}
}

void Heap::ReportLeaks(void)
{
	if (system_allocator) system_allocator->ReportLeaks();
	if (default_allocator) default_allocator->ReportLeaks();
	if (transient_allocator) transient_allocator->ReportLeaks();
	if (permanent_allocator) permanent_allocator->ReportLeaks();
	for (int i = 0; i < LARGE_ALLOCATION_SIZE; i++)
	{
		PoolAllocator* p = pools[i];
		if (p != nullptr) p->ReportLeaks();
	}
}

void* Heap::Allocate(size_t size, New::Hint hint)
{
	if (size >= LARGE_ALLOCATION_SIZE)
	{
		if (!system_allocator) {
			system_allocator = new (HeapAlloc(GetProcessHeap(), 0, sizeof(SystemAllocator))) SystemAllocator();
		}
		return system_allocator->Allocate(size, leak_tracking);
	}
	else
	{
		switch (hint)
		{
		case New::Hint::POOLABLE:
			if (!pools[size]) {
				pools[size] = new (HeapAlloc(GetProcessHeap(), 0, sizeof(PoolAllocator))) PoolAllocator(POOL_PAGE_SIZE, size, append_sentinel);
			}
			return pools[size]->Allocate();
		case New::Hint::TRANSIENT:
			if (!transient_allocator) {
				transient_allocator = new (HeapAlloc(GetProcessHeap(), 0, sizeof(RegionAllocator))) RegionAllocator(REGION_PAGE_SIZE);
			}
			return transient_allocator->Allocate(size, leak_tracking, append_sentinel);
		case New::Hint::PERMANENT:
			if (!permanent_allocator) {
				permanent_allocator = new (HeapAlloc(GetProcessHeap(), 0, sizeof(RegionAllocator))) RegionAllocator(REGION_PAGE_SIZE);
			}
			return permanent_allocator->Allocate(size, leak_tracking, append_sentinel);
		default:
			if (!default_allocator) {
				default_allocator = new (HeapAlloc(GetProcessHeap(), 0, sizeof(RegionAllocator))) RegionAllocator(REGION_PAGE_SIZE);
			}
			return default_allocator->Allocate(size, leak_tracking, append_sentinel);
		}
	}
}

void* Heap::Resize(void* address, size_t new_size)
{
	Page** page = (Page**)ptrsub(address, sizeof(Page*));
	if (*page == nullptr)
	{
		return system_allocator->Resize(address, new_size, fill_on_free);
	}
	else
	{
		Allocator* allocator = ((Page*)*page)->allocator;
		return allocator->Resize(address, new_size, fill_on_free);
	}
}

void* Heap::Relocate(void* address)
{
	Page** page = (Page**)ptrsub(address, sizeof(Page*));
	if (*page == nullptr)
	{
		return system_allocator->Relocate(address, fill_on_free);
	}
	{
		Allocator* allocator = ((Page*)*page)->allocator;
		return allocator->Relocate(address, fill_on_free);
	}
}

void Heap::Free(void* address)
{
	Page** page = (Page**)ptrsub(address, sizeof(Page*));
	if (*page == nullptr)
	{
		system_allocator->Free(address, fill_on_free);
	}
	else
	{
		Allocator* allocator = ((Page*)*page)->allocator;
		allocator->Free(address, fill_on_free);
	}
}

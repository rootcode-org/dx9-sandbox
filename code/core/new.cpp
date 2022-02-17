/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include <memory>
#include "core/heap.h"

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(__formal(0, parameter0)) _VCRT_ALLOCATOR
void* __cdecl operator new (size_t size)
{
	return Heap::GetInstance()->Allocate(size, New::Hint::DEFAULT);
}


_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(__formal(0, parameter0)) _VCRT_ALLOCATOR
void* __cdecl operator new[](size_t size)
{
	return Heap::GetInstance()->Allocate(size, New::Hint::DEFAULT);
}


void* __cdecl operator new (size_t size, New::Hint hint)
{
	return Heap::GetInstance()->Allocate(size, hint);
}


void* __cdecl operator new[](size_t size, New::Hint hint)
{
	return Heap::GetInstance()->Allocate(size, hint);
}


void* resize(void* address, size_t new_size)
{
	if (address) {
		return Heap::GetInstance()->Resize(address, new_size);
	}
	return nullptr;
}


void* relocate(void* address)
{
	if (address) {
		return Heap::GetInstance()->Relocate(address);
	}
	return nullptr;
}


void __cdecl operator delete (void* address)
{
	if (address) {
		Heap::GetInstance()->Free(address);
	}
}


void __cdecl operator delete[](void* address)
{
	if (address) {
		Heap::GetInstance()->Free(address);
	}
}


// These delete operators execute if there is an exception in new(size_t size, New::Hint hint)
void __cdecl operator delete (void* address, New::Hint hint)
{
	(hint);	// unreferenced parameter
	if (address) {
		Heap::GetInstance()->Free(address);
	}
}


void __cdecl operator delete[](void* address, New::Hint hint)
{
	(hint);	// unreferenced parameter
	if (address) {
		Heap::GetInstance()->Free(address);
	}
}

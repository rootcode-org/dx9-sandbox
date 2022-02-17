/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

#include <mutex>
#include "core/new.h"

#define LARGE_ALLOCATION_SIZE ((size_t)32768)	// Allocations of this size or greater are made from system memory

class Heap
{
public:
	Heap() { memset(pools, 0, sizeof(pools)); }
	~Heap(void) {}

	void* Allocate(size_t size, New::Hint hint);
	void* Resize(void* address, size_t new_size);
	void* Relocate(void* address);
	void  Free(void* address);

	void EnableLeakTracking(bool flag) { leak_tracking = flag; }
	void EnableSentinel(bool flag) { append_sentinel = flag; }
	void EnableFillOnFree(bool flag) { fill_on_free = flag; }
	void VerifyIntegrity(void);
	void ReportLeaks(void);
	void TestAllocators(void);

	static Heap* GetInstance(void);

private:
	bool append_sentinel = false;
	bool leak_tracking = false;
	bool fill_on_free = false;
	class SystemAllocator* system_allocator = nullptr;
	class RegionAllocator* default_allocator = nullptr;
	class RegionAllocator* transient_allocator = nullptr;
	class RegionAllocator* permanent_allocator = nullptr;
	class PoolAllocator*   pools[LARGE_ALLOCATION_SIZE];
};

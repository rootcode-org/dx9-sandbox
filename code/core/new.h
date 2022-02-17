/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

// Allocation hints
namespace New
{
	enum class Hint {
		DEFAULT,		// allocation lifetime is undefined
		PERMANENT,		// allocation is permanent
		TRANSIENT,		// allocation is short-lived
		POOLABLE,		// allocations of this size are frequent and may be pooled
	};
}

// Global new operators with hint parameter
void* __cdecl operator new (size_t size, New::Hint hint);
void* __cdecl operator new[] (size_t size, New::Hint hint);

// Global methods to modify memory allocations
void* resize(void* address, size_t new_size);
void* relocate(void* address);

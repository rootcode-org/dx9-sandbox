/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include "stack.h"

#if defined(_WINDOWS)
#include <windows.h>
#endif

void* _stackInterlockedCompareAndExchangePointer (void* volatile* destination, void* exchange, void* comperand)
{
	#if defined(_WINDOWS)
	return InterlockedCompareExchangePointer(destination, exchange, comperand);
	#else
	// Unreferenced parameters
	(destination);(exchange);(comperand);
	return 0;
	#endif
}

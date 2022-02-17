/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include <intrin.h>
#include "math/algebra.h"

#pragma intrinsic(_BitScanReverse)

int algebra::msb32 (unsigned int i)
{
	#if defined(_WINDOWS)

	unsigned long out;
	_BitScanReverse(&out, i);
	return out;

	#else

	int bit = 0;
	if (i >> 16) {i >>= 16; bit  =16;}
	if (i >> 8)  {i >>= 8;  bit +=8;}
	if (i >> 4)  {i >>= 4;  bit +=4;}
	if (i >> 2)  {i >>= 2;  bit +=2;}
	bit += i >> 1;
	return bit;

	#endif
}


int algebra::msb64(unsigned long long i)
{
	#if defined(_WINDOWS)

	unsigned long out;
	_BitScanReverse64(&out, i);
	return out;

	#else

	int bit = 0;
	if (i >> 32) { i >>= 32; bit = 32; }
	if (i >> 16) { i >>= 16; bit += 16; }
	if (i >> 8)  { i >>= 8;  bit += 8; }
	if (i >> 4)  { i >>= 4;  bit += 4; }
	if (i >> 2)  { i >>= 2;  bit += 2; }
	bit += (int) i >> 1;
	return bit;

	#endif
}


bool algebra::isreal (float f)
{
	// If the exponent is 255 then the float represents a NaN or infinity and is therefore not a real number
	int i = *(int*) &f;
	return (i & 0x7f800000) != 0x7f800000;
}

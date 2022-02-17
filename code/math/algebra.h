/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

namespace algebra {

	int  msb32 (unsigned int i);			// Returns most significant bit of specified value
	int  msb64 (unsigned long long i);		// Returns most significant bit of specified value
	bool isreal (float f);					// Returns true if specified value is a real number (false if NaN or infinity)

	inline int log2 (unsigned int i)	{return msb32(i);}		// Returns Log to base 2 of specified integer
	inline bool ispow2 (unsigned int i) {return !(i & (i-1));}	// Returns true if an integer is a power of 2

	// Float constants
	const float FLT_PI = 3.141592654f;				// PI
	const float FLT_2PI = 6.283185307f;				// PI*2
	const float FLT_SQRT2 = 1.414213562f;			// Square root of 2
	const float FLT_SQRT3 = 1.732050808f;			// Square root of 3
	const float FLT_E = 2.718281828f;				// Euler's number (base of the natural logarithm)
	const float FLT_GOLDEN = 1.618033989f;			// Golden ratio

	inline float degrees_to_radians(float x) { return x * FLT_PI / 180.0f; }
	inline float radians_to_degrees(float x) { return x * 180.0f / FLT_PI; }
	inline float clamp(float x, float min, float max) { return x < min ? min : x > max ? max : x; }
	inline float lerp(float start, float end, float time) { return start + ((end - start)*time); }

	// Double constants
	const double DBL_PI     = 3.14159265358979323846;	// PI
	const double DBL_2PI    = 6.28318530717958647693;	// PI*2
	const double DBL_SQRT2  = 1.41421356237309504880;	// Square root of 2
	const double DBL_SQRT3  = 1.73205080756887729353;	// Square root of 3
	const double DBL_E      = 2.71828182845904523536;	// Euler's number (base of the natural logarithm)
	const double DBL_GOLDEN = 1.61803398874989484820;	// Golden ratio

	inline double degrees_to_radians (double x) {return x * DBL_PI / 180.0f;}
	inline double radians_to_degrees (double x) {return x * 180.0f / DBL_PI;}
	inline double clamp (double x, double min, double max) {return (x)<(min) ? (min) : (x)>(max) ? (max) : (x);}
	inline double lerp (double start, double end, double time) {return start+((end-start)*time);}
};

using namespace algebra;

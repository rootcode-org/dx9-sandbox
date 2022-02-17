/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include <cstdlib>
#include "math/polar.h"
#include "math/vector.h"

Polar& Polar::operator= (const Vector& v)
{
	radius = sqrtf((v.x*v.x)+(v.y*v.y)+(v.z*v.z));
	theta  = atan2f(v.z, v.x);
	phi    = acosf(v.y / radius);
	return *this;
}

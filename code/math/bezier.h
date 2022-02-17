/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once
#include "math/vector.h"

class Bezier {
public:
	Vector a,b,c,d;		// Member data is intentionally public

	void Evaluate (float time, Vector& result);		// Evaluate bezier at specified time (0.0 to 1.0)
};

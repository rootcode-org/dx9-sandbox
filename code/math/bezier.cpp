/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include "math/bezier.h"

void Bezier::Evaluate (float t, Vector& result)
{
	// Calculate first order interpolants
	Vector e,f,g;
	e.x = ((b.x - a.x) * t) + a.x;
	e.y = ((b.y - a.y) * t) + a.y;
	e.z = ((b.z - a.z) * t) + a.z;

	f.x = ((c.x - b.x) * t) + b.x;
	f.y = ((c.y - b.y) * t) + b.y;
	f.z = ((c.z - b.z) * t) + b.z;

	g.x = ((d.x - c.x) * t) + c.x;
	g.y = ((d.y - c.y) * t) + c.y;
	g.z = ((d.z - c.z) * t) + c.z;

	// Calculate second order interpolants
	Vector h,i;
	h.x = ((f.x - e.x) * t) + e.x;
	h.y = ((f.y - e.y) * t) + e.y;
	h.z = ((f.z - e.z) * t) + e.z;

	i.x = ((g.x - f.x) * t) + f.x;
	i.y = ((g.y - f.y) * t) + f.y;
	i.z = ((g.z - f.z) * t) + f.z;

	// Calculate third order interpolant
	result.x = ((i.x - h.x) * t) + i.x;
	result.y = ((i.y - h.y) * t) + i.y;
	result.z = ((i.z - h.z) * t) + i.z;
}

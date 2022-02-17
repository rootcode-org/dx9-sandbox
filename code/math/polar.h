/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

class Vector;

class Polar {
public:
	// Member data is intentionally public
	float radius,theta,phi;

	Polar(void) { radius = theta = phi = 0.0f; }
	Polar (float radius, float theta, float phi) {this->radius = radius; this->theta = theta; this->phi = phi;}
	Polar& operator= (const Vector& v);		// Copy operator
};

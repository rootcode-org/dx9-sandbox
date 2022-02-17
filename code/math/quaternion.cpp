/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include <cstdlib>
#include "math/vector.h"
#include "math/matrix.h"
#include "math/quaternion.h"

// Post-multiply and assign quaternion
Quaternion& Quaternion::operator *= (const Quaternion& q)
{
	float nw = w*q.w - x*q.x - y*q.y - z*q.z;
	float nx = w*q.x + x*q.w + y*q.z - z*q.y;
	float ny = w*q.y - x*q.z + y*q.w + z*q.x;
	float nz = w*q.z + x*q.y - y*q.x + z*q.w;
	w = nw; x = nx; y = ny; z = nz;
	return *this;
}


// Post-multiply quaternion
Quaternion Quaternion::operator * (const Quaternion& q) const
{
	float nw = w*q.w - x*q.x - y*q.y - z*q.z;
	float nx = w*q.x + x*q.w + y*q.z - z*q.y;
	float ny = w*q.y - x*q.z + y*q.w + z*q.x;
	float nz = w*q.z + x*q.y - y*q.x + z*q.w;
	return Quaternion(nw, nx, ny, nz);
}


// Initialize quaternion with rotation about the X axis
Quaternion& Quaternion::SetXRotation (float angle)
{
	w = cosf(angle/2);
	x = sinf(angle/2);
	y = 0;
	z = 0;
	return *this;
}


// Initialize quaternion with rotation about the Y axis
Quaternion& Quaternion::SetYRotation (float angle)
{
	w = cosf(angle/2);
	x = 0;
	y = sinf(angle/2);
	z = 0;
	return *this;
}


// Initialize quaternion with rotation about the Z axis
Quaternion& Quaternion::SetZRotation (float angle)
{
	w = cosf(angle/2);
	x = 0;
	y = 0;
	z = sinf(angle/2);
	return *this;
}


// Initialize quaternion with YXZ Euler angles
Quaternion& Quaternion::SetEulerZXY (float input_z, float input_x, float input_y)
{
	float sy = sinf(input_y/2);
	float cy = cosf(input_y/2);
	float sx = sinf(input_x/2);
	float cx = cosf(input_x/2);
	float sz = sinf(input_z/2);
	float cz = cosf(input_z/2);
	this->w = cy*cx*cz + sy*sx*sz;
	this->x = cy*sx*cz + sy*cx*sz;
	this->y = sy*cx*cz - cy*sx*sz;
	this->z = cy*cx*sz - sy*sx*cz;
	return *this;
}


// Initialize quaternion from an axis and angle of rotation
Quaternion& Quaternion::SetAxisAngle (Vector& axis, float angle)
{
	float s = sinf(angle/2);
	w = cosf(angle/2);
	x = s*axis.x;
	y = s*axis.y;
	z = s*axis.z;
	return *this;
}


// Calculate length of quaternion
float Quaternion::Magnitude (void) const
{
	return sqrtf(w*w + x*x + y*y + z*z);
}


// Normalize the quaternion
Quaternion& Quaternion::Normalize (void)
{
	// Calculate the magnitude of the quaternion
	float magnitude = sqrtf(w*w + x*x + y*y + z*z);

	// Divide the magnitude into each element to normalize
	w /= magnitude;
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;

	return *this;
}


// Pre-multiply quaternion
Quaternion& Quaternion::PreMultiply (const Quaternion& q)
{
	float nw = q.w*w - q.x*x - q.y*y - q.z*z;
	float nx = q.x*w + q.w*x + q.z*y - q.y*z;
	float ny = q.y*w - q.z*x + q.w*y + q.x*z;
	float nz = q.z*w + q.y*x - q.x*y + q.w*z;
	w = nw; x = nx; y = ny; z = nz;
	return *this;
}


// Linearly interpolate to another quaternion
Quaternion& Quaternion::Lerp (const Quaternion& q, float t)
{
	w = (1-t)*w + t*q.w;
	x = (1-t)*x + t*q.x;
	y = (1-t)*y + t*q.y;
	z = (1-t)*z + t*q.z;
	Normalize();
	return *this;
}


// Spherical linearly interpolate to another quaternion
Quaternion& Quaternion::Slerp (const Quaternion& q, float t)
{
	// Calculate the angle between the rotations
	float dot   = w*q.w + x*q.x + y*q.y + z*q.z;
	float angle = acosf(dot);

	// Calculate the interpolated quaternion
	float sin   = sinf(angle);
	float sin1t = sinf(angle*(1-t));
	float sint  = sinf(angle*t);
	w = ((w*sin1t) + (q.w*sint)) / sin;
	x = ((x*sin1t) + (q.x*sint)) / sin;
	y = ((y*sin1t) + (q.y*sint)) / sin;
	z = ((z*sin1t) + (q.z*sint)) / sin;

	return *this;
}

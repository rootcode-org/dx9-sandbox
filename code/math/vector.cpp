/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include <cstdlib>
#include "math/algebra.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "math/polar.h"

Vector::Vector (const Polar& p)
{
	x = p.radius * cosf(p.theta) * sinf(p.phi);
	y = p.radius * cosf(p.phi);
	z = p.radius * sinf(p.theta) * sinf(p.phi);
}


Vector& Vector::operator= (const Polar& p)
{
	x = p.radius * cosf(p.theta) * sinf(p.phi);
	y = p.radius * cosf(p.phi);
	z = p.radius * sinf(p.theta) * sinf(p.phi);
	return *this;
}


Vector Vector::operator^ (const Vector&v) const
{
	// Cross product
	float nx = (y*v.z) - (z*v.y);
	float ny = (z*v.x) - (x*v.z);
	float nz = (x*v.y) - (y*v.x);
	return Vector(nx, ny, nz);
}


Vector& Vector::operator^= (const Vector&v)
{
	// Cross product
	float nx = (y*v.z) - (z*v.y);
	float ny = (z*v.x) - (x*v.z);
	float nz = (x*v.y) - (y*v.x);

	// copy back to this vector
	x = nx; y = ny; z = nz;
	return *this;
}


float Vector::Magnitude(void) const
{
	return sqrtf((x*x) + (y*y) + (z*z));
}


float Vector::AngleTo (const Vector& v) const
{
	float a = sqrtf(x*x + y*y + z*z);
	float b = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	float d = x*v.x + y*v.y + z*v.z;
	float c = d / (a * b);
	return acosf(c);
}


float Vector::DistanceTo (const Vector& v) const
{
	float dx = v.x-x;
	float dy = v.y-y;
	float dz = v.z-z;
	return sqrtf ((dx*dx) + (dy*dy) + (dz*dz));
}


float Vector::SquaredDistanceTo (const Vector& v) const
{
	float dx = v.x-x;
	float dy = v.y-y;
	float dz = v.z-z;
	return (dx*dx) + (dy*dy) + (dz*dz);
}


// Determine if vectors are approximately equal
bool Vector::IsEquivalent (const Vector& v, float epsilon) const
{
	float dx = fabsf(x - v.x);
	if (dx > epsilon) return false;

	float dy = fabsf(y - v.y);
	if (dy > epsilon) return false;

	float dz = fabsf(z - v.z);
	if (dz > epsilon) return false;

	return true;
}


Vector& Vector::Normalize (void)
{
	float magnitude = sqrtf ((x*x) + (y*y) + (z*z));
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;
	return *this;
}


Vector& Vector::Lerp (const Vector& v, float t)
{
	x = lerp (x, v.x, t);
	y = lerp (y, v.y, t);
	z = lerp (z, v.z, t);
	return *this;
}


// Spherical linearly interpolate to another vector
Vector& Vector::Slerp (const Vector& v, float t)
{
	// Calculate the normal to the 2 vectors
	float nx = (y*v.z) - (z*v.y);
	float ny = (z*v.x) - (x*v.z);
	float nz = (x*v.y) - (y*v.x);
	Vector normal(nx, ny, nz);

	// Calculate the angle between the input vectors
	float angle = AngleTo(v);

	// Calculate a partial angle of rotation based on the interpolant
	angle *= t;

	// Rotate the vector around the normal by the partial angle
	RotateAboutVector (normal, angle);

	// Return a reference to this vector
	return *this;
}


/*
 Map vector onto another vector
 The mapped vector M is calculated as:
 
 M = (T.V / |V|²) * V

 where,
 M is the resulting mapped vector
 T is this vector
 V is the vector that this vector is being mapped onto
*/
Vector& Vector::Map (const Vector& v)
{
	// Calculate dot product of vectors (T.V)
	float dot = x*v.x + y*v.y + z*v.z;

	// Calculate squared magnitude of the vector to map onto (|V|²)
	float mag = v.x*v.x + v.y*v.y + v.z*v.z;

	// Now calculate the resultant vector
	x = (dot/mag) * v.x;
	y = (dot/mag) * v.y;
	z = (dot/mag) * v.z;
	return *this;
}


Vector& Vector::RotateAboutX (const float angle)
{
	float c  = cosf(angle);
	float s  = sinf(angle);
	float ny = y*c - z*s;
	float nz = y*s + z*c;;
	y = ny; z = nz;
	return *this;
}


Vector& Vector::RotateAboutY (const float angle)
{
	float c  = cosf(angle);
	float s  = sinf(angle);
	float nx = z*s + x*c;
	float nz = z*c - x*s;
	x = nx; z = nz;
	return *this;
}


Vector& Vector::RotateAboutZ (const float angle)
{
	float c  = cosf(angle);
	float s  = sinf(angle);
	float nx = x*c - y*s;
	float ny = x*s + y*c;
	x = nx; y = ny;
	return *this;
}


/*
 Rotate vector about an arbitrary axis
 The matrix for a rotation around a specified axis is,
 _                       _
 | dx²+c   dxy-sz dxz+sy |
 | dxy+sz  dy²+c  dyz-sx |
 | dxz-sy  dyz+sx dz²+c  |
 ¯                       ¯
 where c = cos(angle), s = sin(angle), d = 1-cos(angle), xyz = unit vector

 therefore our rotated vector is,
 _                       _   _ _   _                                   _
 | dx²+c   dxy-sz dxz+sy |   |p|   | p(dx²+c)  + q(dxy-sz) + r(dxz+sy) |
 | dxy+sz  dy²+c  dyz-sx | * |q| = | p(dxy+sz) + q(dy²+c)  + r(dyz-sx) |
 | dxz-sy  dyz+sx dz²+c  |   |r|   | p(dxz-sy) + q(dyz+sx) + r(dz²+c)  |
 ¯                       ¯   ¯ ¯   ¯                                   ¯
*/
Vector& Vector::RotateAboutVector (const Vector& v, float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);
	float d = 1.0f - c;
	float nx = (x*((d*v.x*v.x)+c))     + (y*((d*v.x*v.y)-s*v.z)) + (z*((d*v.x*v.z)+s*v.y));
	float ny = (x*((d*v.x*v.y)+s*v.z)) + (y*((d*v.y*v.y)+c))     + (z*((d*v.y*v.z)-s*v.x));
	float nz = (x*((d*v.x*v.z)-s*v.y)) + (y*((d*v.y*v.z)+s*v.x)) + (z*((d*v.z*v.z)+c));
	x = nx;	y = ny;	z = nz;
	return *this;
}


Vector& Vector::Reflect(const Vector& normal)
{
	float dot2 = 2 * this->DotProduct(normal);
	x = x - (normal.x * dot2);
	y = y - (normal.y * dot2);
	z = z - (normal.z * dot2);
	return *this;
}


Vector& Vector::Refract(const Vector& normal, float index)
{
	float dot = this->DotProduct(normal);
	float k = 1 - (index * index * (1 - (dot * dot)));
	if (k < 0.0) { x = y = z = 0.0; }
	else {
		float s = (index * dot) + sqrtf(k);
		x = (x * index) - (normal.x * s);
		y = (y * index) - (normal.y * s);
		z = (z * index) - (normal.z * s);
	}
	return *this;
}


/*
 Transform vector
 The w component of a vector is implicitly 0, hence the resulting vector does not receive
 the translation component of the matrix. The bottom row of the matrix is ignored.

 Note, this function is agnostic to the matrix format; row vector and column vector format both work correctly.
  _             _   _   _     _    _
  | rx ux ax tx |   | x |     | x` |
  | ry uy ay ty | * | y |  =  | y` |
  | rz uz az tz |   | z |     | z` |
  | 0  0  0  1  |   | 0 |     | 0  |
  -             -   -   -     -    -
*/
Vector& Vector::TransformVector (const Matrix& m)
{
	float nx = (m.rx * x) + (m.ux * y) + (m.ax * z);
	float ny = (m.ry * x) + (m.uy * y) + (m.ay * z);
	float nz = (m.rz * x) + (m.uz * y) + (m.az * z);
	x = nx; y = ny; z = nz;
	return *this;
}


/*
 Transform vector as a point
 The w component of a point is implicitly 1, hence the resulting vector receives the translation
 component of the matrix. The bottom row of the matrix is ignored.

 Note, this function is agnostic to the matrix format; row vector and column vector format both work correctly.
 _             _   _   _     _    _
  | rx ux ax tx |   | x |     | x` |
  | ry uy ay ty | * | y |  =  | y` |
  | rz uz az tz |   | z |     | z` |
  | 0  0  0  1  |   | 1 |     | 1  |
  ¯             ¯   ¯   ¯     ¯    ¯
*/
Vector& Vector::TransformPoint (const Matrix& m)
{
	float nx = (m.rx * x) + (m.ux * y) + (m.ax * z) + m.tx;
	float ny = (m.ry * x) + (m.uy * y) + (m.ay * z) + m.ty;
	float nz = (m.rz * x) + (m.uz * y) + (m.az * z) + m.tz;
	x = nx; y = ny; z = nz;
	return *this;
}

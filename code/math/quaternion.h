/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

class Vector;

class Quaternion {
public:
	// Member data is intentionally public
	float w,x,y,z;

	// Initializers
	Quaternion(void) {}																	// Construct uninitialized quaternion
	Quaternion(float W, float X, float Y, float Z) {w = W; x = X; y = Y; z = Z;}		// Construct and initialize quaternion
	Quaternion& SetIdentity  (void) {w = 1; x = 0; y = 0; z = 0; return *this;}			// Set identity quaternion
	Quaternion& SetXRotation (float angle);												// Initialize quaternion to a rotation about the X axis
	Quaternion& SetYRotation (float angle);												// Initialize quaternion to a rotation about the Y axis
	Quaternion& SetZRotation (float angle);												// Initialize quaternion to a rotation about the Z axis
	Quaternion& SetEulerZXY  (float z, float x, float y);								// Initialize quaternion to an Euler ZXY rotation
	Quaternion& SetAxisAngle (Vector& axis, float angle);								// Initialize quaternion to a rotation about an arbitrary axis

	// Operators
	Quaternion& operator += (const Quaternion& q)       {w+=q.w; x+=q.x; y+=q.y; z+=q.z; return *this;}					// Addition assignment operator
	Quaternion& operator -= (const Quaternion& q)       {w-=q.w; x-=q.x; y-=q.y; z-=q.z; return *this;}					// Subtraction assignment operator
	Quaternion& operator *= (const Quaternion& q);																		// Multiplication assignment operator
	Quaternion& operator *= (const float f)             {w*=f;   x*=f;   y*=f;   z*=f;   return *this;}					// Scalar multiplication assignment operator
	Quaternion& operator /= (const float f)             {w/=f;   x/=f;   y/=f;   z/=f;   return *this;}					// Scalar Division assignment operator
	Quaternion  operator +  (const Quaternion& q) const {return Quaternion(w+q.w, x+q.x, y+q.y, z+q.z);}				// Addition operator
	Quaternion  operator -  (void)                const {return Quaternion(-w,    -x,    -y,    -z)   ;}				// Negation operator
	Quaternion  operator -  (const Quaternion& q) const {return Quaternion(w-q.w, x-q.x, y-q.y, z-q.z);}				// Subtraction operator
    Quaternion  operator *  (const Quaternion& q) const;																// Multiplication operator
	Quaternion  operator *  (const float f)       const {return Quaternion(w*f,   x*f,   y*f,   z*f)  ;}				// Scalar multipliation operator
	Quaternion  operator /  (const float f)       const {return Quaternion(w/f,   x/f,   y/f,   z/f)  ;}				// Scalar division
	friend Quaternion operator* (const float f, const Quaternion& q) {return Quaternion(q.w*f, q.x*f, q.y*f, q.z*f);}	// Friend operator to pre-multiply by a scalar

	// Accessors
	float Magnitude  (void) const;															// Calculate length of quaternion
	float DotProduct (const Quaternion& q) const {return (w*q.w + x*q.x + y*q.y + z*q.z);}	// Calculate dot product

	// Modifiers
	Quaternion& Normalize   (void);									// Normalize to a unit quaternion
	Quaternion& PreMultiply (const Quaternion& q);					// Pre-Multiply by another quaternion
	Quaternion& Lerp        (const Quaternion& q, float t);			// Linear interpolate to another quaternion
	Quaternion& Slerp       (const Quaternion& q, float t);			// Spherical linear interpolate to another quaternion
};

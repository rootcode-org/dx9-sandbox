/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

class Matrix;
class Polar;

class Vector {
public:
	float x,y,z;			// member data is intentionally public

	// Initializers
	Vector(void) { x = y = z = 0.0f; }
	Vector(float x, float y, float z) {this->x = x; this->y = y; this->z = z;}
	Vector(float a[3])                {x = a[0]; y = a[1]; z = a[2];}
	Vector(const Polar& p);

	// Operators
	Vector& operator =  (const Polar& p);
	Vector& operator += (const Vector& v)       {x+=v.x; y+=v.y; z+=v.z; return *this;}
	Vector& operator -= (const Vector& v)       {x-=v.x; y-=v.y; z-=v.z; return *this;}
	Vector& operator *= (const float f)         {x*=f;   y*=f;   z*=f;   return *this;}
	Vector& operator /= (const float f)         {x/=f;   y/=f;   z/=f;   return *this;}
	Vector& operator ^= (const Vector& v);																// cross-product assignment operator
	Vector  operator +  (const Vector& v) const {return Vector(x+v.x, y+v.y, z+v.z);}
	Vector  operator -  (void)            const {return Vector(-x,    -y,    -z)   ;}
	Vector  operator -  (const Vector& v) const {return Vector(x-v.x, y-v.y, z-v.z);}
	Vector  operator *  (const float f)   const {return Vector(x*f,   y*f,   z*f)  ;}
	Vector  operator /  (const float f)   const {return Vector(x/f,   y/f,   z/f)  ;}
	Vector  operator ^  (const Vector& v) const;														// cross-product operator
	friend Vector operator* (const float f, const Vector& v) {return Vector(v.x*f, v.y*f, v.z*f);}		// pre-multiply by a scalar

	// Accessors
	float Magnitude         (void)            const;
	float SquaredMagnitude  (void)            const {return (x*x) + (y*y) + (z*z);}
	float DotProduct        (const Vector& v) const {return (x*v.x) + (y*v.y) + (z*v.z);}
	float Manhattan         (void)            const {return x+y+z;}
	float AngleTo           (const Vector& v) const;
	float DistanceTo        (const Vector& v) const;
	float SquaredDistanceTo (const Vector& v) const;
	bool  IsEquivalent      (const Vector& v, float epsilon) const;			// true if difference is less than or equal to epsilon

	// Modifiers
	Vector& SetIdentity       (void) {x = y = z = 0.0f; return *this;}
	Vector& Normalize         (void);
	Vector& Lerp              (const Vector& v, float t);
	Vector& Slerp             (const Vector& v, float t);
	Vector& Map               (const Vector& v);							// map vector onto another vector
	Vector& RotateAboutX      (const float angle);
	Vector& RotateAboutY      (const float angle);
	Vector& RotateAboutZ      (const float angle);
	Vector& Reflect           (const Vector& normal);						// Reflect about a plane
	Vector& Refract           (const Vector& normal, float index);			// Refract through a plane
	Vector& RotateAboutVector (const Vector& v, const float angle);
	Vector& TransformVector   (const Matrix& m);							// position is ignored
	Vector& TransformPoint    (const Matrix& m);							// position is not ignored
};

/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

class Vector;

// Select matrix format
#define MATRIX_COLUMN_MAJOR

class Matrix {
public:
	// Member data is intentionally public
#if defined(MATRIX_ROW_MAJOR)
	// In row vector format, composite matrix operations apply in left to right order
	float rx,ry,rz,rw;		// [x' y' z' w'] = [x y z w] * |rx ry rz rw|
	float ux,uy,uz,uw;		//                             |ux uy uz uw|
	float ax,ay,az,aw;		//                             |ax ay az aw|
	float tx,ty,tz,tw;		//                             |tx ty tz tw|
#elif defined(MATRIX_COLUMN_MAJOR)
	// In column vector format, composite matrix operations apply in right to left order
	float rx,ux,ax,tx;		// |x'| = |rx ux ax tx| * |x|
	float ry,uy,ay,ty;		// |y'| = |ry uy ay ty|   |y|
	float rz,uz,az,tz;		// |z'| = |rz uz az tz|   |z|
	float rw,uw,aw,tw;		// |w'| = |rw uw aw tw|   |w|
#endif

	// Initializers
	Matrix(void) {}														// Uninitialized matrix
	Matrix& InitWithIdentity     (void);
	Matrix& InitWithTranslation  (float x, float y, float z);
	Matrix& InitWithScale        (float x, float y, float z);
	Matrix& InitWithXRotation    (float angle);
	Matrix& InitWithYRotation    (float angle);
	Matrix& InitWithZRotation    (float angle);
	Matrix& InitWithExtrinsicYXZ (float y, float x, float z);			// Initialize with a Tait-Bryan YXZ rotation
	Matrix& InitWithExtrinsicZXY (float z, float x, float y);			// Initialize with a Tait-Bryan ZXY rotation
	Matrix& InitWithAxisAngle    (const Vector& axis, float angle);

	// Operators
	Matrix& operator += (const Matrix& m);
	Matrix& operator -= (const Matrix& m);
	Matrix& operator *= (const float f);
	Matrix& operator *= (const Matrix& m);
	Matrix& operator /= (const float f);
	Matrix  operator +  (const Matrix& m);
	Matrix  operator -  (void);
	Matrix  operator -  (const Matrix& m);
	Matrix  operator *  (const float f);
	Matrix  operator *  (const Matrix& m);
	Matrix  operator /  (const float f);
	friend Matrix operator * (const float f, const Matrix& m);

	// Accessors
	float  Determinant          (void) const;
	Vector GetTranslationVector (void) const;

	// Modifiers
	Matrix& PreMultiply       (const Matrix& m);
	Matrix& Transpose         (void);
	Matrix& Invert            (void);
	Matrix& InvertOrthogonal  (void);

	Matrix& TranslateX        (float distance);
	Matrix& TranslateY        (float distance);
	Matrix& TranslateZ        (float distance);
	Matrix& PreTranslateX     (float distance);			// translation precedes existing operations in matrix
	Matrix& PreTranslateY     (float distance);			// "
	Matrix& PreTranslateZ     (float distance);			// "

	Matrix& ScaleX            (float scale);
	Matrix& ScaleY            (float scale);
	Matrix& ScaleZ            (float scale);
	Matrix& PreScaleX         (float scale);			// scale precedes existing operations in matrix
	Matrix& PreScaleY         (float scale);			// "
	Matrix& PreScaleZ         (float scale);			// "

	Matrix& RotateAboutX      (float angle);
	Matrix& RotateAboutY      (float angle);
	Matrix& RotateAboutZ      (float angle);
	Matrix& PreRotateAboutX   (float angle);			// rotation precedes existing operations in matrix
	Matrix& PreRotateAboutY   (float angle);			// "
	Matrix& PreRotateAboutZ   (float angle);			// "
};

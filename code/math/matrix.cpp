/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include <cstdlib>
#include "math/vector.h"
#include "math/matrix.h"


Matrix& Matrix::operator += (const Matrix& m)
{
	rx += m.rx; ry += m.ry; rz += m.rz; rw += m.rw;
	ux += m.ux; uy += m.uy; uz += m.uz; uw += m.uw;
	ax += m.ax; ay += m.ay; az += m.az; aw += m.aw;
	tx += m.tx;	ty += m.ty; tz += m.tz; tw += m.tw;
	return *this;
}


Matrix& Matrix::operator -= (const Matrix& m)
{
	rx -= m.rx; ry -= m.ry;	rz -= m.rz;	rw -= m.rw;
	ux -= m.ux;	uy -= m.uy;	uz -= m.uz;	uw -= m.uw;
	ax -= m.ax;	ay -= m.ay;	az -= m.az;	aw -= m.aw;
	tx -= m.tx;	ty -= m.ty;	tz -= m.tz;	tw -= m.tw;
	return *this;
}


Matrix& Matrix::operator *= (const float f)
{
	rx *= f; ry *= f; rz *= f; rw *= f;
	ux *= f; uy *= f; uz *= f; uw *= f;
	ax *= f; ay *= f; az *= f; aw *= f;
	tx *= f; ty *= f; tz *= f; tw *= f;
	return *this;
}


Matrix& Matrix::operator *= (const Matrix& m)
{
	Matrix n;

#if defined(MATRIX_ROW_MAJOR)

	n.rx = rx*m.rx + ry*m.ux + rz*m.ax + rw*m.tx;
	n.ry = rx*m.ry + ry*m.uy + rz*m.ay + rw*m.ty;
	n.rz = rx*m.rz + ry*m.uz + rz*m.az + rw*m.tz;
	n.rw = rx*m.rw + ry*m.uw + rz*m.aw + rw*m.tw;

	n.ux = ux*m.rx + uy*m.ux + uz*m.ax + uw*m.tx;
	n.uy = ux*m.ry + uy*m.uy + uz*m.ay + uw*m.ty;
	n.uz = ux*m.rz + uy*m.uz + uz*m.az + uw*m.tz;
	n.uw = ux*m.rw + uy*m.uw + uz*m.aw + uw*m.tw;

	n.ax = ax*m.rx + ay*m.ux + az*m.ax + aw*m.tx;
	n.ay = ax*m.ry + ay*m.uy + az*m.ay + aw*m.ty;
	n.az = ax*m.rz + ay*m.uz + az*m.az + aw*m.tz;
	n.aw = ax*m.rw + ay*m.uw + az*m.aw + aw*m.tw;

	n.tx = tx*m.rx + ty*m.ux + tz*m.ax + tw*m.tx;
	n.ty = tx*m.ry + ty*m.uy + tz*m.ay + tw*m.ty;
	n.tz = tx*m.rz + ty*m.uz + tz*m.az + tw*m.tz;
	n.tw = tx*m.rw + ty*m.uw + tz*m.aw + tw*m.tw;

#elif defined(MATRIX_COLUMN_MAJOR)

	n.rx = rx*m.rx + ux*m.ry + ax*m.rz + tx*m.rw;
	n.ux = rx*m.ux + ux*m.uy + ax*m.uz + tx*m.uw;
	n.ax = rx*m.ax + ux*m.ay + ax*m.az + tx*m.aw;
	n.tx = rx*m.tx + ux*m.ty + ax*m.tz + tx*m.tw;

	n.ry = ry*m.rx + uy*m.ry + ay*m.rz + ty*m.rw;
	n.uy = ry*m.ux + uy*m.uy + ay*m.uz + ty*m.uw;
	n.ay = ry*m.ax + uy*m.ay + ay*m.az + ty*m.aw;
	n.ty = ry*m.tx + uy*m.ty + ay*m.tz + ty*m.tw;

	n.rz = rz*m.rx + uz*m.ry + az*m.rz + tz*m.rw;
	n.uz = rz*m.ux + uz*m.uy + az*m.uz + tz*m.uw;
	n.az = rz*m.ax + uz*m.ay + az*m.az + tz*m.aw;
	n.tz = rz*m.tx + uz*m.ty + az*m.tz + tz*m.tw;

	n.rw = rw*m.rx + uw*m.ry + aw*m.rz + tw*m.rw;
	n.uw = rw*m.ux + uw*m.uy + aw*m.uz + tw*m.uw;
	n.aw = rw*m.ax + uw*m.ay + aw*m.az + tw*m.aw;
	n.tw = rw*m.tx + uw*m.ty + aw*m.tz + tw*m.tw;

#endif

	// Copy result back to this matrix
	rx = n.rx; ry = n.ry; rz = n.rz; rw = n.rw;
	ux = n.ux; uy = n.uy; uz = n.uz; uw = n.uw;
	ax = n.ax; ay = n.ay; az = n.az; aw = n.aw;
	tx = n.tx; ty = n.ty; tz = n.tz; tw = n.tw;

	return *this;
}


Matrix& Matrix::operator /= (const float f)
{
	rx /= f; ry /= f; rz /= f; rw /= f;
	ux /= f; uy /= f; uz /= f; uw /= f;
	ax /= f; ay /= f; az /= f; aw /= f;
	tx /= f; ty /= f; tz /= f; tw /= f;
	return *this;
}


Matrix Matrix::operator + (const Matrix& m)
{
	Matrix n;
	n.rx = rx+m.rx; n.ry = ry+m.ry; n.rz = rz+m.rz; n.rw = rw+m.rw;
	n.ux = ux+m.ux; n.uy = uy+m.uy; n.uz = uz+m.uz;	n.uw = uw+m.uw;
	n.ax = ax+m.ax;	n.ay = ay+m.ay;	n.az = az+m.az;	n.aw = aw+m.aw;
	n.tx = tx+m.tx;	n.ty = ty+m.ty;	n.tz = tz+m.tz;	n.tw = tw+m.tw;
	return n;
}


Matrix Matrix::operator - (void)
{
	Matrix n;
	n.rx = -rx;	n.ry = -ry;	n.rz = -rz;	n.rw = -rw;
	n.ux = -ux;	n.uy = -uy;	n.uz = -uz;	n.uw = -uw;
	n.ax = -ax;	n.ay = -ay;	n.az = -az;	n.aw = -aw;
	n.tx = -tx;	n.ty = -ty;	n.tz = -tz;	n.tw = -tw;
	return n;
}


Matrix Matrix::operator - (const Matrix& m)
{
	Matrix n;
	n.rx = rx-m.rx; n.ry = ry-m.ry;	n.rz = rz-m.rz;	n.rw = rw-m.rw;
	n.ux = ux-m.ux;	n.uy = uy-m.uy;	n.uz = uz-m.uz;	n.uw = uw-m.uw;
	n.ax = ax-m.ax;	n.ay = ay-m.ay;	n.az = az-m.az;	n.aw = aw-m.aw;
	n.tx = tx-m.tx;	n.ty = ty-m.ty;	n.tz = tz-m.tz;	n.tw = tw-m.tw;
	return n;
}


Matrix Matrix::operator * (const float f)
{
	Matrix n;
	n.rx = rx*f; n.ry = ry*f; n.rz = rz*f; n.rw = rw*f;
	n.ux = ux*f; n.uy = uy*f; n.uz = uz*f; n.uw = uw*f;
	n.ax = ax*f; n.ay = ay*f; n.az = az*f; n.aw = aw*f;
	n.tx = tx*f; n.ty = ty*f; n.tz = tz*f; n.tw = tw*f;
	return n;
}


Matrix Matrix::operator * (const Matrix& m)
{
	Matrix n;

#if defined(MATRIX_ROW_MAJOR)

	n.rx = rx*m.rx + ry*m.ux + rz*m.ax + rw*m.tx;
	n.ry = rx*m.ry + ry*m.uy + rz*m.ay + rw*m.ty;
	n.rz = rx*m.rz + ry*m.uz + rz*m.az + rw*m.tz;
	n.rw = rx*m.rw + ry*m.uw + rz*m.aw + rw*m.tw;

	n.ux = ux*m.rx + uy*m.ux + uz*m.ax + uw*m.tx;
	n.uy = ux*m.ry + uy*m.uy + uz*m.ay + uw*m.ty;
	n.uz = ux*m.rz + uy*m.uz + uz*m.az + uw*m.tz;
	n.uw = ux*m.rw + uy*m.uw + uz*m.aw + uw*m.tw;

	n.ax = ax*m.rx + ay*m.ux + az*m.ax + aw*m.tx;
	n.ay = ax*m.ry + ay*m.uy + az*m.ay + aw*m.ty;
	n.az = ax*m.rz + ay*m.uz + az*m.az + aw*m.tz;
	n.aw = ax*m.rw + ay*m.uw + az*m.aw + aw*m.tw;

	n.tx = tx*m.rx + ty*m.ux + tz*m.ax + tw*m.tx;
	n.ty = tx*m.ry + ty*m.uy + tz*m.ay + tw*m.ty;
	n.tz = tx*m.rz + ty*m.uz + tz*m.az + tw*m.tz;
	n.tw = tx*m.rw + ty*m.uw + tz*m.aw + tw*m.tw;

#elif defined(MATRIX_COLUMN_MAJOR)

	n.rx = rx*m.rx + ux*m.ry + ax*m.rz + tx*m.rw;
	n.ux = rx*m.ux + ux*m.uy + ax*m.uz + tx*m.uw;
	n.ax = rx*m.ax + ux*m.ay + ax*m.az + tx*m.aw;
	n.tx = rx*m.tx + ux*m.ty + ax*m.tz + tx*m.tw;

	n.ry = ry*m.rx + uy*m.ry + ay*m.rz + ty*m.rw;
	n.uy = ry*m.ux + uy*m.uy + ay*m.uz + ty*m.uw;
	n.ay = ry*m.ax + uy*m.ay + ay*m.az + ty*m.aw;
	n.ty = ry*m.tx + uy*m.ty + ay*m.tz + ty*m.tw;

	n.rz = rz*m.rx + uz*m.ry + az*m.rz + tz*m.rw;
	n.uz = rz*m.ux + uz*m.uy + az*m.uz + tz*m.uw;
	n.az = rz*m.ax + uz*m.ay + az*m.az + tz*m.aw;
	n.tz = rz*m.tx + uz*m.ty + az*m.tz + tz*m.tw;

	n.rw = rw*m.rx + uw*m.ry + aw*m.rz + tw*m.rw;
	n.uw = rw*m.ux + uw*m.uy + aw*m.uz + tw*m.uw;
	n.aw = rw*m.ax + uw*m.ay + aw*m.az + tw*m.aw;
	n.tw = rw*m.tx + uw*m.ty + aw*m.tz + tw*m.tw;

#endif

	return n;
}


Matrix Matrix::operator / (const float f)
{
	Matrix n;
	n.rx = rx/f; n.ry = ry/f; n.rz = rz/f; n.rw = rw/f;
	n.ux = ux/f; n.uy = uy/f; n.uz = uz/f; n.uw = uw/f;
	n.ax = ax/f; n.ay = ay/f; n.az = az/f; n.aw = aw/f;
	n.tx = tx/f; n.ty = ty/f; n.tz = tz/f; n.tw = tw/f;
	return n;
}


Matrix operator * (const float f, const Matrix& m)
{
	Matrix n;
	n.rx = m.rx*f; n.ry = m.ry*f; n.rz = m.rz*f; n.rw = m.rw*f;
	n.ux = m.ux*f; n.uy = m.uy*f; n.uz = m.uz*f; n.uw = m.uw*f;
	n.ax = m.ax*f; n.ay = m.ay*f; n.az = m.az*f; n.aw = m.aw*f;
	n.tx = m.tx*f; n.ty = m.ty*f; n.tz = m.tz*f; n.tw = m.tw*f;
	return n;
}


float Matrix::Determinant (void) const
{
#if defined(MATRIX_ROW_MAJOR)

/*
det |rx ry rz rw|
    |ux uy uz uw|
    |ax ay az aw|
    |tx ty tz tw|

= rx * det |uy uz uw| - ry * det |ux uz uw| + rz * det |ux uy uw| - rw * det |ux uy uz|
           |ay az aw|            |ax az aw|	           |ax ay aw|            |ax ay az|
           |ty tz tw|            |tx tz tw|            |tx ty tw|            |tx ty tz|
*/
	float minorRX = uy*(az*tw-tz*aw) - uz*(ay*tw-ty*aw) + uw*(ay*tz-ty*az);
	float minorRY = ux*(az*tw-tz*aw) - uz*(ax*tw-tx*aw) + uw*(ax*tz-tx*az);
	float minorRZ = ux*(ay*tw-ty*aw) - uy*(ax*tw-tx*aw) + uw*(ax*ty-tx*ay);
	float minorRW = ux*(ay*tz-ty*az) - uy*(ax*tz-tx*az) + uz*(ax*ty-tx*ay);
	float determinant = rx*minorRX - ry*minorRY + rz*minorRZ - rw*minorRW;
	return determinant;

#elif defined(MATRIX_COLUMN_MAJOR)

/*
det |rx ux ax tx|
    |ry uy ay ty|
    |rz uz az tz|
    |rw uw aw tw|

= rx * det |uy ay ty| - ux * det |ry ay ty| + ax * det |ry uy ty| - tx * det |ry uy ay|
           |uz az tz|            |rz az tz|	           |rz uz tz|            |rz uz az|
           |uw aw tw|            |rw aw tw|            |rw aw tw|            |rw uw aw|
*/

	float minorRX = uy*(az*tw-aw*tz) - ay*(uz*tw-uw*tz) + ty*(uz*aw-uw*az);
	float minorUX = ry*(az*tw-aw*tz) - ay*(rz*tw-rw*tz) + ty*(rz*aw-rw*az);
	float minorAX = ry*(uz*tw-aw*tz) - uy*(rz*tw-rw*tz) + ty*(rz*aw-rw*uz);
	float minorTX = ry*(uz*aw-uw*az) - uy*(rz*aw-rw*az) + ay*(rz*uw-rw*uz);
	float determinant = rx*minorRX - ux*minorUX + ax*minorAX - tx*minorTX;
	return determinant;

#endif

}


Vector Matrix::GetTranslationVector (void) const
{
	return Vector(tx, ty, tz);
}


Matrix& Matrix::InitWithIdentity (void)
{
	rx = 1; ry = 0; rz = 0; rw = 0;
	ux = 0; uy = 1; uz = 0; uw = 0;
	ax = 0; ay = 0; az = 1; aw = 0;
	tx = 0; ty = 0; tz = 0; tw = 1;
	return *this;
}


Matrix& Matrix::InitWithTranslation (float x, float y, float z)
{
	rx = 1;	ry = 0;	rz = 0;	rw = 0;
	ux = 0;	uy = 1;	uz = 0;	uw = 0;
	ax = 0;	ay = 0;	az = 1;	aw = 0;
	tx = x;	ty = y;	tz = z;	tw = 1;
	return *this;
}


Matrix& Matrix::InitWithScale (float x, float y, float z)
{
	rx = x;	ry = 0;	rz = 0;	rw = 0;
	ux = 0;	uy = y;	uz = 0;	uw = 0;
	ax = 0;	ay = 0;	az = z;	aw = 0;
	tx = 0;	ty = 0;	tz = 0;	tw = 1;
	return *this;
}


Matrix& Matrix::InitWithXRotation (float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);
	rx = 1;	ry = 0;	rz = 0;	rw = 0;
	ux = 0;	uy = c;	uz = s;	uw = 0;
	ax = 0;	ay =-s;	az = c;	aw = 0;
	tx = 0;	ty = 0;	tz = 0;	tw = 1;
	return *this;
}


Matrix& Matrix::InitWithYRotation (float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);
	rx = c;	ry = 0;	rz =-s;	rw = 0;
	ux = 0;	uy = 1;	uz = 0;	uw = 0;
	ax = s;	ay = 0;	az = c;	aw = 0;
	tx = 0;	ty = 0;	tz = 0;	tw = 1;
	return *this;
}


Matrix& Matrix::InitWithZRotation (float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);
	rx = c;	ry = s;	rz = 0;	rw = 0;
	ux =-s;	uy = c;	uz = 0;	uw = 0;
	ax = 0;	ay = 0;	az = 1;	aw = 0;
	tx = 0;	ty = 0;	tz = 0;	tw = 1;
	return *this;
}


/*
 Initialize from an extrinsic YXZ representation

 ROW MAJOR FORMAT (left to right operations)

         Y                  X                    Z
   _            _     _            _     _             _
  | cy  0 -sy  0 |   | 1  0   0   0 |   |  cz  sz  0  0 |
  |  0  1   0  0 | * | 0  cx  sx  0 | * | -sz  cz  0  0 |  =
  | sy  0  cy  0 |   | 0 -sx  cx  0 |   |  0   0   1  0 |
  |  0  0   0  1 |   | 0  0   0   1 |   |  0   0   0  1 |
   ¯            ¯     ¯            ¯     ¯             ¯
   _                                        _
  |  cy.cz-sz.sy.sx  cy.sz+sy.sx.cz -sy.cx  0 |
  | -sz.cx           cx.cz              sx  0 |
  |  sy.cz+sz.cy.sx  sy.sz-cz.cy.sx  cy.cx  0 |
  |  0               0               0      1 |
   -                                        -

COLUMN MAJOR FORMAT (right to left operations)

           Z                  X                  Y
   _            _     _             _     _             _
  | cz -sz  0  0 |   |  1   0  0   0 |   |  cy  0  sy  0 |
  | sz  cz  0  0 | * |  0  cx -sx  0 | * |   0  1   0  0 |  =
  | 0    0  1  0 |   |  0  sx  cx  0 |   | -sy  0  cy  0 |
  | 0    0  0  1 |   |  0   0  0   1 |   |   0  0   0  1 |
   ¯            ¯     ¯             ¯     ¯             ¯
   _                                          _
  | cy.cz-sz.sy.sx  -sz.cx  sy.cz+sz.cy.sx   0 |
  | cy.sz+sy.sx.cz   cx.cz  sy.sz-cz.cy.sx   0 |
  |-sy.cx            sx     cy.cx            0 |
  | 0                0      0                0 |
   -                                          -

where,
 cx = cos(x), cy = cos(y), cz = cos(z)
 sx = sin(x), sy = sin(y), sz = sin(z)
*/
Matrix& Matrix::InitWithExtrinsicYXZ (float y, float x, float z)
{
	float cx = cosf(x);	float cy = cosf(y);	float cz = cosf(z);
	float sx = sinf(x);	float sy = sinf(y);	float sz = sinf(z);

	rx = cy*cz - sz*sy*sx;
	ry = cy*sz + sy*sx*cz;
	rz = -sy*cx;
	
	ux = -sz*cx;
	uy = cx*cz;
	uz = sx;

	ax = sy*cz + sz*cy*sx;
	ay = sy*sz - cz*cy*sx;
	az = cy*cx;

	rw = uw = aw = 0.0f;
	tx = ty = tz = 0.0f;
	tw = 1.0f;

	return *this;
}


/*
 Initialize from an extrinsic ZXY representation

  ROW MAJOR FORMAT (left to right operations)

           Z                 X                  Y
   _             _     _            _     _            _     
  |  cz  sz  0  0 |   | 1   0   0  0 |   | cy  0 -sy  0 |     
  | -sz  cz  0  0 | * | 0  cx  sx  0 | * |  0  1   0  0 |   = 
  |   0   0  1  0 |   | 0 -sx  cx  0 |   | sy  0  cy  0 |     
  |   0   0  0  1 |   | 0   0   0  1 |   |  0  0   0  1 |     
   ¯             ¯     ¯            ¯     ¯            ¯     
   _                                        _
  | cz.cy+sz.sx.sy  sz.cx  sz.sx.cy-cz.sy  0 |
  | cz.sx.sy-sz.cy  cz.cx  sz.sy+cz.sx.cy  0 |
  | cx.sy          -sx     cx.cy           0 |
  | 0               0      0               1 |
   ¯                                        ¯

COLUMN MAJOR FORMAT (right to left operations)

		   Y                 X                  Z
   _             _     _            _     _            _
  |  cy  0  sy  0 |   | 1   0   0  0 |   | cz -sz  0  0 |
  |   0  1  0   0 | * | 0  cx -sx  0 | * | sz  cz  0  0 |   =
  | -sy  0  cy  0 |   | 0  sx  cx  0 |   |  0  0   1  0 |
  |   0  0  0   1 |   | 0   0   0  1 |   |  0  0   0  1 |
   ¯             ¯     ¯            ¯     ¯            ¯
   _                                        _
  | cz.cy+sz.sx.sy  cz.sx.sy-sz.cy  cx.sy  0 |
  | sz.cx           cz.cx          -sx     0 |
  | sz.sx.cy-cz.sy  sz.sy+cz.sx.cy  cx.cy  0 |
  | 0               0               0      1 |
   ¯                                        ¯
where,
 cx = cos(x), cy = cos(y), cz = cos(z)
 sx = sin(x), sy = sin(y), sz = sin(z)
*/
Matrix& Matrix::InitWithExtrinsicZXY (float z, float x, float y)
{
	float cx = cosf(x);	float cy = cosf(y);	float cz = cosf(z);
	float sx = sinf(x);	float sy = sinf(y);	float sz = sinf(z);

	rx = cz*cy+sz*sx*sy;
	ry = sz*cx;
	rz = sz*sx*cy-cz*sy;

	ux = cz*sx*sy-sz*cy;
	uy = cz*cx;
	uz = sz*sy+cz*sx*cy;

	ax = cx*sy;
	ay = -sx;
	az = cx*cy;

	rw = uw = aw = 0.0f;
	tx = ty = tz = 0.0f;
	tw = 1.0f;

	return *this;
}


Matrix& Matrix::InitWithAxisAngle(const Vector& axis, float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);
	float t = 1.0f - c;

	rx = t*axis.x*axis.x + c;
	ry = t*axis.x*axis.y + axis.z*s;
	rz = t*axis.x*axis.z - axis.y*s;

	ux = t*axis.x*axis.y - axis.z*s;
	uy = t*axis.y*axis.y + c;
	uz = t*axis.y*axis.z + axis.x*s;

	ax = t*axis.x*axis.z + axis.y*s;
	ay = t*axis.y*axis.z - axis.x*s;
	az = t*axis.z*axis.z + c;

	rw = uw = aw = 0.0f;
	tx = ty = tz = 0.0f;
	tw = 1.0f;

	return *this;
}


Matrix& Matrix::PreMultiply (const Matrix& m)
{
	Matrix n;

#if defined(MATRIX_ROW_MAJOR)

	n.rx = m.rx*rx + m.ry*ux + m.rz*ax + m.rw*tx;
	n.ry = m.rx*ry + m.ry*uy + m.rz*ay + m.rw*ty;
	n.rz = m.rx*rz + m.ry*uz + m.rz*az + m.rw*tz;
	n.rw = m.rx*rw + m.ry*uw + m.rz*aw + m.rw*tw;

	n.ux = m.ux*rx + m.uy*ux + m.uz*ax + m.uw*tx;
	n.uy = m.ux*ry + m.uy*uy + m.uz*ay + m.uw*ty;
	n.uz = m.ux*rz + m.uy*uz + m.uz*az + m.uw*tz;
	n.uw = m.ux*rw + m.uy*uw + m.uz*aw + m.uw*tw;

	n.ax = m.ax*rx + m.ay*ux + m.az*ax + m.aw*tx;
	n.ay = m.ax*ry + m.ay*uy + m.az*ay + m.aw*ty;
	n.az = m.ax*rz + m.ay*uz + m.az*az + m.aw*tz;
	n.aw = m.ax*rw + m.ay*uw + m.az*aw + m.aw*tw;

	n.tx = m.tx*rx + m.ty*ux + m.tz*ax + m.tw*tx;
	n.ty = m.tx*ry + m.ty*uy + m.tz*ay + m.tw*ty;
	n.tz = m.tx*rz + m.ty*uz + m.tz*az + m.tw*tz;
	n.tw = m.tx*rw + m.ty*uw + m.tz*aw + m.tw*tw;

#elif defined(MATRIX_COLUMN_MAJOR)

	n.rx = m.rx*rx + m.ux*ry + m.ax*rz + m.tx*rw;
	n.ux = m.rx*ux + m.ux*uy + m.ax*uz + m.tx*uw;
	n.ax = m.rx*ax + m.ux*ay + m.ax*az + m.tx*aw;
	n.tx = m.rx*tx + m.ux*ty + m.ax*tz + m.tx*tw;

	n.ry = m.ry*rx + m.uy*ry + m.ay*rz + m.ty*rw;
	n.uy = m.ry*ux + m.uy*uy + m.ay*uz + m.ty*uw;
	n.ay = m.ry*ax + m.uy*ay + m.ay*az + m.ty*aw;
	n.ty = m.ry*tx + m.uy*ty + m.ay*tz + m.ty*tw;

	n.rz = m.rz*rx + m.uz*ry + m.az*rz + m.tz*rw;
	n.uz = m.rz*ux + m.uz*uy + m.az*uz + m.tz*uw;
	n.az = m.rz*ax + m.uz*ay + m.az*az + m.tz*aw;
	n.tz = m.rz*tx + m.uz*ty + m.az*tz + m.tz*tw;

	n.rw = m.rw*rx + m.uw*ry + m.aw*rz + m.tw*rw;
	n.uw = m.rw*ux + m.uw*uy + m.aw*uz + m.tw*uw;
	n.aw = m.rw*ax + m.uw*ay + m.aw*az + m.tw*aw;
	n.tw = m.rw*tx + m.uw*ty + m.aw*tz + m.tw*tw;

#endif

	// Copy result back to this matrix
	rx = n.rx; ry = n.ry; rz = n.rz; rw = n.rw;
	ux = n.ux; uy = n.uy; uz = n.uz; uw = n.uw;
	ax = n.ax; ay = n.ay; az = n.az; aw = n.aw;
	tx = n.tx; ty = n.ty; tz = n.tz; tw = n.tw;
	return *this;
}


Matrix& Matrix::Transpose (void)
{
	float temp;
	temp = ux; ux = ry; ry = temp;
	temp = ax; ax = rz; rz = temp;
	temp = tx; tx = rw; rw = temp;
	temp = ay; ay = uz; uz = temp;
	temp = ty; ty = uw; uw = temp;
	temp = tz; tz = aw; aw = temp;
	return *this;
}


Matrix& Matrix::Invert (void)
{
#if defined(MATRIX_ROW_MAJOR)

	float minorRX = uy*(az*tw-tz*aw) - uz*(ay*tw-ty*aw) + uw*(ay*tz-ty*az);
	float minorRY = ux*(az*tw-tz*aw) - uz*(ax*tw-tx*aw) + uw*(ax*tz-tx*az);
	float minorRZ = ux*(ay*tw-ty*aw) - uy*(ax*tw-tx*aw) + uw*(ax*ty-tx*ay);
	float minorRW = ux*(ay*tz-ty*az) - uy*(ax*tz-tx*az) + uz*(ax*ty-tx*ay);

	float minorUX = ry*(az*tw-tz*aw) - rz*(ay*tw-ty*aw) + rw*(ay*tz-ty*az);
	float minorUY = rx*(az*tw-tz*aw) - rz*(ax*tw-tx*aw) + rw*(ax*tz-tx*az);
	float minorUZ = rx*(ay*tw-ty*aw) - ry*(ax*tw-tx*aw) + rw*(ax*ty-tx*ay);
	float minorUW = rx*(ay*tz-ty*az) - ry*(ax*tz-tx*az) + rz*(ax*ty-tx*ay);

	float minorAX = ry*(uz*tw-tz*uw) - rz*(uy*tw-ty*uw) + rw*(uy*tz-ty*uz);
	float minorAY = rx*(uz*tw-tz*uw) - rz*(ux*tw-tx*uw) + rw*(ux*tz-tx*uz);
	float minorAZ = rx*(uy*tw-ty*uw) - ry*(ux*tw-tx*uw) + rw*(ux*ty-tx*uy);
	float minorAW = rx*(uy*tz-ty*uz) - ry*(ux*tz-tx*uz) + rz*(ux*ty-tx*uy);

	float minorTX = ry*(uz*aw-az*uw) - rz*(uy*aw-ay*uw) + rw*(uy*az-ay*uz);
	float minorTY = rx*(uz*aw-az*uw) - rz*(ux*aw-ax*uw) + rw*(ux*az-ax*uz);
	float minorTZ = rx*(uy*aw-ay*uw) - ry*(ux*aw-ax*uw) + rw*(ux*ay-ax*uy);
	float minorTW = rx*(uy*az-ay*uz) - ry*(ux*az-ax*uz) + rz*(ux*ay-ax*uy);

	// Convert each minor to its cofactor by multiplying by -1 to the power of its added row and column numbers
	// Then divide each cofactor by the determinant and store it back into the matrix in a transposed order
	float determinant = rx*minorRX - ry*minorRY + rz*minorRZ - rw*minorRW;
	rx =  minorRX/determinant;
	ry = -minorUX/determinant;
	rz =  minorAX/determinant;
	rw = -minorTX/determinant;

	ux = -minorRY/determinant;
	uy =  minorUY/determinant;
	uz = -minorAY/determinant;
	uw =  minorTY/determinant;

	ax =  minorRZ/determinant;
	ay = -minorUZ/determinant;
	az =  minorAZ/determinant;
	aw = -minorTZ/determinant;

	tx = -minorRW/determinant;
	ty =  minorUW/determinant;
	tz = -minorAW/determinant;
	tw =  minorTW/determinant;

#elif defined(MATRIX_COLUMN_MAJOR)

	float minorRX = uy*(az*tw-aw*tz) - ay*(uz*tw-uw*tz) + ty*(uz*aw-uw*az);
	float minorUX = ry*(az*tw-aw*tz) - ay*(rz*tw-rw*tz) + ty*(rz*aw-rw*az);
	float minorAX = ry*(uz*tw-uw*tz) - uy*(rz*tw-rw*tz) + ty*(rz*uw-rw*uz);
	float minorTX = ry*(uz*aw-uw*az) - uy*(rz*aw-rw*az) + ay*(rz*uw-rw*uz);

	float minorRY = ux*(az*tw-aw*tz) - ax*(uz*tw-uw*tz) + tx*(uz*aw-uw*az);
	float minorUY = rx*(az*tw-aw*tz) - ax*(rz*tw-rw*tz) + tx*(rz*aw-rw*az);
	float minorAY = rx*(uz*tw-uw*tz) - ux*(rz*tw-rw*tz) + tx*(rz*uw-rw*uz);
	float minorTY = rx*(uz*aw-uw*az) - ux*(rz*aw-rw*az) + ax*(rz*uw-rw*uz);

	float minorRZ = ux*(ay*tw-aw*ty) - ax*(uy*tw-uw*ty) + tx*(uy*aw-uw*ay);
	float minorUZ = rx*(ay*tw-aw*ty) - ax*(ry*tw-rw*ty) + tx*(ry*aw-rw*ay);
	float minorAZ = rx*(uy*tw-uw*ty) - ux*(ry*tw-rw*ty) + tx*(ry*uw-rw*uy);
	float minorTZ = rx*(uy*aw-uw*ay) - ux*(ry*aw-rw*ay) + ax*(ry*uw-rw*uy);

	float minorRW = ux*(ay*tz-az*ty) - ax*(uy*tz-uz*ty) + tx*(uy*az-uz*ay);
	float minorUW = rx*(ay*tz-az*ty) - ax*(ry*tz-rz*ty) + tx*(ry*az-rz*ay);
	float minorAW = rx*(uy*tz-uz*ty) - ux*(ry*tz-rz*ty) + tx*(ry*uz-rz*uy);
	float minorTW = rx*(uy*az-uz*ay) - ux*(ry*az-rz*ay) + ax*(ry*uz-rz*uy);

	// Convert each minor to its cofactor by multiplying it by -1 to the power of its added row and column numbers
	// Then divide each cofactor by the determinant and store it back into the matrix in a transposed order
	float determinant = rx*minorRX - ux*minorUX + ax*minorAX - tx*minorTX;
	rx =  minorRX / determinant;
	ry = -minorUX / determinant;
	rz =  minorAX / determinant;
	rw = -minorTX / determinant;

	ux = -minorRY / determinant;
	uy =  minorUY / determinant;
	uz = -minorAY / determinant;
	uw =  minorTY / determinant;

	ax =  minorRZ / determinant;
	ay = -minorUZ / determinant;
	az =  minorAZ / determinant;
	aw = -minorTZ / determinant;

	tx = -minorRW / determinant;
	ty =  minorUW / determinant;
	tz = -minorAW / determinant;
	tw =  minorTW / determinant;

#endif

	return *this;
}


// If the 3x3 rotation matrix is orthogonal we can invert more quickly by transposing the rotation matrix. However we
// still need to invert the translation.
Matrix& Matrix::InvertOrthogonal(void)
{
#if defined(MATRIX_ROW_MAJOR)

	// Calculate the inverse translation
	float minorRX = uy*(az*tw-tz*aw) - ay*(uz*tw-tz*uw) + ty*(uz*aw-az*uw);
	float minorRY = ux*(az*tw-tz*aw) - ax*(uz*tw-tz*uw) + tx*(uz*aw-az*uw);
	float minorRZ = ux*(ay*tw-ty*aw) - ax*(uy*tw-ty*uw) + tx*(uy*aw-ay*uw);
	float minorRW = ux*(ay*tz-ty*az) - ax*(uy*tz-ty*uz) + tx*(uy*az-ay*uz);
	float determinant = rx*minorRX - ry*minorRY + rz*minorRZ - rw*minorRW;

	float minorUW = rx*(ay*tz-ty*az) - ax*(ry*tz-ty*rz) + tx*(ry*az-ay*rz);
	float minorAW = rx*(uy*tz-ty*uz) - ux*(ry*tz-ty*rz) + tx*(ry*uz-uy*rz);
	tx = -minorRW/determinant;
	ty =  minorUW/determinant;
	tz = -minorAW/determinant;

#elif defined (MATRIX_COLUMN_MAJOR)

	// Calculate the inverse translation
	float minorRX = uy*(az*tw-aw*tz) - ay*(uz*tw-uw*tz) + ty*(uz*aw-uw*az);
	float minorUX = ry*(az*tw-aw*tz) - ay*(rz*tw-rw*tz) + ty*(rz*aw-rw*az);
	float minorAX = ry*(uz*tw-aw*tz) - uy*(rz*tw-rw*tz) + ty*(rz*aw-rw*uz);
	float minorTX = ry*(uz*aw-uw*az) - uy*(rz*aw-rw*az) + ay*(rz*uw-rw*uz);
	float determinant = rx*minorRX - ux*minorUX + ax*minorAX - tx*minorTX;

	float minorRW = ux*(ay*tz-az*ty) - ax*(uy*tz-uz*ty) + tx*(uy*az-uz*ay);
	float minorUW = rx*(ay*tz-az*ty) - ax*(ry*tz-rz*ty) + tx*(ry*az-rz*ay);
	float minorAW = rx*(uy*tz-uz*ty) - ux*(ry*tz-rz*ty) + tx*(ry*uz-rz*uy);
	tx = -minorRW / determinant;
	ty =  minorUW / determinant;
	tz = -minorAW / determinant;

#endif

	// Transpose the 3x3 rotation matrix
	float temp;
	temp = ux; ux = ry; ry = temp;
	temp = ax; ax = rz; rz = temp;
	temp = ay; ay = uz; uz = temp;

	return *this;
}


Matrix& Matrix::TranslateX (float distance)
{
	// In row major format this is equivalent to post-multiplying with a translation matrix
	// In column major format this is equivalent to pre-multiplying with a translation matrix
	rx += distance * rw;
	ux += distance * uw;
	ax += distance * aw;
	tx += distance * tw;
	return *this;
}


Matrix& Matrix::TranslateY (float distance)
{
	ry += distance * rw;
	uy += distance * uw;
	ay += distance * aw;
	ty += distance * tw;
	return *this;
}


Matrix& Matrix::TranslateZ (float distance)
{
	rz += distance * rw;
	uz += distance * uw;
	az += distance * aw;
	tz += distance * tw;
	return *this;
}


Matrix& Matrix::PreTranslateX(float distance)
{
	// In row major format this is equivalent to pre-multiplying with a translation matrix
	// In column major format this is equivalent to post-multiplying with a translation matrix
	tx += distance * rx;
	ty += distance * ry;
	tz += distance * rz;
	tw += distance * rw;
	return *this;
}


Matrix& Matrix::PreTranslateY(float distance)
{
	tx += distance * ux;
	ty += distance * uy;
	tz += distance * uz;
	tw += distance * uw;
	return *this;
}


Matrix& Matrix::PreTranslateZ(float distance)
{
	tx += distance * ax;
	ty += distance * ay;
	tz += distance * az;
	tw += distance * aw;
	return *this;
}


Matrix& Matrix::ScaleX (float scale)
{
	rx *= scale;
	ux *= scale;
	ax *= scale;
	tx *= scale;
	return *this;
}


Matrix& Matrix::ScaleY (float scale)
{
	ry *= scale;
	uy *= scale;
	ay *= scale;
	ty *= scale;
	return *this;
}


Matrix& Matrix::ScaleZ (float scale)
{
	rz *= scale;
	uz *= scale;
	az *= scale;
	tz *= scale;
	return *this;
}


Matrix& Matrix::PreScaleX(float scale)
{
	rx *= scale;
	ry *= scale;
	rz *= scale;
	rw *= scale;
	return *this;
}


Matrix& Matrix::PreScaleY(float scale)
{
	ux *= scale;
	uy *= scale;
	uz *= scale;
	uw *= scale;
	return *this;
}


Matrix& Matrix::PreScaleZ(float scale)
{
	ax *= scale;
	ay *= scale;
	az *= scale;
	aw *= scale;
	return *this;
}


Matrix& Matrix::RotateAboutX (float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);
	float nry = ry*c - rz*s;
	float nrz = rz*c + ry*s;
	float nuy = uy*c - uz*s;
	float nuz = uz*c + uy*s;
	float nay = ay*c - az*s;
	float naz = az*c + ay*s;
	float nty = ty*c - tz*s;
	float ntz = tz*c + ty*s;
	ry = nry; rz = nrz; uy = nuy; uz = nuz;
	ay = nay; az = naz; ty = nty; tz = ntz;
	return *this;
}


Matrix& Matrix::RotateAboutY (float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);
	float nrx = rx*c + rz*s;
	float nrz = rz*c - rx*s;
	float nux = ux*c + uz*s;
	float nuz = uz*c - ux*s;
	float nax = ax*c + az*s;
	float naz = az*c - ax*s;
	float ntx = tx*c + tz*s;
	float ntz = tz*c - tx*s;
	rx = nrx; rz = nrz; ux = nux; uz = nuz;
	ax = nax; az = naz; tx = ntx; tz = ntz;
	return *this;
}


Matrix& Matrix::RotateAboutZ (float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);
	float nrx = rx*c - ry*s;
	float nry = ry*c + rx*s;
	float nux = ux*c - uy*s;
	float nuy = uy*c + ux*s;
	float nax = ax*c - ay*s;
	float nay = ay*c + ax*s;
	float ntx = tx*c - ty*s;
	float nty = ty*c + tx*s;
	rx = nrx; ry = nry; ux = nux; uy = nuy;
	ax = nax; ay = nay; tx = ntx; ty = nty;
	return *this;
}


Matrix& Matrix::PreRotateAboutX(float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);
	float nux = ux*c + ax*s;
	float nuy = uy*c + ay*s;
	float nuz = uz*c + az*s;
	float nuw = uw*c + aw*s;
	float nax = ax*c - ux*s;
	float nay = ay*c - uy*s;
	float naz = az*c - uz*s;
	float naw = aw*c - uw*s;
	ux = nux; uy = nuy; uz = nuz; uw = nuw;
	ax = nax; ay = nay; az = naz; aw = naw;
	return *this;
}


Matrix& Matrix::PreRotateAboutY(float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);
	float nrx = rx*c - ax*s;
	float nry = ry*c - ay*s;
	float nrz = rz*c - az*s;
	float nrw = rw*c - aw*s;
	float nax = ax*c + rx*s;
	float nay = ay*c + ry*s;
	float naz = az*c + rz*s;
	float naw = aw*c + rw*s;
	rx = nrx; ry = nry; rz = nrz; rw = nrw;
	ax = nax; ay = nay; az = naz; aw = naw;
	return *this;
}


Matrix& Matrix::PreRotateAboutZ(float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);
	float nrx = rx*c + ux*s;
	float nry = ry*c + uy*s;
	float nrz = rz*c + uz*s;
	float nrw = rw*c + uw*s;
	float nux = ux*c - rx*s;
	float nuy = uy*c - ry*s;
	float nuz = uz*c - rz*s;
	float nuw = uw*c - rw*s;
	rx = nrx; ry = nry; rz = nrz; rw = nrw;
	ux = nux; uy = nuy; uz = nuz; uw = nuw;
	return *this;
}


//Matrix& Matrix::RotateAboutAxis (const Vector& v, float angle)
//{
//	Matrix r;
//	r.InitWithAxisAngle(v, angle);
//	PreMultiply(r);
//	return *this;
//}


/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "graphics/vertex.h"

void Vertex::Transform (Vertex4C out[], Vertex3C in[], Matrix& m, int num_vertices)
{
	for (; num_vertices; num_vertices--)
	{
		// Transform vector
		out->x = (m.rx * in->x) + (m.ux * in->y) + (m.ax * in->z) + m.tx;
		out->y = (m.ry * in->x) + (m.uy * in->y) + (m.ay * in->z) + m.ty;
		out->z = (m.rz * in->x) + (m.uz * in->y) + (m.az * in->z) + m.tz;
		out->w = (m.rw * in->x) + (m.uw * in->y) + (m.aw * in->z) + m.tw;

		// Convert vertex to cartesian space
		out->x /= out->w;
		out->y /= out->w;
		out->z /= out->w;

		// Copy color
		out->color = in->color;

		// Stride to next
		out++;
		in++;
	}
}

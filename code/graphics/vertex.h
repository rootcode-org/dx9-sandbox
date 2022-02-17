/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once
#include "graphics/d3d9.h"

class Vector;
class Matrix;

namespace Vertex {

	struct Vertex3C {float x, y, z; unsigned color;};
	struct Vertex4C {float x, y, z, w; unsigned color;};

	void Transform (
		Vertex4C out[],		// Array to hold transformed vertices
		Vertex3C in[],		// Array of untransformed vertices
		Matrix& m,			// Transformation matrix
		int num_vertices	// Number of vertices to transform
	);
};

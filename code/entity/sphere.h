/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once
#include "math/matrix.h"
#include "entity/entity.h"

class Viewport;

class Sphere : public Entity {
public:
	Sphere (float x, float y, float z);
	~Sphere (void);

private:
	float m_x, m_y, m_z;
	float m_spin;

	void Update (float frame_time);
	void Draw (Viewport& viewport);
	void CreateResources (void);
	void DestroyResources (void);
};

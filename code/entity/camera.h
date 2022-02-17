/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once
#include "math/matrix.h"
#include "entity/entity.h"
#include "core/keyboard.h"

class Viewport;

class Camera : public Entity {
public:
	Camera (Viewport& viewport);

private:
	void Update (float frame_time);

	float m_x, m_y, m_z;
	float m_heading, m_pitch, m_roll;
	Viewport* m_viewport;
};

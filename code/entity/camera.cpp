/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include "core/keyboard.h"
#include "graphics/viewport.h"
#include "entity/camera.h"

Camera::Camera (Viewport& viewport)
{
	m_x = m_y = m_heading = m_pitch = m_roll = 0.0f;
	m_z = -200.0f;
	m_matrix.InitWithIdentity();
	m_viewport = &viewport;
}


void Camera::Update(float frame_time)
{
	Keyboard* k = Keyboard::GetInstance();
	if (k->IsDown (0x27)) m_heading += (frame_time / 2);		// Right arrow
	if (k->IsDown (0x25)) m_heading -= (frame_time / 2);		// Left arrow
	float xdisp = sinf(m_heading) * 4;
	float zdisp = cosf(m_heading) * 4;

	if (k->IsDown(0x57)) { m_x += xdisp; m_z += zdisp;}		// W
	if (k->IsDown(0x53)) { m_x -= xdisp; m_z -= zdisp;}		// S
	if (k->IsDown(0x44)) { m_x += zdisp; m_z -= xdisp;}		// D
	if (k->IsDown(0x41)) { m_x -= zdisp; m_z += xdisp;}		// A
	if (k->IsDown(0x26)) m_y += 4;							// Up arrow
	if (k->IsDown(0x28)) m_y -= 4;							// Down arrow

	m_matrix.InitWithExtrinsicYXZ(m_heading, m_pitch, m_roll);
	m_matrix.TranslateX(m_x);
	m_matrix.TranslateY(m_y);
	m_matrix.TranslateZ(m_z);
	m_viewport->SetViewPlacement(m_matrix);
}

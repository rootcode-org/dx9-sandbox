/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include "core/mouse.h"

// Singleton
static Mouse instance;
Mouse* Mouse::GetInstance() { return &instance; }

Mouse::Mouse(void) {
	m_delta_x = m_delta_y = m_delta_wheel = 0;
	m_button_state = 0;
}


void Mouse::RawInput(int x, int y, int wheel, unsigned long buttons)
{
	m_delta_x += x;
	m_delta_y += y;
	m_delta_wheel += wheel;
	m_button_state = buttons;
}


int Mouse::GetXDelta(void)
{
	int value = m_delta_x;
	m_delta_x = 0;
	return value;
}


int Mouse::GetYDelta(void)
{
	int value = m_delta_y;
	m_delta_y = 0;
	return value;
}


int Mouse::GetWheelDelta(void)
{
	int value = m_delta_wheel;
	m_delta_wheel = 0;
	return value;
}


bool Mouse::IsDown(int button)
{
	return m_button_state & button;
}

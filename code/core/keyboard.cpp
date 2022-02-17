/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#include "precompiled.h"
#include <string.h>
#include "core/keyboard.h"

// Singleton
static Keyboard instance;
Keyboard* Keyboard::GetInstance() { return &instance; }

Keyboard::Keyboard(void) {
	memset(m_vkey_state, 0, sizeof(m_vkey_state));
}


void Keyboard::RawInput(unsigned short vkey, unsigned short flags)
{
	m_vkey_state[vkey & 0xff] = flags & 0xff;
}


bool Keyboard::IsDown(unsigned short vkey)
{
	return m_vkey_state[vkey] & 0x01;
}

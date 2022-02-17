/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

class Keyboard {
public:
	Keyboard(void);
	void RawInput(unsigned short vkey, unsigned short state);
	bool IsDown(unsigned short vkey);

	static Keyboard* GetInstance(void);

private:
	unsigned char m_vkey_state[256];
};

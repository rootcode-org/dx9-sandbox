/* Copyright is waived. No warranty is provided. Unrestricted use and modification is permitted. */

#pragma once

class Mouse {
public:
	Mouse(void);
	void  RawInput(int x, int y, int wheel, unsigned long buttons);
	int   GetXDelta(void);
	int   GetYDelta(void);
	int   GetWheelDelta(void);
	bool  IsDown(int button);

	static Mouse* GetInstance(void);

private:
	int m_delta_x;
	int m_delta_y;
	int m_delta_wheel;
	unsigned long m_button_state;
};

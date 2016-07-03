#pragma once
#include <stdint.h>

class JoyPad
{
public:
	enum Buttons
	{
		BTN_A = 0,
		BTN_B,
		BTN_SEL,
		BTN_START,
		BTN_UP,
		BTN_DOWN,
		BTN_LEFT,
		BTN_RIGHT,
		BTN_MAX
	};

public:
	JoyPad();

	void setButton(Buttons button, bool value);

	void write(uint8_t value);
	uint8_t read();

private:
	bool mButton[BTN_MAX];
	uint8_t mIndex;
	uint8_t mStrobe;

private:
	void updateStrobeIndex();
};


#include "JoyPad.h"

JoyPad::JoyPad()
	: mIndex(0)
	, mStrobe(0)
{
	for (uint32_t i = 0; i < BTN_MAX; ++i)
	{
		mButton[i] = false;
	}
}

void JoyPad::write(uint8_t value)
{
	mStrobe = value;
	updateStrobeIndex();
}

uint8_t JoyPad::read()
{
	uint8_t value = 0x0;
	if (mIndex < BTN_MAX && mButton[mIndex])
	{
		value = 0x1;
	}
	mIndex++;
	updateStrobeIndex();
	return value;
}

void JoyPad::updateStrobeIndex()
{
	if (mStrobe & 0x1)
	{
		mIndex = 0;
	}
}

void JoyPad::setButton(Buttons button, bool value)
{
	mButton[button] = value;
}
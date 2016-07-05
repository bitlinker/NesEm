#pragma once
#include <stdint.h>
#include "IMemory.h"

class JoyPad : public IMemory
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
    virtual ~JoyPad();

	void setButton(Buttons button, bool value);

    virtual void write(uint16_t address, uint8_t value);
    virtual uint8_t read(uint16_t address);

private:
	bool mButton[BTN_MAX];
	uint8_t mIndex;
	uint8_t mStrobe;

private:
	void updateStrobeIndex();
};


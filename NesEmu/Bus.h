#pragma once
#include <stdint.h>
#include "JoyPad.h"

class Bus
{
public:
	Bus();
	~Bus();

	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t value);

private:
	JoyPad* mJoyPad[2];
};


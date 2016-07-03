#include "Bus.h"

static const uint16_t CONTROLLER_1_REGISTER_ADDR = 0x4016;
static const uint16_t CONTROLLER_2_REGISTER_ADDR = 0x4017;

Bus::Bus()
{
}

Bus::~Bus()
{
}

uint8_t Bus::read(uint16_t address)
{
	// TODO: decode address
	if (address == CONTROLLER_1_REGISTER_ADDR || address == CONTROLLER_2_REGISTER_ADDR)
	{
		// TODO: assert joypads
		return mJoyPad[address - CONTROLLER_1_REGISTER_ADDR]->read();
	}
}

void Bus::write(uint16_t address, uint8_t value)
{
	if (address == CONTROLLER_1_REGISTER_ADDR || address == CONTROLLER_2_REGISTER_ADDR)
	{
		// TODO: assert joypads
		mJoyPad[address - CONTROLLER_1_REGISTER_ADDR]->write(value);
	}
}
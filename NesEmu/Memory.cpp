#include "Memory.h"

static const uint16_t RAM_SIZE = 0x2000;

Memory::Memory()
	: mRAM(RAM_SIZE) // TODO: check, const
{
}

Memory::~Memory()
{
}

void Memory::write(uint16_t address, uint8_t value)
{
	// TODO: memory here
	mRAM[address] = value;
}

uint8_t Memory::read(uint16_t address)
{
	// TODO: memory here
	return mRAM[address];
}
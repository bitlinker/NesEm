#include "Memory.h"

Memory::Memory()
	: mRAM(0xFFFF) // TODO: check, const
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
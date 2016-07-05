#include "RAM.h"
#include <cassert>

static const uint16_t RAM_SIZE = 0x2000;

RAM::RAM(uint16_t capacity)
	: mRAM(capacity)
{
}

RAM::~RAM()
{
}

void RAM::write(uint16_t address, uint8_t value)
{
    assert(address < mRAM.size());
    mRAM[address] = value;    
}

uint8_t RAM::read(uint16_t address)
{
    assert(address < mRAM.size());
	return mRAM[address];
}
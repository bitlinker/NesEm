#include "VRAM.h"
#include <cassert>

VRAM::VRAM(uint16_t capacity)
    : mMemory(capacity)
{
}

VRAM::~VRAM()
{
}

void VRAM::writePpu(uint16_t address, uint8_t value)
{
    assert(address < mMemory.size());
    mMemory[address] = value;
}

uint8_t VRAM::readPpu(uint16_t address)
{
    assert(address < mMemory.size());
    return mMemory[address];
}
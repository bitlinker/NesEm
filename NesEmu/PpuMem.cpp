#include "PpuMem.h"
#include <cassert>

PpuMem::PpuMem(uint16_t capacity, bool isWritable)
    : mMemory(capacity)
	, mIsWritable(isWritable)
{
}

PpuMem::~PpuMem()
{
}

void PpuMem::writePpu(uint16_t address, uint8_t value)
{
	assert(mIsWritable);
	/*if (!mIsWritable)
	{
		// TODO: DBG; do nothing
		return;
	}*/
	
    assert(address < mMemory.size());	
    mMemory[address] = value;
}

uint8_t PpuMem::readPpu(uint16_t address)
{
    assert(address < mMemory.size());
    return mMemory[address];
}
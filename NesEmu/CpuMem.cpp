#include "CpuMem.h"
#include <cassert>

CpuMem::CpuMem(uint16_t capacity, bool isWritable)
	: mRAM(capacity)
	, mIsWritable(isWritable)
{
}

CpuMem::~CpuMem()
{
}

void CpuMem::writeCpu(uint16_t address, uint8_t value)
{
	assert(mIsWritable);
    assert(address < mRAM.size());	
    mRAM[address] = value;    
}

uint8_t CpuMem::readCpu(uint16_t address)
{
    assert(address < mRAM.size());
	return mRAM[address];
}
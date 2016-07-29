#pragma once
#include "IMemory.h"
#include <vector>

class RAM : public ICpuMemory
{
public:
	RAM(uint16_t capacity);
	virtual ~RAM();

	uint8_t* getRawPtr() { return &mRAM[0]; }

	virtual void writeCpu(uint16_t address, uint8_t value);
	virtual uint8_t readCpu(uint16_t address);

private:
    std::vector<uint8_t> mRAM;
};


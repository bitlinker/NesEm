#pragma once
#include "IMemory.h"
#include <vector>

class CpuMem : public ICpuMemory
{
public:
	CpuMem(uint16_t capacity, bool isWritable);
	virtual ~CpuMem();

	uint8_t* getRawPtr() { return &mRAM[0]; }
	uint16_t getCapacity() const { return static_cast<uint16_t>(mRAM.size()); }

	virtual void writeCpu(uint16_t address, uint8_t value);
	virtual uint8_t readCpu(uint16_t address);

private:
    std::vector<uint8_t> mRAM;
	bool mIsWritable;
};


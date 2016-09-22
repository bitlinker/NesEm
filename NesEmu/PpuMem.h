#pragma once
#include "IMemory.h"
#include <vector>

// TODO: ugly; RAM clone with different interface
class PpuMem : public IPpuMemory
{
public:
    PpuMem(uint16_t capacity, bool isWritable);
    virtual ~PpuMem();

    uint8_t* getRawPtr() { return &mMemory[0]; }

    virtual void writePpu(uint16_t address, uint8_t value);
    virtual uint8_t readPpu(uint16_t address);

private:
    std::vector<uint8_t> mMemory;
	bool mIsWritable;
};


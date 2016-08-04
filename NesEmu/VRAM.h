#pragma once
#include "IMemory.h"
#include <vector>


// TODO: ugly; RAM clone with different interface

class VRAM : public IPpuMemory
{
public:
    VRAM(uint16_t capacity);
    virtual ~VRAM();

    uint8_t* getRawPtr() { return &mMemory[0]; }

    virtual void writePpu(uint16_t address, uint8_t value);
    virtual uint8_t readPpu(uint16_t address);

private:
    std::vector<uint8_t> mMemory;
};


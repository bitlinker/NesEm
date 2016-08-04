#pragma once
#include "IMemory.h"
#include "IMapper.h"
#include <stdint.h>

class PPU;

class PPUBus : public IPpuMemory
{
public:
    PPUBus();
    virtual ~PPUBus() {}

    void PPUBus::init(IMapper* mapper, IPpuMemory* vram, PPU* ppu)
    {
        mMapper = mapper;
        mPPU = ppu;
        mVRam = vram;
    }

    virtual void writePpu(uint16_t address, uint8_t value);
    virtual uint8_t readPpu(uint16_t address);

private:
    IPpuMemory* mapPpuAddress(uint16_t address, uint16_t& mirroredAddress);   

private:
    IMapper* mMapper;
    PPU* mPPU;
    IPpuMemory* mVRam;
};

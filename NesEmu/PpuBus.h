#pragma once
#include "IMemory.h"
#include "IMapper.h"
#include <stdint.h>

class PPU;

// TODO: ppu reads from mapper only!
// no need for bus
// TODO: cpu also?

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
    IPpuMemory* mapPpuAddress(uint16_t address, uint16_t& mappedAddress);

private:
    IMapper* mMapper;
    PPU* mPPU;
    IPpuMemory* mVRam;
};

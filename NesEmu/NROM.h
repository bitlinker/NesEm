#pragma once
#include "Cartridge.h"
#include "PpuMem.h"
#include "IMapper.h"
#include "PPU.h"

class NROM : public IMapper
{
public:
    NROM(Cartridge* mCartridge, PpuMem* vram, PPU* ppu);
    virtual ~NROM();

public:
    // Mapper implementation:
    virtual void writeCpu(uint16_t address, uint8_t value);
    virtual uint8_t readCpu(uint16_t address);

    virtual Mirroring getMirroring();

    virtual void writePpu(uint16_t address, uint8_t value);
    virtual uint8_t readPpu(uint16_t address);

private:
	// TODO: virtual
    ICpuMemory* mapCpuAddress(uint16_t address, uint16_t& mappedAddress);
	IPpuMemory* mapPpuAddress(uint16_t address, uint16_t& mappedAddress);

private:
    Cartridge* mCartridge;
	PpuMem* mVram;
	PPU* mPPU;
};


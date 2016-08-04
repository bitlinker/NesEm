#pragma once
#include "Cartridge.h"
#include "IMapper.h"

class NROM : public IMapper
{
public:
    NROM(Cartridge* mCartridge);
    virtual ~NROM();

public:
    // Mapper implementation:
    virtual void writeCpu(uint16_t address, uint8_t value);
    virtual uint8_t readCpu(uint16_t address);

    virtual Mirroring getMirroring();

    virtual void writePpu(uint16_t address, uint8_t value);
    virtual uint8_t readPpu(uint16_t address);

private:
    bool mapCpuAddress(uint16_t address, uint8_t** ptr);
    bool mapPpuAddress(uint16_t address, uint8_t** ptr);

private:
    Cartridge* mCartridge;
};


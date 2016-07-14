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
    virtual void write(uint16_t address, uint8_t value);
    virtual uint8_t read(uint16_t address);

    virtual Mirroring getMirroring();

private:
    bool mapAddress(uint16_t address, uint8_t** ptr);

private:
    Cartridge* mCartridge;
};


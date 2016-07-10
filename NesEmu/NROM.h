#pragma once
#include "IMemory.h"
#include "Cartridge.h"

class NROM : public IMemory
{
public:
    NROM(Cartridge* mCartridge);
    ~NROM();

public:
    virtual void write(uint16_t address, uint8_t value);
    virtual uint8_t read(uint16_t address);

private:
    bool mapAddress(uint16_t address, uint8_t** ptr);

private:
    Cartridge* mCartridge;
};


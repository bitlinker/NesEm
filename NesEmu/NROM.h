#pragma once
#include "IMemory.h"

class NROM : public IMemory
{
public:
    NROM();
    ~NROM();

public:
    virtual void write(uint16_t address, uint8_t value);
    virtual uint8_t read(uint16_t address);
};


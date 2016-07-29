#pragma once
#include <stdint.h>

class ICpuMemory
{
public:
	virtual void writeCpu(uint16_t address, uint8_t value) = 0;
	virtual uint8_t readCpu(uint16_t address) = 0;

	virtual ~ICpuMemory() {};
};

class IPpuMemory
{
public:
    virtual void writePpu(uint16_t address, uint8_t value) = 0;
    virtual uint8_t readPpu(uint16_t address) = 0;

    virtual ~IPpuMemory() {};
};
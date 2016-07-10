#pragma once
#include <stdint.h>
#include "IMemory.h"

class Bus : public IMemory
{
public:
	Bus(IMemory* ram, IMemory* mapper, IMemory* joypad1, IMemory* joypad2);
	virtual ~Bus();

	virtual uint8_t read(uint16_t address);
    virtual void write(uint16_t address, uint8_t value);

private:
    IMemory* mapAddress(uint16_t address, bool bRead, uint16_t& mirroredAddress);

private:
    IMemory* mRam;
    IMemory* mPPU;
    IMemory* mAPU;
    IMemory* mMapper;
	IMemory* mJoyPad[2];    
};


#pragma once
#include <stdint.h>
#include "IMemory.h"

#include "CPU6502.h"
#include "RAM.h"
#include "JoyPad.h"

class Bus : public IMemory
{
public:
	Bus(RAM* ram, JoyPad* joypad1, JoyPad* joypad2, CPU6502* cpu);
	virtual ~Bus();

	virtual uint8_t read(uint16_t address);
    virtual void write(uint16_t address, uint8_t value);

private:
    IMemory* mapAddress(uint16_t address, bool bRead, uint16_t& mirroredAddress);

private:
    RAM* mRam;
    CPU6502* mCPU;
    IMemory* mPPU;
    IMemory* mAPU;
    IMemory* mCartridge;
	JoyPad* mJoyPad[2];
};


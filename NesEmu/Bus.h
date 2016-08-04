#pragma once
#include <stdint.h>
#include "IMapper.h"
#include "IMemory.h"

class Bus : public ICpuMemory
{
public:
	Bus(ICpuMemory* ram, IMapper* mapper, ICpuMemory* joypad1, ICpuMemory* joypad2, ICpuMemory* apu, ICpuMemory* ppu);
	virtual ~Bus();

	virtual uint8_t readCpu(uint16_t address);
    virtual void writeCpu(uint16_t address, uint8_t value);

private:
    ICpuMemory* mapCpuAddress(uint16_t address, bool bRead, uint16_t& mirroredAddress);

private:
    ICpuMemory* mRam;
    ICpuMemory* mPPU;
    ICpuMemory* mAPU;
    IMapper* mMapper;
	ICpuMemory* mJoyPad[2];    
};


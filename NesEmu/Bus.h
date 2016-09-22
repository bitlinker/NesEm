#pragma once
#include <stdint.h>
#include "IMapper.h"
#include "IMemory.h"
#include "PPU.h"

class Bus : public ICpuMemory
{
private:
	class OamDmaModule : public ICpuMemory
	{
	public:
		OamDmaModule(Bus* bus)
			: mBus(bus) {}
	
		virtual void writeCpu(uint16_t address, uint8_t value);
		virtual uint8_t readCpu(uint16_t address);

	private:
		Bus* mBus;
	};

public:
	Bus(ICpuMemory* ram, IMapper* mapper, ICpuMemory* joypad1, ICpuMemory* joypad2, ICpuMemory* apu, PPU* ppu);
	virtual ~Bus();

	virtual uint8_t readCpu(uint16_t address);
    virtual void writeCpu(uint16_t address, uint8_t value);

private:
    ICpuMemory* mapCpuAddress(uint16_t address, bool bRead, uint16_t& mirroredAddress);

private:
    ICpuMemory* mRam;
    PPU* mPPU;
    ICpuMemory* mAPU;
    IMapper* mMapper;
	ICpuMemory* mJoyPad[2];
	OamDmaModule mOamDma;
};


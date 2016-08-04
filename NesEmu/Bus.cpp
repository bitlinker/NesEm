#include <cassert>
#include "Bus.h"

// TODO
//static const uint16_t CONTROLLER_1_REGISTER_ADDR = 0x4016;
//static const uint16_t CONTROLLER_2_REGISTER_ADDR = 0x4017;

Bus::Bus(ICpuMemory* ram, IMapper* mapper, ICpuMemory* joypad1, ICpuMemory* joypad2, ICpuMemory* apu, ICpuMemory* ppu)
    : mRam(ram)
    , mMapper(mapper)
    , mAPU(apu)
    , mPPU(ppu)
{
    mJoyPad[0] = joypad1;
    mJoyPad[1] = joypad2;
    // TODO: init all
}

Bus::~Bus()
{
}

ICpuMemory* Bus::mapCpuAddress(uint16_t address, bool bRead, uint16_t& mirroredAddress)
{
    ICpuMemory* pModule = nullptr; // TODO: make devices itself decide memory operations?
    mirroredAddress = address;

    if (address < 0x2000)
    {
        mirroredAddress = address % 0x800;
        pModule = mRam;
    }
    else if (address < 0x4000)
    {
        mirroredAddress = 0x2000 + address % 0x8;
        pModule = mPPU;
    }
    else if (address <= 0x4015 && address != 0x4014)
    {
        pModule = mAPU;
    }
    else if (address == 0x4014)
    {
        // TODO: OAMDMA
        assert(false || "OAMDMA not implemented");
    }
    else if (address < 0x4018)
    {
        if (address == 0x4016)
            pModule = mJoyPad[0];
        else if ((address == 0x4017) && bRead)
            pModule = mJoyPad[1];
        else
            pModule = mAPU;
    }
    else
    {
        pModule = mMapper;
    }
    return pModule;
}

uint8_t Bus::readCpu(uint16_t address)
{
    uint16_t mirroredAddress;
    ICpuMemory* pModule = mapCpuAddress(address, true, mirroredAddress);
    assert(pModule);    
    return pModule->readCpu(mirroredAddress);
}

void Bus::writeCpu(uint16_t address, uint8_t value)
{
    uint16_t mirroredAddress;
    ICpuMemory* pModule = mapCpuAddress(address, false, mirroredAddress);
    assert(pModule);
    pModule->writeCpu(mirroredAddress, value);
}
#include <cassert>
#include "Bus.h"

// TODO
//static const uint16_t CONTROLLER_1_REGISTER_ADDR = 0x4016;
//static const uint16_t CONTROLLER_2_REGISTER_ADDR = 0x4017;

Bus::Bus(IMemory* ram, IMemory* mapper, IMemory* joypad1, IMemory* joypad2)
    : mRam(ram)
    , mMapper(mapper)
{
    mJoyPad[0] = joypad1;
    mJoyPad[1] = joypad2;
    // TODO: init all
}

Bus::~Bus()
{
}

IMemory* Bus::mapAddress(uint16_t address, bool bRead, uint16_t& mirroredAddress)
{
    IMemory* pModule = nullptr; // TODO: make devices itself decide memory operations?
    mirroredAddress = address;

    if (address < 0x2000)
    {
        mirroredAddress = address % 0x800;
        pModule = mRam;
    }
    else if (address < 0x4000)
    {
        mirroredAddress = address % 0x8;
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

uint8_t Bus::read(uint16_t address)
{
    uint16_t mirroredAddress;
    IMemory* pModule = mapAddress(address, true, mirroredAddress);
    assert(pModule);    
    return pModule->read(mirroredAddress);
}

void Bus::write(uint16_t address, uint8_t value)
{
    uint16_t mirroredAddress;
    IMemory* pModule = mapAddress(address, false, mirroredAddress);
    assert(pModule);
    pModule->write(mirroredAddress, value);
}
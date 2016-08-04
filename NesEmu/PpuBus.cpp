#include "PpuBus.h"
#include "Ppu.h"

PPUBus::PPUBus()
    : mMapper(nullptr)
    , mPPU(nullptr)
    , mVRam(nullptr)
{
}

IPpuMemory* PPUBus::mapPpuAddress(uint16_t address, uint16_t& mirroredAddress)
{
    // TODO: implement it so:
    if (mMapper->mapVRam(address, mVRam))
    {
        // TODO: return pointer from mapper here...
    }
    else if (address < 0x4000) // Palettes RAM
    {
        mirroredAddress = 0x3F00 + address % 0x20;
        // TODO: ppu ram
        mPPU->getPaletteRam();
    }
    else
    {
        return nullptr;
    }
}

void PPUBus::writePpu(uint16_t address, uint8_t value)
{
    mapPpuAddress(address);
    // TODO
}

uint8_t PPUBus::readPpu(uint16_t address)
{
    mapPpuAddress(address);
    // TODO
    return 0;
}
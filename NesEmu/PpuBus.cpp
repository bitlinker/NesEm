#include "PpuBus.h"
#include "Ppu.h"

PPUBus::PPUBus()
    : mMapper(nullptr)
    , mPPU(nullptr)
    , mVRam(nullptr)
{
}

IPpuMemory* PPUBus::mapPpuAddress(uint16_t address, uint16_t& mappedAddress)
{
	IPpuMemory* pModule = nullptr;
	mappedAddress = address;
	if (address < 0x3F00)
	{
		pModule = mMapper;
	}
    else if (address < 0x4000) // Palettes RAM // TODO: custom class
    {
		pModule = &mPPU->getPaletteRam();
    }
    else
    {
        return nullptr;
    }
	return pModule;
}

void PPUBus::writePpu(uint16_t address, uint8_t value)
{
	uint16_t mappedAddress;
	IPpuMemory* device = mapPpuAddress(address, mappedAddress);
	if (device)
	{
		device->writePpu(mappedAddress, value);
	}
	else
	{
		assert(false && "writing to unknown address"); // TODO: log here!
	}
}

uint8_t PPUBus::readPpu(uint16_t address)
{
	uint16_t mappedAddress;
	IPpuMemory* device = mapPpuAddress(address, mappedAddress);
	if (device)
	{
		return device->readPpu(mappedAddress);
	}
	else
	{
		assert(false && "writing to unknown address"); // TODO: log here!
		return 0;
	}
}
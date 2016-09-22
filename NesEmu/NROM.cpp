#include "NROM.h"
#include <cassert>
#include "PPU.h"

NROM::NROM(Cartridge* cartridge, PpuMem* vram, PPU* ppu)
    : mCartridge(cartridge)
	, mVram(vram)
	, mPPU(ppu)
{
}

NROM::~NROM()
{
}

ICpuMemory* NROM::mapCpuAddress(uint16_t address, uint16_t& mappedAddress)
{
	mappedAddress = address;
    if (address < 0x6000)
    {
        // TODO: log this!
		assert(false);
        return nullptr;
    }

    // TODO: trainer?
    if (address < 0x8000) // 0x6000 - 0x8000 PRG RAM
    {		
		CpuMem& ram = mCartridge->getPrgRAM();
		mappedAddress = (address - 0x6000) % ram.getCapacity();
		return &ram;
    }
    else // 0x8000 - 0xFFFF PRG ROM
    {
		CpuMem& rom = mCartridge->getPrgROM();        
		mappedAddress = (address - 0x8000) % rom.getCapacity();
        return &rom;
    }
    return nullptr;
}

void NROM::writeCpu(uint16_t address, uint8_t value)
{
	uint16_t mappedAddress;
	ICpuMemory* module = mapCpuAddress(address, mappedAddress);
	if (module)
	{
		module->writeCpu(mappedAddress, value);
	}
	else
	{
		assert(false);
	}
}

uint8_t NROM::readCpu(uint16_t address)
{
	uint16_t mappedAddress;
	ICpuMemory* module = mapCpuAddress(address, mappedAddress);
	if (module)
	{
		return module->readCpu(mappedAddress);
	}
	else
	{
		assert(false);
		return 0;
	}
}

// Pages:
// 0 1
// 2 3
// This should be in sync with IMapper::Mirroring
static uint16_t ADDRESS_MAPS[4][4] = {
	{ 0, 0, 1, 1 },
	{ 0, 1, 0, 1 },
	{ 0, 0, 0, 0 },
	{ 0, 1, 2, 3 },
};

// Address should be relative (starting from 0) here
uint16_t mapMirroring(uint16_t address, IMapper::Mirroring mirroring)
{
    uint16_t* addressMap = ADDRESS_MAPS[mirroring];
	const uint16_t MIRROR_PAGE_SIZE = 0x400; // 1024 bytes
    uint16_t page = address / MIRROR_PAGE_SIZE;
    uint16_t offset = address % MIRROR_PAGE_SIZE;
    uint16_t finalAddress = addressMap[page] * MIRROR_PAGE_SIZE + offset;
    return finalAddress;
}

IPpuMemory* NROM::mapPpuAddress(uint16_t address, uint16_t& mappedAddress)
{	
    if (address < 0x2000) // Pattern table 1, 2
    {
		mappedAddress = address;
		return &mCartridge->getChrROM();
    }
    else if (address < 0x3F00) // Name tables
    {
        mappedAddress = mapMirroring(address % 0x1000, getMirroring());
		return mVram;
    }
	else if (address < 0x4000) // Palettes
	{
		mappedAddress = (address - 0x3F00) % 0x20;
		return &mPPU->getPaletteRam();
	}

	return nullptr;
}

IMapper::Mirroring NROM::getMirroring()
{
    return mCartridge->isHorizontalMirroring() ? MIRROR_HORIZONTAL : MIRROR_VERTICAL;
}

void NROM::writePpu(uint16_t address, uint8_t value)
{
	uint16_t mappedAddress;
	IPpuMemory* module = mapPpuAddress(address, mappedAddress);
	if (module)
	{
		module->writePpu(mappedAddress, value);
	}
	else
	{
		assert(false || "Writing incorrect VRAM address"); // TODO: log error?
	}
}

uint8_t NROM::readPpu(uint16_t address)
{
	uint16_t mappedAddress;
	IPpuMemory* module = mapPpuAddress(address, mappedAddress);
	if (module)
	{
		return module->readPpu(mappedAddress);
	}
	else
	{
		assert(false || "Writing incorrect VRAM address"); // TODO: log error?
		return 0;
	}	
}

// TODO: saving
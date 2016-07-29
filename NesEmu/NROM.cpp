#include "NROM.h"
#include <cassert>

NROM::NROM(Cartridge* cartridge)
    : mCartridge(cartridge)
{
}

NROM::~NROM()
{
}

bool NROM::mapAddress(uint16_t address, uint8_t** ptr)
{
    if (address < 0x6000)
    {
        // TODO: log
        return false;
    }

    // TODO: trainer?
    if (address < 0x8000) // 0x6000 - 0x8000 PRG RAM
    {
        std::vector<uint8_t>& ram = mCartridge->getPrgRAM();
        if (ram.empty())
        {
            assert(false || "PRG RAM is empty");
        }
        uint16_t realAddress = (address - 0x6000) % ram.size();
        *ptr = &ram[realAddress];
    }
    else // 0x8000 - 0xFFFF PRG ROM
    {
        std::vector<uint8_t>& rom = mCartridge->getPrgROM();
        if (rom.empty())
        {
            assert(false || "PRG ROM is empty");
        }
        uint16_t realAddress = (address - 0x8000) % rom.size();
        *ptr = &rom[realAddress];
    }
    return true;
}

void NROM::writeCpu(uint16_t address, uint8_t value)
{
    uint8_t* ptr = nullptr;
    if (mapAddress(address, &ptr))
    {
        *ptr = value;
    }

    // TODO: write-protected areas?
}

uint8_t NROM::readCpu(uint16_t address)
{
    uint8_t* ptr = nullptr;
    if (mapAddress(address, &ptr))
    {
        return *ptr;
    }

    return 0;
}

IMapper::Mirroring NROM::getMirroring()
{
    return mCartridge->isHorizontalMirroring() ? MIRROR_HORIZONTAL : MIRROR_VERTICAL;
}

void NROM::writePpu(uint16_t address, uint8_t value)
{

}

uint8_t NROM::readPpu(uint16_t address)
{
    return 0;
}

// TODO: saving
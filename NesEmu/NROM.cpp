#include "NROM.h"
#include <cassert>
#include "PPU.h"

NROM::NROM(Cartridge* cartridge)
    : mCartridge(cartridge)
{
}

NROM::~NROM()
{
}

bool NROM::mapCpuAddress(uint16_t address, uint8_t** ptr)
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
    if (mapCpuAddress(address, &ptr))
    {
        *ptr = value;
    }

    // TODO: write-protected areas?
}

uint8_t NROM::readCpu(uint16_t address)
{
    uint8_t* ptr = nullptr;
    if (mapCpuAddress(address, &ptr))
    {
        return *ptr;
    }

    return 0;
}

uint16_t mapMirroring(uint16_t address, IMapper::Mirroring mirroring)
{
    address = address % 0x4000;

    // Address should be relative (starting from 0)

    // 0 1
    // 2 3
    uint16_t addressMap[4] = { 0 };
    switch (mirroring) // TODO: get this map from mirroring class?
    {
    case IMapper::MIRROR_HORIZONTAL:
        //addressMap = {0, 0, 0, 0};
        break;
    default:
        break;
    }

    // TODO: 2k ram. Nametables normally

    const uint16_t MIRROR_PAGE_SIZE = 0x400; // 1024 bytes
    uint16_t page = address / MIRROR_PAGE_SIZE;
    uint16_t offset = address % MIRROR_PAGE_SIZE;
    uint16_t finalAddress = addressMap[page] * MIRROR_PAGE_SIZE + offset;
    return finalAddress;
}

bool NROM::mapPpuAddress(uint16_t address, uint8_t** ptr)
{
    uint16_t mirroredAddress = address;
    if (address < 0x2000) // Pattern table 1, 2
    {
        *ptr = &mCartridge->getChrROM()[address];
    }
    else if (address < 0x3F00) // Name tables
    {
        mirroredAddress = address % 0x1000;
        mirroredAddress = 0x2000 + mapMirroring(mirroredAddress, getMirroring());
    }
    else
    {
        return false;
    }
    // TODO: all above can be configured by mapper!

   

    // TODO: io devices
    return true;
}

IMapper::Mirroring NROM::getMirroring()
{
    return mCartridge->isHorizontalMirroring() ? MIRROR_HORIZONTAL : MIRROR_VERTICAL;
}

void NROM::writePpu(uint16_t address, uint8_t value)
{
    uint8_t* ptr = nullptr;
    assert(mapPpuAddress(address, &ptr)); // TODO: log error?
    *ptr = value;
}

uint8_t NROM::readPpu(uint16_t address)
{
    uint8_t* ptr = nullptr;
    assert(mapPpuAddress(address, &ptr)); // TODO: log error?
    return *ptr;
}

// TODO: saving
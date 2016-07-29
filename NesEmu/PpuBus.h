#pragma once
#include "IMemory.h"
#include "IMapper.h"
#include <stdint.h>

class PPUBus : public ICpuMemory
{
public:
    PPUBus(IMapper* mapper)
        : mMapper(mapper)
    {
    }

    // TODO: and mirroring
    virtual void writeCpu(uint16_t address, uint8_t value)
    {
    }
    virtual uint8_t readCpu(uint16_t address)
    {
        return 0;
    }

private:
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

    uint16_t mapAddress(uint16_t address)
    {
        uint16_t mirroredAddress = address;
        if (address < 0x2000) // Pattern table 1, 2
        {
            // TODO: from mapper
        }
        else if (address < 0x3F00) // Name tables
        {
            mirroredAddress = address % 0x1000;
            mirroredAddress = 0x2000 + mapMirroring(mirroredAddress, mMapper->getMirroring());
        }
        // TODO: all above can be configured by mapper!
        else // Palettes RAM
        {
            mirroredAddress = 0x3F00 + address % 0x20;
            // TODO: ppu ram
        }

        // TODO: io devices
        return mirroredAddress;
    }

private:
    IMapper* mMapper;
    //PaletteRam mPaletteRam;
};

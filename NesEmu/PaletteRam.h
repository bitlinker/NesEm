#pragma once
#include "IMemory.h"
#include <stdint.h>
#include <vector>
#include <cassert>

// TODO: inherit from vram?
class PaletteRam : public IPpuMemory
{
public:
    static const uint16_t PALETTE_RAM_SIZE = 0x20;
public:
    PaletteRam()
        : mValues(PALETTE_RAM_SIZE)
    {
    }

	virtual ~PaletteRam() {}

    uint8_t getColorIndex(uint8_t indexRaw)
    {
        assert(indexRaw < PALETTE_RAM_SIZE);
        return mValues[indexRaw];
    }

    // pixelClr : 2, palIndex : 2, isSprite : 1
    uint8_t getColorIndex(uint8_t pixelClr, uint8_t palIndex, uint8_t isSprite)
    {
        uint8_t index = (pixelClr & 0x3) | ((palIndex & 0x3) << 2) | (isSprite << 4);
        return getColorIndex(index);
    }

    uint8_t* getRawPtr()
    {
        return &mValues[0];
    }

	virtual void writePpu(uint16_t address, uint8_t value)
	{
		mValues[address] = value;
	}

	virtual uint8_t readPpu(uint16_t address)
	{
		return mValues[address];
	}

public:
    std::vector<uint8_t> mValues;
};
#pragma once
#include <stdint.h>
#include <vector>
#include "IMemory.h"
#include "IMapper.h"

class CPU6502;

class PaletteRam
{
public:
    static const uint16_t PALETTE_RAM_SIZE = 0x20;
public:
    PaletteRam()
        : mValues(PALETTE_RAM_SIZE)
    {
    }

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

public:
    std::vector<uint8_t> mValues;
};


class PPUBus : public IMemory
{
public:
    PPUBus(IMapper* mapper)
        : mMapper(mapper)
    {
    }

    // TODO: and mirroring
    virtual void write(uint16_t address, uint8_t value)
    {
    }
    virtual uint8_t read(uint16_t address)
    {
        return 0;
    }

private:
    uint16_t mapAddress(uint16_t address)
    {
        uint16_t mirroredAddress = address;
        if (address < 0x1000) // Pattern table 1
        {
            
        }
        else if (address < 0x2000) // Pattern table 2
        {
            
        }
        else if (address < 0x3F00) // Name tables
        {
            mirroredAddress = 0x2000 + address % 0x1000;

            // TODO: nametable mirroring
            // TODO: 2k ram. Nametables normally
            mMapper->getMirroring();
        }
        // TODO: all above can be configured by mapper!
        else // Palettes RAM
        {
            mirroredAddress = 0x3F00 + address % 0x20;
        }

        // TODO: io devices
        return mirroredAddress;
    }

private:
    IMapper* mMapper;
    PaletteRam mPaletteRam;
};

class PPU : public IMemory
{
public:
	union Control
	{
		struct
		{
            uint8_t nameTableAddress : 2;   // Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
			uint8_t addressIncrement : 1;   // VRAM address increment per CPU read/write of PPUDATA (0: add 1, going across; 1: add 32, going down)
			uint8_t spriteTable : 1;        // Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
			uint8_t bgTable : 1;            // Background pattern table address(0: $0000; 1: $1000)
			uint8_t spriteSize : 1;         // Sprite size (0: 8x8; 1: 8x16)
			uint8_t masterSlaveMode : 1;    // PPU master/slave select (0: read backdrop from EXT pins; 1: output color on EXT pins)
            uint8_t nmiEnable : 1;          // Generate an NMI at the start of the vertical blanking interval(0: off; 1: on)
		};
		uint8_t value;
	};

    union Mask
    {
        struct
        {
            uint8_t grayscale : 1;          // Greyscale (0: normal color, 1: produce a greyscale display)
            uint8_t showLeftmostBg : 1;     // 1: Show background in leftmost 8 pixels of screen, 0 : Hide
            uint8_t showLeftmostSpr : 1;    // 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
            uint8_t showBg : 1;             // 1: Show background 
            uint8_t showSprites : 1;        // 1: Show sprites
            uint8_t tint : 3;               // tint; r, g, b*
        };
        uint8_t value;
    };

	union Status
	{
		struct
		{
			uint8_t unused : 5;             // Least significant bits previously written into a PPU register
            uint8_t spritesOverflow : 1;    // Sprite overflow.
			uint8_t sprite0hit : 1;         // Sprite 0 Hit.
			uint8_t isVBlank : 1;           // Vertical blank has started (0: not in vblank; 1: in vblank)
		};
		uint8_t value;
	};
	
public:
	PPU();
	virtual ~PPU();

    virtual void write(uint16_t address, uint8_t value);
    virtual uint8_t read(uint16_t address);

    void setCPU(CPU6502* cpu) { mCPU = cpu; }
    void exec();

private:
    void incAddress();

private:
    Control mControl;
    Mask mMask;
    Status mStatus;
    uint8_t mOamAddress;
    std::vector<uint8_t> mOamData;

    uint8_t mXScroll;
    uint8_t mYScroll;

    uint8_t mAddressLatch;
    uint16_t mAddress;

    PPUBus mVRAM;

    uint8_t mScanline;
    uint16_t mRowCycle;

    CPU6502* mCPU;
};


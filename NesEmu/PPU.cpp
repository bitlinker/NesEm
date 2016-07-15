#include <cassert>
#include "PPU.h"
#include "CPU6502.h"
#include <windows.h>





// Pattern table pixel fetch
uint8_t fetchPointNT(uint8_t x, uint8_t y)
{
    uint8_t* pAddress = 0;
    return *(pAddress + y) << (1 << x) | *(pAddress + y + 8) << (1 << x); // TODO: check
}

// Nametable fetch:
// 1024 bytes
// 30x32 tiles, 8x8 tile (960 bytes)
// Attribute table: 64 bytes: 2 bit pal index, byte controls 2x2 tiles block
// value = (topleft << 0) | (topright << 2) | (bottomleft << 4) | (bottomright << 6)





static uint16_t PPUCTRL_ADDRESS = 0x2000;
static uint16_t PPUMASK_ADDRESS = 0x2001;
static uint16_t PPUSTATUS_ADDRESS = 0x2002; // resets write pair for $2005 / 2006
static uint16_t OAMADDR_ADDRESS = 0x2003;
static uint16_t OAMDATA_ADDRESS = 0x2004;
static uint16_t PPUSCROLL_ADDRESS = 0x2005;
static uint16_t PPUADDR_ADDRESS = 0x2006;
static uint16_t PPUDATA_ADDRESS = 0x2007;
static uint16_t OAMDMA_ADDRESS = 0x4014;

PPU::PPU(const TvSystem& system)
{
    // TODO: init registers
    mOamData.resize(0xFF);
    mOamAddress = 0;

    mRowCycle = 0;
    mScanline = 0;

    /*ChipPalette pal(PALETTE_2C03);
    uint32_t c = pal.getColor(0);*/
}


PPU::~PPU()
{
}

uint8_t PPU::read(uint16_t address)
{
    if (address == PPUSTATUS_ADDRESS)
    {
        mAddressLatch = 0;
        return mStatus.value;
    }
    else if (address == OAMDATA_ADDRESS)
    {
        // TODO: check render / vblank behavior
        return mOamData[mOamAddress];
    }
    else if (address == PPUDATA_ADDRESS)
    {
        // TODO: check render / vblank behavior
        uint8_t value = mVRAM.read(mAddress);
        incAddress();
    }
    return 0;
}

void PPU::write(uint16_t address, uint8_t value)
{
    if (address == PPUCTRL_ADDRESS)
    {
        mControl.value = value;
    }
    else if (address == PPUMASK_ADDRESS)
    {
        mMask.value = value;
    }
    else if (address == OAMADDR_ADDRESS)
    {
        mOamAddress = value;
    }
    else if (address == OAMDATA_ADDRESS)
    {
        // TODO: check render / vblank behavior
        mOamData[mOamAddress++] = value;
    }
    else if (address == PPUSCROLL_ADDRESS)
    {
        switch (mAddressLatch++)
        {
        case 0:
            mXScroll = value; break;
        case 1:
            mYScroll = value; break;
        }
    }
    else if (address == PPUADDR_ADDRESS)
    {
        switch (mAddressLatch++)
        {
        case 0:
            mAddress = (mAddress & 0xFF) | (value << 8); break;
        case 1:
            mAddress = (mAddress & 0xFF00) | value; break;
        }
    }
    else if (address == PPUDATA_ADDRESS)
    {
        // TODO: check render / vblank behavior
        mVRAM.write(mAddress, value);
        incAddress();
    }
    else if (address == OAMDMA_ADDRESS) // TODO: this is on CPU...
    {
        for (uint8_t i = 0; i < 0xFF; ++i)
            mOamData[mOamAddress++] = 0; // TODO: cpu ram here...
    }
}


void PPU::incAddress()
{
    mAddress += mControl.addressIncrement ? 32 : 1;
}

void PPU::exec()
{
    mRowCycle++;
    if (mRowCycle == 341)
    {
        mRowCycle = 0;
        mScanline++;
        if (mScanline == 262) 
        {
            mScanline = 0;
        }
    }
    
    if (mScanline == 241 && mRowCycle == 1) {
        mStatus.isVBlank = 1;
        if (mControl.nmiEnable)
        {
            mCPU->nmi();
        }
    }

    if (mScanline > 260) {
        mStatus.isVBlank = 0;
    }
}
#include <cassert>
#include "PPU.h"
#include "CPU6502.h"


// TODO: rm?

static uint8_t PALETTE_2C02[Palette::MAX_COLORS * 3] = {
    84,  84,  84,     0,  30, 116,    8,  16, 144,   48,   0, 136,   68,   0, 100,   92,   0,  48,   84,   4,   0,   60,  24,   0,   32,  42,   0,    8,  58,   0,    0,  64,   0,    0,  60,   0,    0,  50,  60,    0,   0,   0,    0,   0,   0,    0,   0,   0,
    152, 150, 152,    8,  76, 196,   48,  50, 236,   92,  30, 228,  136,  20, 176,  160,  20, 100,  152,  34,  32,  120,  60,   0,   84,  90,   0,   40, 114,   0,    8, 124,   0,    0, 118,  40,    0, 102, 120,    0,   0,   0,    0,   0,   0,    0,   0,   0,
    236, 238, 236,   76, 154, 236,  120, 124, 236,  176,  98, 236,  228,  84, 236,  236,  88, 180,  236, 106, 100,  212, 136,  32,  160, 170,   0,  116, 196,   0,   76, 208,  32,   56, 204, 108,   56, 180, 204,   60,  60,  60,    0,   0,   0,    0,   0,   0,
    236, 238, 236,  168, 204, 236,  188, 188, 236,  212, 178, 236,  236, 174, 236,  236, 174, 212,  236, 180, 176,  228, 196, 144,  204, 210, 120,  180, 222, 120,  168, 226, 144,  152, 226, 180,  160, 214, 228,  160, 162, 160,    0,   0,   0,    0,   0,   0,
};

static uint16_t PALETTE_2C03[Palette::MAX_COLORS] = {
    333, 014, 006, 326, 403, 503, 510, 420, 320, 120, 031, 040, 022, 000, 000, 000,
    555, 036, 027, 407, 507, 704, 700, 630, 430, 140, 040, 053, 044, 000, 000, 000,
    777, 357, 447, 637, 707, 737, 740, 750, 660, 360, 070, 276, 077, 000, 000, 000,
    777, 567, 657, 757, 747, 755, 764, 772, 773, 572, 473, 276, 467, 000, 000, 000,
};



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
	//: //mVRAM(nullptr) // TODO
    : mTvSystem(system)
    , mNmiListener(nullptr)
    , mPalette(PALETTE_2C02)
{
    // TODO: init registers
    mOamData.resize(0xFF);
    mOamAddress = 0;

    mRowCycle = 0;
    mScanline = 0;

    // Back buffer
    mBackBuffer.resize(getWidth() * getHeight());
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
        //uint8_t value = mVRAM.read(mAddress);
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
        //mVRAM.write(mAddress, value);
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

    if (mScanline == 241 && mRowCycle == 1) { // Vblank start
        mStatus.isVBlank = 1; 
        drawStupid();
        if (mControl.nmiEnable)
        {
            if (mNmiListener)
            {
                mNmiListener->onNMI();
            }
            mCPU->nmi();
        }
    }

    if (mScanline > 260) { // Vblank end
        mStatus.isVBlank = 0;
    }
}

void PPU::drawStupid()
{
    for (uint32_t y = 0; y < getHeight(); ++y)
    {
        for (uint32_t x = 0; x < getWidth(); ++x)
        {
            // Background evaluation:
            if (mMask.showBg)
            {
                if (!mMask.showLeftmostBg && x < 8) 
                    continue;

                // Nametable fetch:
                uint16_t ntX = x / 8;
                uint16_t ntY = y / 8;

                uint16_t ntAddress = 0x2000 + mControl.nameTableAddress * 0x400;
                // TODO: scrolling?
                uint16_t ntCharAddress = ntAddress + ntY * 32 + ntX;
                uint8_t ntChar = mPpuBus->read8(ntCharAddress);
                uint16_t ntAttrAddress = ntAddress + (32 * 30) + (ntY * 32 + ntX) / 4; // TODO: check
                uint8_t ntAttr = mPpuBus->read8(ntAttrAddress);
                ntAttr &= 0x3; // TODO: offset for char

                // Pattern table fetch:
                uint16_t ptAddress = mControl.bgTable * 0x1000;
                uint16_t ptCharAddress = ptAddress + ntChar * 16;

                // TODO: y offset
                uint8_t cX = x % 8;
                uint8_t cY = y % 8;

                uint8_t charLow = mPpuBus->read8(ptCharAddress + cY);
                uint8_t charHigh = mPpuBus->read8(ptCharAddress + 8 + cY);
                
                uint8_t pixelClr = ((charLow << cX) & 0x1) | (((charHigh << cX) & 0x1) << 0x1);

                // Final color:
                uint8_t palIndex = pixelClr | (ntAttr << 2);
                //ntAttr 
                mBackBuffer[y * getWidth() + x] = mPalette.getColor(palIndex);
            }

            
        }
    }
}
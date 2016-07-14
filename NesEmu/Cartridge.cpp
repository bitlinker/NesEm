#include "Cartridge.h"
#include <stdint.h>
#include <cstdio>
#include <vector>

// TODO: iNES2 support
static const char HEADER_MAGIC[] = { 'N', 'E', 'S', 0x1A };

Cartridge::Cartridge(const std::string& filename)
{
    FILE *f = nullptr;
    fopen_s(&f, filename.c_str(), "rb");
	fread(&mHeader, sizeof(mHeader), 1, f);

    if (::memcmp(mHeader.mMagic, HEADER_MAGIC, 4) != 0)
    {
        throw 1; // TODO: throw here
    }

	uint32_t prgRomSize = mHeader.mPrgROMsize * PRG_BANK_SIZE;
	uint32_t chrRomSize = mHeader.mChrROMsize * CHR_BANK_SIZE;

    uint32_t prgRamSize = mHeader.mPrgRAMsize;
    if (prgRamSize == 0)
    {
        prgRamSize = 8 * 1024; // 8Kb for compatibility
    }

    mPrgRAM.resize(prgRamSize);

    mPrgROM.resize(prgRomSize);
    if (prgRomSize > 0)
    {
        fread(&mPrgROM[0], prgRomSize, 1, f);
    }

	mChrROM.resize(chrRomSize);
    if (chrRomSize > 0)
    {
        fread(&mChrROM[0], chrRomSize, 1, f);
    }

	fclose(f);
}

Cartridge::~Cartridge()
{
}

uint32_t Cartridge::getMapper()
{
    uint8_t mapper = (mHeader.mFlags7.mMapperHigh << 4) | mHeader.mFlags6.mMapperLow;
    return mapper;
}

bool Cartridge::isHorizontalMirroring()
{
    return mHeader.mFlags6.mHorizontalMirroring > 0;
}
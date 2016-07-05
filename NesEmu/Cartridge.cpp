#include "Cartridge.h"
#include <stdint.h>
#include <cstdio>
#include <vector>

// TODO: iNES2 support

static const uint32_t PRG_BANK_SIZE = 16384;
static const uint32_t CHR_BANK_SIZE = 8192;
static const char HEADER_MAGIC[] = { 'N', 'E', 'S', 0x1A };

union Flags6
{
    struct  
    {
        uint8_t mHorizontalMirroring : 1;
        uint8_t mBatteryBackedMem : 1;
        uint8_t mTrainer : 1;
        uint8_t mFourScreenVRAM : 1;
        uint8_t mMapperLow : 4;
    };
    uint8_t mValues;
};

union Flags7
{
    struct
    {
        uint8_t mVSUnisystem : 1;
        uint8_t mPlayChoice10 : 1;
        uint8_t mINES2 : 2;
        uint8_t mMapperHigh : 4;
    };
    uint8_t mValues;
};

union Flags9 // INES 1 only! almost not used
{
    struct
    {
        uint8_t mTVSystem : 2;      // TV system (0: NTSC; 2: PAL; 1/3: dual compatible)
        uint8_t mUnused : 2;        // PRG RAM ($6000-$7FFF) (0: present; 1: not present)
        uint8_t mBoardConflicts : 1; // Board has no bus conflicts; 1: Board has bus conflicts
    };
    uint8_t mValues;
};

class Header
{
public:
    char mMagic[4];
    uint8_t mPrgROMsize;
    uint8_t mChrROMsize;
    Flags6 mFlags6;
    Flags7 mFlags7;
    uint8_t mPrgRAMsize;
    Flags9 mFlags9;
    uint8_t mFlags10;
    uint8_t mReserved[5];
};

Cartridge::Cartridge(const std::string& filename)
{
	//FILE *f = fopen("d:\\Games\\NES\\!games\\Super Mario Bros. (Japan, USA).nes", "rb");
    FILE *f = fopen(filename.c_str(), "rb");
	Header header;
	fread(&header, sizeof(header), 1, f);

    if (::memcmp(header.mMagic, HEADER_MAGIC, 4) != 0)
    {
        throw 1; // TODO: throw here
    }

	uint32_t prgSize = header.mPrgROMsize * PRG_BANK_SIZE;
	uint32_t chrSize = header.mChrROMsize * CHR_BANK_SIZE;

    mPrgROM.resize(prgSize);
	fread(&mPrgROM[0], prgSize, 1, f);

	mChrROM.resize(chrSize);
	fread(&mChrROM[0], chrSize, 1, f);

	fclose(f);
}


Cartridge::~Cartridge()
{
}

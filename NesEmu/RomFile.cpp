#include "RomFile.h"
#include <stdint.h>
#include <cstdio>
#include <vector>

class Header
{
public:
	char mMagic[4];
	uint8_t mPRGROMsize;
	uint8_t mCHRROMsize;
	uint8_t mFlags6;
	uint8_t mFlags7;
	uint8_t mPRGRAMsize;
	uint8_t mFlags9;
	uint8_t mFlags10;
	uint8_t mReserved[5];
};

RomFile::RomFile()
{
	FILE *f = fopen("d:\\Games\\NES\\!games\\Super Mario Bros. (Japan, USA).nes", "rb");
	Header header;
	fread(&header, sizeof(header), 1, f);

	uint32_t prgSize = header.mPRGROMsize * 16384;
	uint32_t chrSize = header.mCHRROMsize * 8192;

	std::vector<uint8_t> PRGROM(prgSize);
	fread(&PRGROM[0], prgSize, 1, f);

	std::vector<uint8_t> CHRROM(chrSize);
	fread(&CHRROM[0], chrSize, 1, f);
	// Load to 0x8000


	fclose(f);
}


RomFile::~RomFile()
{
}

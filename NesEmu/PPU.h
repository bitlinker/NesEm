#pragma once
#include <stdint.h>

class PPU
{
public:
	union Control1
	{
		struct
		{
			uint8_t nameTableAddress : 2;
			uint8_t addressIncrement : 1;
			uint8_t spriteTable : 1;
			uint8_t bgTable : 1;
			uint8_t spriteSize : 1;
			uint8_t masterSlaveMode : 1;
			uint8_t nmiEnable : 1;
		};
		uint8_t value;
	};

	union Control2
	{
		struct
		{
			uint8_t isColor : 1;
			uint8_t showLeftBg : 1;
			uint8_t showLeftSprites : 1;
			uint8_t showBg : 1;
			uint8_t showSprites : 1;
			uint8_t tint : 3;
		};
		uint8_t value;
	};

	union Status
	{
		struct
		{
			uint8_t unused : 4;
			uint8_t ignoreVRAMwrites : 1;
			uint8_t spritesOverflow : 1;
			uint8_t sprite0hit : 4;
			uint8_t isVBlank : 1;
		};
		uint8_t value;
	};
	
public:
	PPU();
	~PPU();

	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t value);
};


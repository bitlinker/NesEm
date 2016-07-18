#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <cassert>
#include <vector>
#include <sstream>
#include <iomanip>

#include "MainWindow.h"

#include "CPU6502.h"
#include "Cartridge.h"
#include "NROM.h"
#include "RAM.h"
#include "APU.h"
#include "PPU.h"
#include "JoyPad.h"
#include "Bus.h"

int main()
{
	//Cartridge testRom("d:\\!git_projects\\NesEmu\\Tests\\nestest\\nestest.nes");
	Cartridge testRom("D:\\!git_projects\\NesEmu\\Roms\\Battle City (Japan).nes");

	uint32_t mapper = testRom.getMapper();
	assert(mapper == 0);

	NROM nrom(&testRom);
	RAM ram(0x800);
	APU apu;
	TvSystem sysetm;
	PPU ppu(sysetm);
	JoyPad jp1;
	JoyPad jp2;

	Bus bus(&ram, &nrom, &jp1, &jp2, &apu, &ppu);

	CPU6502 cpu(&bus);
	ppu.setCPU(&cpu);
	cpu.setPC(0xC000); // Start location for auto tests

	MainWindow wnd;

	while (wnd.update())
	{
		ppu.exec();
		ppu.exec();
		ppu.exec();
		cpu.exec();
	}

	return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <cassert>
#include <vector>
#include <sstream>
#include <iomanip>

#include "MainWindow.h"

#include "Console.h"

#include "CPU6502.h"
#include "Cartridge.h"
#include "NROM.h"
#include "CpuMem.h"
#include "APU.h"
#include "PPU.h"
#include "JoyPad.h"
#include "Bus.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: NesEm rom.nes" << std::endl;
        return -1;
    }

    Console console;

	//Cartridge testRom("d:\\!git_projects\\NesEmu\\Tests\\nestest\\nestest.nes");
	Cartridge testRom(argv[1]);

	uint32_t mapper = testRom.getMapper();
	assert(mapper == 0);

	static const uint16_t RAM_SIZE = 0x800;

	CpuMem ram(RAM_SIZE, true);
	PpuMem vram(RAM_SIZE, true); // TODO: get size from cart?	
	NROM nrom(&testRom, &vram, console.getPPU().get());	
    APU apu;

	console.getPPU()->setMapper(&nrom); // TODO: rm

	Bus bus(&ram, &nrom, console.getJoyPad(0).get(), console.getJoyPad(1).get(), &apu, console.getPPU().get());

	CPU6502 cpu(&bus);
    console.getPPU()->setCPU(&cpu); // TODO: rm
	cpu.setPC(0xC000); // Start location for auto tests

	MainWindow wnd(console);

	while (wnd.update())
	{
        console.getPPU()->exec();
        console.getPPU()->exec();
        console.getPPU()->exec();
		cpu.exec();
	}

	return 0;
}
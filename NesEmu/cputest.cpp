#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <cassert>
#include <vector>
#include <sstream>
#include <iomanip>

#include "CPU6502.h"
#include "Cartridge.h"
#include "NROM.h"
#include "RAM.h"
#include "APU.h"
#include "PPU.h"
#include "JoyPad.h"
#include "Bus.h"


class CPUState
{
public:
    uint32_t address;
    uint32_t instrData[3];
    std::string instructionStr;
    uint32_t a, x, y, p, sp;
    uint32_t cyc;

public:
    CPUState()
    {
        clear();
    }

    void clear()
    {
        address = 0;
        for (int i = 0; i < 3; ++i)
            instrData[i] = 0;
        instructionStr.clear();
        a = x = y = p = sp = 0;
        cyc = 0;
    }

    void parseString(const std::string& str)
    {
        clear();

        // Address & opcode
        sscanf_s(str.c_str(), "%x %x %x %x", &address, &instrData[0], &instrData[1], &instrData[2]);

        // Status
        sscanf_s(str.c_str() + 48, "A:%x X:%x Y:%x P:%x SP:%x CYC:%d", &a, &x, &y, &p, &sp, &cyc);

        std::string opcodeStr;
        opcodeStr = str.substr(6, 10);

        instructionStr = str.substr(16, 32);
    }

    void toString(std::string& str)
    {
        const uint32_t SIZE = 128;
        char formatStr[SIZE];
        sprintf_s(formatStr, SIZE, "%4X  %02X %02X %02X  %s %28s A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d",
            address,
            instrData[0],
            instrData[1], 
            instrData[2], 
            "",
            "",
            a, x, y, p, sp, cyc);            
        str.assign(formatStr);
    }
};

bool operator==(const CPUState& lhs, const CPUState& rhs)
{
    return  lhs.address == rhs.address &&
        //lhs.instrData[0] == rhs.instrData[0] &&
        lhs.a == rhs.a &&
        lhs.x == rhs.x &&
        lhs.y == rhs.y &&
        lhs.sp == rhs.sp &&
        lhs.p == rhs.p &&
        lhs.cyc == rhs.cyc;
}

int main()
{
    // http://www.qmtpro.com/~nes/misc/nestest.txt test    
    Cartridge testRom("d:\\!github\\NesEm\\Tests\\nestest\\nestest.nes");
    uint32_t mapper = testRom.getMapper();
    assert(mapper == 0);

    NROM nrom(&testRom);
    RAM ram(0x800);
    APU apu;
    PPU ppu;
    JoyPad jp1;
    JoyPad jp2;
    
    Bus bus(&ram, &nrom, &jp1, &jp2, &apu, &ppu);

    CPUState state;

    CPU6502 cpu(&bus);
    ppu.setCPU(&cpu);
    cpu.setPC(0xC000); // Start location for auto tests
    
    std::string inStr;
    std::string outStr;
    std::ifstream stream("d:\\!github\\NesEm\\Tests\\nestest\\nestest.log", std::ifstream::in);
    while (stream.good())
    {
        std::getline(stream, inStr);
        if (inStr.empty())
        {
            return 0;
        }

        state.parseString(inStr);

        CPUState realState;
        realState.a = cpu.getA();
        realState.x = cpu.getX();
        realState.y = cpu.getY();
        realState.sp = cpu.getSP();
        realState.address = cpu.getPC();
        realState.cyc = (cpu.getCycles() * 3) % 341; // Strange cycles conversion to fit the log
        realState.p = cpu.getFlags();
        for (int i = 0; i < 3; ++i) {
            realState.instrData[i] = bus.read(realState.address + i);
        }
        
        realState.toString(outStr);        
        ::printf("%s\n", outStr.c_str());

        if (!(state == realState))
        {
            return 1;
        }

        //ppu.exec();
        //ppu.exec();
        //ppu.exec();

        cpu.exec();
    }

    
    return 0;
}
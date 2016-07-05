#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <vector>
#include "CPU6502.h"
#include "Cartridge.h"


class TestRam : public IMemory
{
public:
    TestRam()
        : mRAM(0xFFFF)
    {
    }

    virtual void write(uint16_t address, uint8_t value)
    {
        mRAM[address] = value;
    }

    virtual uint8_t read(uint16_t address)
    {
        return mRAM[address];
    }

private:
    std::vector<uint8_t> mRAM;
};

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
};

int main()
{
    // http://www.qmtpro.com/~nes/misc/nestest.txt test    
    TestRam testRam; // TODO: rm

    Cartridge testRom("d:\\!github\\NesEm\\Tests\\nestest\\nestest.nes");

    CPUState state;

    CPU6502 cpu(&testRam);
    cpu.setPC(0xC00); // Start location for auto tests

    // TODO: load rom

    std::string tmpstr;
    std::ifstream stream("d:\\!github\\NesEm\\Tests\\nestest\\nestest.log", std::ifstream::in);
    while (stream.good())
    {
        std::getline(stream, tmpstr);
        state.parseString(tmpstr);
    }

    cpu.exec();

    return 0;
}
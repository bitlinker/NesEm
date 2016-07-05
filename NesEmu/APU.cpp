#include "APU.h"

APU::APU()
{
}

APU::~APU()
{
}

void APU::write(uint16_t address, uint8_t value)
{
    if (address == 0x4000) // Pulse control 1
    {
        //mPulse1.writeControl1(value);
    }
    else if (address == 0x4004) // Pulse control 2
    {
        //mPulse2.writeControl1(value);
    }

    // TODO
}

uint8_t APU::read(uint16_t address)
{
    // TODO
    return 0;
}

#pragma once
#include <stdint.h>
#include "IMemory.h"

class PulseChannel
{
public:
    union Control
    {
        struct
        {
            uint8_t mDuty : 2;                  // Duty cycle
            uint8_t mLengthCounterHalt : 1;     // Length counter halt flag
            uint8_t mConstantVol : 1;           // Constant volume/envelope flag
            uint8_t mVolEnv : 4;                // Volume/envelope divider
        };
        uint8_t mValue;
    };

    union Sweep
    {
        struct
        {
            // TODO
        };
        uint8_t mValue;
    };

public:
    void writeControl1(uint8_t value);
    void writeTimer(uint8_t value);
    void writeLengthTimer(uint8_t value);
    void writeSweep(uint8_t value);


private:
    Control mControl;
    uint8_t mTimerLow;
    uint8_t mTimerHi;
    uint8_t mLengthCounter;
};

class TriangleChannel
{

};

class DMCChannel
{

};

class NoiseChannel
{

};

class APU : public IMemory
{
public:
	APU();
	virtual ~APU();

    virtual void write(uint16_t address, uint8_t value);
    virtual uint8_t read(uint16_t address);

private:
    PulseChannel mPulse1;
    PulseChannel mPulse2;
    TriangleChannel mTriangle;
    TriangleChannel mNoise;
    TriangleChannel mDMC;
};


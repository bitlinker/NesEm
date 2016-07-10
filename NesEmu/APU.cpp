#include "APU.h"

// Pulse 1
static uint32_t PULSE1_VOL_ADDRESS = 0x4000;
static uint32_t PULSE1_SWEEP_ADDRESS = 0x4001;
static uint32_t PULSE1_TIMER_LOW_ADDRESS = 0x4002;
static uint32_t PULSE1_TIMER_HIGH_ADDRESS = 0x4003;

// Pulse 2
static uint32_t PULSE2_VOL_ADDRESS = 0x4004;
static uint32_t PULSE2_SWEEP_ADDRESS = 0x4005;
static uint32_t PULSE2_TIMER_LOW_ADDRESS = 0x4006;
static uint32_t PULSE2_TIMER_HIGH_ADDRESS = 0x4007;

// Triangle
static uint32_t TRIANGLE_LINEAR_ADDRESS = 0x4008;
static uint32_t TRIANGLE_TIMER_LOW_ADDRESS = 0x400A;
static uint32_t TRIANGLE_TIMER_HIGH_ADDRESS = 0x400B;

// Noise
static uint32_t NOISE_VOL_ADDRESS = 0x400C;
static uint32_t NOISE_TIMER_LOW_ADDRESS = 0x400E;
static uint32_t NOISE_TIMER_HIGH_ADDRESS = 0x400F;

// DMC
static uint32_t DMC_FREQ_ADDRESS = 0x4010;
static uint32_t DMC_RAW_ADDRESS = 0x4011;
static uint32_t DMC_START_ADDRESS = 0x4012;
static uint32_t DMC_LEN_ADDRESS = 0x4013;

// Mixer & status
static uint32_t APU_MIXER_STATUS_ADDRESS = 0x4015;

static uint8_t PULSE_DUTY_LUT[4][8] = {
    { 0, 1, 0, 0, 0, 0, 0, 0 }, // 12.5%
    { 0, 1, 1, 0, 0, 0, 0, 0 }, // 25%
    { 0, 1, 1, 1, 1, 0, 0, 0 }, // 50%
    { 1, 0, 0, 1, 1, 1, 1, 1 }, // 25% negated
};

// 11bit timer
// APU cycle == every 2 CPU cycle


void PulseChannel::writeControl(uint8_t value)
{

}

void PulseChannel::writeSweep(uint8_t value)
{

}

void PulseChannel::writeTimerLo(uint8_t value)
{

}

void PulseChannel::writeTimerHi(uint8_t value)
{

}




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
        mPulse1.writeControl(value);
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

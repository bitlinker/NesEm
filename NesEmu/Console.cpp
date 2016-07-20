#include <cassert>
#include "Console.h"

Console::Console()
    : mTvSystem(TvSystem::CreateNTSC())
    , mPPU()
    , mNmiListener(nullptr)
{    
    mPPU = std::make_shared<PPU>(mTvSystem);
    mPPU->setNmiListener(this);
    for (uint32_t i = 0; i < Console::NUM_JOYPADS; ++i)
    {
        mJoyPad[i] = std::make_shared<JoyPad>();
    }

    // TODO: init bus here...
}

Console::~Console()
{
}

const TvSystem& Console::getTvSystem() const
{
    return mTvSystem;
}

const std::shared_ptr<PPU>& Console::getPPU() const
{
    return mPPU;
}

const std::shared_ptr<JoyPad>& Console::getJoyPad(uint32_t index) const
{
    assert(index < NUM_JOYPADS);
    return mJoyPad[index];
}

void Console::onNMI()
{
    if (mNmiListener)
    {
        mNmiListener->onNMI();
    }
}
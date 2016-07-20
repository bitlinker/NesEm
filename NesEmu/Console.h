#pragma once

#include <memory>

#include "JoyPad.h"
#include "PPU.h"
#include "APU.h"
#include "CPU6502.h"
#include "Cartridge.h"

#include "TvSystem.h"
#include "INmiListener.h"

class Console : public INmiListener
{
public:
    static const uint32_t NUM_JOYPADS = 2;
public:
    Console();
    virtual ~Console();

    const TvSystem& getTvSystem() const;
    const std::shared_ptr<PPU>& getPPU() const;
    const std::shared_ptr<JoyPad>& getJoyPad(uint32_t index) const;

    void setNmiListener(INmiListener* nmiListener) { mNmiListener = nmiListener; }

    // INmiListener:
    virtual void onNMI();

private:
    TvSystem mTvSystem;
    std::shared_ptr<JoyPad> mJoyPad[NUM_JOYPADS];
    std::shared_ptr<PPU> mPPU;    

    INmiListener* mNmiListener;
};


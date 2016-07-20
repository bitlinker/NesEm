#include "TvSystem.h"

TvSystem TvSystem::CreateNTSC()
{
    TvSystem system;
    system.mBaseClock = 21477272; // TODO: this is double?
    system.mCpuDivider = 12;
    system.mApuFrameClock = 60; // TODO: double?
    system.mPpuDivider = 4;
    system.mPpuHeight = 240;
    system.mPpuWidth = 256;
    system.mPpuPostRenderScanlines = 1;
    system.mPpuVblankScanlines = 20;
    system.mIsBordersBlack = false;
    system.mIsEmphasisRGInverted = false;
    return system;
}
#pragma once
#include <stdint.h>

class TvSystem
{
private:
    TvSystem() {};

public:
    static TvSystem CreateNTSC();

    uint32_t getPpuWidth() const { return mPpuWidth; }
    uint32_t getPpuHeight() const { return mPpuHeight; }

private:
    uint64_t mBaseClock;
    uint32_t mCpuDivider;
    uint32_t mPpuDivider;
    uint32_t mApuFrameClock;

    uint32_t mPpuWidth;
    uint32_t mPpuHeight;

    uint32_t mPpuPostRenderScanlines; // "Post-render" blanking lines between end of picture and NMI
    uint32_t mPpuVblankScanlines; // Length of vertical blanking after NMI
                                  // Time during which OAM can be written TODO
    uint32_t mPpuPreRenderScanlines; // "Pre-render" lines between vertical blanking and next picture
    bool mIsBordersBlack;
    bool mIsEmphasisRGInverted;
};
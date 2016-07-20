#pragma once
#include <stdint.h>
#include <vector>
#include <cassert>

// TODO: GPU package
class Palette
{
public:
    static const uint32_t MAX_COLORS = 64;

public:
    explicit Palette(uint8_t* dataRgb24bit)
        : mPal(MAX_COLORS)
    {
        uint8_t* dst = reinterpret_cast<uint8_t*>(&mPal[0]);
        for (uint32_t c = 0; c < MAX_COLORS; ++c)
        {
            *dst++ = *dataRgb24bit++;
            *dst++ = *dataRgb24bit++;
            *dst++ = *dataRgb24bit++;
            *dst++ = 0xFF;
        }
    }

    explicit Palette(uint16_t* dataBcd9bit)
        : mPal(MAX_COLORS)
    {
        uint32_t* dst = &mPal[0];
        for (uint32_t c = 0; c < MAX_COLORS; ++c)
        {
            uint16_t src = *dataBcd9bit++;
            uint8_t r = (src / 100);
            uint8_t g = ((src - r * 100) / 10);
            uint8_t b = (src - r * 100 - g * 10);
            r *= 36;
            g *= 36;
            b *= 36;
            *dst++ = (r | g << 8 | (b << 16) | (0xFF << 24));
        }
    }

    uint32_t getColor(uint8_t color)
    {
        assert(color < MAX_COLORS);
        return mPal[color];
    }
private:
    std::vector<uint32_t> mPal;
};

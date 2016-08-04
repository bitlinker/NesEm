#pragma once
#include "IMemory.h"

class IMapper : public ICpuMemory, public IPpuMemory
{
public:
    enum Mirroring
    {
        MIRROR_VERTICAL,
        MIRROR_HORIZONTAL,
        MIRROR_ONESCREEN,
        MIRROR_FOURSCREEN
    };

public:
    virtual Mirroring getMirroring() = 0;

    virtual ~IMapper() {};
};
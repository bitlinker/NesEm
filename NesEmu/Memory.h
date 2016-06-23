#pragma once
#include "IMemory.h"
#include <vector>

class Memory : public IMemory
{
private:
	std::vector<uint8_t> mRAM;

public:
	Memory();
	virtual ~Memory();

	uint8_t* getRawPtr() { return &mRAM[0]; }

	virtual void write(uint16_t address, uint8_t value);
	virtual uint8_t read(uint16_t address);
};


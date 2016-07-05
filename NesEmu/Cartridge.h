#pragma once
#include <string>
#include <vector>

class Cartridge
{
public:
	Cartridge(const std::string& filename);
	~Cartridge();

    // TODO
    bool hasPersistantMemory();

private:
    std::vector<uint8_t> mTrainer;
    std::vector<uint8_t> mPrgROM;
    std::vector<uint8_t> mChrROM;
    uint32_t mMapper;
};


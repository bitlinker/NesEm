#include <cstdio>
#include <vector>
#include <stdint.h>

#include "CPU6502.h"
#include "Memory.h"

int main()
{
	Memory mem;
	CPU6502 cpu(&mem);

	FILE* f = ::fopen("rom.data", "rb");
	::fseek(f, 0L, SEEK_END);
	size_t size = ::ftell(f);
	::fseek(f, 0L, SEEK_SET);
	::fread(mem.getRawPtr() + 0x600, size, 1, f);
	::fclose(f);
	
	while (cpu.exec())
	{
		//printf("Cycles: %d", cpu.cycles);
	}
}
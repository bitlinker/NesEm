#include "CPU6502.h"
#include <cstring>

typedef void (CPU6502::*pCpuFunc)();

static const uint32_t MAX_INSTR_NAME = 8;
class Instruction
{
public:
	static const uint32_t FLAG_CYCLE_PAGE_CROSSED = 1;

public:
	Instruction()
		: mAddrMode(ADDR_D)
		, mMemMode(MEM_NONE)
		, mCycles(0)
		, mFunc(nullptr)
		, mName("empty")
		, mFlags(0)
	{
	}

	Instruction(AddressMode addressMode, MemMode memMode, uint32_t cycles, pCpuFunc cpuFunc, const char* name, uint32_t flags)
		: mAddrMode(addressMode)
		, mMemMode(memMode)
		, mCycles(cycles)
		, mFunc(cpuFunc)
		, mFlags(flags)
	{
		::strncpy(mName, name, MAX_INSTR_NAME);
	}
public:
	AddressMode mAddrMode;
	MemMode mMemMode;
	uint32_t mCycles;
	pCpuFunc mFunc;
	char mName[MAX_INSTR_NAME];
	uint32_t mFlags;
};
#define MAKE_INSTR(addr_mode, mem_mode, cycles, func, flags) Instruction(addr_mode, mem_mode, cycles, &CPU6502::##func, #func, flags)


static const uint32_t MAX_INSTRUCTIONS = 0xFF;
static Instruction INSTRUCTIONS[MAX_INSTRUCTIONS];

static void InitInstructionTable()
{
	// TODO: sort by opcode?
	// LDA
	INSTRUCTIONS[0xA9] = MAKE_INSTR(ADDR_D, MEM_READ, 2, lda, 0);
	INSTRUCTIONS[0xA5] = MAKE_INSTR(ADDR_ZP, MEM_READ, 3, lda, 0);
	INSTRUCTIONS[0xB5] = MAKE_INSTR(ADDR_ZPX, MEM_READ, 4, lda, 0);
	INSTRUCTIONS[0xAD] = MAKE_INSTR(ADDR_A, MEM_READ, 4, lda, 0);
	INSTRUCTIONS[0xBD] = MAKE_INSTR(ADDR_AX, MEM_READ, 4, lda, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0xB9] = MAKE_INSTR(ADDR_AY, MEM_READ, 4, lda, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0xA1] = MAKE_INSTR(ADDR_IX, MEM_READ, 6, lda, 0);
	INSTRUCTIONS[0xA1] = MAKE_INSTR(ADDR_IY, MEM_READ, 5, lda, Instruction::FLAG_CYCLE_PAGE_CROSSED);

	// LDX
	//
	//{ ADDR_ZPX, MEM_READ, 2, &CPU6502::lda, "LDA", 0 },  // B5
	//{ ADDR_D, MEM_READ, 2, &CPU6502::lda, "LDA", 0 },  // A9

	// STA
	INSTRUCTIONS[0x85] = MAKE_INSTR(ADDR_ZP, MEM_WRITE, 3, sta, 0);
	INSTRUCTIONS[0x95] = MAKE_INSTR(ADDR_ZPX, MEM_WRITE, 4, sta, 0);
	INSTRUCTIONS[0x8D] = MAKE_INSTR(ADDR_A, MEM_WRITE, 4, sta, 0);
	INSTRUCTIONS[0x9D] = MAKE_INSTR(ADDR_AX, MEM_WRITE, 5, sta, 0);
	INSTRUCTIONS[0x99] = MAKE_INSTR(ADDR_AY, MEM_WRITE, 5, sta, 0);
	INSTRUCTIONS[0x81] = MAKE_INSTR(ADDR_IX, MEM_WRITE, 6, sta, 0);
	INSTRUCTIONS[0x91] = MAKE_INSTR(ADDR_IY, MEM_WRITE, 6, sta, 0);
}

CPU6502::CPU6502(IMemory* mem)
	: pc(0x600) // TODO: const
	, a(0)
	, x(0)
	, y(0)
	, stack(0xFF) // TODO: const
	, cycles(0)
	, flags({ 0 })
	, memory(mem)
	, src(0)
	, dst(0)
{
	// TODO: init static?
	InitInstructionTable();
}

CPU6502::~CPU6502()
{
}

inline static uint16_t MakeAddress(uint8_t first, uint8_t second)
{
	// TODO: check
	return (static_cast<uint16_t>(first) << 8) | static_cast<uint16_t>(second);
}

void CPU6502::readMem(AddressMode mode)
{
	switch (mode)
	{
	case ADDR_D:
	{
		src = readPC();
		break;
	}
	case ADDR_ZP:
	{
		uint8_t addr = readPC();
		src = memory->read(addr);
		break;
	}
	case ADDR_ZPX:
	{
		uint8_t addr = readPC() + x;
		src = memory->read(addr);
		break;
	}
	case ADDR_ZPY:
	{
		uint8_t addr = readPC() + y;
		src = memory->read(addr);
		break;
	}
	case ADDR_A:
	{
		uint8_t first = readPC();
		uint8_t second = readPC();
		uint16_t addr = MakeAddress(first, second);
		src = memory->read(addr);
		break;
	}
	case ADDR_AX:
	{
		uint8_t first = readPC();
		uint8_t second = readPC();
		uint16_t addr = MakeAddress(first, second) + x;
		src = memory->read(addr);
		break;
	}
	case ADDR_AY:
	{
		uint8_t first = readPC(); // TODO: copypaste
		uint8_t second = readPC();
		uint16_t addr = MakeAddress(first, second) + y;
		src = memory->read(addr);
		break;
	}
	// TODO: indirect
	case ADDR_IX:
	{
		uint8_t first = readPC(); // TODO: copypaste
		uint8_t second = readPC();
		uint16_t addr = MakeAddress(first, second);
		uint8_t val_first = memory->read(addr) + x;
		uint8_t val_second = memory->read(addr) + x + 1;
		src = memory->read(MakeAddress(val_first, val_second));
		break;
	}
	case ADDR_IY:
		 // TODO
		break;
	default:
		// TODO: assert?
		break;
	}
}

void CPU6502::writeMem(AddressMode mode)
{

}

uint8_t CPU6502::readPC()
{
	return memory->read(pc++);
}

bool CPU6502::exec()
{
	uint8_t opcode = readPC();
	const Instruction& cmd = INSTRUCTIONS[opcode];

	if (cmd.mFunc == nullptr) {
		// TODO: assert;
		return false;
	}

	if (cmd.mMemMode == MEM_READ)
	{
		readMem(cmd.mAddrMode);
	}

	(*this.*cmd.mFunc)();

	if (cmd.mMemMode == MEM_WRITE)
	{
		writeMem(cmd.mAddrMode);
	}

	cycles += cmd.mCycles;
	// TODO: page crossed flag

	// TODO: check?
	return !flags.brk;
}

// Load / Store Operations
void CPU6502::lda()
{
	a = src;
	flags.zero = a ? 0 : 1;
	flags.negative = (a & 0x80) ? 1 : 0;
}

void CPU6502::ldx()
{
	x = src;
	// TODO
}

void CPU6502::ldy()
{
	y = src;
	// TODO
}

void CPU6502::sta()
{
	dst = a;
	// TODO
}

void CPU6502::stx()
{
	dst = x;
	// TODO
}

void CPU6502::sty()
{
	dst = y;
	// TODO
}

// Register Transfers
void CPU6502::tax()
{
}

void CPU6502::tay()
{

}

void CPU6502::txa()
{

}

void CPU6502::tya()
{

}

// Stack Operations
void CPU6502::tsx()
{

}

void CPU6502::txs()
{

}

void CPU6502::pha()
{

}

void CPU6502::php()
{

}

void CPU6502::pla()
{

}

void CPU6502::plp()
{

}

// Logical
void CPU6502::and()
{

}

void CPU6502::eor()
{

}

void CPU6502::ora()
{

}

void CPU6502::bit()
{

}

// Arithmetic
void CPU6502::adc()
{

}

void CPU6502::sbc()
{

}

void CPU6502::cmp()
{

}

void CPU6502::cpx()
{
}

void CPU6502::cpy()
{

}

// Increments & Decrements
void CPU6502::inc()
{

}

void CPU6502::inx()
{

}

void CPU6502::iny()
{

}

void CPU6502::dec()
{

}

void CPU6502::dex()
{

}

void CPU6502::dey()
{

}

// Shifts
void CPU6502::asl()
{

}

void CPU6502::lsr()
{

}

void CPU6502::rol()
{

}

void CPU6502::ror()
{

}

// Jumps & Calls
void CPU6502::jmp()
{

}

void CPU6502::jsr()
{

}

void CPU6502::rts()
{

}

// Branches
void CPU6502::bcc()
{

}

void CPU6502::bcs()
{

}

void CPU6502::beq()
{
}

void CPU6502::bmi()
{

}

void CPU6502::bne()
{

}

void CPU6502::bpl()
{

}

void CPU6502::bvc()
{

}

void CPU6502::bvs()
{

}

// Status Flag Changes
void CPU6502::clc()
{
	flags.carry = 0;
}

void CPU6502::cld()
{
	flags.decimal_mode = 0;
	// TODO: warn?
}

void CPU6502::cli()
{
	flags.interrupt_disable = 0;
}

void CPU6502::clv()
{
	flags.overflow = 0;
}

void CPU6502::sec()
{
	flags.carry = 1;
}

void CPU6502::sed()
{
	flags.decimal_mode = 1;
	// TODO: warn?
}

void CPU6502::sei()
{
	flags.interrupt_disable = 1;
}

// System Functions
void CPU6502::brk()
{

}

void CPU6502::nop()
{
	// Do nothing
}

void CPU6502::rti()
{

}
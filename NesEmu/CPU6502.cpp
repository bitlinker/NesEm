#include "CPU6502.h"
#include <cstring>
#include <cassert>

typedef void (CPU6502::*pCpuFunc)();

static const uint32_t MAX_INSTR_NAME = 8;
class Instruction
{
public:
	static const uint32_t FLAG_CYCLE_PAGE_CROSSED = 1;

public:
	Instruction()
		: mAddressFunc(nullptr)
		, mFunc(nullptr)
		, mName("empty")
		, mCycles(0)
		, mFlags(0)
	{
	}

	Instruction(pCpuFunc addressFunc, pCpuFunc cpuFunc, const char* name, uint32_t cycles, uint32_t flags)
		: mAddressFunc(nullptr)
		, mCycles(cycles)
		, mFunc(cpuFunc)
		, mFlags(flags)
	{
		::strncpy(mName, name, MAX_INSTR_NAME);
	}
public:
	pCpuFunc mAddressFunc;
	pCpuFunc mFunc;
	char mName[MAX_INSTR_NAME];
	uint32_t mCycles;
	uint32_t mFlags;
};
#define MAKE_INSTR(addressFunc, func, cycles, flags) Instruction(&CPU6502::##addressFunc, &CPU6502::##func, #func, cycles, flags)


static const uint32_t MAX_INSTRUCTIONS = 0xFF;
static Instruction INSTRUCTIONS[MAX_INSTRUCTIONS];

static void InitInstructionTable()
{
	// TODO: sort by opcode?
	// ADC
	INSTRUCTIONS[0x69] = MAKE_INSTR(addressImmediate, adc, 2, 0);
	INSTRUCTIONS[0x65] = MAKE_INSTR(addressZP, adc, 3, 0);
	INSTRUCTIONS[0x75] = MAKE_INSTR(addressZPX, adc, 4, 0);
	INSTRUCTIONS[0x6D] = MAKE_INSTR(addressAbs, adc, 4, 0);
	INSTRUCTIONS[0x7D] = MAKE_INSTR(addressAbsX, adc, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0x79] = MAKE_INSTR(addressAbsY, adc, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0x61] = MAKE_INSTR(addressIX, adc, 6, 0);
	INSTRUCTIONS[0x71] = MAKE_INSTR(addressIY, adc, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

	// AND
	INSTRUCTIONS[0x29] = MAKE_INSTR(addressImmediate, and, 2, 0);
	INSTRUCTIONS[0x25] = MAKE_INSTR(addressZP, and, 3, 0);
	INSTRUCTIONS[0x35] = MAKE_INSTR(addressZPX, and, 4, 0);
	INSTRUCTIONS[0x2D] = MAKE_INSTR(addressAbs, and, 4, 0);
	INSTRUCTIONS[0x3D] = MAKE_INSTR(addressAbsX, and, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0x39] = MAKE_INSTR(addressAbsY, and, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0x21] = MAKE_INSTR(addressIX, and, 6, 0);
	INSTRUCTIONS[0x31] = MAKE_INSTR(addressIY, and, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

	// ASL
	INSTRUCTIONS[0x0A] = MAKE_INSTR(addressImplied, asl_a, 2, 0);
	INSTRUCTIONS[0x06] = MAKE_INSTR(addressZP, asl_m, 5, 0);
	INSTRUCTIONS[0x16] = MAKE_INSTR(addressZPX, asl_m, 6, 0);
	INSTRUCTIONS[0x0E] = MAKE_INSTR(addressAbs, asl_m, 6, 0);
	INSTRUCTIONS[0x1E] = MAKE_INSTR(addressAbsX, asl_m, 7, 0);

	// BCC
	INSTRUCTIONS[0x90] = MAKE_INSTR(addressRelative, bcc, 2, 0);

	// BCS
	INSTRUCTIONS[0xB0] = MAKE_INSTR(addressRelative, bcs, 2, 0);

	// BEQ
	INSTRUCTIONS[0xF0] = MAKE_INSTR(addressRelative, beq, 2, 0);

	// BIT
	INSTRUCTIONS[0x24] = MAKE_INSTR(addressZP, bit, 3, 0);
	INSTRUCTIONS[0x2C] = MAKE_INSTR(addressAbs, bit, 4, 0);

	// BMI
	INSTRUCTIONS[0x30] = MAKE_INSTR(addressRelative, bmi, 2, 0);

	// BNE
	INSTRUCTIONS[0xD0] = MAKE_INSTR(addressRelative, bne, 2, 0);

	// BPL
	INSTRUCTIONS[0x10] = MAKE_INSTR(addressRelative, bpl, 2, 0);

	// BRK
	INSTRUCTIONS[0x00] = MAKE_INSTR(addressImplied, brk, 7, 0);

	// BVC
	INSTRUCTIONS[0x50] = MAKE_INSTR(addressRelative, bvc, 2, 0);

	// BVS
	INSTRUCTIONS[0x70] = MAKE_INSTR(addressRelative, bvs, 2, 0);

	// CLC
	INSTRUCTIONS[0x18] = MAKE_INSTR(addressImplied, clc, 2, 0);

	// CLD
	INSTRUCTIONS[0xD8] = MAKE_INSTR(addressImplied, cld, 2, 0);

	// CLI
	INSTRUCTIONS[0x58] = MAKE_INSTR(addressImplied, cli, 2, 0);

	// CLV
	INSTRUCTIONS[0xB8] = MAKE_INSTR(addressImplied, clv, 2, 0);

	// CMP
	INSTRUCTIONS[0xC9] = MAKE_INSTR(addressImmediate, cmp, 2, 0);
	INSTRUCTIONS[0xC5] = MAKE_INSTR(addressZP, cmp, 3, 0);
	INSTRUCTIONS[0xD5] = MAKE_INSTR(addressZPX, cmp, 4, 0);
	INSTRUCTIONS[0xCD] = MAKE_INSTR(addressAbs, cmp, 4, 0);
	INSTRUCTIONS[0xDD] = MAKE_INSTR(addressAbsX, cmp, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0xD9] = MAKE_INSTR(addressAbsY, cmp, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0xC1] = MAKE_INSTR(addressIX, cmp, 6, 0);
	INSTRUCTIONS[0xD1] = MAKE_INSTR(addressIY, cmp, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

	// CPX
	INSTRUCTIONS[0xE0] = MAKE_INSTR(addressImmediate, cpx, 2, 0);
	INSTRUCTIONS[0xE4] = MAKE_INSTR(addressZP, cpx, 3, 0);
	INSTRUCTIONS[0xEC] = MAKE_INSTR(addressAbs, cpx, 4, 0);

	// CPY
	INSTRUCTIONS[0xC0] = MAKE_INSTR(addressImmediate, cpy, 2, 0);
	INSTRUCTIONS[0xC4] = MAKE_INSTR(addressZP, cpy, 3, 0);
	INSTRUCTIONS[0xCC] = MAKE_INSTR(addressAbs, cpy, 4, 0);

	// DEC
	INSTRUCTIONS[0xC6] = MAKE_INSTR(addressZP, dec, 5, 0);
	INSTRUCTIONS[0xD6] = MAKE_INSTR(addressZPX, dec, 6, 0);
	INSTRUCTIONS[0xCE] = MAKE_INSTR(addressAbs, dec, 6, 0);
	INSTRUCTIONS[0xDE] = MAKE_INSTR(addressAbsX, dec, 7, 0);

	// DEX
	INSTRUCTIONS[0xCA] = MAKE_INSTR(addressImplied, dex, 2, 0);

	// DEY
	INSTRUCTIONS[0x88] = MAKE_INSTR(addressImplied, dey, 2, 0);

	// EOR
	INSTRUCTIONS[0x49] = MAKE_INSTR(addressImmediate, eor, 2, 0);
	INSTRUCTIONS[0x45] = MAKE_INSTR(addressZP, eor, 3, 0);
	INSTRUCTIONS[0x55] = MAKE_INSTR(addressZPX, eor, 4, 0);
	INSTRUCTIONS[0x4D] = MAKE_INSTR(addressAbs, eor, 4, 0);
	INSTRUCTIONS[0x5D] = MAKE_INSTR(addressAbsX, eor, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0x59] = MAKE_INSTR(addressAbsY, eor, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0x41] = MAKE_INSTR(addressIX, eor, 6, 0);
	INSTRUCTIONS[0x51] = MAKE_INSTR(addressIY, eor, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

	// INC
	INSTRUCTIONS[0xE6] = MAKE_INSTR(addressZP, inc, 5, 0);
	INSTRUCTIONS[0xF6] = MAKE_INSTR(addressZPX, inc, 6, 0);
	INSTRUCTIONS[0xEE] = MAKE_INSTR(addressAbs, inc, 6, 0);
	INSTRUCTIONS[0xFE] = MAKE_INSTR(addressAbsX, inc, 7, 0);

	// INX
	INSTRUCTIONS[0xE8] = MAKE_INSTR(addressImplied, inx, 2, 0);

	// INY
	INSTRUCTIONS[0xC8] = MAKE_INSTR(addressImplied, iny, 2, 0);

	// JMP
	INSTRUCTIONS[0x4C] = MAKE_INSTR(addressAbs, jmp, 3, 0);
	INSTRUCTIONS[0x6C] = MAKE_INSTR(addressI, jmp, 5, 0);
	// TODO: indirect fetch bug here...

	// JSR
	INSTRUCTIONS[0x20] = MAKE_INSTR(addressAbs, jsr, 6, 0);

	// LDA
	INSTRUCTIONS[0xA9] = MAKE_INSTR(addressImmediate, lda, 2, 0);
	INSTRUCTIONS[0xA5] = MAKE_INSTR(addressZP, lda, 3, 0);
	INSTRUCTIONS[0xB5] = MAKE_INSTR(addressZPX, lda, 4, 0);
	INSTRUCTIONS[0xAD] = MAKE_INSTR(addressAbs, lda, 4, 0);
	INSTRUCTIONS[0xBD] = MAKE_INSTR(addressAbsX, lda, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0xB9] = MAKE_INSTR(addressAbsY, lda, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0xA1] = MAKE_INSTR(addressIX, lda, 6, 0);
	INSTRUCTIONS[0xA1] = MAKE_INSTR(addressIY, lda, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

	// LDX
	INSTRUCTIONS[0xA2] = MAKE_INSTR(addressImmediate, ldx, 2, 0);
	INSTRUCTIONS[0xA6] = MAKE_INSTR(addressZP, ldx, 3, 0);
	INSTRUCTIONS[0xB6] = MAKE_INSTR(addressZPY, ldx, 4, 0);
	INSTRUCTIONS[0xAE] = MAKE_INSTR(addressAbs, ldx, 4, 0);
	INSTRUCTIONS[0xBE] = MAKE_INSTR(addressAbsY, ldx, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);

	// LDY
	INSTRUCTIONS[0xA0] = MAKE_INSTR(addressImmediate, ldy, 2, 0);
	INSTRUCTIONS[0xA4] = MAKE_INSTR(addressZP, ldy, 3, 0);
	INSTRUCTIONS[0xB4] = MAKE_INSTR(addressZPX, ldy, 4, 0);
	INSTRUCTIONS[0xAC] = MAKE_INSTR(addressAbs, ldy, 4, 0);
	INSTRUCTIONS[0xBC] = MAKE_INSTR(addressAbsX, ldy, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);

	// LSR
	INSTRUCTIONS[0x4A] = MAKE_INSTR(addressImplied, lsr_a, 2, 0);
	INSTRUCTIONS[0x46] = MAKE_INSTR(addressZP, lsr_m, 5, 0);
	INSTRUCTIONS[0x56] = MAKE_INSTR(addressZPX, lsr_m, 6, 0);
	INSTRUCTIONS[0x4E] = MAKE_INSTR(addressAbs, lsr_m, 6, 0);
	INSTRUCTIONS[0x5E] = MAKE_INSTR(addressAbsX, lsr_m, 7, 0);

	// NOP
	INSTRUCTIONS[0xEA] = MAKE_INSTR(addressImplied, nop, 2, 0);

	// ORA
	INSTRUCTIONS[0x09] = MAKE_INSTR(addressImmediate, ora, 2, 0);
	INSTRUCTIONS[0x05] = MAKE_INSTR(addressZP, ora, 3, 0);
	INSTRUCTIONS[0x15] = MAKE_INSTR(addressZPX, ora, 4, 0);
	INSTRUCTIONS[0x0D] = MAKE_INSTR(addressAbs, ora, 4, 0);
	INSTRUCTIONS[0x1D] = MAKE_INSTR(addressAbsX, ora, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0x19] = MAKE_INSTR(addressAbsY, ora, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0x01] = MAKE_INSTR(addressIX, ora, 6, 0);
	INSTRUCTIONS[0x11] = MAKE_INSTR(addressIY, ora, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

	// PHA
	INSTRUCTIONS[0x48] = MAKE_INSTR(addressImplied, pha, 3, 0);

	// PHP
	INSTRUCTIONS[0x08] = MAKE_INSTR(addressImplied, php, 3, 0);

	// PLA
	INSTRUCTIONS[0x68] = MAKE_INSTR(addressImplied, pla , 4, 0);

	// PLP
	INSTRUCTIONS[0x28] = MAKE_INSTR(addressImplied, plp, 4, 0);

	// ROL
	INSTRUCTIONS[0x2A] = MAKE_INSTR(addressImplied, rol_a, 2, 0);
	INSTRUCTIONS[0x26] = MAKE_INSTR(addressZP, rol_m, 5, 0);
	INSTRUCTIONS[0x36] = MAKE_INSTR(addressZPX, rol_m, 6, 0);
	INSTRUCTIONS[0x2E] = MAKE_INSTR(addressAbs, rol_m, 6, 0);
	INSTRUCTIONS[0x3E] = MAKE_INSTR(addressAbsX, rol_m, 7, 0);

	// ROR
	INSTRUCTIONS[0x6A] = MAKE_INSTR(addressImplied, ror_a, 2, 0);
	INSTRUCTIONS[0x66] = MAKE_INSTR(addressZP, ror_m, 5, 0);
	INSTRUCTIONS[0x76] = MAKE_INSTR(addressZPX, ror_m, 6, 0);
	INSTRUCTIONS[0x6E] = MAKE_INSTR(addressAbs, ror_m, 6, 0);
	INSTRUCTIONS[0x7E] = MAKE_INSTR(addressAbsX, ror_m, 7, 0);

	// RTI
	INSTRUCTIONS[0x40] = MAKE_INSTR(addressImplied, rti, 6, 0);

	// RTS
	INSTRUCTIONS[0x60] = MAKE_INSTR(addressImplied, rts, 6, 0);

	// SBC
	INSTRUCTIONS[0xE9] = MAKE_INSTR(addressImmediate, sbc, 2, 0);
	INSTRUCTIONS[0xE5] = MAKE_INSTR(addressZP, sbc, 3, 0);
	INSTRUCTIONS[0xF5] = MAKE_INSTR(addressZPX, sbc, 4, 0);
	INSTRUCTIONS[0xED] = MAKE_INSTR(addressAbs, sbc, 4, 0);
	INSTRUCTIONS[0xFD] = MAKE_INSTR(addressAbsX, sbc, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0xF9] = MAKE_INSTR(addressAbsY, sbc, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
	INSTRUCTIONS[0xE1] = MAKE_INSTR(addressIX, sbc, 6, 0);
	INSTRUCTIONS[0xF1] = MAKE_INSTR(addressIY, sbc, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

	// SEC
	INSTRUCTIONS[0x38] = MAKE_INSTR(addressImplied, sec, 2, 0);

	// SED
	INSTRUCTIONS[0xF8] = MAKE_INSTR(addressImplied, sed, 2, 0);

	// SEI
	INSTRUCTIONS[0x78] = MAKE_INSTR(addressImplied, sei, 2, 0);

	// STA
	INSTRUCTIONS[0x85] = MAKE_INSTR(addressZP, sta, 3, 0);
	INSTRUCTIONS[0x95] = MAKE_INSTR(addressZPX, sta, 4, 0);
	INSTRUCTIONS[0x8D] = MAKE_INSTR(addressAbs, sta, 4, 0);
	INSTRUCTIONS[0x9D] = MAKE_INSTR(addressAbsX, sta, 5, 0);
	INSTRUCTIONS[0x99] = MAKE_INSTR(addressAbsY, sta, 5, 0);
	INSTRUCTIONS[0x81] = MAKE_INSTR(addressIX, sta, 6, 0);
	INSTRUCTIONS[0x91] = MAKE_INSTR(addressIY, sta, 6, 0);

	// STX
	INSTRUCTIONS[0x86] = MAKE_INSTR(addressZP, stx, 3, 0);
	INSTRUCTIONS[0x96] = MAKE_INSTR(addressZPY, stx, 4, 0);
	INSTRUCTIONS[0x8E] = MAKE_INSTR(addressAbs, stx, 4, 0);

	// STY
	INSTRUCTIONS[0x84] = MAKE_INSTR(addressZP, sty, 3, 0);
	INSTRUCTIONS[0x94] = MAKE_INSTR(addressZPX, sty, 4, 0);
	INSTRUCTIONS[0x8C] = MAKE_INSTR(addressAbs, sty, 4, 0);

	// TAX
	INSTRUCTIONS[0xAA] = MAKE_INSTR(addressImplied, tax, 2, 0);

	// TAY
	INSTRUCTIONS[0xA8] = MAKE_INSTR(addressImplied, tay, 2, 0);

	// TSX
	INSTRUCTIONS[0xBA] = MAKE_INSTR(addressImplied, tsx, 2, 0);

	// TXA
	INSTRUCTIONS[0x8A] = MAKE_INSTR(addressImplied, txa, 2, 0);

	// TXS
	INSTRUCTIONS[0x9A] = MAKE_INSTR(addressImplied, txs, 2, 0);

	// TYA
	INSTRUCTIONS[0x98] = MAKE_INSTR(addressImplied, tya, 2, 0);
}

static const uint16_t PC_BASE_ADDRESS = 0x0600;
static const uint16_t STACK_BASE_ADDRESS = 0x0100;
static const uint8_t STACK_MAX_VAL = 0xFF;

static const uint16_t PAGE_SIZE = 0xFF;

// Interrupt table:
static const uint16_t NMI_HANDLER_ADDR = 0xFFFA;
static const uint16_t RST_HANDLER_ADDR = 0xFFFC;
static const uint16_t BRK_HANDLER_ADDR = 0xFFFE;

CPU6502::CPU6502(IMemory* mem)
	: pc(PC_BASE_ADDRESS)
	, a(0)
	, x(0)
	, y(0)
	, stack(STACK_MAX_VAL)
	, cycles(0)
	, flags({ 0 })
	, memory(mem)
	, address(0)
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

inline static bool IsSamePage(uint16_t address1, uint16_t address2)
{
	// TODO: check
	return (address1 & 0xFF) == (address2 & 0xFF);
}

uint8_t CPU6502::read8()
{
	return memory->read(pc++);
}

uint16_t CPU6502::read16()
{
	uint8_t first = read8();
	uint8_t second = read8();
	return MakeAddress(first, second);
}

uint16_t CPU6502::read16_bug()
{
	// TODO: do
	uint8_t first = read8();
	uint8_t second = read8();
	return MakeAddress(first, second);
}

bool CPU6502::exec()
{
	uint8_t opcode = read8();
	const Instruction& cmd = INSTRUCTIONS[opcode];

	if (cmd.mFunc == nullptr) {
		// TODO: assert;
		return false;
	}

	if (cmd.mAddressFunc != nullptr)
	{
		(*this.*cmd.mAddressFunc)();
	}

	(*this.*cmd.mFunc)();

	cycles += cmd.mCycles;

	if (cmd.mFlags & Instruction::FLAG_CYCLE_PAGE_CROSSED)
	{

		// TODO: page crossed flag
	}
	
	// TODO: check?
	return !flags.brk;
}

// Memory addressation modes:
void CPU6502::addressImplied()
{
	address = 0;
	// Do nothing
}

void CPU6502::addressImmediate()
{
	address = pc++;
}

void CPU6502::addressRelative()
{
	// Relative offset is stored in address here...
	address = read8();
}

void CPU6502::addressZP()
{
	addressZPWOffs(0);
}

void CPU6502::addressZPX()
{
	addressZPWOffs(x);
}

void CPU6502::addressZPY()
{
	addressZPWOffs(y);
}

void CPU6502::addressZPWOffs(uint8_t offset)
{
	uint8_t value = read8() + offset;
	address = value;
}

void CPU6502::addressAbs()
{
	addressAbsWOfs(0);
}

void CPU6502::addressAbsX()
{
	addressAbsWOfs(x);
}

void CPU6502::addressAbsY()
{
	addressAbsWOfs(y);
}

void CPU6502::addressAbsWOfs(uint8_t offset)
{
	address = read16() + offset;
}

void CPU6502::addressIX()
{
	// TODO: check if have bug
	addressIWOffs(x);
}

void CPU6502::addressIY()
{
	// TODO: check if have bug
	addressIWOffs(y);
}

void CPU6502::addressI()
{
	addressIWOffs(0);
}

void CPU6502::addressIWOffs(uint8_t offset)
{
	uint16_t addr = read16();
	// TODO: bug simulator
	uint8_t val_first = memory->read(addr);
	uint8_t val_second = memory->read(addr + 1) + offset; // TODO: check
	address = MakeAddress(val_first, val_second);
}

void CPU6502::updateZeroNegativeFlags(uint8_t value)
{
	flags.zero = value ? 0 : 1;
	flags.negative = (value & 0x80) ? 1 : 0;
}

void CPU6502::stackPush8(uint8_t value)
{
	assert(stack > 0);
	memory->write(STACK_BASE_ADDRESS + stack--, value);
}

void CPU6502::stackPush16(uint16_t value)
{
	stackPush8(value & 0xFF);
	stackPush8((value >> 8) & 0xFF); // TODO: check
}

uint8_t CPU6502::stackPop8()
{
	assert(stack < STACK_MAX_VAL);
	return memory->read(STACK_BASE_ADDRESS + ++stack);
}

uint16_t CPU6502::stackPop16()
{
	uint8_t second = stackPop8();
	uint8_t first = stackPop8();
	return MakeAddress(first, second); // TODO: check
}

// Load / Store Operations
void CPU6502::lda()
{
	a = memory->read(address);
	updateZeroNegativeFlags(a);
}

void CPU6502::ldx()
{
	x = memory->read(address);
	updateZeroNegativeFlags(x);
}

void CPU6502::ldy()
{
	y = memory->read(address);
	updateZeroNegativeFlags(y);
}

void CPU6502::sta()
{
	memory->write(address, a);
}

void CPU6502::stx()
{
	memory->write(address, x);
}

void CPU6502::sty()
{
	memory->write(address, y);
}

// Register Transfers
void CPU6502::tax()
{
	x = a;
	updateZeroNegativeFlags(x);
}

void CPU6502::tay()
{
	x = a;
	updateZeroNegativeFlags(y);
}

void CPU6502::txa()
{
	a = x;
	updateZeroNegativeFlags(a);
}

void CPU6502::tya()
{
	a = y;
	updateZeroNegativeFlags(a);
}

// Stack Operations
void CPU6502::tsx()
{
	x = stack;
	updateZeroNegativeFlags(x);
}

void CPU6502::txs()
{
	stack = x;
}

void CPU6502::pha()
{
	stackPush8(a);
}

void CPU6502::php()
{
	stackPush8(flags.values);
}

void CPU6502::pla()
{
	a = stackPop8();
	updateZeroNegativeFlags(a);
}

void CPU6502::plp()
{
	flags.values = stackPop8();
}

// Logical
void CPU6502::and()
{
	a &= memory->read(address);
	updateZeroNegativeFlags(a);
}

void CPU6502::eor()
{
	a ^= memory->read(address);
	updateZeroNegativeFlags(a);
}

void CPU6502::ora()
{
	a |= memory->read(address);
	updateZeroNegativeFlags(a);
}

void CPU6502::bit()
{
	uint8_t res = a & memory->read(address);
	flags.negative = res & (1 << 7) ? 1 : 0;
	flags.overflow = res & (1 << 6) ? 1 : 0;
	flags.zero = res ? 0 : 1;
}

static uint8_t CalcOverflow(uint8_t a, uint8_t b, uint8_t res)
{
	return (a ^ res) & (b ^ res) & 0x80;
}

// Arithmetic
void CPU6502::adc()
{
	uint8_t src = memory->read(address);
	uint16_t res = static_cast<uint16_t>(a) + src + flags.carry;
	flags.carry = ((res & 0xFF00) > 0) ? 1 : 0;
	flags.overflow = CalcOverflow(a, src, res);
	a = static_cast<uint8_t>(res);
	updateZeroNegativeFlags(a);
}

void CPU6502::sbc()
{
	uint8_t src = memory->read(address);
	uint16_t res = a - static_cast<uint16_t>(a) - src - ~flags.carry;
	flags.carry = ((res & 0xFF00) > 0) ? 1 : 0;
	flags.overflow = CalcOverflow(a, src, res);
	a = static_cast<uint8_t>(res);
	updateZeroNegativeFlags(a);
}

void CPU6502::cmp()
{
	cmpWVal(y);
}

void CPU6502::cpx()
{
	cmpWVal(x);
}

void CPU6502::cpy()
{
	cmpWVal(y);
}

void CPU6502::cmpWVal(uint8_t value)
{
	uint8_t res = value - memory->read(address);
	flags.carry = (res >= 0) ? 1 : 0;
	updateZeroNegativeFlags(res);
}

// Increments & Decrements
void CPU6502::inc()
{
	uint8_t dst = memory->read(address) + 1;
	updateZeroNegativeFlags(dst);
	memory->write(address, dst);
}

void CPU6502::inx()
{
	++x;
	updateZeroNegativeFlags(x);
}

void CPU6502::iny()
{
	++y;
	updateZeroNegativeFlags(y);
}

void CPU6502::dec()
{
	uint8_t dst = memory->read(address) - 1;
	updateZeroNegativeFlags(dst);
	memory->write(address, dst);
}

void CPU6502::dex()
{
	--x;
	updateZeroNegativeFlags(x);
}

void CPU6502::dey()
{
	--y;
	updateZeroNegativeFlags(y);
}

// Shifts
void CPU6502::asl_m()
{
	uint8_t value = memory->read(address);
	memory->write(address, asl(value));
}

void CPU6502::asl_a()
{
	a = asl(a);
}

uint8_t CPU6502::asl(uint8_t value)
{
	flags.carry = value & 0x80;
	value <<= 1;
	updateZeroNegativeFlags(value);
	return value;
}

void CPU6502::lsr_m()
{
	uint8_t value = memory->read(address);
	memory->write(address, lsr(value));
}

void CPU6502::lsr_a()
{
	a = lsr(a);
}

uint8_t CPU6502::lsr(uint8_t value)
{
	flags.carry = value & 0x1;
	value >>= 1;
	updateZeroNegativeFlags(value);
	return value;
}

uint8_t CPU6502::rol(uint8_t value)
{
	uint8_t dst = value;
	dst <<= 1;
	// TODO: set bit 0 to carry
	flags.carry = value & 0x80;
	updateZeroNegativeFlags(dst);
	return dst;
}

void CPU6502::rol_m()
{
	uint8_t value = memory->read(address);
	memory->write(address, rol(value));
}

void CPU6502::rol_a()
{
	a = rol(a);
}

uint8_t CPU6502::ror(uint8_t value)
{
	uint8_t dst = value;
	dst >>= 1;
	// TODO: set bit 0 to carry
	flags.carry = value & 0x1;
	updateZeroNegativeFlags(dst);
	return dst;
}

void CPU6502::ror_m()
{
	uint8_t value = memory->read(address);
	memory->write(address, ror(value));
}

void CPU6502::ror_a()
{
	a = ror(a);
}

// Jumps & Calls
void CPU6502::jmp()
{
	pc = address;
}

void CPU6502::jsr()
{
	uint16_t ret = pc - 1;
	stackPush16(ret);
	pc = address;
}

void CPU6502::rts()
{
	pc = stackPop16() + 1;
}

void CPU6502::branchRelative()
{
	uint16_t oldPc = pc;
	int8_t val = static_cast<int8_t>(address & 0xFF); // TODO: check
	pc += val;
	// TODO: location check?
	cycles += 1;
	if (!IsSamePage(oldPc, pc))
		cycles += 2;
}

// Branches
void CPU6502::bcc()
{
	if (!flags.carry)
	{
		branchRelative();
	}
}

void CPU6502::bcs()
{
	if (flags.carry)
	{
		branchRelative();
	}
}

void CPU6502::beq()
{
	if (flags.zero)
	{
		branchRelative();
	}
}

void CPU6502::bmi()
{
	if (flags.negative)
	{
		branchRelative();
	}
}

void CPU6502::bne()
{
	if (!flags.zero)
	{
		branchRelative();
	}
}

void CPU6502::bpl()
{
	if (!flags.negative)
	{
		branchRelative();
	}
}

void CPU6502::bvc()
{
	if (!flags.overflow)
	{
		branchRelative();
	}
}

void CPU6502::bvs()
{
	if (flags.overflow)
	{
		branchRelative();
	}
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
	if (!flags.interrupt_disable)
	{
		interrupt(BRK_HANDLER_ADDR);
	}
}

void CPU6502::interrupt(uint16_t address)
{
	stackPush16(pc);
	stackPush8(flags.values);
	uint8_t byte1 = memory->read(address); // TODO: read16 method?
	uint8_t byte2 = memory->read(address + 1);
	pc = MakeAddress(byte1, byte2);
	flags.brk = 1;
}

void CPU6502::nop()
{
	// Do nothing
}

void CPU6502::rti()
{
	flags.values = stackPop8();
	pc = stackPop16();
}
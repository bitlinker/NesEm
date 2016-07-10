#include "CPU6502.h"
#include <cstring>
#include <cassert>

typedef void (CPU6502::*pCpuFunc)();

enum AddressMode
{

};

static const uint32_t MAX_INSTR_NAME = 8;
class Instruction
{
public:
	static const uint32_t FLAG_CYCLE_PAGE_CROSSED = 1;
    static const uint32_t FLAG_UNDOCUMENTED = 2;

public:
	Instruction()
		: mAddressFunc(nullptr)
		, mFunc(nullptr)
		, mName("empty")
		, mCycles(0)
		, mFlags(0)
        , mBytes(0)
	{
	}

	Instruction(pCpuFunc addressFunc, pCpuFunc cpuFunc, const char* name, uint32_t cycles, uint32_t flags)
		: mAddressFunc(addressFunc)
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
    uint32_t mBytes;
};
#define MAKE_INSTR(addressFunc, func, cycles, flags) Instruction(&CPU6502::##addressFunc, &CPU6502::##func, #func, cycles, flags)


static const uint32_t MAX_INSTRUCTIONS = 0xFF;
static Instruction INSTRUCTIONS[MAX_INSTRUCTIONS];

static void InitInstructionTable()
{
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
	INSTRUCTIONS[0xB1] = MAKE_INSTR(addressIY, lda, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

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

    // Undocumented instructions:
    // AAC (ANC)
    INSTRUCTIONS[0x0B] = MAKE_INSTR(addressImmediate, aac, 2, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x2B] = MAKE_INSTR(addressImmediate, aac, 2, Instruction::FLAG_UNDOCUMENTED);

    // SAX
    INSTRUCTIONS[0x87] = MAKE_INSTR(addressZP, sax, 3, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x97] = MAKE_INSTR(addressZPY, sax, 4, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x83] = MAKE_INSTR(addressIX, sax, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x8F] = MAKE_INSTR(addressAbs, sax, 4, Instruction::FLAG_UNDOCUMENTED);

    // ARR
    INSTRUCTIONS[0x6B] = MAKE_INSTR(addressImmediate, arr, 2, Instruction::FLAG_UNDOCUMENTED);

    // ASR (ALR)
    INSTRUCTIONS[0x4B] = MAKE_INSTR(addressImmediate, asr, 2, Instruction::FLAG_UNDOCUMENTED);

    // ATX
    INSTRUCTIONS[0xAB] = MAKE_INSTR(addressImmediate, atx, 2, Instruction::FLAG_UNDOCUMENTED);

    // AXA
    INSTRUCTIONS[0x9F] = MAKE_INSTR(addressAbsY, axa, 5, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x93] = MAKE_INSTR(addressIY, axa, 6, Instruction::FLAG_UNDOCUMENTED);

    // AXS
    INSTRUCTIONS[0xCB] = MAKE_INSTR(addressImmediate, axs, 2, Instruction::FLAG_UNDOCUMENTED);

    // DCP
    INSTRUCTIONS[0xC7] = MAKE_INSTR(addressZP, dcp, 5, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xD7] = MAKE_INSTR(addressZPX, dcp, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xCF] = MAKE_INSTR(addressAbs, dcp, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xDF] = MAKE_INSTR(addressAbsX, dcp, 7, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xDB] = MAKE_INSTR(addressAbsY, dcp, 7, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xC3] = MAKE_INSTR(addressIX, dcp, 8, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xD3] = MAKE_INSTR(addressIY, dcp, 8, Instruction::FLAG_UNDOCUMENTED);

    // DOP
    INSTRUCTIONS[0x04] = MAKE_INSTR(addressZP, nop, 3, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x14] = MAKE_INSTR(addressZPX, nop, 4, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x34] = MAKE_INSTR(addressZPX, nop, 4, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x44] = MAKE_INSTR(addressZP, nop, 3, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x54] = MAKE_INSTR(addressZPX, nop, 4, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x64] = MAKE_INSTR(addressZP, nop, 3, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x74] = MAKE_INSTR(addressZPX, nop, 4, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x80] = MAKE_INSTR(addressImmediate, nop, 2, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x82] = MAKE_INSTR(addressImmediate, nop, 2, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x89] = MAKE_INSTR(addressImmediate, nop, 2, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xC2] = MAKE_INSTR(addressImmediate, nop, 2, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xD4] = MAKE_INSTR(addressZPX, nop, 4, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xE2] = MAKE_INSTR(addressImmediate, nop, 2, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xF4] = MAKE_INSTR(addressZPX, nop, 4, Instruction::FLAG_UNDOCUMENTED);

    // ISC
    INSTRUCTIONS[0xE7] = MAKE_INSTR(addressZP, isc, 5, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xF7] = MAKE_INSTR(addressZPX, isc, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xEF] = MAKE_INSTR(addressAbs, isc, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xFF] = MAKE_INSTR(addressAbsX, isc, 7, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xFB] = MAKE_INSTR(addressAbsY, isc, 7, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xE3] = MAKE_INSTR(addressIX, isc, 8, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xF3] = MAKE_INSTR(addressIY, isc, 8, Instruction::FLAG_UNDOCUMENTED);

    // KIL
    INSTRUCTIONS[0x02] = MAKE_INSTR(addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x12] = MAKE_INSTR(addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x22] = MAKE_INSTR(addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x32] = MAKE_INSTR(addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x42] = MAKE_INSTR(addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x52] = MAKE_INSTR(addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x62] = MAKE_INSTR(addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x72] = MAKE_INSTR(addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x92] = MAKE_INSTR(addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xB2] = MAKE_INSTR(addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xD2] = MAKE_INSTR(addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xF2] = MAKE_INSTR(addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);

    // LAR
    INSTRUCTIONS[0xBB] = MAKE_INSTR(addressAbsY, lar, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);

    // LAX
    INSTRUCTIONS[0xA7] = MAKE_INSTR(addressZP, lax, 3, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xB7] = MAKE_INSTR(addressZPY, lax, 4, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xAF] = MAKE_INSTR(addressAbs, lax, 4, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xBF] = MAKE_INSTR(addressAbsY, lax, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);
    INSTRUCTIONS[0xA3] = MAKE_INSTR(addressIX, lax, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xB3] = MAKE_INSTR(addressIY, lax, 5, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);

    // NOP
    INSTRUCTIONS[0x1A] = MAKE_INSTR(addressImplied, nop, 2, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x3A] = MAKE_INSTR(addressImplied, nop, 2, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x5A] = MAKE_INSTR(addressImplied, nop, 2, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x7A] = MAKE_INSTR(addressImplied, nop, 2, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xDA] = MAKE_INSTR(addressImplied, nop, 2, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0xFA] = MAKE_INSTR(addressImplied, nop, 2, Instruction::FLAG_UNDOCUMENTED);

    // RLA
    INSTRUCTIONS[0x27] = MAKE_INSTR(addressZP, rla, 5, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x37] = MAKE_INSTR(addressZPX, rla, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x2F] = MAKE_INSTR(addressAbs, rla, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x3F] = MAKE_INSTR(addressAbsX, rla, 7, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x3B] = MAKE_INSTR(addressAbsY, rla, 7, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x23] = MAKE_INSTR(addressIX, rla, 8, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x33] = MAKE_INSTR(addressIY, rla, 8, Instruction::FLAG_UNDOCUMENTED);

    // RRA
    INSTRUCTIONS[0x67] = MAKE_INSTR(addressZP, rra, 5, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x77] = MAKE_INSTR(addressZPX, rra, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x6F] = MAKE_INSTR(addressAbs, rra, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x7F] = MAKE_INSTR(addressAbsX, rra, 7, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x7B] = MAKE_INSTR(addressAbsY, rra, 7, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x63] = MAKE_INSTR(addressIX, rra, 8, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x73] = MAKE_INSTR(addressIY, rra, 8, Instruction::FLAG_UNDOCUMENTED);

    // SBC
    INSTRUCTIONS[0xEB] = MAKE_INSTR(addressImmediate, sbc, 2, Instruction::FLAG_UNDOCUMENTED);

    // SLO
    INSTRUCTIONS[0x07] = MAKE_INSTR(addressZP, slo, 5, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x17] = MAKE_INSTR(addressZPX, slo, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x0F] = MAKE_INSTR(addressAbs, slo, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x1F] = MAKE_INSTR(addressAbsX, slo, 7, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x1B] = MAKE_INSTR(addressAbsY, slo, 7, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x03] = MAKE_INSTR(addressIX, slo, 8, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x13] = MAKE_INSTR(addressIY, slo, 8, Instruction::FLAG_UNDOCUMENTED);

    // SRE
    INSTRUCTIONS[0x47] = MAKE_INSTR(addressZP, sre, 5, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x57] = MAKE_INSTR(addressZPX, sre, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x4F] = MAKE_INSTR(addressAbs, sre, 6, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x5F] = MAKE_INSTR(addressAbsX, sre, 7, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x5B] = MAKE_INSTR(addressAbsY, sre, 7, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x43] = MAKE_INSTR(addressIX, sre, 8, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x53] = MAKE_INSTR(addressIY, sre, 8, Instruction::FLAG_UNDOCUMENTED);

    // SXA
    INSTRUCTIONS[0x9E] = MAKE_INSTR(addressAbsY, sxa, 5, Instruction::FLAG_UNDOCUMENTED);

    // SYA
    INSTRUCTIONS[0x9C] = MAKE_INSTR(addressAbsX, sya, 5, Instruction::FLAG_UNDOCUMENTED);

    // TOP
    // TODO: simple nop?
    INSTRUCTIONS[0x0C] = MAKE_INSTR(addressAbs, nop, 4, Instruction::FLAG_UNDOCUMENTED);
    INSTRUCTIONS[0x1C] = MAKE_INSTR(addressAbsX, nop, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);
    INSTRUCTIONS[0x3C] = MAKE_INSTR(addressAbsX, nop, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);
    INSTRUCTIONS[0x5C] = MAKE_INSTR(addressAbsX, nop, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);
    INSTRUCTIONS[0x7C] = MAKE_INSTR(addressAbsX, nop, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);
    INSTRUCTIONS[0xDC] = MAKE_INSTR(addressAbsX, nop, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);
    INSTRUCTIONS[0xFC] = MAKE_INSTR(addressAbsX, nop, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);

    // XAA
    INSTRUCTIONS[0x8B] = MAKE_INSTR(addressImmediate, xaa, 2, Instruction::FLAG_UNDOCUMENTED);

    // XAS
    INSTRUCTIONS[0x9B] = MAKE_INSTR(addressAbsY, xas, 5, Instruction::FLAG_UNDOCUMENTED);
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
    , mInstruction(nullptr)
    , mCmdCnt(0)
{
	// TODO: init static?
	InitInstructionTable();

    powerOn();
}

CPU6502::~CPU6502()
{
}

inline static uint16_t MakeAddress(uint8_t first, uint8_t second)
{
	return (static_cast<uint16_t>(second) << 8) | static_cast<uint16_t>(first);
}

inline static bool IsPageCrossed(uint16_t address1, uint16_t address2)
{
	return (address1 & 0xFF00) != (address2 & 0xFF00);
}

static uint8_t FlagsFromStack(uint8_t value)
{
    return value & 0xEF | 0x20;
}

static uint8_t FlagsToStack(uint8_t value)
{
    return value | 0x10;
}


void CPU6502::powerOn()
{
    // TODO: check
    a = 0;
    x = 0;
    y = 0;
    flags.values = 0x24;
    stack = 0xFD;
    cycles = 0;
}

void CPU6502::reset()
{
    stack -= 3;
}

// TODO: memory reader class?
uint8_t CPU6502::read8(uint16_t address)
{
	return memory->read(address);
}

uint16_t CPU6502::read16(uint16_t address)
{
	uint8_t first = read8(address);
	uint8_t second = read8(address + 1);
	return MakeAddress(first, second);
}

uint16_t CPU6502::read16_bug(uint16_t address)
{
	uint16_t first = address;
    uint8_t lowPart = static_cast<uint8_t>(first) + 1;
	uint16_t second = (address & 0xFF00) | lowPart;
	uint8_t first_val = memory->read(first);
	uint8_t second_val = memory->read(second);
	return MakeAddress(first_val, second_val);
}

bool CPU6502::exec()
{
	uint8_t opcode = read8(pc++);
    mInstruction = &INSTRUCTIONS[opcode];

	if (mInstruction->mFunc == nullptr) {
		// TODO: assert;
        assert(false && "Unknown instruction");
		return false;
	}

    mIsPageCrossed = false;
	if (mInstruction->mAddressFunc != nullptr)
	{
		(*this.*mInstruction->mAddressFunc)();
	}

	(*this.*mInstruction->mFunc)();

    // Update cycles
	cycles += mInstruction->mCycles;
	if (mInstruction->mFlags & Instruction::FLAG_CYCLE_PAGE_CROSSED && mIsPageCrossed) // TODO: in address func?
	{
        ++cycles;
	}

	// TODO: return cycles?
    ++mCmdCnt;
	return true;
}

void CPU6502::setPC(uint16_t address)
{
    pc = address;
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
	address = read8(pc++);
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
	uint8_t value = read8(pc++) + offset;
	address = value;
}

void CPU6502::addressAbs()
{
	addressAbsWOfs(0);
}

void CPU6502::addressAbsX()
{
	mIsPageCrossed = addressAbsWOfs(x);
}

void CPU6502::addressAbsY()
{
    mIsPageCrossed = addressAbsWOfs(y);
}

bool CPU6502::addressAbsWOfs(uint8_t offset)
{
    uint16_t baseAddress = read16(pc);
    pc += 2;
	address = baseAddress + offset;
    return IsPageCrossed(address, baseAddress);
}

void CPU6502::addressIX()
{
	addressIWOffs(x, 0);
}

void CPU6502::addressIY()
{
    mIsPageCrossed = addressIWOffs(0, y);
}

void CPU6502::addressI()
{
    uint16_t baseAddress = read16(pc);
    pc += 2;
    address = read16_bug(baseAddress);
}

bool CPU6502::addressIWOffs(uint8_t x, uint8_t y)
{
    uint8_t indirectionAddr = read8(pc++) + x;
    uint16_t baseAddress = read16_bug(indirectionAddr);
    address = baseAddress + static_cast<uint16_t>(y);
    return IsPageCrossed(baseAddress, address);
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
    uint8_t valLow = value & 0xFF;
    uint8_t valHi = (value >> 8) & 0xFF;	
	stackPush8(valHi);
    stackPush8(valLow);
}

uint8_t CPU6502::stackPop8()
{
	assert(stack < STACK_MAX_VAL);
	return memory->read(STACK_BASE_ADDRESS + ++stack);
}

uint16_t CPU6502::stackPop16()
{	
	uint8_t valLow = stackPop8();
    uint8_t valHigh = stackPop8();
	return MakeAddress(valLow, valHigh);
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
	y = a;
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
	stackPush8(FlagsToStack(flags.values));
}

void CPU6502::pla()
{
	a = stackPop8();
	updateZeroNegativeFlags(a);
}

void CPU6502::plp()
{
    flags.values = FlagsFromStack(stackPop8());
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
	uint8_t value = memory->read(address);
	flags.negative = value & (1 << 7) ? 1 : 0; // TODO: optimize flag sets
	flags.overflow = value & (1 << 6) ? 1 : 0;
	flags.zero = (value & a) == 0 ? 1 : 0;
}

// TODO: eliminate
static uint8_t CalcOverflowADC(uint8_t a, uint8_t b, uint8_t res)
{
    return (((a^b) & 0x80) == 0 && ((a^res) & 0x80) != 0) ? 1 : 0;
}

static uint8_t CalcOverflowSBC(uint8_t a, uint8_t b, uint8_t res)
{
    return (((a^b) & 0x80) != 0 && ((a^res) & 0x80) != 0) ? 1 : 0;
}

// Arithmetic
void CPU6502::adc()
{
	uint8_t src = memory->read(address);
    int16_t res = static_cast<int16_t>(a) + src + flags.carry;
    flags.carry = res > 0xFF ? 1 : 0;
	flags.overflow = CalcOverflowADC(a, src, res);
    a = static_cast<uint8_t>(res);
    updateZeroNegativeFlags(a);
}

void CPU6502::sbc()
{
	uint8_t src = memory->read(address);
	int16_t res = static_cast<int16_t>(a) - src - (1 - flags.carry);
	flags.carry = res >= 0 ? 1 : 0;
    flags.overflow = CalcOverflowSBC(a, src, res);
    a = static_cast<uint8_t>(res);
    updateZeroNegativeFlags(a);
}

void CPU6502::cmp()
{
	cmpWVal(a);
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
	uint8_t res = value;
    uint8_t src = memory->read(address);
	flags.carry = (value >= src) ? 1 : 0;
	updateZeroNegativeFlags(value - src);
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
	flags.carry = (value & 0x80) > 0 ? 1 : 0;
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
	uint8_t dst = (value << 1) | flags.carry;	
	flags.carry = ((value >> 7) & 0x1) > 0 ? 1 : 0;
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
    dst |= (flags.carry << 7);
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
	if (IsPageCrossed(oldPc, pc))
		cycles += 1;
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
	stackPush8(FlagsToStack(flags.values));
    pc = read16(address);
	flags.brk = 1;
}

void CPU6502::nop()
{
	// Do nothing
}

void CPU6502::rti()
{
    flags.values = FlagsFromStack(stackPop8());
	pc = stackPop16();
}

// Undocumented instructions:
void CPU6502::aac()
{
    // TODO: not tested
    //uint8_t value = memory->read(address) & a;
    //updateZeroNegativeFlags(value);
    //flags.carry = flags.negative;
}

void CPU6502::sax()
{  
    uint8_t value = a & x;
    memory->write(address, value);
}

void CPU6502::arr()
{
    // TODO: not tested
    // TODO: eliminate dup read
    //and();
    //ror_a();
    //// TODO: except sets the flags differently. N and Z are normal, but C is bit 6 and V is bit 6 xor bit 5.
}

void CPU6502::asr()
{
    // TODO: not tested
    // TODO: eliminate dup read
    /*and();
    lsr_a();*/
}

void CPU6502::atx()
{
    // TODO: not tested
    /*a &= memory->read(address);
    x = a;
    updateZeroNegativeFlags(a);*/
}

void CPU6502::axa()
{
    // TODO: not tested
    /*uint8_t value = (a & x) & 7;
    memory->write(address, value);*/
}

void CPU6502::axs()
{
    // TODO: not tested
    //x &= a;
    //x -= memory->read(address);
    //updateZeroNegativeFlags(x);
}

void CPU6502::dcp()
{
    // TODO: eliminate dup read
    dec();
    cmp();
}

void CPU6502::isc()
{
    // TODO: eliminate dup read
    inc();
    sbc();
}

void CPU6502::kil()
{
    // TODO: not tested
    // TODO: lock up
}

void CPU6502::lar()
{
    // TODO: not tested
    //uint8_t value = memory->read(address) & stack;
    //x = a = stack = value;
    //updateZeroNegativeFlags(value);
}

void CPU6502::lax()
{
    uint8_t value = memory->read(address);
    x = a = value;
    updateZeroNegativeFlags(value);
}

void CPU6502::rla()
{
    // TODO: eliminate dup read
    rol_m();
    and();
}

void CPU6502::rra()
{
    // TODO: eliminate dup read
    ror_m();
    adc();
}

void CPU6502::slo()
{
    // TODO: eliminate dup read
    asl_m();
    ora();
}

void CPU6502::sre()
{
    // TODO: eliminate dup read
    lsr_m();
    eor();
}

void CPU6502::sxa()
{
    // TODO: not tested
}

void CPU6502::sya()
{
    // TODO: not tested
}

void CPU6502::xaa()
{
    // TODO: not tested
}

void CPU6502::xas()
{
    // TODO: not tested
}
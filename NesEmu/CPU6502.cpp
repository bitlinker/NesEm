#include "CPU6502.h"
#include <string>
#include <cassert>

typedef void (CPU6502::*pCpuFunc)();

class Instruction
{
public:
	static const uint32_t FLAG_CYCLE_PAGE_CROSSED = 1;
    static const uint32_t FLAG_UNDOCUMENTED = 2;

    enum AddressMode
    {
        addressImplied,
        addressImmediate,
        addressRelative,
        addressZP,
        addressZPX,
        addressZPY,
        addressAbs,
        addressAbsX,
        addressAbsY,
        addressIX,
        addressIY,
        addressI,
        addressMAX,
    };

public:
	Instruction()
		: mAddressMode(addressImplied)
		, mFunc(nullptr)
		, mName("empty")
		, mCycles(0)
		, mFlags(0)
	{
	}

	Instruction(AddressMode addressMode, pCpuFunc cpuFunc, const char* name, uint32_t cycles, uint32_t flags)
		: mAddressMode(addressMode)
		, mFunc(cpuFunc)
        , mName(name)
        , mCycles(cycles)
		, mFlags(flags)
	{
	}

    // TODO: get number of bytes
public:
    AddressMode mAddressMode;
	pCpuFunc mFunc;
	std::string mName;
	uint32_t mCycles;
	uint32_t mFlags;
};
#define MAKE_INSTR(addressMode, func, cycles, flags) Instruction(addressMode, &CPU6502::##func, #func, cycles, flags)

class InstructionTable
{
public:
    static InstructionTable& getInstance()
    {
        static InstructionTable mInstance;
        return mInstance;
    }

    const Instruction& getInstruction(uint8_t opcode)
    {
        return mInstructions[opcode];
    }

    pCpuFunc getAddressFunc(Instruction::AddressMode mode)
    {
        return mAddressFuncs[mode];
    }

private:
    Instruction mInstructions[0x100];
    pCpuFunc mAddressFuncs[Instruction::addressMAX];

private:
    InstructionTable()
    {
        // Address mode functions
        mAddressFuncs[Instruction::addressImplied] = &CPU6502::addressImplied;
        mAddressFuncs[Instruction::addressImmediate] = &CPU6502::addressImmediate;
        mAddressFuncs[Instruction::addressRelative] = &CPU6502::addressRelative;
        mAddressFuncs[Instruction::addressZP] = &CPU6502::addressZP;
        mAddressFuncs[Instruction::addressZPX] = &CPU6502::addressZPX;
        mAddressFuncs[Instruction::addressZPY] = &CPU6502::addressZPY;
        mAddressFuncs[Instruction::addressAbs] = &CPU6502::addressAbs;
        mAddressFuncs[Instruction::addressAbsX] = &CPU6502::addressAbsX;
        mAddressFuncs[Instruction::addressAbsY] = &CPU6502::addressAbsY;
        mAddressFuncs[Instruction::addressIX] = &CPU6502::addressIX;
        mAddressFuncs[Instruction::addressIY] = &CPU6502::addressIY;
        mAddressFuncs[Instruction::addressI] = &CPU6502::addressI;

        // ADC
        mInstructions[0x69] = MAKE_INSTR(Instruction::addressImmediate, adc, 2, 0);
        mInstructions[0x65] = MAKE_INSTR(Instruction::addressZP, adc, 3, 0);
        mInstructions[0x75] = MAKE_INSTR(Instruction::addressZPX, adc, 4, 0);
        mInstructions[0x6D] = MAKE_INSTR(Instruction::addressAbs, adc, 4, 0);
        mInstructions[0x7D] = MAKE_INSTR(Instruction::addressAbsX, adc, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0x79] = MAKE_INSTR(Instruction::addressAbsY, adc, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0x61] = MAKE_INSTR(Instruction::addressIX, adc, 6, 0);
        mInstructions[0x71] = MAKE_INSTR(Instruction::addressIY, adc, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

        // AND
        mInstructions[0x29] = MAKE_INSTR(Instruction::addressImmediate, and, 2, 0);
        mInstructions[0x25] = MAKE_INSTR(Instruction::addressZP, and, 3, 0);
        mInstructions[0x35] = MAKE_INSTR(Instruction::addressZPX, and, 4, 0);
        mInstructions[0x2D] = MAKE_INSTR(Instruction::addressAbs, and, 4, 0);
        mInstructions[0x3D] = MAKE_INSTR(Instruction::addressAbsX, and, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0x39] = MAKE_INSTR(Instruction::addressAbsY, and, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0x21] = MAKE_INSTR(Instruction::addressIX, and, 6, 0);
        mInstructions[0x31] = MAKE_INSTR(Instruction::addressIY, and, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

        // ASL
        mInstructions[0x0A] = MAKE_INSTR(Instruction::addressImplied, asl_a, 2, 0);
        mInstructions[0x06] = MAKE_INSTR(Instruction::addressZP, asl_m, 5, 0);
        mInstructions[0x16] = MAKE_INSTR(Instruction::addressZPX, asl_m, 6, 0);
        mInstructions[0x0E] = MAKE_INSTR(Instruction::addressAbs, asl_m, 6, 0);
        mInstructions[0x1E] = MAKE_INSTR(Instruction::addressAbsX, asl_m, 7, 0);

        // BCC
        mInstructions[0x90] = MAKE_INSTR(Instruction::addressRelative, bcc, 2, 0);

        // BCS
        mInstructions[0xB0] = MAKE_INSTR(Instruction::addressRelative, bcs, 2, 0);

        // BEQ
        mInstructions[0xF0] = MAKE_INSTR(Instruction::addressRelative, beq, 2, 0);

        // BIT
        mInstructions[0x24] = MAKE_INSTR(Instruction::addressZP, bit, 3, 0);
        mInstructions[0x2C] = MAKE_INSTR(Instruction::addressAbs, bit, 4, 0);

        // BMI
        mInstructions[0x30] = MAKE_INSTR(Instruction::addressRelative, bmi, 2, 0);

        // BNE
        mInstructions[0xD0] = MAKE_INSTR(Instruction::addressRelative, bne, 2, 0);

        // BPL
        mInstructions[0x10] = MAKE_INSTR(Instruction::addressRelative, bpl, 2, 0);

        // BRK
        mInstructions[0x00] = MAKE_INSTR(Instruction::addressImplied, brk, 7, 0);

        // BVC
        mInstructions[0x50] = MAKE_INSTR(Instruction::addressRelative, bvc, 2, 0);

        // BVS
        mInstructions[0x70] = MAKE_INSTR(Instruction::addressRelative, bvs, 2, 0);

        // CLC
        mInstructions[0x18] = MAKE_INSTR(Instruction::addressImplied, clc, 2, 0);

        // CLD
        mInstructions[0xD8] = MAKE_INSTR(Instruction::addressImplied, cld, 2, 0);

        // CLI
        mInstructions[0x58] = MAKE_INSTR(Instruction::addressImplied, cli, 2, 0);

        // CLV
        mInstructions[0xB8] = MAKE_INSTR(Instruction::addressImplied, clv, 2, 0);

        // CMP
        mInstructions[0xC9] = MAKE_INSTR(Instruction::addressImmediate, cmp, 2, 0);
        mInstructions[0xC5] = MAKE_INSTR(Instruction::addressZP, cmp, 3, 0);
        mInstructions[0xD5] = MAKE_INSTR(Instruction::addressZPX, cmp, 4, 0);
        mInstructions[0xCD] = MAKE_INSTR(Instruction::addressAbs, cmp, 4, 0);
        mInstructions[0xDD] = MAKE_INSTR(Instruction::addressAbsX, cmp, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0xD9] = MAKE_INSTR(Instruction::addressAbsY, cmp, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0xC1] = MAKE_INSTR(Instruction::addressIX, cmp, 6, 0);
        mInstructions[0xD1] = MAKE_INSTR(Instruction::addressIY, cmp, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

        // CPX
        mInstructions[0xE0] = MAKE_INSTR(Instruction::addressImmediate, cpx, 2, 0);
        mInstructions[0xE4] = MAKE_INSTR(Instruction::addressZP, cpx, 3, 0);
        mInstructions[0xEC] = MAKE_INSTR(Instruction::addressAbs, cpx, 4, 0);

        // CPY
        mInstructions[0xC0] = MAKE_INSTR(Instruction::addressImmediate, cpy, 2, 0);
        mInstructions[0xC4] = MAKE_INSTR(Instruction::addressZP, cpy, 3, 0);
        mInstructions[0xCC] = MAKE_INSTR(Instruction::addressAbs, cpy, 4, 0);

        // DEC
        mInstructions[0xC6] = MAKE_INSTR(Instruction::addressZP, dec, 5, 0);
        mInstructions[0xD6] = MAKE_INSTR(Instruction::addressZPX, dec, 6, 0);
        mInstructions[0xCE] = MAKE_INSTR(Instruction::addressAbs, dec, 6, 0);
        mInstructions[0xDE] = MAKE_INSTR(Instruction::addressAbsX, dec, 7, 0);

        // DEX
        mInstructions[0xCA] = MAKE_INSTR(Instruction::addressImplied, dex, 2, 0);

        // DEY
        mInstructions[0x88] = MAKE_INSTR(Instruction::addressImplied, dey, 2, 0);

        // EOR
        mInstructions[0x49] = MAKE_INSTR(Instruction::addressImmediate, eor, 2, 0);
        mInstructions[0x45] = MAKE_INSTR(Instruction::addressZP, eor, 3, 0);
        mInstructions[0x55] = MAKE_INSTR(Instruction::addressZPX, eor, 4, 0);
        mInstructions[0x4D] = MAKE_INSTR(Instruction::addressAbs, eor, 4, 0);
        mInstructions[0x5D] = MAKE_INSTR(Instruction::addressAbsX, eor, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0x59] = MAKE_INSTR(Instruction::addressAbsY, eor, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0x41] = MAKE_INSTR(Instruction::addressIX, eor, 6, 0);
        mInstructions[0x51] = MAKE_INSTR(Instruction::addressIY, eor, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

        // INC
        mInstructions[0xE6] = MAKE_INSTR(Instruction::addressZP, inc, 5, 0);
        mInstructions[0xF6] = MAKE_INSTR(Instruction::addressZPX, inc, 6, 0);
        mInstructions[0xEE] = MAKE_INSTR(Instruction::addressAbs, inc, 6, 0);
        mInstructions[0xFE] = MAKE_INSTR(Instruction::addressAbsX, inc, 7, 0);

        // INX
        mInstructions[0xE8] = MAKE_INSTR(Instruction::addressImplied, inx, 2, 0);

        // INY
        mInstructions[0xC8] = MAKE_INSTR(Instruction::addressImplied, iny, 2, 0);

        // JMP
        mInstructions[0x4C] = MAKE_INSTR(Instruction::addressAbs, jmp, 3, 0);
        mInstructions[0x6C] = MAKE_INSTR(Instruction::addressI, jmp, 5, 0);

        // JSR
        mInstructions[0x20] = MAKE_INSTR(Instruction::addressAbs, jsr, 6, 0);

        // LDA
        mInstructions[0xA9] = MAKE_INSTR(Instruction::addressImmediate, lda, 2, 0);
        mInstructions[0xA5] = MAKE_INSTR(Instruction::addressZP, lda, 3, 0);
        mInstructions[0xB5] = MAKE_INSTR(Instruction::addressZPX, lda, 4, 0);
        mInstructions[0xAD] = MAKE_INSTR(Instruction::addressAbs, lda, 4, 0);
        mInstructions[0xBD] = MAKE_INSTR(Instruction::addressAbsX, lda, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0xB9] = MAKE_INSTR(Instruction::addressAbsY, lda, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0xA1] = MAKE_INSTR(Instruction::addressIX, lda, 6, 0);
        mInstructions[0xB1] = MAKE_INSTR(Instruction::addressIY, lda, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

        // LDX
        mInstructions[0xA2] = MAKE_INSTR(Instruction::addressImmediate, ldx, 2, 0);
        mInstructions[0xA6] = MAKE_INSTR(Instruction::addressZP, ldx, 3, 0);
        mInstructions[0xB6] = MAKE_INSTR(Instruction::addressZPY, ldx, 4, 0);
        mInstructions[0xAE] = MAKE_INSTR(Instruction::addressAbs, ldx, 4, 0);
        mInstructions[0xBE] = MAKE_INSTR(Instruction::addressAbsY, ldx, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);

        // LDY
        mInstructions[0xA0] = MAKE_INSTR(Instruction::addressImmediate, ldy, 2, 0);
        mInstructions[0xA4] = MAKE_INSTR(Instruction::addressZP, ldy, 3, 0);
        mInstructions[0xB4] = MAKE_INSTR(Instruction::addressZPX, ldy, 4, 0);
        mInstructions[0xAC] = MAKE_INSTR(Instruction::addressAbs, ldy, 4, 0);
        mInstructions[0xBC] = MAKE_INSTR(Instruction::addressAbsX, ldy, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);

        // LSR
        mInstructions[0x4A] = MAKE_INSTR(Instruction::addressImplied, lsr_a, 2, 0);
        mInstructions[0x46] = MAKE_INSTR(Instruction::addressZP, lsr_m, 5, 0);
        mInstructions[0x56] = MAKE_INSTR(Instruction::addressZPX, lsr_m, 6, 0);
        mInstructions[0x4E] = MAKE_INSTR(Instruction::addressAbs, lsr_m, 6, 0);
        mInstructions[0x5E] = MAKE_INSTR(Instruction::addressAbsX, lsr_m, 7, 0);

        // NOP
        mInstructions[0xEA] = MAKE_INSTR(Instruction::addressImplied, nop, 2, 0);

        // ORA
        mInstructions[0x09] = MAKE_INSTR(Instruction::addressImmediate, ora, 2, 0);
        mInstructions[0x05] = MAKE_INSTR(Instruction::addressZP, ora, 3, 0);
        mInstructions[0x15] = MAKE_INSTR(Instruction::addressZPX, ora, 4, 0);
        mInstructions[0x0D] = MAKE_INSTR(Instruction::addressAbs, ora, 4, 0);
        mInstructions[0x1D] = MAKE_INSTR(Instruction::addressAbsX, ora, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0x19] = MAKE_INSTR(Instruction::addressAbsY, ora, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0x01] = MAKE_INSTR(Instruction::addressIX, ora, 6, 0);
        mInstructions[0x11] = MAKE_INSTR(Instruction::addressIY, ora, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

        // PHA
        mInstructions[0x48] = MAKE_INSTR(Instruction::addressImplied, pha, 3, 0);

        // PHP
        mInstructions[0x08] = MAKE_INSTR(Instruction::addressImplied, php, 3, 0);

        // PLA
        mInstructions[0x68] = MAKE_INSTR(Instruction::addressImplied, pla, 4, 0);

        // PLP
        mInstructions[0x28] = MAKE_INSTR(Instruction::addressImplied, plp, 4, 0);

        // ROL
        mInstructions[0x2A] = MAKE_INSTR(Instruction::addressImplied, rol_a, 2, 0);
        mInstructions[0x26] = MAKE_INSTR(Instruction::addressZP, rol_m, 5, 0);
        mInstructions[0x36] = MAKE_INSTR(Instruction::addressZPX, rol_m, 6, 0);
        mInstructions[0x2E] = MAKE_INSTR(Instruction::addressAbs, rol_m, 6, 0);
        mInstructions[0x3E] = MAKE_INSTR(Instruction::addressAbsX, rol_m, 7, 0);

        // ROR
        mInstructions[0x6A] = MAKE_INSTR(Instruction::addressImplied, ror_a, 2, 0);
        mInstructions[0x66] = MAKE_INSTR(Instruction::addressZP, ror_m, 5, 0);
        mInstructions[0x76] = MAKE_INSTR(Instruction::addressZPX, ror_m, 6, 0);
        mInstructions[0x6E] = MAKE_INSTR(Instruction::addressAbs, ror_m, 6, 0);
        mInstructions[0x7E] = MAKE_INSTR(Instruction::addressAbsX, ror_m, 7, 0);

        // RTI
        mInstructions[0x40] = MAKE_INSTR(Instruction::addressImplied, rti, 6, 0);

        // RTS
        mInstructions[0x60] = MAKE_INSTR(Instruction::addressImplied, rts, 6, 0);

        // SBC
        mInstructions[0xE9] = MAKE_INSTR(Instruction::addressImmediate, sbc, 2, 0);
        mInstructions[0xE5] = MAKE_INSTR(Instruction::addressZP, sbc, 3, 0);
        mInstructions[0xF5] = MAKE_INSTR(Instruction::addressZPX, sbc, 4, 0);
        mInstructions[0xED] = MAKE_INSTR(Instruction::addressAbs, sbc, 4, 0);
        mInstructions[0xFD] = MAKE_INSTR(Instruction::addressAbsX, sbc, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0xF9] = MAKE_INSTR(Instruction::addressAbsY, sbc, 4, Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0xE1] = MAKE_INSTR(Instruction::addressIX, sbc, 6, 0);
        mInstructions[0xF1] = MAKE_INSTR(Instruction::addressIY, sbc, 5, Instruction::FLAG_CYCLE_PAGE_CROSSED);

        // SEC
        mInstructions[0x38] = MAKE_INSTR(Instruction::addressImplied, sec, 2, 0);

        // SED
        mInstructions[0xF8] = MAKE_INSTR(Instruction::addressImplied, sed, 2, 0);

        // SEI
        mInstructions[0x78] = MAKE_INSTR(Instruction::addressImplied, sei, 2, 0);

        // STA
        mInstructions[0x85] = MAKE_INSTR(Instruction::addressZP, sta, 3, 0);
        mInstructions[0x95] = MAKE_INSTR(Instruction::addressZPX, sta, 4, 0);
        mInstructions[0x8D] = MAKE_INSTR(Instruction::addressAbs, sta, 4, 0);
        mInstructions[0x9D] = MAKE_INSTR(Instruction::addressAbsX, sta, 5, 0);
        mInstructions[0x99] = MAKE_INSTR(Instruction::addressAbsY, sta, 5, 0);
        mInstructions[0x81] = MAKE_INSTR(Instruction::addressIX, sta, 6, 0);
        mInstructions[0x91] = MAKE_INSTR(Instruction::addressIY, sta, 6, 0);

        // STX
        mInstructions[0x86] = MAKE_INSTR(Instruction::addressZP, stx, 3, 0);
        mInstructions[0x96] = MAKE_INSTR(Instruction::addressZPY, stx, 4, 0);
        mInstructions[0x8E] = MAKE_INSTR(Instruction::addressAbs, stx, 4, 0);

        // STY
        mInstructions[0x84] = MAKE_INSTR(Instruction::addressZP, sty, 3, 0);
        mInstructions[0x94] = MAKE_INSTR(Instruction::addressZPX, sty, 4, 0);
        mInstructions[0x8C] = MAKE_INSTR(Instruction::addressAbs, sty, 4, 0);

        // TAX
        mInstructions[0xAA] = MAKE_INSTR(Instruction::addressImplied, tax, 2, 0);

        // TAY
        mInstructions[0xA8] = MAKE_INSTR(Instruction::addressImplied, tay, 2, 0);

        // TSX
        mInstructions[0xBA] = MAKE_INSTR(Instruction::addressImplied, tsx, 2, 0);

        // TXA
        mInstructions[0x8A] = MAKE_INSTR(Instruction::addressImplied, txa, 2, 0);

        // TXS
        mInstructions[0x9A] = MAKE_INSTR(Instruction::addressImplied, txs, 2, 0);

        // TYA
        mInstructions[0x98] = MAKE_INSTR(Instruction::addressImplied, tya, 2, 0);

        // Undocumented instructions:
        // AAC (ANC)
        mInstructions[0x0B] = MAKE_INSTR(Instruction::addressImmediate, aac, 2, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x2B] = MAKE_INSTR(Instruction::addressImmediate, aac, 2, Instruction::FLAG_UNDOCUMENTED);

        // SAX
        mInstructions[0x87] = MAKE_INSTR(Instruction::addressZP, sax, 3, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x97] = MAKE_INSTR(Instruction::addressZPY, sax, 4, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x83] = MAKE_INSTR(Instruction::addressIX, sax, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x8F] = MAKE_INSTR(Instruction::addressAbs, sax, 4, Instruction::FLAG_UNDOCUMENTED);

        // ARR
        mInstructions[0x6B] = MAKE_INSTR(Instruction::addressImmediate, arr, 2, Instruction::FLAG_UNDOCUMENTED);

        // ASR (ALR)
        mInstructions[0x4B] = MAKE_INSTR(Instruction::addressImmediate, asr, 2, Instruction::FLAG_UNDOCUMENTED);

        // ATX
        mInstructions[0xAB] = MAKE_INSTR(Instruction::addressImmediate, atx, 2, Instruction::FLAG_UNDOCUMENTED);

        // AXA
        mInstructions[0x9F] = MAKE_INSTR(Instruction::addressAbsY, axa, 5, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x93] = MAKE_INSTR(Instruction::addressIY, axa, 6, Instruction::FLAG_UNDOCUMENTED);

        // AXS
        mInstructions[0xCB] = MAKE_INSTR(Instruction::addressImmediate, axs, 2, Instruction::FLAG_UNDOCUMENTED);

        // DCP
        mInstructions[0xC7] = MAKE_INSTR(Instruction::addressZP, dcp, 5, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xD7] = MAKE_INSTR(Instruction::addressZPX, dcp, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xCF] = MAKE_INSTR(Instruction::addressAbs, dcp, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xDF] = MAKE_INSTR(Instruction::addressAbsX, dcp, 7, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xDB] = MAKE_INSTR(Instruction::addressAbsY, dcp, 7, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xC3] = MAKE_INSTR(Instruction::addressIX, dcp, 8, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xD3] = MAKE_INSTR(Instruction::addressIY, dcp, 8, Instruction::FLAG_UNDOCUMENTED);

        // DOP
        mInstructions[0x04] = MAKE_INSTR(Instruction::addressZP, nop, 3, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x14] = MAKE_INSTR(Instruction::addressZPX, nop, 4, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x34] = MAKE_INSTR(Instruction::addressZPX, nop, 4, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x44] = MAKE_INSTR(Instruction::addressZP, nop, 3, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x54] = MAKE_INSTR(Instruction::addressZPX, nop, 4, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x64] = MAKE_INSTR(Instruction::addressZP, nop, 3, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x74] = MAKE_INSTR(Instruction::addressZPX, nop, 4, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x80] = MAKE_INSTR(Instruction::addressImmediate, nop, 2, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x82] = MAKE_INSTR(Instruction::addressImmediate, nop, 2, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x89] = MAKE_INSTR(Instruction::addressImmediate, nop, 2, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xC2] = MAKE_INSTR(Instruction::addressImmediate, nop, 2, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xD4] = MAKE_INSTR(Instruction::addressZPX, nop, 4, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xE2] = MAKE_INSTR(Instruction::addressImmediate, nop, 2, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xF4] = MAKE_INSTR(Instruction::addressZPX, nop, 4, Instruction::FLAG_UNDOCUMENTED);

        // ISC
        mInstructions[0xE7] = MAKE_INSTR(Instruction::addressZP, isc, 5, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xF7] = MAKE_INSTR(Instruction::addressZPX, isc, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xEF] = MAKE_INSTR(Instruction::addressAbs, isc, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xFF] = MAKE_INSTR(Instruction::addressAbsX, isc, 7, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xFB] = MAKE_INSTR(Instruction::addressAbsY, isc, 7, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xE3] = MAKE_INSTR(Instruction::addressIX, isc, 8, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xF3] = MAKE_INSTR(Instruction::addressIY, isc, 8, Instruction::FLAG_UNDOCUMENTED);

        // KIL
        mInstructions[0x02] = MAKE_INSTR(Instruction::addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x12] = MAKE_INSTR(Instruction::addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x22] = MAKE_INSTR(Instruction::addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x32] = MAKE_INSTR(Instruction::addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x42] = MAKE_INSTR(Instruction::addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x52] = MAKE_INSTR(Instruction::addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x62] = MAKE_INSTR(Instruction::addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x72] = MAKE_INSTR(Instruction::addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x92] = MAKE_INSTR(Instruction::addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xB2] = MAKE_INSTR(Instruction::addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xD2] = MAKE_INSTR(Instruction::addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xF2] = MAKE_INSTR(Instruction::addressImplied, kil, 0, Instruction::FLAG_UNDOCUMENTED);

        // LAR
        mInstructions[0xBB] = MAKE_INSTR(Instruction::addressAbsY, lar, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);

        // LAX
        mInstructions[0xA7] = MAKE_INSTR(Instruction::addressZP, lax, 3, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xB7] = MAKE_INSTR(Instruction::addressZPY, lax, 4, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xAF] = MAKE_INSTR(Instruction::addressAbs, lax, 4, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xBF] = MAKE_INSTR(Instruction::addressAbsY, lax, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0xA3] = MAKE_INSTR(Instruction::addressIX, lax, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xB3] = MAKE_INSTR(Instruction::addressIY, lax, 5, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);

        // NOP
        mInstructions[0x1A] = MAKE_INSTR(Instruction::addressImplied, nop, 2, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x3A] = MAKE_INSTR(Instruction::addressImplied, nop, 2, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x5A] = MAKE_INSTR(Instruction::addressImplied, nop, 2, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x7A] = MAKE_INSTR(Instruction::addressImplied, nop, 2, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xDA] = MAKE_INSTR(Instruction::addressImplied, nop, 2, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0xFA] = MAKE_INSTR(Instruction::addressImplied, nop, 2, Instruction::FLAG_UNDOCUMENTED);

        // RLA
        mInstructions[0x27] = MAKE_INSTR(Instruction::addressZP, rla, 5, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x37] = MAKE_INSTR(Instruction::addressZPX, rla, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x2F] = MAKE_INSTR(Instruction::addressAbs, rla, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x3F] = MAKE_INSTR(Instruction::addressAbsX, rla, 7, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x3B] = MAKE_INSTR(Instruction::addressAbsY, rla, 7, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x23] = MAKE_INSTR(Instruction::addressIX, rla, 8, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x33] = MAKE_INSTR(Instruction::addressIY, rla, 8, Instruction::FLAG_UNDOCUMENTED);

        // RRA
        mInstructions[0x67] = MAKE_INSTR(Instruction::addressZP, rra, 5, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x77] = MAKE_INSTR(Instruction::addressZPX, rra, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x6F] = MAKE_INSTR(Instruction::addressAbs, rra, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x7F] = MAKE_INSTR(Instruction::addressAbsX, rra, 7, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x7B] = MAKE_INSTR(Instruction::addressAbsY, rra, 7, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x63] = MAKE_INSTR(Instruction::addressIX, rra, 8, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x73] = MAKE_INSTR(Instruction::addressIY, rra, 8, Instruction::FLAG_UNDOCUMENTED);

        // SBC
        mInstructions[0xEB] = MAKE_INSTR(Instruction::addressImmediate, sbc, 2, Instruction::FLAG_UNDOCUMENTED);

        // SLO
        mInstructions[0x07] = MAKE_INSTR(Instruction::addressZP, slo, 5, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x17] = MAKE_INSTR(Instruction::addressZPX, slo, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x0F] = MAKE_INSTR(Instruction::addressAbs, slo, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x1F] = MAKE_INSTR(Instruction::addressAbsX, slo, 7, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x1B] = MAKE_INSTR(Instruction::addressAbsY, slo, 7, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x03] = MAKE_INSTR(Instruction::addressIX, slo, 8, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x13] = MAKE_INSTR(Instruction::addressIY, slo, 8, Instruction::FLAG_UNDOCUMENTED);

        // SRE
        mInstructions[0x47] = MAKE_INSTR(Instruction::addressZP, sre, 5, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x57] = MAKE_INSTR(Instruction::addressZPX, sre, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x4F] = MAKE_INSTR(Instruction::addressAbs, sre, 6, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x5F] = MAKE_INSTR(Instruction::addressAbsX, sre, 7, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x5B] = MAKE_INSTR(Instruction::addressAbsY, sre, 7, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x43] = MAKE_INSTR(Instruction::addressIX, sre, 8, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x53] = MAKE_INSTR(Instruction::addressIY, sre, 8, Instruction::FLAG_UNDOCUMENTED);

        // SXA
        mInstructions[0x9E] = MAKE_INSTR(Instruction::addressAbsY, sxa, 5, Instruction::FLAG_UNDOCUMENTED);

        // SYA
        mInstructions[0x9C] = MAKE_INSTR(Instruction::addressAbsX, sya, 5, Instruction::FLAG_UNDOCUMENTED);

        // TOP
        mInstructions[0x0C] = MAKE_INSTR(Instruction::addressAbs, nop, 4, Instruction::FLAG_UNDOCUMENTED);
        mInstructions[0x1C] = MAKE_INSTR(Instruction::addressAbsX, nop, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0x3C] = MAKE_INSTR(Instruction::addressAbsX, nop, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0x5C] = MAKE_INSTR(Instruction::addressAbsX, nop, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0x7C] = MAKE_INSTR(Instruction::addressAbsX, nop, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0xDC] = MAKE_INSTR(Instruction::addressAbsX, nop, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);
        mInstructions[0xFC] = MAKE_INSTR(Instruction::addressAbsX, nop, 4, Instruction::FLAG_UNDOCUMENTED | Instruction::FLAG_CYCLE_PAGE_CROSSED);

        // XAA
        mInstructions[0x8B] = MAKE_INSTR(Instruction::addressImmediate, xaa, 2, Instruction::FLAG_UNDOCUMENTED);

        // XAS
        mInstructions[0x9B] = MAKE_INSTR(Instruction::addressAbsY, xas, 5, Instruction::FLAG_UNDOCUMENTED);
    }
};

// TODO: class members?
static const uint16_t PC_BASE_ADDRESS = 0x0600;
static const uint16_t STACK_BASE_ADDRESS = 0x0100;
static const uint8_t STACK_MAX_VAL = 0xFF;

// Interrupt table:
static const uint16_t NMI_HANDLER_ADDR = 0xFFFA;
static const uint16_t RST_HANDLER_ADDR = 0xFFFC;
static const uint16_t IRQ_HANDLER_ADDR = 0xFFFE;

CPU6502::CPU6502(IMemory* mem)
	: mPC(PC_BASE_ADDRESS)
	, mA(0)
	, mX(0)
	, mY(0)
	, mSP(STACK_MAX_VAL)
	, mCycles(0)
	, mFlags({ 0 })
	, mMemory(mem)
	, mAddress(0)
    , mInstruction(nullptr)
    , mCmdCnt(0)
    , mInterruptMode(InterruptNone)
{
    powerOn();
}

CPU6502::~CPU6502()
{
}

inline static uint16_t Make16(uint8_t first, uint8_t second)
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
    mA = 0;
    mX = 0;
    mY = 0;
    mCycles = 0;
    mCmdCnt = 0;
    reset();
}

void CPU6502::reset()
{
    mPC = read16(RST_HANDLER_ADDR);    
    mFlags.values = 0x24;
    mSP = 0xFD;
}

void CPU6502::nmi()
{
    mInterruptMode = InterruptNMI;
}

void CPU6502::irq()
{
    if (mFlags.interrupt_disable == 0)
    {
        mInterruptMode = InterruptIRQ;
    }
}

uint8_t CPU6502::read8(uint16_t address)
{
	return mMemory->read(address);
}

uint16_t CPU6502::read16(uint16_t address)
{
	uint8_t first = read8(address);
	uint8_t second = read8(address + 1);
	return Make16(first, second);
}

uint16_t CPU6502::read16_bug(uint16_t address)
{
	uint16_t first = address;
    uint8_t lowPart = static_cast<uint8_t>(first) + 1;
	uint16_t second = (address & 0xFF00) | lowPart;
	uint8_t first_val = mMemory->read(first);
	uint8_t second_val = mMemory->read(second);
	return Make16(first_val, second_val);
}

uint64_t CPU6502::exec()
{
    uint64_t startCycles = mCycles;

    switch (mInterruptMode)
    {
    case InterruptNMI:
        interrupt(NMI_HANDLER_ADDR);
        break;
    case InterruptIRQ:
        interrupt(IRQ_HANDLER_ADDR);
        break;
    }
    mInterruptMode = InterruptNone;

	uint8_t opcode = read8(mPC++);
    mInstruction = &InstructionTable::getInstance().getInstruction(opcode);

	if (mInstruction->mFunc == nullptr) {
		// TODO: assert;
        assert(false && "Unknown instruction");
		return false;
	}

    mIsPageCrossed = false;
    pCpuFunc addressFunc = InstructionTable::getInstance().getAddressFunc(mInstruction->mAddressMode); 
	if (addressFunc != nullptr)
	{
		(*this.*addressFunc)();
	}

	(*this.*mInstruction->mFunc)();

    // Update cycles
	mCycles += mInstruction->mCycles;
	if (mInstruction->mFlags & Instruction::FLAG_CYCLE_PAGE_CROSSED && mIsPageCrossed)
	{
        ++mCycles;
	}
    ++mCmdCnt;

	return mCycles - startCycles;
}

void CPU6502::setPC(uint16_t address)
{
    mPC = address;
}

// Memory address modes:
void CPU6502::addressImplied()
{
	mAddress = 0;
	// Do nothing
}

void CPU6502::addressImmediate()
{
	mAddress = mPC++;
}

void CPU6502::addressRelative()
{
	// Relative offset is stored in address here...
	mAddress = read8(mPC++);
}

void CPU6502::addressZP()
{
	addressZPWOffs(0);
}

void CPU6502::addressZPX()
{
	addressZPWOffs(mX);
}

void CPU6502::addressZPY()
{
	addressZPWOffs(mY);
}

void CPU6502::addressZPWOffs(uint8_t offset)
{
	uint8_t value = read8(mPC++) + offset;
	mAddress = value;
}

void CPU6502::addressAbs()
{
	addressAbsWOfs(0);
}

void CPU6502::addressAbsX()
{
	mIsPageCrossed = addressAbsWOfs(mX);
}

void CPU6502::addressAbsY()
{
    mIsPageCrossed = addressAbsWOfs(mY);
}

bool CPU6502::addressAbsWOfs(uint8_t offset)
{
    uint16_t baseAddress = read16(mPC);
    mPC += 2;
	mAddress = baseAddress + offset;
    return IsPageCrossed(mAddress, baseAddress);
}

void CPU6502::addressIX()
{
	addressIWOffs(mX, 0);
}

void CPU6502::addressIY()
{
    mIsPageCrossed = addressIWOffs(0, mY);
}

void CPU6502::addressI()
{
    uint16_t baseAddress = read16(mPC);
    mPC += 2;
    mAddress = read16_bug(baseAddress);
}

bool CPU6502::addressIWOffs(uint8_t x, uint8_t y)
{
    uint8_t indirectionAddr = read8(mPC++) + x;
    uint16_t baseAddress = read16_bug(indirectionAddr);
    mAddress = baseAddress + static_cast<uint16_t>(y);
    return IsPageCrossed(baseAddress, mAddress);
}

void CPU6502::updateZNFlags(uint8_t value)
{
	mFlags.zero = value ? 0 : 1;
	mFlags.negative = (value & 0x80) ? 1 : 0;
}

void CPU6502::stackPush8(uint8_t value)
{
    // TODO: disabled for DBG
	//assert(mSP > 0);
	mMemory->write(STACK_BASE_ADDRESS + mSP--, value);
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
    // TODO: disabled for DBG
	//assert(mSP < STACK_MAX_VAL);
	return mMemory->read(STACK_BASE_ADDRESS + ++mSP);
}

uint16_t CPU6502::stackPop16()
{	
	uint8_t valLow = stackPop8();
    uint8_t valHigh = stackPop8();
	return Make16(valLow, valHigh);
}

// Load / Store Operations
void CPU6502::lda()
{
	mA = mMemory->read(mAddress);
	updateZNFlags(mA);
}

void CPU6502::ldx()
{
	mX = mMemory->read(mAddress);
	updateZNFlags(mX);
}

void CPU6502::ldy()
{
	mY = mMemory->read(mAddress);
	updateZNFlags(mY);
}

void CPU6502::sta()
{
	mMemory->write(mAddress, mA);
}

void CPU6502::stx()
{
	mMemory->write(mAddress, mX);
}

void CPU6502::sty()
{
	mMemory->write(mAddress, mY);
}

// Register Transfers
void CPU6502::tax()
{
	mX = mA;
	updateZNFlags(mX);
}

void CPU6502::tay()
{
	mY = mA;
	updateZNFlags(mY);
}

void CPU6502::txa()
{
	mA = mX;
	updateZNFlags(mA);
}

void CPU6502::tya()
{
	mA = mY;
	updateZNFlags(mA);
}

// Stack Operations
void CPU6502::tsx()
{
	mX = mSP;
	updateZNFlags(mX);
}

void CPU6502::txs()
{
	mSP = mX;
}

void CPU6502::pha()
{
	stackPush8(mA);
}

void CPU6502::php()
{
	stackPush8(FlagsToStack(mFlags.values));
}

void CPU6502::pla()
{
	mA = stackPop8();
	updateZNFlags(mA);
}

void CPU6502::plp()
{
    mFlags.values = FlagsFromStack(stackPop8());
}

// Logical
void CPU6502::and()
{
	mA &= mMemory->read(mAddress);
	updateZNFlags(mA);
}

void CPU6502::eor()
{
	mA ^= mMemory->read(mAddress);
	updateZNFlags(mA);
}

void CPU6502::ora()
{
	mA |= mMemory->read(mAddress);
	updateZNFlags(mA);
}

void CPU6502::bit()
{
	uint8_t value = mMemory->read(mAddress);
	mFlags.negative = value & (1 << 7) ? 1 : 0; // TODO: optimize flag sets
	mFlags.overflow = value & (1 << 6) ? 1 : 0;
	mFlags.zero = (value & mA) == 0 ? 1 : 0;
}

// Arithmetic
void CPU6502::adc()
{
	uint8_t src = mMemory->read(mAddress);
    int16_t res = static_cast<int16_t>(mA) + src + mFlags.carry;
    mFlags.carry = res > 0xFF ? 1 : 0;
	mFlags.overflow = (((mA ^ src) & 0x80) == 0 && ((mA ^ res) & 0x80) != 0) ? 1 : 0;
    mA = static_cast<uint8_t>(res);
    updateZNFlags(mA);
}

void CPU6502::sbc()
{
	uint8_t src = mMemory->read(mAddress);
	int16_t res = static_cast<int16_t>(mA) - src - (1 - mFlags.carry);
	mFlags.carry = res >= 0 ? 1 : 0;
    mFlags.overflow = (((mA ^ src) & 0x80) != 0 && ((mA ^ res) & 0x80) != 0) ? 1 : 0;
    mA = static_cast<uint8_t>(res);
    updateZNFlags(mA);
}

void CPU6502::cmp()
{
	cmpWVal(mA);
}

void CPU6502::cpx()
{
	cmpWVal(mX);
}

void CPU6502::cpy()
{
	cmpWVal(mY);
}

void CPU6502::cmpWVal(uint8_t value)
{
	uint8_t res = value;
    uint8_t src = mMemory->read(mAddress);
	mFlags.carry = (value >= src) ? 1 : 0;
	updateZNFlags(value - src);
}

// Increments & Decrements
void CPU6502::inc()
{
	uint8_t dst = mMemory->read(mAddress) + 1;
	updateZNFlags(dst);
	mMemory->write(mAddress, dst);
}

void CPU6502::inx()
{
	++mX;
	updateZNFlags(mX);
}

void CPU6502::iny()
{
	++mY;
	updateZNFlags(mY);
}

void CPU6502::dec()
{
	uint8_t dst = mMemory->read(mAddress) - 1;
	updateZNFlags(dst);
	mMemory->write(mAddress, dst);
}

void CPU6502::dex()
{
	--mX;
	updateZNFlags(mX);
}

void CPU6502::dey()
{
	--mY;
	updateZNFlags(mY);
}

// Shifts
void CPU6502::asl_m()
{
	uint8_t value = mMemory->read(mAddress);
	mMemory->write(mAddress, asl(value));
}

void CPU6502::asl_a()
{
	mA = asl(mA);
}

uint8_t CPU6502::asl(uint8_t value)
{
	mFlags.carry = (value & 0x80) > 0 ? 1 : 0;
	value <<= 1;
	updateZNFlags(value);
	return value;
}

void CPU6502::lsr_m()
{
	uint8_t value = mMemory->read(mAddress);
	mMemory->write(mAddress, lsr(value));
}

void CPU6502::lsr_a()
{
	mA = lsr(mA);
}

uint8_t CPU6502::lsr(uint8_t value)
{
	mFlags.carry = value & 0x1;
	value >>= 1;
	updateZNFlags(value);
	return value;
}

uint8_t CPU6502::rol(uint8_t value)
{
	uint8_t dst = (value << 1) | mFlags.carry;	
	mFlags.carry = ((value >> 7) & 0x1) > 0 ? 1 : 0;
	updateZNFlags(dst);
	return dst;
}

void CPU6502::rol_m()
{
	uint8_t value = mMemory->read(mAddress);
	mMemory->write(mAddress, rol(value));
}

void CPU6502::rol_a()
{
	mA = rol(mA);
}

uint8_t CPU6502::ror(uint8_t value)
{
	uint8_t dst = value;
	dst >>= 1;
    dst |= (mFlags.carry << 7);
	mFlags.carry = value & 0x1;
	updateZNFlags(dst);
	return dst;
}

void CPU6502::ror_m()
{
	uint8_t value = mMemory->read(mAddress);
	mMemory->write(mAddress, ror(value));
}

void CPU6502::ror_a()
{
	mA = ror(mA);
}

// Jumps & Calls
void CPU6502::jmp()
{
	mPC = mAddress;
}

void CPU6502::jsr()
{
	uint16_t ret = mPC - 1;
	stackPush16(ret);
	mPC = mAddress;
}

void CPU6502::rts()
{
	mPC = stackPop16() + 1;
}

void CPU6502::branchRelative()
{
	uint16_t oldPc = mPC;
	int8_t val = static_cast<int8_t>(mAddress & 0xFF); // TODO: check
	mPC += val;
	// TODO: location check?
	mCycles += 1;
	if (IsPageCrossed(oldPc, mPC))
		mCycles += 1;
}

// Branches
void CPU6502::bcc()
{
	if (!mFlags.carry)
	{
		branchRelative();
	}
}

void CPU6502::bcs()
{
	if (mFlags.carry)
	{
		branchRelative();
	}
}

void CPU6502::beq()
{
	if (mFlags.zero)
	{
		branchRelative();
	}
}

void CPU6502::bmi()
{
	if (mFlags.negative)
	{
		branchRelative();
	}
}

void CPU6502::bne()
{
	if (!mFlags.zero)
	{
		branchRelative();
	}
}

void CPU6502::bpl()
{
	if (!mFlags.negative)
	{
		branchRelative();
	}
}

void CPU6502::bvc()
{
	if (!mFlags.overflow)
	{
		branchRelative();
	}
}

void CPU6502::bvs()
{
	if (mFlags.overflow)
	{
		branchRelative();
	}
}

// Status Flag Changes
void CPU6502::clc()
{
	mFlags.carry = 0;
}

void CPU6502::cld()
{
	mFlags.decimal_mode = 0;
	// TODO: warn?
}

void CPU6502::cli()
{
	mFlags.interrupt_disable = 0;
}

void CPU6502::clv()
{
	mFlags.overflow = 0;
}

void CPU6502::sec()
{
	mFlags.carry = 1;
}

void CPU6502::sed()
{
	mFlags.decimal_mode = 1;
	// TODO: warn?
}

void CPU6502::sei()
{
	mFlags.interrupt_disable = 1;
}

// System Functions
void CPU6502::brk()
{
    interrupt(IRQ_HANDLER_ADDR);
}

void CPU6502::interrupt(uint16_t address)
{
	stackPush16(mPC);
	stackPush8(FlagsToStack(mFlags.values));
    mPC = read16(address);
	mFlags.interrupt_disable = 1;
    mCycles += 7;
}

void CPU6502::nop()
{
	// Do nothing
}

void CPU6502::rti()
{
    mFlags.values = FlagsFromStack(stackPop8());
	mPC = stackPop16();
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
    uint8_t value = mA & mX;
    mMemory->write(mAddress, value);
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
    uint8_t value = mMemory->read(mAddress);
    mX = mA = value;
    updateZNFlags(value);
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
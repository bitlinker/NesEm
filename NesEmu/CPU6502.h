#pragma once
#include <stdint.h>
#include "IMemory.h"

// Little endian
//The first 256 byte page of memory ($0000-$00FF) is referred to as 'Zero Page' and is the focus of a number of special addressing modes that result in shorter (and quicker) instructions or allow indirect access to the memory.
// The second page of memory ($0100-$01FF) is reserved for the system stack and which cannot be relocated.

// The only other reserved locations in the memory map are the very last 6 bytes of memory $FFFA to $FFFF which must be programmed with the addresses of the non-maskable interrupt handler ($FFFA/B), 
// the power on reset location ($FFFC/D) and the BRK/interrupt request handler ($FFFE/F) respectively.

//The NTSC NES runs at 1.7897725MHz, and 1.773447MHz for PAL


class Instruction;

class CPU6502
{
private:
	uint16_t pc;
	uint8_t a, x, y;
	uint8_t stack;
	uint32_t cycles;

	union TFlags
	{
		struct
		{
			uint8_t carry : 1;
			uint8_t zero : 1;
			uint8_t interrupt_disable : 1;
			uint8_t decimal_mode : 1;
			uint8_t brk : 1;
			uint8_t unused : 1;
			uint8_t overflow : 1;
			uint8_t negative : 1;
		};
		uint8_t values;
	};
	TFlags flags;

	IMemory* memory;


    // Current internal data:	
	uint16_t address;                          // Current address for instruction
    bool mIsPageCrossed;
    Instruction* mInstruction;           // Instruction
    uint32_t mCmdCnt;
	  
public:
	CPU6502(IMemory* memory);
	~CPU6502();

    void powerOn();
    void reset();

	bool exec();

    void setPC(uint16_t address);

    uint8_t getA() { return a; }
    uint8_t getX() { return x; }
    uint8_t getY() { return y; }
    uint8_t getSP() { return stack; }
    uint16_t getPC() { return pc; }
    uint8_t getFlags() { return flags.values; }
    uint32_t getCycles() { return cycles; }

public:
	void addressImplied();
	void addressImmediate();
	void addressRelative();
	void addressZP();
	void addressZPX();
	void addressZPY();
	void addressAbs();
	void addressAbsX();
	void addressAbsY();
	void addressIX();
	void addressIY();
	void addressI();

private:
	void addressZPWOffs(uint8_t offset);
    bool addressAbsWOfs(uint8_t offset);
    bool addressIWOffs(uint8_t x, uint8_t y);

	uint8_t read8(uint16_t address);
	uint16_t read16(uint16_t address);
	uint16_t read16_bug(uint16_t address);

	void updateZeroNegativeFlags(uint8_t value);
	void cmpWVal(uint8_t value);

	void branchRelative();

	uint8_t asl(uint8_t value);
	uint8_t lsr(uint8_t value);
	uint8_t rol(uint8_t value);
	uint8_t ror(uint8_t value);

	void interrupt(uint16_t address);

	void stackPush8(uint8_t value);
	void stackPush16(uint16_t value);
	uint8_t stackPop8();
	uint16_t stackPop16();

public:
	// Load / Store Operations
	void lda();
	void ldx();
	void ldy();
	void sta();
	void stx();
	void sty();

	// Register Transfers
	void tax();
	void tay();
	void txa();
	void tya();

	// Stack Operations
	void tsx();
	void txs();
	void pha();
	void php();
	void pla();
	void plp();

	// Logical
	void and();
	void eor();
	void ora();
	void bit();

	// Arithmetic
	void adc();
	void sbc();
	void cmp();
	void cpx();
	void cpy();

	// Increments & Decrements
	void inc();
	void inx();
	void iny();
	void dec();
	void dex();
	void dey();

	// Shifts
	void asl_a();
	void asl_m();
	void lsr_a();
	void lsr_m();
	void rol_a();
	void rol_m();
	void ror_a();
	void ror_m();

	// Jumps & Calls
	void jmp();
	void jsr();
	void rts();

	// Branches
	void bcc();
	void bcs();
	void beq();
	void bmi();
	void bne();
	void bpl();
	void bvc();
	void bvs();

	// Status Flag Changes
	void clc();
	void cld();
	void cli();
	void clv();
	void sec();
	void sed();
	void sei();

	// System Functions
	void brk();
	void nop();
	void rti();

    // Invalid instructions:
    void invNop(); // TODO???
    void lax();
    void sax();
    void invSbc();
    void dcp();
    void isb();
    void slo();
    void rla();
    void sre();
    void rra();
};


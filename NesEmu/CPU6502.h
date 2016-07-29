#pragma once
#include <stdint.h>
#include "IMemory.h"

// Little endian
//The first 256 byte page of memory ($0000-$00FF) is referred to as 'Zero Page' and is the focus of a number of special addressing modes that result in shorter (and quicker) instructions or allow indirect access to the memory.
// The second page of memory ($0100-$01FF) is reserved for the system stack and which cannot be relocated.

// The only other reserved locations in the memory map are the very last 6 bytes of memory $FFFA to $FFFF which must be programmed with the addresses of the non-maskable interrupt handler ($FFFA/B), 
// the power on reset location ($FFFC/D) and the BRK/interrupt request handler ($FFFE/F) respectively.

//The NTSC NES runs at 1.7897725MHz, and 1.773447MHz for PAL

// TODO:
// - Move instruction to class
// - Make address modes enum (or classes)
// - Get number of bytes from address mode
// - Make printout from address mode & instruction
// - Update static instruction table
// - Update interrupt routines & priorities
// - Logger class

class Instruction;

class CPU6502
{
public:
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

    enum InterruptMode
    {
        InterruptNone,
        InterruptNMI,
        InterruptIRQ,
    };

private:
	uint16_t mPC;
	uint8_t mA, mX, mY;
	uint8_t mSP;
	TFlags mFlags;
    InterruptMode mInterruptMode;

    uint64_t mCycles;

	ICpuMemory* mMemory;


    // Current instruction execution data:	
	uint16_t mAddress;                      // Current address for instruction
    bool mIsPageCrossed;                    // Page cross flag
    const Instruction* mInstruction;        // Current instruction
    uint32_t mCmdCnt;
	  
public:
	CPU6502(ICpuMemory* memory);
	~CPU6502();

    void powerOn();
    void reset();

    void nmi();
    void irq();

    uint64_t exec();

    void setPC(uint16_t address);

    uint8_t getA() { return mA; }
    uint8_t getX() { return mX; }
    uint8_t getY() { return mY; }
    uint8_t getSP() { return mSP; }
    uint16_t getPC() { return mPC; }
    uint8_t getFlags() { return mFlags.values; }
    uint64_t getCycles() { return mCycles; }

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

	void updateZNFlags(uint8_t value);
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

    // Undocumented instructions:
    void aac();
    void sax();
    void arr();
    void asr();
    void atx();
    void axa();
    void axs();
    void dcp();
    void isc();
    void kil();
    void lar();
    void lax();
    void rla();
    void rra();
    void slo();
    void sre();
    void sxa();
    void sya();
    void xaa();
    void xas();
};


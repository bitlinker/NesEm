#pragma once
#include <stdint.h>
#include "IMemory.h"

// Little endian
//The first 256 byte page of memory ($0000-$00FF) is referred to as 'Zero Page' and is the focus of a number of special addressing modes that result in shorter (and quicker) instructions or allow indirect access to the memory.
// The second page of memory ($0100-$01FF) is reserved for the system stack and which cannot be relocated.

// The only other reserved locations in the memory map are the very last 6 bytes of memory $FFFA to $FFFF which must be programmed with the addresses of the non-maskable interrupt handler ($FFFA/B), 
// the power on reset location ($FFFC/D) and the BRK/interrupt request handler ($FFFE/F) respectively.

// Addressation mode
enum AddressMode
{
	ADDR_D,		// Direct (Immediate)
	ADDR_ZP,	// Zero page
	ADDR_ZPX,	// Zero page, X
	ADDR_ZPY,	// Zero page, Y
	ADDR_A,		// Absolute
	ADDR_AX,	// Absolute, X
	ADDR_AY,	// Absolute, Y
	ADDR_IX,	// Inderect, X
	ADDR_IY		// Inderect, Y
};

// Memory direction mode
enum MemMode
{
	MEM_NONE,	// Ignore memory
	MEM_READ,	// Read from memory
	MEM_WRITE	// Write to memory
};

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
			uint8_t overflow : 1;
			uint8_t negative : 1;
		};
		uint8_t values;
	};
	TFlags flags;

	IMemory* memory;
	uint8_t src, dst;
	  
public:
	CPU6502(IMemory* memory);
	~CPU6502();

	bool exec();

private:
	void readMem(AddressMode mode);
	void writeMem(AddressMode mode);
	uint8_t readPC();

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
	void asl();
	void lsr();
	void rol();
	void ror();

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
};


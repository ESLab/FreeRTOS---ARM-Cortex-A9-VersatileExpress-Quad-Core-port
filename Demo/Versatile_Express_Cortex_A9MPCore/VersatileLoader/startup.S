	.global Vector_Init

	.balign 4
	.code 32

	.section .isr_vector, "x"

#define Core0EntryPoint 0x0010000
#define Core1EntryPoint 0x1010000
#define Core2EntryPoint 0x2010000
#define Core3EntryPoint 0x3010000

// This jump table is somewhat useless, as it will be in the wrong place anyway.
// The code could perhaps be updated to set VBAR, but as the table is not supposed to be used
// anyway, that would not be very useful either.
Vector_Init:
	b	Reset_Handler
	b	No_Handler
	b	No_Handler
	b	No_Handler
	b	No_Handler
	nop
	b	No_Handler
	b	No_Handler

No_Handler:
	b	No_Handler

Stack:
	.space	1024
StackEnd:

Reset_Handler:
	adr		sp,StackEnd

	mrc		p15,0,r1,c1,c0,0		// Read control register configuration data.
	bic		r1,r1,#(1<<0)			// Disable MMU.
	bic		r1,r1,#(1<<12)			// Disable I Cache.
	bic		r1,r1,#(1<<2)			// Disable D Cache.
	mcr		p15,0,r1,c1,c0,0		// Write control register configuration data.

	ldr		r1,=0x1e001000			// ICDDCR
	mov		r2,#0x01
	str		r2,[r1]					// Enable GIC.

	ldr		r1,=0x10000000
	mvn		r2,#0
	str		r2,[r1,#0x34]			// SYS_FLAGSCLR
	adr		r2,Launcher
	str		r2,[r1,#0x30]			// SYS_FLAGSSET

	ldr		r1,=0x1e001f00			// ICDSGIR
	ldr		r2,=0x000e0001			// Using target list, target cores 1, 2 and 3, interrupt #1.
	str		r2,[r1]					// Send software interrupt.

	// Fall through to the launcher.

Launcher:
	mrc		p15,0,r2,c0,c0,5
	and		r2,r2,#3			// Find Core ID

	adr		r1,JumpTable
	ldr		pc,[r1,r2,lsl #2]

JumpTable:
	.word	Core0EntryPoint
	.word	Core1EntryPoint
	.word	Core2EntryPoint
	.word	Core3EntryPoint

    .end


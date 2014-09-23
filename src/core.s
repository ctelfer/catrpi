;@----------------
;@ Memory layout
;@   0-0x1f			-- exception vectors
;@   0x20 - 0x3f		-- exception pointers
;@   0x4000 - 0x7fff		-- IRQ stack 
;@   0x8000 - 0x0effffff	-- code + data
;@   0x04000000 - 0x07ffffff	-- stack 
;@      0x04000000 - 0x04007fff -- Sys stack
;@      0x04008000 - 0x0400ffff -- IRQ stack
;@      0x04010000 - 0x04017fff -- FIQ stack
;@      0x04018000 - 0x07ffffff -- Supervisor stack
;@   0x08000000 - 0x1fffffff	-- Heap
;@   0x20000000 - 0x202fffff	-- Device I/O
;@----------------

.text
;@------------------
;@ Main entry point 
;@------------------
	;@ This code is loaded at 0x8000
.global _start
_start:
	;@ Interrupt vector
	;@ -- use the reset handler to install it on start
	;@ -- 8 32-bit words long
	ldr pc, reset_handler
	ldr pc, undef_handler
	ldr pc, swi_handler
	ldr pc, preabt_handler
	ldr pc, databt_handler
	ldr pc, unused_handler ;@ Not currently assigned
	ldr pc, irq_handler
	ldr pc, fiq_handler
	;@ This would be the ideal location to put the FIQ handler
	;@ as it would save having to branch to it.  However,
	;@ we're not going to optimize for that now.

	;@ Saved locations of the handlers
	;@ -- 8 32-bit words long
ex_tab:
reset_handler:
	.word exh_reset
undef_handler:
	.word hang
swi_handler:
	.word hang
preabt_handler:
	.word hang
databt_handler:
	.word hang
unused_handler:
	.word hang
irq_handler:
	.word exh_irq
fiq_handler:
	.word exh_fiq


sys_sp: .word 0x04008000
irq_sp: .word 0x04010000
fiq_sp: .word 0x04018000
svc_sp: .word 0x08000000


exh_reset:
	;@ Copy the first 16 words (above) to the interrupt vector location: 0
	mov r0, #origin
	mov r1, #0
	ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
	stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}
	ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
	stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}

	;@ Set stack pointer for system mode
	;@ Disable IRQ, FIQ, set mode = sys
	mov r0, #0xDF
	msr cpsr_c, r0
	ldr sp, sys_sp

	;@ Set stack pointer for IRQ mode
	;@ Disable IRQ, FIQ, set mode = IRQ
	mov r0, #0xD2
	msr cpsr_c, r0
	ldr sp, irq_sp

	;@ Set stack pointer for FIQ mode
	;@ Disable IRQ, FIQ, set mode = FIQ
	mov r0, #0xD1
	msr cpsr_c, r0
	ldr sp, fiq_sp

	;@ Set stack pointer for Supervisor
	;@ Disable IRQ, FIQ, set mode = SVC
	mov r0, #0xD3
	msr cpsr_c, r0
	ldr sp, svc_sp

	;@ Jump to the main program
	bl main

	;@ Hang routine -- just spin indefinitely
	;@ -- afer 'main's return so should it happen, it will hang
hang:	b hang


exh_irq:
	;@ Save all registers except the stack pointer (r13) and program counter
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	bl irq_u_handler
	;@ Restore all saved registers
	pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	;@ The link register contains the next instruction + 4
	;@ See section A2.6.8 in the ARM ARM
	subs pc, lr, #4

exh_fiq:
	;@ Save all registers except the stack pointer (r13) and program counter
	push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	bl fiq_u_handler
	;@ Restore all saved registers
	pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
	;@ The link register contains the next instruction + 4
	;@ See section A2.6.8 in the ARM ARM
	subs pc, lr, #4

	;@ void peek32(void)
.global pass
pass:
	bx lr

	;@ uint8_t peek8(uint8_t addr)
.global peek8
peek8:
	ldrb r0, [r0]
	bx lr

	;@ void poke8(uint8_t addr, uint8_t value)
.global poke8
poke8:
	strb r1, [r0]
	bx lr

	;@ uint16_t peek16(uint16_t addr)
.global peek16
peek16:
	ldrh r0, [r0]
	bx lr

	;@ void poke16(uint16_t addr, uint16_t value)
.global poke16
poke16:
	strh r1, [r0]
	bx lr

	;@ uint32_t peek32(uint32_t addr)
.global peek32
peek32:
	ldr r0, [r0]
	bx lr

	;@ void poke32(uint32_t addr, uint32_t value)
.global poke32
poke32:
	str r1, [r0]
	bx lr

	;@ void _irq_disable(void);
.global _irq_disable
_irq_disable:
	;@ See section A2.5 of the ARM ARM
	mrs r0, cpsr
	orr r0, r0, #0x80
	msr cpsr_c, r0		;@ update only control bits
	bx lr

	;@ void _irq_enable(void);
.global _irq_enable
_irq_enable:
	;@ See section A2.5 of the ARM ARM
	mrs r0, cpsr
	bic r0, r0, #0x80
	msr cpsr_c, r0		;@ update only control bits
	bx lr

	;@ void fiq_disable(void);
.global fiq_disable
fiq_disable:
	;@ See section A2.5 of the ARM ARM
	mrs r0, cpsr
	orr r0, r0, #0x40
	msr cpsr_c, r0		;@ update only control bits
	bx lr

	;@ void fiq_enable(void);
.global fiq_enable
fiq_enable:
	;@ See section A2.5 of the ARM ARM
	mrs r0, cpsr
	bic r0, r0, #0x40
	msr cpsr_c, r0		;@ update only control bits
	bx lr

	;@ int nlz(uint32_t word)
	;@ count the number of leading zeros in a word
.global nlz
nlz:
	clz r0, r0
	bx lr


	;@ int ffs(uint32_t word)
	;@  -- return the first bit set in a word or -1 if 0
	;@  -- starts with the LSB
.global ffs
ffs:
	push {r1}
	sub r1, r0, #1
	eor r0, r0, r1
	beq ffs_zero
	clz r0, r0
	rsb r0, r0, #31
	pop {r1}
	bx lr
ffs_zero:
	mvn r0, #0
	pop {r1}
	bx lr

.global get_cpsr
get_cpsr:
	mrs r0, cpsr
	bx lr


	;@ int _cpop(int arg0, int arg1)
.global _cpop
_cpop:
	mov r1, #0
	mcr p15, 0, r1, c7, c5, 0	;@ Invalidate icache
	mcr p15, 0, r1, c7, c10, 5	;@ Data Sync Barrier
	mcr p15, 0, r1, c7, c5, 4	;@ Flush Prefetch Buffer 
	nop
	bx lr


	;@ void _mb(void)
.global _mb
_mb:
	mcr p15, 0, r0, c7, c10, 5	;@ Data Sync Barrier
	bx lr


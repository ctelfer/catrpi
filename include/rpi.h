/*
 * include/rpi.h -- API header for core RPI library code.
 *
 * by Christopher Adam Telfer
 *
 * Copyright 2014 See accompanying license
 *
 */

#ifndef __RPI_H
#define __RPI_H

#include <stddef.h>
#include <stdint.h>

/* DATA TYPES */

typedef void (*irq_handler_f)(void);


/* GENERAL FUNCTIONS */

void poke8(uint8_t addr, uint8_t val);

uint8_t peek8(uint8_t addr);

void poke16(uint16_t addr, uint16_t val);

uint16_t peek16(uint16_t addr);

void poke32(uint32_t addr, uint32_t val);

uint32_t peek32(uint32_t addr);

void pass(void);

/* return the number of leading zeros in x (starting with the MSB) */
int nlz(uint32_t x);

/* Return the first bit set in x starting with the LSB or -1 if zero */
int ffs(uint32_t x);

void *memmove(void *d, const void *s, uint len);

void *memcpy(void *d, const void *s, uint len);

void *memset(void *d, uchar x, uint len);

int memcmp(const void *s1, const void *s2, uint len);

uint strlen(const char *s);

/* GPIO */
#define GPIO_FSEL0	0x20200000
#define GPIO_FSEL1	0x20200004
#define GPIO_SET0	0x2020001c
#define GPIO_CLR0	0x20200028
#define GPIO_PUD	0x20200094
#define GPIO_PUD_CLK0	0x20200098

#define GPIO_FSEL_msk		0x7
#define GPIO_FSEL_IN		0x0
#define GPIO_FSEL_OUT		0x1
#define GPIO_FSEL_ALT0		0x4
#define GPIO_FSEL_ALT1		0x5
#define GPIO_FSEL_ALT2		0x6
#define GPIO_FSEL_ALT3		0x7
#define GPIO_FSEL_ALT4		0x3
#define GPIO_FSEL_ALT5		0x2
#define GPIO_FSEL_P14_shf	12
#define GPIO_FSEL_P15_shf	15

#define GPIO_PUD_OFF	0
#define GPIO_PUD_DOWN	1
#define GPIO_PUD_UP	2

/* INTERRUPTS */

#define IRQ_BASE	0x2000B000
#define IRQ_PENDB	(IRQ_BASE + 0x200)	/* "basic" pending interrups */
#define IRQ_PEND1	(IRQ_BASE + 0x204)	/* interrupts 0-31 pending */
#define IRQ_PEND2	(IRQ_BASE + 0x208)	/* interrupts 32-63 pending */
#define IRQ_FIQCTL	(IRQ_BASE + 0x20c)
#define IRQ_EN1		(IRQ_BASE + 0x210)	/* enable interrupts 0-31 */
#define IRQ_EN2		(IRQ_BASE + 0x214)	/* enable interrupts 32-63 */
#define IRQ_ENB		(IRQ_BASE + 0x218)	/* enable "basic" interrupts */
#define IRQ_DIS1	(IRQ_BASE + 0x21c)	/* disable interrupts 0-31 */
#define IRQ_DIS2	(IRQ_BASE + 0x220)	/* disable interrupts 32-63 */
#define IRQ_DISB	(IRQ_BASE + 0x224)	/* disable "basic" interrupts */

/*
 * The first 8 of these are used in all the "basic" registers above.
 * The remainder are used in the "basic pending" register.
 */
#define IRQ_B_TIMER	(1 << 0)		/* ARM timer intr */
#define IRQ_B_MB	(1 << 1)		/* Mailbox intr */
#define IRQ_B_DB0	(1 << 2)		/* Doorbell0 intr */
#define IRQ_B_DB1	(1 << 3)		/* Doorbell1 intr */
#define IRQ_B_GPU0H	(1 << 4)		/* GPU0 Halted intr */
#define IRQ_B_GPU1H	(1 << 5)		/* GPU1 Halted intr */
#define IRQ_B_ILLACC0	(1 << 6)		/* Illegal acc type 0 intr */
#define IRQ_B_ILLACC1	(1 << 7)		/* Illegal acc type 1 intr */
#define IRQ_B_ARM_msk	0xFF
/* the remainder of these are only in the basic pending register */
#define IRQ_B_PEND1	(1 << 8)		/* Intr pending in PEND1 */
#define IRQ_B_PEND2	(1 << 9)		/* Intr pending in PEND2 */
#define IRQ_B_GPU7	(1 << 10)		/* GPU7 intr */
#define IRQ_B_GPU9	(1 << 11)		/* GPU9 intr */
#define IRQ_B_GPU10	(1 << 12)		/* GPU10 intr */
#define IRQ_B_GPU18	(1 << 13)		/* GPU18 intr */
#define IRQ_B_GPU19	(1 << 14)		/* GPU19 intr */
#define IRQ_B_PEND1_msk	\
	(IRQ_B_PEND1 | IRQ_B_GPU7 | IRQ_B_GPU9 | IRQ_B_GPU10 | IRQ_B_GPU18 |\
	 IRQ_B_GPU19)
#define IRQ_B_GPU53	(1 << 15)		/* GPU53 intr */
#define IRQ_B_GPU54	(1 << 16)		/* GPU54 intr */
#define IRQ_B_GPU55	(1 << 17)		/* GPU55 intr */
#define IRQ_B_GPU56	(1 << 18)		/* GPU56 intr */
#define IRQ_B_GPU57	(1 << 19)		/* GPU57 intr */
#define IRQ_B_GPU62	(1 << 20)		/* GPU62 intr */
#define IRQ_B_PEND2_msk	\
	(IRQ_B_PEND2 | IRQ_B_GPU53 | IRQ_B_GPU54 | IRQ_B_GPU55 | IRQ_B_GPU56 |\
	 IRQ_B_GPU57 | IRQ_B_GPU62)

#define IRQ_TIMER		0
#define IRQ_TIMER_PEND_CSR	IRQ_PENDB
#define IRQ_TIMER_EN_CSR	IRQ_ENB
#define IRQ_TIMER_DIS_CSR	IRQ_DISB

#define IRQ_AUX			29
#define IRQ_AUX_PEND_CSR	IRQ_PEND1
#define IRQ_AUX_EN_CSR		IRQ_EN1
#define IRQ_AUX_DIS_CSR		IRQ_DIS1

#define IRQ_FIQ_SRC_shf	0
#define IRQ_FIQ_SRC_msk	0x7F			/* FIQ interrupt source */
#define IRQ_FIQ_EN	(1 << 7)		/* FIQ's enabled */

/* FIQ sources */
#define FIQ_GPU_BASE(_x)	(_x & 0x3F)	/* GPU interrupts 0-63 */
#define FIQ_TIMER		64		/* Timer FIQ */
#define FIQ_MB			65		/* Mailbox FIQ */
#define FIQ_DB0			66		/* Doorbell0 FIQ */
#define FIQ_DB1			67		/* Doorbell1 FIQ */
#define FIQ_GPU0H		68		/* GPU0 halted FIQ */
#define FIQ_GPU1H		69		/* GPU1 halted FIQ */
#define FIQ_ILLACC0		70		/* Illegal acc type 0 FIQ */
#define FIQ_ILLACC1		71		/* Illegal acc type 1 FIQ */

void irq_init(void);

void irq_disable(void);

void irq_enable(void);

void fiq_disable(void);

void fiq_enable(void);



/* UART */

/* (full not mini) UART CSRs */
#define UART_BASE	0x20201000
#define UART_DR		(UART_BASE+0x00)
#define UART_RSRECR	(UART_BASE+0x04)
#define UART_FR		(UART_BASE+0x18)
#define UART_ILPR	(UART_BASE+0x20)
#define UART_IBRD	(UART_BASE+0x24)
#define UART_FBRD	(UART_BASE+0x28)
#define UART_LCRH	(UART_BASE+0x2C)
#define UART_CR		(UART_BASE+0x30)
#define UART_IFLS	(UART_BASE+0x34)
#define UART_IMSC	(UART_BASE+0x38)
#define UART_RIS	(UART_BASE+0x3C)
#define UART_MIS	(UART_BASE+0x40)
#define UART_ICR	(UART_BASE+0x44)
#define UART_DMACR	(UART_BASE+0x48)
#define UART_ITCR	(UART_BASE+0x80)
#define UART_ITIP	(UART_BASE+0x84)
#define UART_ITOP	(UART_BASE+0x88)
#define UART_TDR	(UART_BASE+0x8C)


#define UART_FR_BUSY	(1 << 3)	/* UART busy */
#define UART_FR_RXFE	(1 << 4)	/* RX fifo empty */
#define UART_FR_TXFF	(1 << 5)	/* TX fifo full */
#define UART_FR_RXFF	(1 << 6)	/* RX fifo full */
#define UART_FR_TXFE	(1 << 7)	/* TX fifo empty */


#define UART_CR_DISABLE	0		/* disable UART completely */
#define UART_CR_EN	(1 << 0)	/* enable uart */
#define UART_CR_LBE	(1 << 7)	/* enable loopback */
#define UART_CR_TXE	(1 << 8)	/* TX enable */
#define UART_CR_RXE	(1 << 9)	/* RX enable */
#define UART_CR_RTS	(1 << 11)	/* Request to send: ~RTS line */
#define UART_CR_RTSEN	(1 << 14)	/* RTS flow control enable */
#define UART_CR_CTSEN	(1 << 15)	/* CTS flow control enable */


#define UART_LCRH_BRK		(1 << 0)
#define UART_LCRH_PEN		(1 << 1)
#define UART_LCRH_EPS_EVEN	(0 << 2)
#define UART_LCRH_EPS_ODD	(1 << 2)
#define UART_LCRH_2STP		(1 << 3)
#define UART_LCRH_FEN		(1 << 4)
#define UART_LCRH_WLEN_5B	(0 << 5)
#define UART_LCRH_WLEN_6B	(1 << 5)
#define UART_LCRH_WLEN_7B	(2 << 5)
#define UART_LCRH_WLEN_8B	(3 << 5)
#define UART_LCRH_WLEN_msk	(3 << 5)
#define UART_LCRH_SPAR_EN	(1 << 7)

#define UART_HZ			3000000ul
#define UART_IBRD_val(baud)	(UART_HZ / (baud * 16))
/* fraction in 64ths of the baud divisor */
#define UART_FBRD_val(baud)	\
	(UART_HZ * 1000 / (baud * 16) % 1000 * 64 / 1000)

#define UART_RFIFO_EMPTY	(1 << 4)
#define UART_TFIFO_FULL		(1 << 5)
#define UART_RFIFO_FULL		(1 << 6)
#define UART_TFIFO_EMPTY	(1 << 7)

#define UART_IFLS_1o8		0x0
#define UART_IFLS_1o4		0x1
#define UART_IFLS_1o2		0x2
#define UART_IFLS_3o4		0x3
#define UART_IFLS_7o8		0x4
#define UART_IFLS_msk		0x7
#define UART_IFLS_TX_shf	0
#define UART_IFLS_RX_shf	3

/* pertains to IMSC, RIS, MIS, ICR */
#define UART_INTR_RIM		(1 << 0)	/* ignored */
#define UART_INTR_CTS		(1 << 1)	/* Clear to send */
#define UART_INTR_DCD		(1 << 2)	/* ignored */
#define UART_INTR_DSR		(1 << 3)	/* ignored */
#define UART_INTR_RX		(1 << 4)	/* RX interrupt */
#define UART_INTR_TX		(1 << 5)	/* TX interrupt */
#define UART_INTR_RTIM		(1 << 6)	/* RX timeout */
#define UART_INTR_FERR		(1 << 7)	/* Frame error */
#define UART_INTR_PERR		(1 << 8)	/* Parity error */
#define UART_INTR_BERR		(1 << 9)	/* Break error */
#define UART_INTR_OERR		(1 << 10)	/* Overrun error */
#define UART_INTR_ALL		(0x7F1)

#define IRQ_UART		57
#define IRQ_B_UART		IRQ_B_GPU57
#define UART_IRQ_EN_REG		IRQ_EN2
#define UART_IRQ_DIS_REG	IRQ_DIS2
#define UART_IRQ_BIT		(1 << 25)

void rpi_uart_init(void);

void rpi_uart_disable(void);

int rpi_uart_recv(void *data, int maxlen);

int rpi_uart_send(const void *data, int len);

int rpi_uart_rsts_get(void);

void rpi_uart_rsts_clr(void);

void rpi_uart_flush_tx(void);


/* TIMER FUNCTIONS */

#define TIMER_BASE	0x2000B000
#define TIMER_LOAD	(TIMER_BASE + 0x400)	/* load countdown value */
#define TIMER_VAL	(TIMER_BASE + 0x404)	/* (RO) current value */
#define TIMER_CTL	(TIMER_BASE + 0x408)	/* control params */
#define TIMER_IRQ_ACK	(TIMER_BASE + 0x40C)	/* (WO) Clear timer */
#define TIMER_RAW_IRQ	(TIMER_BASE + 0x410)	/* (RO) raw IRQ state */
#define TIMER_MSK_IRQ	(TIMER_BASE + 0x414)	/* (RO) masked IRQ state */
#define TIMER_RELOAD	(TIMER_BASE + 0x418)	/* value loaded when hits 0 */
#define TIMER_PREDIV	(TIMER_BASE + 0x41C)	/* pre-timer divider */
#define TIMER_CTR	(TIMER_BASE + 0x420)	/* (RO) Free running counter */

#define TIMER_CTL_CTRSIZ	(1 << 1)	/* 0->16-bit, 1->32-bit ctr */
#define TIMER_CTL_PRESCL1	(0 << 2)	/* pre-scale by 1 */
#define TIMER_CTL_PRESCL16	(1 << 2)	/* pre-scale by 16 */
#define TIMER_CTL_PRESCL256	(2 << 2)	/* pre-scale by 256 */
#define TIMER_CTL_INTR_EN	(1 << 5)	/* timer intr en/disable */
#define TIMER_CTL_EN		(1 << 7)	/* timer en/disabled */
#define TIMER_CTL_DBG_HLT	(1 << 8)	/* timer halt in dbg halt */
#define TIMER_CTL_CTR_EN	(1 << 9)	/* ctr en/disable */
#define TIMER_CTL_CTRDIV_shf	16
#define TIMER_CTL_CTRDIV_msk	0xFF
#define TIMER_CTL_CTRDIV_val(_x) \
	((((_x) - 1) & TIMER_CTL_CTRDIV_msk) << TIMER_CTL_CTRDIV_shf)

/* 
 * The timer subsystem expects a 1Mhz clock which the ARM doesn't have.
 * So the pre-divider divides the APB clock first.
 */
#define TIMER_PREDIV_val(_x)  (((_x) - 1) & 0x3FF)

/* Must be called with interrupts disabled */
void timer_init(uint usec, irq_handler_f f);

uint32_t timer_ctr(void);


/* SYSTEM TIMER */

#define SYSTIME_BASE	0x20003000
#define SYSTIME_CTL	(SYSTIME_BASE + 0x00)
#define SYSTIME_LO	(SYSTIME_BASE + 0x04)
#define SYSTIME_HI	(SYSTIME_BASE + 0x08)
#define SYSTIME_CMP0	(SYSTIME_BASE + 0x0C)
#define SYSTIME_CMP1	(SYSTIME_BASE + 0x10)
#define SYSTIME_CMP2	(SYSTIME_BASE + 0x14)
#define SYSTIME_CMP3	(SYSTIME_BASE + 0x18)

#define SYSTIME_CTL_MATCH0	(1 << 0)
#define SYSTIME_CTL_MATCH1	(1 << 1)
#define SYSTIME_CTL_MATCH2	(1 << 2)
#define SYSTIME_CTL_MATCH3	(1 << 3)

uint32_t systime(void);

/* COPROCESSOR OPERATIONS */

#define ARM_INSTR_COND_shf	28
#define ARM_INSTR_COND_msk	0xF

#define ARM_COND_ALWAYS		0xE
#define ARM_CPOP_OC		0xE
#define ARM_CPOP_OC_shf		24
#define ARM_CPOP_COC1_shf	21
#define ARM_CPOP_COC1_msk	0x7
#define ARM_CPOP_DIR_shf	20
#define ARM_CPOP_DIR_msk	0x1
#define ARM_CPOP_CRN_shf	16
#define ARM_CPOP_CRN_msk	0xF
#define ARM_CPOP_RDN_shf	12
#define ARM_CPOP_RDN_msk	0xF
#define ARM_CPOP_CPNUM_shf	8
#define ARM_CPOP_CPNUM_msk	0xF
#define ARM_CPOP_COC2_shf	5
#define ARM_CPOP_COC2_msk	0x7
#define ARM_CPOP_ONE_shf	4
#define ARM_CPOP_ONE_msk	0x1
#define ARM_CPOP_CRM_shf	0
#define ARM_CPOP_CRM_msk	0xF

#define ARM_CPOP_TOARM		(1 << ARM_CPOP_DIR_shf)
#define ARM_CPOP_TOCP		(0 << ARM_CPOP_DIR_shf)

#define ARM_CP_OP(_cp, _oc1, _crn, _oc2, _crm, _reg) 		\
	((ARM_COND_ALWAYS << ARM_INSTR_COND_shf) | 		\
	 (ARM_CPOP_OC << ARM_CPOP_OC_shf) |			\
	 (((_oc1) & ARM_CPOP_COC1_msk) << ARM_CPOP_COC1_shf) | 	\
	 (((_crn) & ARM_CPOP_CRN_msk) << ARM_CPOP_CRN_shf) | 	\
	 (((_reg) & ARM_CPOP_RDN_msk) << ARM_CPOP_RDN_shf) | 	\
	 ((_cp) << ARM_CPOP_CPNUM_shf) | 			\
	 (((_oc2) & ARM_CPOP_COC2_msk) << ARM_CPOP_COC2_shf) | 	\
	 (1 << ARM_CPOP_ONE_shf) |				\
	 (((_crm) & ARM_CPOP_CRM_msk) << ARM_CPOP_CRM_shf))

#define ARM_MCR(_cp, _oc1, _crn, _oc2, _crm, _reg) 		\
	(ARM_CP_OP(_cp, _oc1, _crn, _oc2, _crm, _reg) | ARM_CPOP_TOCP)

#define ARM_MRC(_cp, _oc1, _crn, _oc2, _crm, _reg) 		\
	(ARM_CP_OP(_cp, _oc1, _crn, _oc2, _crm, _reg) | ARM_CPOP_TOARM)

uint32_t coproc_read(uint cp, uint crn, uint op1, uint crm, uint op2);

void coproc_write(uint cp, uint crn, uint op1, uint crm, uint op2,
		  uint32_t val);


/* CP15 == the system control coprocessor */
enum {
	cpr_c0, cpr_c1, cpr_c2, cpr_c3, cpr_c4, cpr_c5, cpr_c6, cpr_c7, 
	cpr_c8, cpr_c9, cpr_c10, cpr_c11, cpr_c12, cpr_c13, cpr_c14, 
	cpr_c15, 
};
uint32_t cp15_read(uint crn, uint op1, uint crm, uint op2);

void cp15_write(uint crn, uint op1, uint crm, uint op2, uint32_t val);

void cp15_set(uint crn, uint op1, uint crm, uint op2, uint32_t val);

void cp15_clr(uint crn, uint op1, uint crm, uint op2, uint32_t val);


#define ARM_CP15_C1_MMU_shf	0
#define ARM_CP15_C1_MMU_bit	(1 << ARM_CP15_C1_MMU_shf)
#define ARM_CP15_C1_AFAULT_shf	1
#define ARM_CP15_C1_AFAULT_bit	(1 << ARM_CP15_C1_AFAULT_shf)
#define ARM_CP15_C1_DCACHE_shf	2
#define ARM_CP15_C1_DCACHE_bit	(1 << ARM_CP15_C1_DCACHE_shf)
#define ARM_CP15_C1_BRPRED_shf	11
#define ARM_CP15_C1_BRPRED_bit	(1 << ARM_CP15_C1_BRPRED_shf)
#define ARM_CP15_C1_ICACHE_shf	12
#define ARM_CP15_C1_ICACHE_bit	(1 << ARM_CP15_C1_ICACHE_shf)
#define ARM_CP15_C1_HIVEC_shf	13
#define ARM_CP15_C1_HIVEC_bit	(1 << ARM_CP15_C1_HIVEC_shf)
#define ARM_CP15_C1_CACHERR_shf	14
#define ARM_CP15_C1_CACHERR_bit	(1 << ARM_CP15_C1_CACHERR_shf)
#define ARM_CP15_C1_UNALIGN_shf 22
#define ARM_CP15_C1_UNALIGN_bit	(1 << ARM_CP15_C1_UNALIGN_shf)
#define ARM_CP15_C1_XPG_shf 	23
#define ARM_CP15_C1_XPG_bit	(1 << ARM_CP15_C1_XPG_shf)


/* MMU */

#define MMU_PERM_NA		0x0
#define MMU_PERM_PRW_UNA	0x1
#define MMU_PERM_PRW_URO	0x2
#define MMU_PERM_PRW_URW	0x3
#define MMU_PERM_RESV		0x4
#define MMU_PERM_PRO_UNA	0x5
#define MMU_PERM_PRO_URO	0x6
#define MMU_PERM_PRO_URO_2	0x7


#define MMU_P1_NUMENT(_ksplit)	(1 << (12 - (_ksplit)))
#define MMU_P1_FULL_LEN		MMU_P1_NUMENT(0)
#define MMU_P1_FULL_SZ		(MMU_P1_FULL_LEN * 4)
#define MMU_P1_FULL_ALIGN(_a)	\
	(((uint)(_a) + (MMU_P1_FULL_SZ - 1)) & ~(MMU_P1_FULL_SZ - 1))

#define MMU_CP15_TEX_REMAP_shf	28

#define MMU_TEX_NORMAL		0

#define MMU_PG_TYPE_shf		0
#define MMU_PG_TYPE_msk		0x3
#define MMU_PG_TYPE_FAULT	0
#define MMU_PG_TYPE_COARSE	1
#define MMU_PG_TYPE_SECTION	2
#define MMU_PG_TYPE_RESV	3

#define MMU_SEC_TYPE_shf	0
#define MMU_SEC_TYPE_msk	0x3	/* MMU_PG_TYPE_SECTION */
#define MMU_SEC_BUFABLE_shf	2
#define MMU_SEC_CACHEABLE_shf	3
#define MMU_SEC_XN_shf		4
#define MMU_SEC_DOMAIN_shf	5
#define MMU_SEC_DOMAIN_msk	0xF
#define MMU_SEC_AP_shf		10
#define MMU_SEC_AP_msk		0x3
#define MMU_SEC_TEX_shf		12
#define MMU_SEC_TEX_msk		0x7
#define MMU_SEC_APX_shf		15
#define MMU_SEC_SHARED_shf 	16
#define MMU_SEC_NOTGLOB_shf	17
#define MMU_SEC_SUPER_shf	18
#define MMU_SEC_NS_shf		19
#define MMU_SEC_BASE_shf	20
#define MMU_SEC_BASE_msk	0xFFF

#define MMU_SEC_DEVICE		((0 << MMU_SEC_BUFABLE_shf) | \
				 (0 << MMU_SEC_CACHEABLE_shf))
#define MMU_SEC_SHDEV		((0 << MMU_SEC_BUFABLE_shf) | \
				 (0 << MMU_SEC_CACHEABLE_shf))
#define MMU_SEC_WRTHRU		((0 << MMU_SEC_BUFABLE_shf) | \
				 (1 << MMU_SEC_CACHEABLE_shf))
#define MMU_SEC_WRBACK		((1 << MMU_SEC_BUFABLE_shf) | \
				 (1 << MMU_SEC_CACHEABLE_shf))

#define MMU_SECTION_NOMAP	(MMU_PG_TYPE_FAULT)

#define MMU_SECTION_DEVICE(_addr, _ap)					\
	((MMU_PG_TYPE_SECTION << MMU_SEC_TYPE_shf) |			\
	 (MMU_SEC_DEVICE) |						\
	 (((_ap) & MMU_SEC_AP_msk) << MMU_SEC_AP_shf) |			\
	 (MMU_TEX_NORMAL << MMU_SEC_DOMAIN_shf) |			\
	 ((((_ap) >> 2 & 1)) << MMU_SEC_APX_shf) |			\
	 ((_addr) & (MMU_SEC_BASE_msk << MMU_SEC_BASE_shf)))

#define MMU_SECTION_SHDEV(_addr, _ap)					\
	((MMU_PG_TYPE_SECTION << MMU_SEC_TYPE_shf) |			\
	 (MMU_SEC_NORMAL) |						\
	 (((_ap) & MMU_SEC_AP_msk) << MMU_SEC_AP_shf) |			\
	 (MMU_TEX_NORMAL << MMU_SEC_DOMAIN_shf) |			\
	 ((((_ap) >> 2 & 1)) << MMU_SEC_APX_shf) |			\
	 ((_addr) & (MMU_SEC_BASE_msk << MMU_SEC_BASE_shf)))

#define MMU_SECTION_WRTHRU(_addr, _ap)					\
	((MMU_PG_TYPE_SECTION << MMU_SEC_TYPE_shf) |			\
	 (MMU_SEC_WRTHRU) |						\
	 (((_ap) & MMU_SEC_AP_msk) << MMU_SEC_AP_shf) |			\
	 (MMU_TEX_NORMAL << MMU_SEC_DOMAIN_shf) |			\
	 ((((_ap) >> 2 & 1)) << MMU_SEC_APX_shf) |			\
	 ((_addr) & (MMU_SEC_BASE_msk << MMU_SEC_BASE_shf)))

#define MMU_SECTION_WRBACK(_addr, _ap)					\
	((MMU_PG_TYPE_SECTION << MMU_SEC_TYPE_shf) |			\
	 (MMU_SEC_WRBACK) |						\
	 (((_ap) & MMU_SEC_AP_msk) << MMU_SEC_AP_shf) |			\
	 (MMU_TEX_NORMAL << MMU_SEC_DOMAIN_shf) |			\
	 ((((_ap) >> 2 & 1)) << MMU_SEC_APX_shf) |			\
	 ((_addr) & (MMU_SEC_BASE_msk << MMU_SEC_BASE_shf)))

#define MMU_SEC_SET_DOMAIN(_sec, _domain)				\
	(((_sec) & ~(MMU_SEC_DOMAIN_msk << MMU_SEC_DOMAIN_shf)) | 	\
	 ((_domain) << MMU_SEC_DOMAIN_shf))


#define RPI_ADDR2SEC(_x) ((_x) >> 20)
#define RPI_ASPC_MEM_START	RPI_ADDR2SEC(0x00000000)
#define RPI_ASPC_MEM_END  	RPI_ADDR2SEC(0x20000000)
#define RPI_ASPC_DEV_START	RPI_ADDR2SEC(0x20000000)
#define RPI_ASPC_DEV_END  	RPI_ADDR2SEC(0x40000000)

#define RPI_TTB_PD1_shf		5
#define RPI_TTB_PD0_shf		4
#define RPI_TTB_SZ_shf		0

#define RPI_TTB_SZ_16K		0
#define RPI_TTB_SZ_8K		1
#define RPI_TTB_SZ_4K		2
#define RPI_TTB_SZ_2K		3
#define RPI_TTB_SZ_1K		4
#define RPI_TTB_SZ_512		5
#define RPI_TTB_SZ_256		6
#define RPI_TTB_SZ_128		7


void rpi_mmu_enable(uint32_t pgbase);

void rpi_mmu_disable(void);

void rpi_mmu_simple_phymap(uint l1pt[MMU_P1_FULL_LEN]);


/* CACHE */


#endif /* __RPI_H */

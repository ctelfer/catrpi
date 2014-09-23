/*
 * src/rpi.c -- Core C API functions
 *
 * by Christopher Adam Telfer
 *
 * Copyright 2014 See accompanying license
 *
 */

#include <rpi.h>

#ifndef UART_USE_IRQ
#define UART_USE_IRQ	0
#endif /* UART_USE_IRQ */

#ifndef MUART_USE_IRQ
#define MUART_USE_IRQ	0
#endif /* MUART_USE_IRQ */


volatile irq_handler_f irq_arm_vec[8];
volatile irq_handler_f irq_gpu_vec[64];
volatile static uint irq_cnt;

extern void _irq_disable(void);
extern void _irq_enable(void);

void irq_init(void)
{
	int i;
	_irq_disable();
	for ( i = 0; i < 8 ; ++i )
		irq_arm_vec[i] = NULL;
	for ( i = 0; i < 64 ; ++i )
		irq_gpu_vec[i] = NULL;
	irq_cnt = 1;
}


void irq_disable(void)
{
	_irq_disable();
	++irq_cnt;
}


void irq_enable(void)
{
	if ( !--irq_cnt )
		_irq_enable();
}


void irq_u_handler(void)
{
	uint pendb = peek32(IRQ_PENDB);
	uint irqs = pendb & IRQ_B_ARM_msk;
	uint irq;

	while ( irqs ) {
		irq = ffs(irqs);
		if ( irq_arm_vec[irq] != NULL )
			(*irq_arm_vec[irq])();
		irqs = irqs & (irqs - 1);
	}

	if ( pendb & IRQ_B_PEND1_msk ) {
		irqs = peek32(IRQ_PEND1);
		while ( irqs ) {
			irq = ffs(irqs);
			if ( irq_gpu_vec[irq] != NULL )
				(*irq_gpu_vec[irq])();
			irqs = irqs & (irqs - 1);
		}
	}

	if ( pendb & IRQ_B_PEND2_msk ) {
		irqs = peek32(IRQ_PEND2);
		while ( irqs ) {
			irq = ffs(irqs) + 32;
			if ( irq_gpu_vec[irq] != NULL )
				(*irq_gpu_vec[irq])();
			irqs = irqs & (irqs - 1);
		}
	}
}


void fiq_u_handler(void)
{
}




#define UART_BUFSIZE	1024
struct rpi_uart {
	uint rsts;
	uint rhead;
	uint rtail;
	uint shead;
	uint stail;
	uchar rbuf[UART_BUFSIZE];
	uchar sbuf[UART_BUFSIZE];
};

static struct rpi_uart uart;


int uart_txfifo_empty(void)
{
	return peek32(UART_FR) & UART_FR_TXFE;
}


int uart_txfifo_full(void)
{
	return peek32(UART_FR) & UART_FR_TXFF;
}


int uart_txfifo_fill(void)
{
	while ( uart.shead < uart.stail && !uart_txfifo_full() ) {
		poke32(UART_DR, uart.sbuf[uart.shead & (UART_BUFSIZE - 1)]);
		++uart.shead;
	}
	return uart.shead < uart.stail;
}


int uart_rxfifo_empty(void)
{
	return peek32(UART_FR) & UART_FR_RXFE;
}


void uart_rxfifo_drain(void)
{
	while ( !uart_rxfifo_empty() && 
		uart.rtail - uart.rhead < UART_BUFSIZE ) {
		uart.rbuf[uart.rtail & (UART_BUFSIZE - 1)] = peek32(UART_DR);
		++uart.rtail;
	}
}


void rpi_uart_irq_f(void)
{
	uart_rxfifo_drain();
	uart_txfifo_fill();
	poke32(UART_ICR, UART_INTR_ALL);
}


void rpi_uart_init(void)
{
	uint32_t x;

	poke32(UART_CR, UART_CR_DISABLE);

	uart.rsts = 0;

	/* Purpose GPIO pins 14 and 15 for the UART */
	/* See section 6.1 in the BCM2845 ARM */
	x = peek32(GPIO_FSEL1);
	x &= ~(GPIO_FSEL_msk << GPIO_FSEL_P14_shf);
	x &= ~(GPIO_FSEL_msk << GPIO_FSEL_P15_shf);
	x |= GPIO_FSEL_ALT0 << GPIO_FSEL_P14_shf;
	x |= GPIO_FSEL_ALT0 << GPIO_FSEL_P15_shf;
	poke32(GPIO_FSEL1, x);

	/* See table A6.28: turn off pull downs then spin for 150+ cycles */
	/* Next enable the clock for GPIO pins 14 and 15 and wait 150 more */
	/* Finally unclock the pins again. */
	poke32(GPIO_PUD, GPIO_PUD_OFF);
	for (x = 0; x < 150; x++) pass();
	poke32(GPIO_PUD_CLK0, (1 << 14) | (1 << 15));
	for (x = 0; x < 150; x++) pass();
	poke32(GPIO_PUD_CLK0, 0);

	/* clear interrupts, set baud divisor, set 8N1, enable UART */
	poke32(UART_ICR, UART_INTR_ALL);
	poke32(UART_IBRD, UART_IBRD_val(115200));
	poke32(UART_FBRD, UART_FBRD_val(115200));

#if UART_USE_IRQ
	/* set IRQ handler, set 8N1 and enable FIFO, turn on IRQs for UART */
	uart.rhead = 0;
	uart.rtail = 0;
	uart.shead = 0;
	uart.stail = 0;
	poke32(UART_IMSC, (UART_INTR_RX | UART_INTR_TX | UART_INTR_RTIM));
	irq_gpu_vec[IRQ_UART] = rpi_uart_irq_f;
	poke32(UART_LCRH, UART_LCRH_WLEN_8B | UART_LCRH_FEN);
	poke32(UART_IRQ_EN_REG, UART_IRQ_BIT);
#else
	poke32(UART_LCRH, UART_LCRH_WLEN_8B);
#endif
	/* Enable the UART, TX and RX */
	poke32(UART_CR, UART_CR_EN | UART_CR_TXE | UART_CR_RXE);
}


void rpi_uart_disable(void)
{
	irq_disable();
	poke32(UART_CR, UART_CR_DISABLE);
#if UART_USE_IRQ
	poke32(UART_LCRH, UART_LCRH_WLEN_8B);
	poke32(UART_IRQ_DIS_REG, UART_IRQ_BIT);
	poke32(UART_IMSC, 0);
	poke32(UART_ICR, UART_INTR_ALL);
	irq_gpu_vec[IRQ_UART] = NULL;
#endif
	irq_enable();
}


int rpi_uart_recv(void *data, int maxlen)
{
	uchar *dp = data;
#if !UART_USE_IRQ
	uart.rsts |= peek32(UART_RSRECR);
	/* copy in from input buffer */
	if ( maxlen <= 0 || (peek32(UART_FR) & UART_RFIFO_EMPTY) ) {
		return 0;
	} else {
		*dp = peek32(UART_DR);
		return 1;
	}
#else
	int i = 0;
	irq_disable();
	uart.rsts |= peek32(UART_RSRECR);
	while ( i < maxlen && uart.rhead < uart.rtail ) {
		*dp++ = uart.rbuf[uart.rhead & (UART_BUFSIZE - 1)];
		++uart.rhead;
		++i;
	}
	irq_enable();
	return i;
#endif
}


int rpi_uart_send(const void *data, int len)
{
	int i = 0;
	const uchar *dp = data;

#if !UART_USE_IRQ
	for ( i = 0; i < len; i++ ) {
		while ( peek32(UART_FR) & UART_TFIFO_FULL )
			;
		poke32(UART_DR, dp[i]);
	}
	return len;
#else
	irq_disable();
	while ( i < len && uart.stail - uart.shead < UART_BUFSIZE ) {
		uart.sbuf[uart.stail & (UART_BUFSIZE - 1)] = *dp++;
		++uart.stail;
		++i;
	}
	if ( uart_txfifo_empty() )
		uart_txfifo_fill();
	irq_enable();
	return i;
#endif
}


int rpi_uart_rsts_get(void) { return uart.rsts; } 

void rpi_uart_rsts_clr(void) { uart.rsts = 0; } 


void rpi_uart_flush_tx(void)
{
#if UART_USE_IRQ
	uint empty = 0;
	do {
		irq_disable();
		empty = uart.stail == uart.shead;
		irq_enable();
	} while ( !empty );
#endif
}


void timer_init(uint usec, irq_handler_f f)
{
	uint32_t x;

	/* disable for now */
	poke32(IRQ_TIMER_DIS_CSR, (1 << IRQ_TIMER));
	x = TIMER_CTL_CTRDIV_val(63);
	poke32(TIMER_CTL, x);

	irq_arm_vec[IRQ_TIMER] = f;
	poke32(TIMER_LOAD, usec-1);
	poke32(TIMER_RELOAD, usec-1);
	/* Timer comes up at 250 Mhz.  We want it at 1 Mhz */
	poke32(TIMER_PREDIV, TIMER_PREDIV_val(250));
	poke32(TIMER_IRQ_ACK, 0);
	x = TIMER_CTL_INTR_EN | TIMER_CTL_EN | TIMER_CTL_EN | TIMER_CTL_CTRSIZ;
	x |= TIMER_CTL_CTRDIV_val(63);
	poke32(TIMER_CTL, x);

	/* enable timer interrupt */
	poke32(IRQ_TIMER_EN_CSR, (1 << IRQ_TIMER));
}


uint32_t timer_ctr(void)
{
	return peek32(TIMER_CTR);
}


uint32_t systime(void)
{
	return peek32(SYSTIME_LO);
}


/* compute return x / y, r = x % y */
uint uidivmod(uint x, uint y, uint *r)
{
	int i;
	uint z = 0;

	if ( x == 0 ) {
		if ( r ) 
			*r = 0;
		return 0xFFFFFFFF;
	}

	/* treat z||x as the dividend */
	/* shift answer into bottom of x */
	for ( i = 0; i < 31; ++i ) {
		z = (z << 1) | (x >> 31);
		x <<= 1;
		if ( z >= y ) {
			z = z - y;
			x = x + 1;
		}
	}

	if ( r )
		*r = z;

	return x;
}


uint __aeabi_uidiv(uint x, uint y)
{
	return uidivmod(x, y, NULL);
}


ullong __aeabi_uidivmod(uint x, uint y)
{
	uint q;
	uint r;
	q = uidivmod(x, y, &r);
	return (ullong)q | (ullong)r << 32;
}


extern uint _cpop(uint arg1, uint arg2);

uint coproc_read(uint cp, uint crn, uint op1, uint crm, uint op2)
{
	volatile uint *op = (volatile uint *)&_cpop + 4;
       	*op =  ARM_MRC(cp, op1, crn, op2, crm, 0);
	return _cpop(0, 0);
}


void coproc_write(uint cp, uint crn, uint op1, uint crm, uint op2, uint val)
{
	volatile uint *op = (volatile uint *)&_cpop + 4;
	*op = ARM_MCR(cp, op1, crn, op2, crm, 0);
	_cpop(val, 0);
}


uint cp15_read(uint crn, uint op1, uint crm, uint op2)
{
	return coproc_read(15, crn, op1, crm, op2);
}


void cp15_write(uint crn, uint op1, uint crm, uint op2, uint val)
{
	coproc_write(15, crn, op1, crm, op2, val);
}


void cp15_set(uint crn, uint op1, uint crm, uint op2, uint bits)
{
	uint x;
	x = cp15_read(crn, op1, crm, op2);
	x |= bits;
	cp15_write(crn, op1, crm, op2, x);
}


void cp15_clr(uint crn, uint op1, uint crm, uint op2, uint bits)
{
	uint x;
	x = cp15_read(crn, op1, crm, op2);
	x &= ~bits;
	cp15_write(crn, op1, crm, op2, x);
}


void rpi_mmu_disable(void)
{
	uint x;

	/*
	 * From the databook:
	 * 1) Clear bits [2:0] of CP15 c1 of the corresponding world to
	 *    disable the data cache while disabling MMU.
	 */
	irq_disable();
	x = ARM_CP15_C1_MMU_bit |
	    ARM_CP15_C1_AFAULT_bit |
	    ARM_CP15_C1_DCACHE_bit |
	    ARM_CP15_C1_ICACHE_bit;
	cp15_clr(cpr_c1, 0, cpr_c0, 0, x);
	irq_enable();
}


void rpi_mmu_enable(uint32_t pgbase)
{
	/* MMU_P1_FULL_ALIGN & pgbase must equal 0 */

	/*
	 * From the databook:
	 * 1) Program "all relevant CP15 registers for the corresponding world.
	 * 2) Program first and second level page tables (assumed done already)
	 * 3) Disable and invalidate the instruction cache for the world
	 * 4) Enable MMU by setting bit 0 in the CP15 c1 control register of the
	 *    corresponding world. 
	 */
	irq_disable();

	/* Disable both caches */
	cp15_clr(cpr_c1, 0, cpr_c0, 0, 
		 ARM_CP15_C1_ICACHE_bit | ARM_CP15_C1_DCACHE_bit |
		 ARM_CP15_C1_AFAULT_bit | ARM_CP15_C1_MMU_bit);

	/* Invalidate both caches */
	cp15_write(cpr_c7, 0, cpr_c7, 0, 0);

	/* Invalidate TLBs */
	cp15_write(cpr_c8, 0, cpr_c7, 0, 0);

	/* Set all domains to full access */
	cp15_write(cpr_c3, 0, cpr_c0, 0, 0xFFFFFFFF);

	/* TTBCR */
	cp15_write(cpr_c2, 0, cpr_c0, 2, RPI_TTB_SZ_16K << RPI_TTB_SZ_shf);

	/* TTB0 */
	cp15_write(cpr_c2, 0, cpr_c0, 0, pgbase);

	/* TTB1 */
	cp15_write(cpr_c2, 0, cpr_c0, 1, pgbase);

	/* Enable MMU */
	cp15_set(cpr_c1, 0, cpr_c0, 0, 
		 ARM_CP15_C1_ICACHE_bit | 
		 ARM_CP15_C1_DCACHE_bit | 
		 ARM_CP15_C1_MMU_bit);

	irq_enable();

}


void rpi_mmu_simple_phymap(uint l1pt[MMU_P1_FULL_LEN])
{
	int i;

	for ( i = 0; i < MMU_P1_FULL_LEN; ++i ) {
		if ( i >= RPI_ASPC_MEM_START && i < RPI_ASPC_MEM_END ) {
			l1pt[i] = MMU_SECTION_WRBACK(i << MMU_SEC_BASE_shf, 
						     MMU_PERM_PRW_URW);
		} else if ( i >= RPI_ASPC_DEV_START && i < RPI_ASPC_DEV_END ) {
			l1pt[i] = MMU_SECTION_DEVICE(i << MMU_SEC_BASE_shf, 
						     MMU_PERM_PRW_URW);
		} else {
			l1pt[i] = MMU_SECTION_NOMAP;
		}
	}
}

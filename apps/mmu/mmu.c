#include <rpi.h>
#include <stdio.h>
#include <cat/catlibc.h>

void uprintf(const char *fmt, ...)
{
	char str[256];
	int len;
	va_list ap;
	va_start(ap, fmt);
	len = vsnprintf(str, sizeof(str), fmt, ap);
	va_end(ap);
	rpi_uart_send(str, len);
}


void ptwalk(uint32_t addr)
{
	uint32_t x;
	uint32_t pte;
	uint32_t va;
	uint32_t dp;

	uprintf("Reading address %08x\r\n", addr);
	x = cp15_read(cpr_c2, 0, cpr_c0, 0);
	uprintf("L1 page table base address %08x\r\n", x);
	x = (x & 0xFFFFC000) | (addr >> 20) << 2;
	pte = peek32(x);
	uprintf("L1 page table entry at %08x: %08x\r\n", x, pte);
	switch ((pte >> MMU_PG_TYPE_shf) & MMU_PG_TYPE_msk) {
	case MMU_PG_TYPE_FAULT:
		uprintf("\tFault type:  abort search\r\n");
		break;
	case MMU_PG_TYPE_COARSE:
		uprintf("\tCoarse type:  abort search\r\n");
		break;
	case MMU_PG_TYPE_SECTION:
		uprintf("\tSection type\r\n");
		break;
	case MMU_PG_TYPE_RESV:
		uprintf("\tReserved type:  abort search\r\n");
		break;
	}

	va = (addr & 0x000FFFFF) | (pte & 0xFFF00000);
	uprintf("\tTranslated address = %08x\r\n", va);
	uprintf("\tWorld = %s\r\n", (pte & (1 << MMU_SEC_NS_shf)) ? 
				  "not secure" :
				  "secure");
	uprintf("\tBufferable = %x\r\n", (pte >> MMU_SEC_BUFABLE_shf) & 1);
	uprintf("\tCacheable = %x\r\n", (pte >> MMU_SEC_CACHEABLE_shf) & 1);
	uprintf("\tTEX = %x\r\n", (pte >> MMU_SEC_TEX_shf) & MMU_SEC_TEX_msk);
	uprintf("\tAP = %x\r\n", (pte >> MMU_SEC_AP_shf) & MMU_SEC_AP_msk);
	x = (pte >> MMU_SEC_DOMAIN_shf) & MMU_SEC_DOMAIN_msk;
	dp = (cp15_read(cpr_c3, 0, cpr_c0, 0) >> (x * 2)) & 3;
	uprintf("\tDomain = %x -> permission = %x\r\n", x, dp);
	x = peek32(va);
	uprintf("\tValue = %08x / %u\r\n", x, x);
}


uint _cpop(uint x);

uint l1pt[MMU_P1_FULL_LEN*2];
int X;


void main(void)
{
	uint *rpt;

	X = 12345;

	irq_init();
	rpi_uart_init();
	irq_enable();

	uprintf("starting up\r\n");
	uprintf("CP15.c1.c0 = %08x\r\n", cp15_read(cpr_c1, 0, cpr_c0, 0));
	rpt = (uint *)MMU_P1_FULL_ALIGN(l1pt);
	uprintf("Page table base = %08x\r\n", (uint)rpt);
	rpi_mmu_simple_phymap(rpt);
	uprintf("Page table\r\n", (uint)rpt);
	uprintf("0x00000000: %08x\r\n", rpt[0]);
	uprintf("0x00100000: %08x\r\n", rpt[1]);
	uprintf("0x1ff00000: %08x\r\n", rpt[511]);
	uprintf("0x20000000: %08x\r\n", rpt[512]);
	uprintf("CP15.c3.c0 = %08x\r\n", cp15_read(cpr_c3, 0, cpr_c0, 0));
	uprintf("_cpop instr0 = 0x%08x\r\n", peek32(((uint)&_cpop) + 0));
	uprintf("_cpop instr1 = 0x%08x\r\n", peek32(((uint)&_cpop) + 4));
	uprintf("_cpop instr2 = 0x%08x\r\n", peek32(((uint)&_cpop) + 8));
	uprintf("_cpop instr3 = 0x%08x\r\n", peek32(((uint)&_cpop) + 12));
	uprintf("_cpop instr4 = 0x%08x\r\n", peek32(((uint)&_cpop) + 16));
	uprintf("_cpop instr5 = 0x%08x\r\n", peek32(((uint)&_cpop) + 20));
	uprintf("Enabling MMU\r\n");
	rpi_uart_flush_tx();
	rpi_mmu_enable((uint)rpt);
	uprintf("MMU is running now\r\n");
	uprintf("Domain = %08x\r\n", cp15_read(cpr_c3, 0, cpr_c0, 0));
	uprintf("CP15.c1.c0 = %08x\r\n", cp15_read(cpr_c1, 0, cpr_c0, 0));
	ptwalk(UART_CR);
	ptwalk((uint32_t)&X);
}

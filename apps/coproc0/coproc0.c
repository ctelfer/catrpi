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


void main(void)
{
	irq_init();
	rpi_uart_init();
	irq_enable();
	uprintf("starting up\r\n");
	uprintf("Main ID (CP15.c0.c0:0) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c0, 0));
	uprintf("Cache type (CP15.c0.c0:1) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c0, 1));
	uprintf("TCM status (CP15.c0.c0:2) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c0, 2));
	uprintf("TLB type (CP15.c0.c0:3) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c0, 3));
	uprintf("Processor feature 0 (CP15.c0.c1:0) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c1, 0));
	uprintf("Processor feature 1 (CP15.c0.c1:1) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c1, 1));
	uprintf("Debug feature 0 (CP15.c0.c1:2) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c1, 2));
	uprintf("Auxilliary feature 0 (CP15.c0.c1:3) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c1, 3));
	uprintf("Memory feature 0 (CP15.c0.c1:4) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c1, 4));
	uprintf("Memory feature 1 (CP15.c0.c1:5) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c1, 5));
	uprintf("Memory feature 2 (CP15.c0.c1:6) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c1, 6));
	uprintf("Memory feature 3 (CP15.c0.c1:7) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c1, 7));
	uprintf("Instr set featurn 0 (CP15.c0.c2:0) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c2, 0));
	uprintf("Instr set featurn 1 (CP15.c0.c2:1) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c2, 1));
	uprintf("Instr set featurn 2 (CP15.c0.c2:2) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c2, 2));
	uprintf("Instr set featurn 3 (CP15.c0.c2:3) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c2, 3));
	uprintf("Instr set featurn 4 (CP15.c0.c2:4) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c2, 4));
	uprintf("Instr set featurn 5 (CP15.c0.c2:5) = %08x\r\n",
		cp15_read(cpr_c0, 0, cpr_c2, 5));
	uprintf("CP15.c1.c0 = %08x\r\n", cp15_read(cpr_c1, 0, cpr_c0, 0));
	uprintf("CP15.c2.c0.op2->0 = %08x\r\n",
		cp15_read(cpr_c2, 0, cpr_c0, 0));
	uprintf("CP15.c2.c0.op2->1 = %08x\r\n",
		cp15_read(cpr_c2, 0, cpr_c0, 1));
	uprintf("CP15.c2.c0.op2->2 = %08x\r\n",
		cp15_read(cpr_c2, 0, cpr_c0, 2));
	uprintf("enabling alignment checking\r\n");
	cp15_set(cpr_c1, 0, cpr_c0, 0, ARM_CP15_C1_AFAULT_bit);
	uprintf("CP15.c1.c0 = %08x\r\n", cp15_read(cpr_c1, 0, cpr_c0, 0));
}

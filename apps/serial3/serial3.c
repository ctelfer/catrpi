#include <rpi.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <cat/str.h>

void prx(uint32_t x)
{
	char str[64] = "0x";
	snprintf(str, sizeof(str), "0x%08x\r\n", x);
	rpi_uart_send(str, strlen(str));
}


void prd(uint32_t x)
{
	char str[32] = "dd";
	snprintf(str, sizeof(str), "%u\r\n", x);
	rpi_uart_send(str, strlen(str));
}


void uart_puts(const char *s)
{
	rpi_uart_send(s, strlen(s));
}


void main(void)
{
	uchar c;
	char nl = '\n';
	const char hw[] = "Hello World!\r\n";
	char str[80];

	irq_init();
	rpi_uart_init();
	irq_enable();
	rpi_uart_send(hw, sizeof(hw)-1);
	uart_puts(hw);
	str_copy(str, "hi there again\r\n", sizeof(str));
	uart_puts(str);
	snprintf(str, sizeof(str), "snprintf() test\r\n");
	uart_puts(str);
	prx(peek32(0));
	prx(peek32((uint)hw));
	prx(12345);
	prx(0xFFFFFFFF);
	prd(peek32(0));
	prd(peek32((uint)hw));
	prd(12345);
	prd(0xFFFFFFFF);

	while ( 1 ) {
		if ( rpi_uart_recv(&c, 1) ) {
			if ( c == '\r' )
				rpi_uart_send(&nl, 1);
			rpi_uart_send(&c, 1);
		} 
	}
}

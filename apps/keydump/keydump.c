#include <rpi.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <cat/str.h>

void uart_puts(const char *s)
{
	rpi_uart_send(s, strlen(s));
}


void main(void)
{
	uchar c;
	const char hw[] = "Hello World!\r\n";
	char str[80];
	int n;

	irq_init();
	rpi_uart_init();
	irq_enable();
	rpi_uart_send(hw, sizeof(hw)-1);
	uart_puts("Enter your keys now\r\n");

	while ( 1 ) {
		if ( !rpi_uart_recv(&c, 1) )
			continue;

		n = snprintf(str, sizeof(str), "Received 0x%02x/%d\r\n", c, c);
		rpi_uart_send(str, n);
	}
}

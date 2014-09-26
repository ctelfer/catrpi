#include <rpi.h>

extern int uart_txfifo_fill(void);


uint strlen(const char *s)
{
	uint len = 0;
	while ( *s++ != '\0' ) ++len;
	return len;
}


void prx(uint32_t x)
{
	char str[16] = "0x";
	char *cp = str + 2;
	int i;
	int d;
	for ( i = 28; i >= 0; i -= 4 ) {
		d = (x >> i) & 0xF;
		*cp++ = d < 10 ? d + '0' : d - 10 + 'A';
	}
	*cp++ = '\r';
	*cp++ = '\n';
	rpi_uart_send(str, cp - str);
}


void uart_puts(const char *s)
{
	rpi_uart_send(s, strlen(s));
}


void uart_tx_flush(void)
{
	while ( uart_txfifo_fill() )
		;
}


void main(void)
{
	uchar c;
	char nl = '\n';
	const char hw[] = "Hello World!\r\n";
	uint i;

	irq_init();
	rpi_uart_init();
	irq_enable();
	rpi_uart_send(hw, sizeof(hw)-1);

	i = 0;
	while ( 1 ) {
		if ( rpi_uart_recv(&c, 1) ) {
			if ( c == '\r' )
				rpi_uart_send(&nl, 1);
			rpi_uart_send(&c, 1);
			i = 0;
		} else {
			++i;
			if ( i > 0xFFFFF ) {
				irq_disable();
				uart_puts("Idle ping...\r\n");
				uart_tx_flush();
				irq_enable();
				i = 0;
			}
		}
	}
}

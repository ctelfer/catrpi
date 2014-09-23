#include <rpi.h>

void main(void)
{
	uchar c;
	char nl = '\n';
	const char hw[] = "\r\nHello World!\r\n";

	rpi_uart_init();
	irq_enable();

	rpi_uart_send(hw, sizeof(hw)-1);

	while ( 1 ) {
		if ( rpi_uart_recv(&c, 1) >= 1 ) {
			rpi_uart_send(&c, 1);
			if ( c == '\r' )
				rpi_uart_send(&nl, 1);
		}
	}
}

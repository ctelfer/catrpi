#include <rpi.h>

uint32_t ticks;
uint32_t ncc;

void tick(void)
{
	uchar cc = 'C';
	uchar nl[] = "\r\n";
	++ticks;
	if ( (ticks & 1023) == 0 ) {
		++ncc;
		rpi_uart_send(&cc, 1);
		if ( ncc == 64 )
			rpi_uart_send(&nl, sizeof(nl)-1);
	}
	poke32(TIMER_IRQ_ACK, 0);
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


uint32_t get_cpsr(void);


void main(void)
{
	const char hw[] = "\r\nHello World!\r\n";
	int i;

	ticks = 0;
	ncc = 0;

	irq_init();
	rpi_uart_init();
	rpi_uart_send(hw, sizeof(hw)-1);
	timer_init(1000, &tick);

	for ( i = 0; i < 64 ; i += 4 )
		prx(*(uint *)i);
	prx(get_cpsr());

	irq_enable();

	prx(get_cpsr());
	prx(peek32(IRQ_PENDB));
	prx(0xdeadbeef);

	while ( 1 ) ;
#if 0
	{
		++cnt;
		if ( peek32(TIMER_MSK_IRQ) ) {
			++ms;
			if ( ms >= 1000 ) {
				prx(nirq);
				rpi_uart_send(&ca, 1);
				ms = 0;
			}
			poke32(TIMER_IRQ_ACK, 0);
			cnt = 0;
		} else if ( cnt > 2000000000 ) {
			rpi_uart_send(&cb, 1);
			cnt = 0;
		}
	}
#endif
}

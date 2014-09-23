#include <rpi.h>
#include <xmodem.h>

static int uart_send_all(const void *p, int len);
static uint get_ms(void);

struct xm_ops xmo = { &rpi_uart_recv, &uart_send_all, &get_ms };
struct xm_receiver xmr;

/* approximate */
uint get_ms(void) { return systime() >> 10; }


static int uart_send_all(const void *p, int len)
{
	int nr;
	int sent = 0;
	const uchar *cp = p;
	while ( sent < len ) {
		nr = rpi_uart_send(cp, len - sent);
		if ( nr < 0 )
			return -1;
		cp += nr;
		sent += nr;
	}
	return sent;
}

static void uart_puts(const char *s)
{
	const char *sp = s;
	uint len = 0;
	while ( *sp++ != '\0' ) ++len;
	rpi_uart_send(s, len);
}


static void uart_gets(char *s, uint maxlen)
{
	uint n = 0;
	uchar nl = '\n';

	*s = '\0';
	while ( *s != '\r' ) {
		while ( rpi_uart_recv(s, 1) < 1 ) ;
		rpi_uart_send(s, 1);	/* echo */
		if ( *s == '\b' ) {
			if ( n > 0 ) {
				--s;
				--n;
			}
		} else if ( *s == '\r' ) {
			rpi_uart_send(&nl, 1);
		} else if ( n < maxlen - 1 ) {
			++s;
			++n;
		}
	}
	*s = '\0';
}


static int ishex(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || 
	       (c >= 'a' && c <= 'f');
}


const char *skipws(const char *cp)
{
	while ( *cp == ' ' ) ++cp;
	return cp;
}


uint xstr2int(const char **cp)
{
	uint x = 0;
	while ( ishex(**cp) ) {
		if ( **cp >= '0' && **cp <= '9' )
			x = (x << 4) | (**cp - '0');
		else if ( **cp >= 'A' && **cp <= 'F' )
			x = (x << 4) | (**cp - 'A' + 10);
		else
			x = (x << 4) | (**cp - 'a' + 10);
		*cp += 1;
	}
	return x;
}


void prxb(uchar x)
{
	char str[8];
	char *cp = str;
	uint d;
	d = (x >> 4) & 0xF;
	*cp++ = d < 10 ? d + '0' : d - 10 + 'A';
	d = x & 0xF;
	*cp++ = d < 10 ? d + '0' : d - 10 + 'A';
	rpi_uart_send(str, cp - str);
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


static void mcpy(uchar *dst, uchar *src, uint len)
{
	while ( len > 0 ) {
		*dst++ = *src++;
		--len;
	}
}


static void xmrecv(uint32_t addr)
{
	uchar *p = (uchar *)addr;
	int rv; 

	rpi_uart_rsts_clr();
	xmr_init(&xmr, &xmo);
	while ( (rv = xmr_recv(&xmr)) != 0 ) {
		if ( rv == 1 ) {
			mcpy(p, xmr_data(&xmr), XM_BLKSIZ);
			p += XM_BLKSIZ;
		} else {
			/* error */
			return;
		}
	}
}


const char greeting[] = 
	"\r\nRaspberry PI boot loader\r\n"
	"All numbers are in hex.\r\n";
const char commands[] =
	"Commands are:\r\n"
	"\tRead word:\tr ADDR\r\n"
	"\tWrite word:\tw ADDR VAL\r\n"
	"\tXmodem recv:\tx [ADDR]\r\n"
	"\tStart at:\ts [ADDR]\r\n"
	"\tSystem time:\tt\r\n"
	"\tXmodem status:\te\r\n"
	"\tPrint help:\t?\r\n";
const char prompt[] = "> ";


#define DEFAULT_XMODEM_ADDR 0x10000
void runcmd(const char *cp)
{
	uint addr;
	uint val;
	int i;

	cp = skipws(cp);
	if ( *cp == 'r' ) {
		cp = skipws(cp+1);
		prx(peek32(xstr2int(&cp)));
	} else if ( *cp == 'w' ) { 
		cp = skipws(cp+1);
		addr = xstr2int(&cp);
		cp = skipws(cp);
		val = xstr2int(&cp);
		poke32(addr, val);
	} else if ( *cp == 'x' ) { 
		cp = skipws(cp+1);
		addr = DEFAULT_XMODEM_ADDR;
		if ( ishex(*cp) )
			addr = xstr2int(&cp);
		xmrecv(addr);
	} else if ( *cp == 's' ) { 
		cp = skipws(cp+1);
		addr = DEFAULT_XMODEM_ADDR;
		if ( ishex(*cp) )
			addr = xstr2int(&cp);
		irq_init();
		rpi_uart_disable();
		(*(void (*)(void))addr)();
	} else if ( *cp == 't' ) { 
		uart_puts("The time is: ");
		prx(get_ms() >> 10);
	} else if ( *cp == 'e' ) {
		uart_puts("Last XMODEM error: ");
		uart_puts(xm_errstr[xmr.error]);
		uart_puts("\r\n");
		uart_puts("# of bytes received: ");
		prx((xmr.nxtblk - 1) * 128);
		uart_puts("Expected next block: ");
		prx(xmr.nxtblk);
		uart_puts("Receive status: ");
		prx(rpi_uart_rsts_get());
		uart_puts("Last block:\r\n");
		for ( i = 0; i < 132; i += 4 ) {
			prxb(xmr.buf[i]);
			prxb(xmr.buf[i+1]);
			prxb(xmr.buf[i+2]);
			prxb(xmr.buf[i+3]);
			uart_puts("\r\n");
		}
	} else {
		if ( *cp != 'h' && *cp != '?' )
			uart_puts("Invalid command\r\n");
		uart_puts(commands);
	}
}


void main(void)
{
	char line[256];

	irq_init();
	rpi_uart_init();
	xmr_init(&xmr, &xmo);

	uart_puts(greeting);
	uart_puts(commands);
	while ( 1 ) {
		uart_puts(prompt);
		uart_gets(line, sizeof(line));
		runcmd(line);
	}
}

#include <rpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cat/catlibc.h>
#include <cat/str.h>
#include <cat/rbtree.h>
#include <cat/stduse.h>

#define NOPS 256
#define NITER (NOPS * 128)


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


void rbtest(void)
{
	struct rbtree t;
	struct rbnode nodes[NOPS];
	struct rbnode *old;
	uint start;
	uint end;
	int i;
	int j;
	int dir;
	char *k;

	uprintf("Initializing RB-tree test\r\n");
	for ( i = 0; i < NOPS; ++i ) {
		k = str_fmt_a("node%d", i);
		rb_ninit(&nodes[i], k, NULL);

		if (i % 128 == 0)
			uprintf("key %d = %s @ %x\r\n", i, k, (uint)k);
	}
	rb_init(&t, cmp_str);

	uprintf("Starting test\r\n");
	start = systime();
	for ( i = 0; i < NITER / NOPS; ++i ) {
		for ( j = 0; j < NOPS; ++j ) {
			old = rb_lkup(&t, nodes[j].key, &dir);
			if ( dir == CRB_N ) {
				uprintf("Found duplicate key %s on %d,%d\r\n",
					nodes[j].key, i, j);
				return;
			}
			rb_ins(&t, &nodes[j], old, dir);
		}
		for ( j = 0; j < NOPS; ++j )
			rb_rem(&nodes[j]);
	}
	end = systime();

	uprintf("%u ticks to perform %u ops\r\n", end - start, NITER);
	
	for ( i = 0; i < NOPS; ++i ) {
		if ( i % 128 == 0 )
		       uprintf("freeing key %i = %s -- %p\r\n", i,
			       nodes[i].key, nodes[i].key);
		free(nodes[i].key);
	}

	uprintf("Test complete: memory freed\r\n");
}


uint l1pt[MMU_P1_FULL_LEN*2];


void main(void)
{
	uint *rpt;
	ulong heap[8 * 1024 * 1024];
	struct catlibc_cfg ccfg;

	memset(&ccfg, 0, sizeof(ccfg));
	ccfg.heap_base = heap;
	ccfg.heap_sz = sizeof(heap);
	catlibc_reset(&ccfg);

	irq_init();
	rpi_uart_init();
	irq_enable();

	uprintf("starting up\r\n");
	rpt = (uint *)MMU_P1_FULL_ALIGN(l1pt);
	uprintf("Page table base = %08x\r\n", (uint)rpt);
	rpi_mmu_simple_phymap(rpt);

	uprintf("Enabling MMU\r\n");
	rpi_uart_flush_tx();
	rpi_mmu_enable((uint)rpt);
	uprintf("CP15.c1.c0 = %08x\r\n", cp15_read(cpr_c1, 0, cpr_c0, 0));

	rbtest();

	while ( 1 ) ;
}

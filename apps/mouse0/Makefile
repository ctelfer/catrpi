BASE=../..
CATLIB=$(BASE)/../catlib
ARMGNU=arm-none-eabi
ORIGIN=0x10000
LENGTH=0x1000000


ASFLAGS=--defsym origin=$(ORIGIN)

CFLAGS=-Wall -O2 -nostdlib -nostartfiles -ffreestanding -DUART_USE_IRQ=1 \
       -DCAT_USE_STDLIB=0 -DCAT_HAS_POSIX=0 -DCAT_HAS_LONGLONG=0 \
       -I$(BASE)/src -I$(CATLIB)/include/std -I$(CATLIB)/include

LDFLAGS= -L$(CATLIB)/lib -lcat_nolibc


all: mouse0.hex mouse0.bin

clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.hex
	rm -f *.elf
	rm -f *.list
	rm -f memmap

memmap:
	ORIGIN=$(ORIGIN) LENGTH=$(LENGTH) $(BASE)/scripts/ldmm.sh

core.o: $(BASE)/src/core.s
	$(ARMGNU)-as $(ASFLAGS) $(BASE)/src/core.s -o core.o

rpi.o: $(BASE)/src/rpi.c
	$(ARMGNU)-gcc $(CFLAGS) -c $(BASE)/src/rpi.c -o rpi.o

ansi.o: $(BASE)/src/ansi.c
	$(ARMGNU)-gcc $(CFLAGS) -c $(BASE)/src/ansi.c -o ansi.o

mouse0.o: mouse0.c
	$(ARMGNU)-gcc $(CFLAGS) -c mouse0.c -o mouse0.o

mouse0.elf: memmap core.o rpi.o ansi.o mouse0.o
	$(ARMGNU)-ld core.o rpi.o ansi.o mouse0.o -T memmap -o mouse0.elf \
	       	$(LDFLAGS)
	$(ARMGNU)-objdump -D mouse0.elf > mouse0.list

mouse0.bin: mouse0.elf
	$(ARMGNU)-objcopy mouse0.elf -O binary mouse0.bin

mouse0.hex : mouse0.elf
	$(ARMGNU)-objcopy mouse0.elf -O ihex mouse0.hex


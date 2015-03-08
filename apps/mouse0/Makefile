BASE=../..
CATLIB=$(BASE)/../catlib
ARMGNU=arm-none-eabi
ORIGIN=0x10000
LENGTH=0x1000000


ASFLAGS=--defsym origin=$(ORIGIN)

CFLAGS=-Wall -O2 -nostdlib -nostartfiles -ffreestanding -DUART_USE_IRQ=1 \
       -DCAT_USE_STDLIB=0 -DCAT_HAS_POSIX=0 -DCAT_HAS_LONGLONG=0 \
       -I$(BASE)/include -I$(CATLIB)/include/std -I$(CATLIB)/include

LDFLAGS= -L$(CATLIB)/lib -lcat_nolibc


all: mouse.hex mouse.bin

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

mouse.o: mouse.c
	$(ARMGNU)-gcc $(CFLAGS) -c mouse.c -o mouse.o

mouse.elf: memmap core.o rpi.o ansi.o mouse.o
	$(ARMGNU)-ld core.o rpi.o ansi.o mouse.o -T memmap -o mouse.elf \
	       	$(LDFLAGS)
	$(ARMGNU)-objdump -D mouse.elf > mouse.list

mouse.bin: mouse.elf
	$(ARMGNU)-objcopy mouse.elf -O binary mouse.bin

mouse.hex : mouse.elf
	$(ARMGNU)-objcopy mouse.elf -O ihex mouse.hex


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


all: keydump.hex keydump.bin

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

keydump.o: keydump.c
	$(ARMGNU)-gcc $(CFLAGS) -c keydump.c -o keydump.o

keydump.elf: memmap core.o rpi.o keydump.o 
	$(ARMGNU)-ld core.o keydump.o rpi.o -T memmap -o keydump.elf $(LDFLAGS)
	$(ARMGNU)-objdump -D keydump.elf > keydump.list

keydump.bin: keydump.elf
	$(ARMGNU)-objcopy keydump.elf -O binary keydump.bin

keydump.hex : keydump.elf
	$(ARMGNU)-objcopy keydump.elf -O ihex keydump.hex


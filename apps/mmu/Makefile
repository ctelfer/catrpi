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


all: mmu.hex mmu.bin

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

mmu.o: mmu.c
	$(ARMGNU)-gcc $(CFLAGS) -c mmu.c -o mmu.o

mmu.elf: memmap core.o rpi.o mmu.o 
	$(ARMGNU)-ld core.o mmu.o rpi.o -T memmap -o mmu.elf $(LDFLAGS)
	$(ARMGNU)-objdump -D mmu.elf > mmu.list

mmu.bin: mmu.elf
	$(ARMGNU)-objcopy mmu.elf -O binary mmu.bin

mmu.hex : mmu.elf
	$(ARMGNU)-objcopy mmu.elf -O ihex mmu.hex


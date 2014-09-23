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


all: coproc0.hex coproc0.bin

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

coproc0.o: coproc0.c
	$(ARMGNU)-gcc $(CFLAGS) -c coproc0.c -o coproc0.o

coproc0.elf: memmap core.o rpi.o coproc0.o 
	$(ARMGNU)-ld core.o coproc0.o rpi.o -T memmap -o coproc0.elf $(LDFLAGS)
	$(ARMGNU)-objdump -D coproc0.elf > coproc0.list

coproc0.bin: coproc0.elf
	$(ARMGNU)-objcopy coproc0.elf -O binary coproc0.bin

coproc0.hex : coproc0.elf
	$(ARMGNU)-objcopy coproc0.elf -O ihex coproc0.hex


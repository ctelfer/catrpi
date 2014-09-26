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


all: serial3.hex serial3.bin

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

serial3.o: serial3.c
	$(ARMGNU)-gcc $(CFLAGS) -c serial3.c -o serial3.o

serial3.elf: memmap core.o rpi.o serial3.o 
	$(ARMGNU)-ld core.o serial3.o rpi.o -T memmap -o serial3.elf $(LDFLAGS)
	$(ARMGNU)-objdump -D serial3.elf > serial3.list

serial3.bin: serial3.elf
	$(ARMGNU)-objcopy serial3.elf -O binary serial3.bin

serial3.hex : serial3.elf
	$(ARMGNU)-objcopy serial3.elf -O ihex serial3.hex


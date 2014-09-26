BASE=../..
CATLIB=$(BASE)/../catlib
ARMGNU=arm-none-eabi
ORIGIN=0x8000
LENGTH=0x10000


ASFLAGS=--defsym origin=$(ORIGIN)

CFLAGS=-Wall -O2 -nostdlib -nostartfiles -ffreestanding -I$(BASE)/include \
       -DUART_USE_IRQ=1

LDFLAGS=


all: serial1.hex serial1.bin

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

serial1.o: serial1.c
	$(ARMGNU)-gcc $(CFLAGS) -c serial1.c -o serial1.o

serial1.elf: memmap core.o rpi.o serial1.o 
	$(ARMGNU)-ld core.o serial1.o rpi.o -T memmap -o serial1.elf $(LDFLAGS)
	$(ARMGNU)-objdump -D serial1.elf > serial1.list

serial1.bin: serial1.elf
	$(ARMGNU)-objcopy serial1.elf -O binary serial1.bin

serial1.hex : serial1.elf
	$(ARMGNU)-objcopy serial1.elf -O ihex serial1.hex


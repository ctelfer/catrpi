BASE=../..
ARMGNU=arm-none-eabi
ORIGIN=0x10000
LENGTH=0x10000


ASFLAGS=--defsym origin=$(ORIGIN)

CFLAGS=-Wall -O2 -nostdlib -nostartfiles -ffreestanding -I$(BASE)/src \
       -DUART_USE_IRQ=1

LDFLAGS=


all: serial2.hex serial2.bin

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

serial2.o: serial2.c
	$(ARMGNU)-gcc $(CFLAGS) -c serial2.c -o serial2.o

serial2.elf: memmap core.o rpi.o serial2.o 
	$(ARMGNU)-ld core.o serial2.o rpi.o -T memmap -o serial2.elf $(LDFLAGS)
	$(ARMGNU)-objdump -D serial2.elf > serial2.list

serial2.bin: serial2.elf
	$(ARMGNU)-objcopy serial2.elf -O binary serial2.bin

serial2.hex : serial2.elf
	$(ARMGNU)-objcopy serial2.elf -O ihex serial2.hex


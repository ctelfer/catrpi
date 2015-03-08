BASE=../..
ARMGNU=arm-none-eabi

ASFLAGS=--defsym origin=0x8000 
CFLAGS=-Wall -O2 -nostdlib -nostartfiles -ffreestanding \
       -I$(BASE)/src -DUART_USE_IRQ=0

LDFLAGS=

all: loader.hex loader.bin


clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.hex
	rm -f *.elf
	rm -f *.list

core.o: $(BASE)/src/core.s
	$(ARMGNU)-as $(ASFLAGS) $(BASE)/src/core.s -o core.o

rpi.o: $(BASE)/src/rpi.c $(BASE)/src/rpi.h
	$(ARMGNU)-gcc $(CFLAGS) -c $(BASE)/src/rpi.c -o rpi.o

xmodem.o: $(BASE)/src/xmodem.c $(BASE)/src/xmodem.h
	$(ARMGNU)-gcc $(CFLAGS) -c $(BASE)/src/xmodem.c -o xmodem.o

loader.o: loader.c
	$(ARMGNU)-gcc $(CFLAGS) -c loader.c -o loader.o

loader.elf: memmap core.o rpi.o xmodem.o loader.o 
	$(ARMGNU)-ld core.o xmodem.o rpi.o loader.o \
		-T memmap -o loader.elf $(LDFLAGS)
	$(ARMGNU)-objdump -D loader.elf > loader.list

loader.bin: loader.elf
	$(ARMGNU)-objcopy loader.elf -O binary loader.bin

loader.hex : loader.elf
	$(ARMGNU)-objcopy loader.elf -O ihex loader.hex





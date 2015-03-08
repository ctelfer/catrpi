BASE=../..
ARMGNU=arm-none-eabi

ASFLAGS=--defsym origin=0x8000 
CFLAGS=-Wall -O2 -nostdlib -nostartfiles -ffreestanding \
       -I$(BASE)/src -DUART_USE_IRQ=0

LDFLAGS=

all: serial0.hex serial0.bin


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

serial0.o: serial0.c $(BASE)/src/rpi.h
	$(ARMGNU)-gcc $(CFLAGS) -c serial0.c -o serial0.o

serial0.elf: memmap core.o rpi.o serial0.o 
	$(ARMGNU)-ld core.o serial0.o rpi.o -T memmap -o serial0.elf $(LDFLAGS)
	$(ARMGNU)-objdump -D serial0.elf > serial0.list

serial0.bin: serial0.elf
	$(ARMGNU)-objcopy serial0.elf -O binary serial0.bin

serial0.hex : serial0.elf
	$(ARMGNU)-objcopy serial0.elf -O ihex serial0.hex





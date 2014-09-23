BASE=../..
ARMGNU=arm-none-eabi

ASFLAGS=--defsym origin=0x8000 
CFLAGS=-Wall -O2 -nostdlib -nostartfiles -ffreestanding \
       -I$(BASE)/include

LDFLAGS=

all: timer0.hex timer0.bin


clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.hex
	rm -f *.elf
	rm -f *.list

core.o: $(BASE)/src/core.s
	$(ARMGNU)-as $(ASFLAGS) $(BASE)/src/core.s -o core.o

rpi.o: $(BASE)/src/rpi.c $(BASE)/include/rpi.h
	$(ARMGNU)-gcc $(CFLAGS) -c $(BASE)/src/rpi.c -o rpi.o

timer0.o: timer0.c
	$(ARMGNU)-gcc $(CFLAGS) -c timer0.c -o timer0.o

timer0.elf: memmap core.o rpi.o timer0.o 
	$(ARMGNU)-ld core.o timer0.o rpi.o -T memmap -o timer0.elf $(LDFLAGS)
	$(ARMGNU)-objdump -D timer0.elf > timer0.list

timer0.bin: timer0.elf
	$(ARMGNU)-objcopy timer0.elf -O binary timer0.bin

timer0.hex : timer0.elf
	$(ARMGNU)-objcopy timer0.elf -O ihex timer0.hex





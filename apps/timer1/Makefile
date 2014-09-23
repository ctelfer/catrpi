BASE=../..
ARMGNU=arm-none-eabi

ASFLAGS=--defsym origin=0x10000 
CFLAGS=-Wall -O2 -nostdlib -nostartfiles -ffreestanding \
       -I$(BASE)/include

LDFLAGS=

all: timer1.hex timer1.bin


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

timer1.o: timer1.c
	$(ARMGNU)-gcc $(CFLAGS) -c timer1.c -o timer1.o

timer1.elf: memmap core.o rpi.o timer1.o 
	$(ARMGNU)-ld core.o timer1.o rpi.o -T memmap -o timer1.elf $(LDFLAGS)
	$(ARMGNU)-objdump -D timer1.elf > timer1.list

timer1.bin: timer1.elf
	$(ARMGNU)-objcopy timer1.elf -O binary timer1.bin

timer1.hex : timer1.elf
	$(ARMGNU)-objcopy timer1.elf -O ihex timer1.hex





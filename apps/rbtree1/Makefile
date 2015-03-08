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


all: rbtree1.hex rbtree1.bin

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

rbtree1.o: rbtree1.c
	$(ARMGNU)-gcc $(CFLAGS) -c rbtree1.c -o rbtree1.o

rbtree1.elf: memmap core.o rpi.o rbtree1.o 
	$(ARMGNU)-ld core.o rbtree1.o rpi.o -T memmap -o rbtree1.elf $(LDFLAGS)
	$(ARMGNU)-objdump -D rbtree1.elf > rbtree1.list

rbtree1.bin: rbtree1.elf
	$(ARMGNU)-objcopy rbtree1.elf -O binary rbtree1.bin

rbtree1.hex : rbtree1.elf
	$(ARMGNU)-objcopy rbtree1.elf -O ihex rbtree1.hex


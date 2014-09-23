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


all: rbtree0.hex rbtree0.bin

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

rbtree0.o: rbtree0.c
	$(ARMGNU)-gcc $(CFLAGS) -c rbtree0.c -o rbtree0.o

rbtree0.elf: memmap core.o rpi.o rbtree0.o 
	$(ARMGNU)-ld core.o rbtree0.o rpi.o -T memmap -o rbtree0.elf $(LDFLAGS)
	$(ARMGNU)-objdump -D rbtree0.elf > rbtree0.list

rbtree0.bin: rbtree0.elf
	$(ARMGNU)-objcopy rbtree0.elf -O binary rbtree0.bin

rbtree0.hex : rbtree0.elf
	$(ARMGNU)-objcopy rbtree0.elf -O ihex rbtree0.hex


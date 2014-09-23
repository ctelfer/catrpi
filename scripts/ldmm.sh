#!/bin/sh
OUTFILE=memmap
if [ $# -gt 0 ]
then
	OUTFILE=$1
fi

cat > $OUTFILE <<EOFMM
MEMORY
{
    ram : ORIGIN = ${ORIGIN:-0x10000}, LENGTH = ${LENGTH:-0x4000000}
}

SECTIONS
{
    .txt : { *(.text*) } > ram
    .bss : { *(.bss*) } > ram
}
EOFMM

This repo contains a small set of programs and libraries that I wrote to
experiment with some bare metal programming on the Raspberry Pi.  Some
of this code is standalone while other parts use the catlib library
found at git@gitorious.org:catlib/catlib.git.  The directory layout for this
repo is as follows:

  - apps/
	Contains a series of specific applications.  Some of these are
	built solely from the code in this repository while others
	link against the catlib library.

  - include/
	Headers for the stand-alone utility code in this repository.  These
	headers also contain constants specific to programming the
	Raspberry Pi such as the addresesses of system registers or the
	offsets, shifts and masks of required to program the ARM and its
	peripherals in the RPI.

  - scripts/
	Helper scripts, currently right now only for assisting the build
	process.

  - src/
	Common code for bare metal programming on the Raspberry Pi.  This
	code forms a small library for basic functionality.
	

------------
APPLICATIONS
------------

The applications in this repository are as follows:

  - loader
	A bootloader for the RPI that can load other programs that are built
	to run starting from address 0x10000 instead of 0x8000.  It uses
	the UART without interrupts and transfers the program code into place
	using the XMODEM protocol.  This program can be built solely from
	this repo and must be loaded onto a bootable RPI SD Card.

  - serial0
	A basic program that prints hello world and echos characters using
	the RPIs full UART.  This program can be built soley from this
	repo and must be installed on a bootable RPI SD Card.

  - serial1
	As serial0 but drives the UART using RPI interrupts.  This program
	can be built soley from this repo and must be installed on a bootable
	RPI SD Card.

  - serial2
	An interrupt-driven serial application with an idle ping in it.  This
	program can be built soley from this repo and must be installed by
	the "loader" application.

  - serial3
	An interrupt-driven serial application that uses the catlib for
	formatted printing.  This program must be linked against catlib and
	must be installed by the "loader" application.

  - timer0
	A simple program to read the system timer in the RPI.  The program
	can be built soley from this repo and must be installed on a bootable
	RPI SD Card.

  - timer1
	As timer0 but must be booted from the "loader" program.

  - coproc0
	A simple program to test the coprocessor access routines.  This program
	can be built soley from this repo and must be loaded by the "loader"
	program.

  - mmu
	A program that tests enabling the MMU and first level caches in the RPI.
	This program can be built soley from this repo and must be loaded by
	the "loader" program.

  - rbtree0
	A program that runs a test of some a red-black tree and a dynamic memory
	allocator implementation in catlib.  This program must be built against
	catlib and must be loaded by the "loader" program.

  - rbtree1
	As per rbtree0, but this program also enables the MMU and data and
	instruction caches so that one can observe the performance difference
	compared to rbtree0.  This program must be built against catlib and must
	be loaded by the "loader" program.


--------
BUILDING
--------

To build the applications that run standalone one must have install the 
arm-none-eabi gcc toolchain and binutils tool suite.  These programs must be
in the default path.  One builds the programs by simply typing "make" in
each programs respective directory.

To build applications that depend on catlib one must first build catlib
for Raspberry Pi.  To to so:

  # Clone the git repository
  # Do this at the same directory level as the catrpi repo.  So if the 
  # catrpi repo is at /a/b/c/catrpi, then issue the clone command in
  # the /a/b/c/ directory.
  git clone git@gitorious.org:catlib/catlib.git

  # Configure the catlib for building against the Raspberry Pi
  cd catlib/conf
  cp build_system.conf.arm build_system.conf

  # Edit build_system.conf to make sure it has the proper paths to the
  # toolchain (gcc, ar, ranlib) and the toolchain headers
  # /path/to/arm/toolchain-root/lib/gcc/arm-none-eabi/4.8.3/include
  vi build_system.conf

  # build the catlib libraries only (not the test programs)
  cd ../src
  make veryclean
  make

Now one can change into the appropriate application directories that
depend on catlib and build in those directories by simply typing
'make'.


-------
RUNNING
-------

To run a program that must be loaded onto a SD card:
  * Create a bootable SD Card for the Raspberry Pi.  For example install
    Raspbian on some SD Card.
  * Copy the application binary named APP.bin over the file kernel.img
    on the SD Card.
  * Plug the SD Card into the Raspberry Pi.
  * Connect a serial cable to the appropriate GPiO pins (see below).
  * Start minicom or some other serial driver to connect to the serial driver.
  * Power on the Raspberry Pi.


To run a program that must be loaded by the "loader" program:
  * Perform the steps listed above for the "loader" program to run it on the
    RPI.
  * Build the program to run.
  * When the "loader" program boots up issue the 'x' command to have it start
    receiving a file via xmodem.
  * Use the terminal program to send the file (APP.bin) via xmodem.  (example 
    CTRL-A S in minicom.)
  * Start the program using the 's' command in the loader.


-------------------------
CONNECTING A SERIAL CABLE
-------------------------

The programs in this repo use the full UART of the RPI for their basic I/O.
One must interface with this UART using GPiO pins 6, 8 and 10 and optionally
2.  One must wire the UART as follows:

There are four wires.  On my serial TTL cable they are:
	red power
	black ground
	white RX into USB port
	green TX out of the USB port

Connect them to the PI as follows

Corner   Edge of Pi
 +------------------------------------------------
 |
E|         Blk
d|   Red    | Wht
g|    |     |  | Grn
e|    |     |  |  |
 |    2  4  6  8 10 12 14 16 18 20 22 24 26
o|  
f|  
 |    1  3  5  7  9 11 13 15 17 19 21 23 25
P|
i|
 |

Do NOT connect the power line (red) if powering the Pi via the regular USB
power.  Only connect it if you want to actually POWER the Pi via the serial
connection.

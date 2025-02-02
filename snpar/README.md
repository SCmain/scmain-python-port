# Makefile:

# Main variables:
READ_PROGRAM and WRITE_PROGRAM are the names of the generated executables.
SRC_READ and SRC_WRITE point to the corresponding source files.

# Specific compilers:
gcc -m32 : To compile in 32 bits (i386).
gcc -m64 : To compile in 64 bits (x86_64).
aarch64-linux-gnu-gcc : To compile in 64 bits for the ARM64 architecture (aarch64). You will need to have the cross-compilation toolchain installed.

# Rules:
Each environment has its own compilation rules (i386, x86_64, aarch64).
Each program is compiled for each architecture, with a suffix indicating the architecture (e.g. readsnpar_i386, writesnpar_x86_64).

make clean command:
Removes all generated files.

make help command:
Provides help for using the Makefile.

# Usage
To compile all environments:

make

# To compile only for i386:

make i386

# To compile only for x86_64:

make x86_64

# To compile only for aarch64:

make aarch64

# To clean generated files:

make clean

# Prerequisites

Make sure the necessary tools are installed:

gcc for i386 and x86_64.
aarch64-linux-gnu-gcc for aarch64. 

This can be installed via:

sudo zypper in cross-aarch64-gcc13-bootstrap cross-aarch64-gcc7 

If you are building for i386 on a 64-bit system, you may need to install the 32-bit compatibility libraries:

sudo zypper in libgcc_s1-32bit patterns-base-base-32bit

This Makefile allows for easy and flexible compilation management for your different environments.
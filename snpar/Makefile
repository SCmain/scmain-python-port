# Programs names
READ_PROGRAM = readsnpar
WRITE_PROGRAM = writesnpar

# Source files
SRC_READ = readsnpar.c
SRC_WRITE = writesnpar.c

# Compilation options
CFLAGS = -Wall -Wextra -O2

# Compilers for different architectures
CC = gcc
CC_I386 = gcc -m32
CC_X86_64 = gcc -m64
CC_AARCH64 = aarch64-linux-gnu-gcc

# Default rules
all: i386 x86_64 aarch64

# Compiling for i386 (32-bit)
i386: $(READ_PROGRAM)_i386 $(WRITE_PROGRAM)_i386

$(READ_PROGRAM)_i386: $(SRC_READ)
	$(CC_I386) $(CFLAGS) -o $@ $<

$(WRITE_PROGRAM)_i386: $(SRC_WRITE)
	$(CC_I386) $(CFLAGS) -o $@ $<

# Compiling for x86_64 (64 bits)
x86_64: $(READ_PROGRAM)_x86_64 $(WRITE_PROGRAM)_x86_64

$(READ_PROGRAM)_x86_64: $(SRC_READ)
	$(CC_X86_64) $(CFLAGS) -o $@ $<

$(WRITE_PROGRAM)_x86_64: $(SRC_WRITE)
	$(CC_X86_64) $(CFLAGS) -o $@ $<

# Compiling for aarch64 (64 bits)
aarch64: $(READ_PROGRAM)_aarch64 $(WRITE_PROGRAM)_aarch64

$(READ_PROGRAM)_aarch64: $(SRC_READ)
	$(CC_AARCH64) $(CFLAGS) -o $@ $<

$(WRITE_PROGRAM)_aarch64: $(SRC_WRITE)
	$(CC_AARCH64) $(CFLAGS) -o $@ $<

# Cleaning generated files
clean:
	rm -f $(READ_PROGRAM)_i386 $(WRITE_PROGRAM)_i386
	rm -f $(READ_PROGRAM)_x86_64 $(WRITE_PROGRAM)_x86_64
	rm -f $(READ_PROGRAM)_aarch64 $(WRITE_PROGRAM)_aarch64

# User Help
help:
	@echo "Usage:"
	@echo " make - Compile all environments (i386, x86_64, aarch64)"
	@echo " make i386 - Compile for i386 (32-bit)"
	@echo " make x86_64 - Compile for x86_64 (64-bit)"
	@echo " make aarch64 - Compile for aarch64 (64-bit)"
	@echo " make clean - Remove generated files"
# Program names
READ_PROGRAM = readsnpar
WRITE_PROGRAM = writesnpar

# Source files
SRC_READ = readsnpar.c
SRC_WRITE = writesnpar.c

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -static

# Default target: compile both programs
all: $(READ_PROGRAM) $(WRITE_PROGRAM)

# Compile readsnpar
$(READ_PROGRAM): $(SRC_READ)
	$(CC) $(CFLAGS) -o $@ $<

# Compile writesnpar
$(WRITE_PROGRAM): $(SRC_WRITE)
	$(CC) $(CFLAGS) -o $@ $<

# Clean up generated files
clean:
	rm -f $(READ_PROGRAM) $(WRITE_PROGRAM)

# Help message
help:
	@echo "Usage:"
	@echo "  make           - Compile both programs statically"
	@echo "  make clean     - Remove generated binaries"
	@echo "  make help      - Show this help message"

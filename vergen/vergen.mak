INCDIR = ./inc
CFLAGS = -g -Wall -I$(INCDIR)

vergen : vergen.o
	cc -o vergen $(CFLAGS) vergen.c


clean: 
	rm *~

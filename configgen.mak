INCDIR = ./inc
CFLAGS = -g -Wall -I$(INCDIR)

configgen : configgen.o
	cc -o configgen $(CFLAGS) configgen.c


clean: 
	rm *~

INCDIR = ./inc
CFLAGS = -g -Wall -I$(INCDIR)
LFLAGS = -lm

otest : otest.o
	cc -o otest $(CFLAGS) otest.c dmclnx.a


clean: 
	rm *~

INCDIR = ./inc
CFLAGS = -g -Wall -I$(INCDIR)
LFLAGS = -lm

ethgaread : ethgaread.o
	cc -o ethgaread $(CFLAGS) ethgaread.c dmclnx.a


clean: 
	rm *~

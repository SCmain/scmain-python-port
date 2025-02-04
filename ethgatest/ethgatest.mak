INCDIR = ./inc
CFLAGS = -g -Wall -I$(INCDIR)
LFLAGS = -lm

ethgatest : ethgatest.o
	cc -o ethgatest $(CFLAGS) ethgatest.c dmclnx.a


clean: 
	rm *~

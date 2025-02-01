all: windows simppci simpeth term

windows: windows.c
	gcc -g -Wall -DLINUX windows.c dmclnxlib/dmclnx.a -Idmclnxlib -owindows

simppci: simppci.c
	gcc -g -Wall simppci.c dmclnxlib/dmclnx.a -Idmclnxlib -osimppci

simpeth: simpeth.c
	gcc -g -Wall simpeth.c dmclnxlib/dmclnx.a -Idmclnxlib -osimpeth

term: term.c
	gcc -g -Wall term.c dmclnxlib/dmclnx.a -Idmclnxlib -oterm

clean:
	rm windows simppci simpeth term *~

INCDIR = ./inc
CFLAGS = -g -Wall -I$(INCDIR)
LFLAGS = -lm -lpthread -lrt
LOBJS = alcomp.o alfio.o alfns.o alisr.o alstep.o \
	cmdal.o cmdex.o cmdfn2.o cmdfns.o cmdga.o cmdinf.o cmdmac.o cmdmap.o cmdro.o \
	cmdser.o cmdsp.o cmdstn.o cmdval.o dg.o fiog.o fiol.o gafn.o gag.o \
	gamn.o gatst.o maclab.o map.o mapfn.o mapio.o mapstn.o otf.o ro.o \
	rodg.o roga.o rofio.o cmdxq.o \
	scintr.o scio.o scmac.o scmem.o scproc.o screg.o scstat.o scta.o sctch.o \
	sctim.o scttr.o scver.o secsfndev.o secsfnhost.o secssxdev.o secssxhost.o \
	ser.o pdiog.o eth.o

scmain : scmain.o $(LOBJS)
	cc -o scmain $(CFLAGS) $(LFLAGS) $(LOBJS) scmain.c dmclnxlib/dmclnx.a

alcomp.o : alcomp.c inc/sctim.h inc/sck.h inc/ser.h inc/romain.h inc/rofio.h inc/ro.h \
	inc/roga.h inc/fio.h inc/fiol.h inc/alk.h inc/alcomp.h inc/alisr.h inc/alstep.h \
	 inc/alpre.h inc/scintr.h inc/scio.h inc/gag.h inc/sctag.h inc/alfio.h
	cc $(CFLAGS) -c alcomp.c

alfio.o : alfio.c inc/rofio.h inc/sck.h inc/alk.h inc/alpre.h inc/alfio.h \
	inc/fiol.h inc/fiog.h 
	cc $(CFLAGS) -c alfio.c

alfns.o : alfns.c inc/sck.h inc/alk.h inc/alpre.h inc/alcomp.h inc/alfns.h inc/ro.h \
	inc/fiog.h inc/alisr.h inc/sctim.h
	cc $(CFLAGS) -c alfns.c

alisr.o : alisr.c inc/sck.h inc/alk.h inc/alpre.h inc/alisr.h inc/sctim.h inc/scintr.h \
	inc/ser.h inc/scttr.h inc/fiol.h
	cc $(CFLAGS) -c alisr.c

alstep.o : alstep.c inc/sck.h inc/alk.h inc/alpre.h inc/alstep.h inc/alisr.h inc/fio.h \
	inc/romain.h inc/ro.h inc/sctim.h inc/ser.h inc/cmdfns.h inc/roga.h
	cc $(CFLAGS) -c alstep.c

cmdal.o : cmdal.c inc/cmdfns.h inc/cmdsp.h inc/cmdal.h inc/scregg.h inc/ser.h inc/ro.h \
	inc/roga.h inc/alk.h inc/alfio.h inc/alstep.h inc/alcomp.h inc/alisr.h \
	inc/fiol.h inc/fiog.h inc/mapstn.h inc/scta.h inc/scintr.h inc/scstat.h \
	inc/scmem.h
	cc $(CFLAGS) -c cmdal.c

cmdex.o : cmdex.c inc/cmdfns.h inc/cmdex.h inc/cmdsp.h inc/scmac.h inc/scio.h \
	inc/sctim.h inc/gag.h inc/scstat.h inc/ser.h inc/fiog.h inc/fiol.h \
	inc/sctag.h inc/scver.h inc/scregg.h inc/scmem.h inc/ro.h inc/rofio.h \
	inc/sctch.h inc/dg.h inc/scttr.h inc/scio.h inc/mapfn.h \
	inc/mapstn.h inc/map.h inc/scproc.h inc/secsg.h inc/scintr.h inc/alk.h
	cc $(CFLAGS) -c cmdex.c

cmdfn2.o : cmdfn2.c inc/cmdfns.h inc/cmdfn2.h inc/cmdsp.h inc/scmac.h inc/scio.h \
	inc/sctim.h inc/gag.h inc/scstat.h inc/ser.h inc/fiog.h \
	inc/sctag.h inc/scver.h inc/scregg.h inc/scmem.h inc/ro.h inc/rofio.h \
	inc/sctch.h inc/dg.h inc/scttr.h inc/mapio.h inc/mapfn.h \
	inc/mapstn.h inc/map.h inc/scproc.h inc/secsg.h
	cc $(CFLAGS) -c cmdfn2.c

cmdfns.o : cmdfns.c inc/cmdfns.h inc/cmdval.h inc/cmdex.h inc/cmdser.h inc/cmdinf.h \
	inc/scmac.h inc/cmdfn2.h inc/cmdga.h inc/cmdal.h inc/cmdro.h inc/cmdmap.h \
	inc/cmdstn.h inc/cmdsp.h \
	inc/sctag.h inc/ser.h inc/scregg.h inc/scmac.h inc/sctim.h inc/fiog.h \
	inc/gag.h inc/scmem.h inc/pdio.h
	cc $(CFLAGS) -c cmdfns.c

cmdga.o : cmdga.c inc/cmdfns.h inc/cmdga.h inc/cmdsp.h inc/scmac.h inc/scio.h \
	inc/sctim.h inc/gag.h inc/scstat.h inc/ser.h inc/fiog.h \
	inc/sctag.h inc/scver.h inc/scregg.h inc/scmem.h inc/ro.h inc/roga.h inc/rofio.h \
	inc/roloc.h inc/sctch.h inc/dg.h inc/scttr.h inc/mapio.h inc/mapfn.h \
	inc/mapstn.h inc/map.h inc/scproc.h inc/secsg.h inc/alstep.h
	cc $(CFLAGS) -c cmdga.c

cmdinf.o : cmdinf.c inc/cmdfns.h inc/cmdinf.h inc/cmdsp.h inc/cmdal.h inc/scmac.h inc/scio.h \
	inc/sctim.h inc/gag.h inc/scstat.h inc/ser.h inc/fiog.h \
	inc/sctag.h inc/scver.h inc/scregg.h inc/scmem.h inc/ro.h inc/rofio.h \
	inc/gafn.h inc/sctch.h inc/dg.h inc/scttr.h inc/mapfn.h \
	inc/mapstn.h inc/map.h inc/scproc.h inc/secsg.h
	cc $(CFLAGS) -c cmdinf.c

cmdmac.o : cmdmac.c inc/cmdfns.h inc/cmdmac.h inc/cmdsp.h inc/scmac.h inc/scio.h \
	inc/sctim.h inc/gag.h inc/scstat.h inc/ser.h inc/fiog.h \
	inc/sctag.h inc/scver.h inc/scregg.h inc/scmem.h inc/ro.h inc/rofio.h \
	inc/sctch.h inc/dg.h inc/scttr.h inc/mapio.h inc/mapfn.h \
	inc/mapstn.h inc/map.h inc/scproc.h inc/secsg.h
	cc $(CFLAGS) -c cmdmac.c

cmdmap.o : cmdmap.c inc/cmdfns.h inc/cmdmap.h inc/cmdsp.h inc/scmac.h inc/scio.h \
	inc/sctim.h inc/gag.h inc/scstat.h inc/ser.h inc/fiog.h inc/fiol.h \
	inc/sctag.h inc/scver.h inc/scregg.h inc/scmem.h inc/ro.h inc/rofio.h \
	inc/sctch.h inc/dg.h inc/scttr.h inc/mapk.h inc/mapio.h inc/mapfn.h \
	inc/mapstn.h inc/map.h inc/scproc.h inc/secsg.h inc/otf.h
	cc $(CFLAGS) -c cmdmap.c

cmdro.o : cmdro.c inc/cmdfns.h inc/cmdro.h inc/cmdsp.h inc/scmac.h inc/scio.h \
	inc/sctim.h inc/gag.h inc/scstat.h inc/ser.h inc/fiog.h \
	inc/sctag.h inc/scver.h inc/scregg.h inc/scmem.h inc/ro.h inc/rofio.h \
	inc/sctch.h inc/dg.h inc/scttr.h inc/mapio.h inc/mapfn.h \
	inc/mapstn.h inc/map.h inc/scproc.h inc/secsg.h
	cc $(CFLAGS) -c cmdro.c

cmdser.o : cmdser.c inc/cmdfns.h inc/cmdser.h inc/cmdsp.h inc/scmac.h inc/scio.h \
	inc/sctim.h inc/gag.h inc/scstat.h inc/ser.h inc/fiog.h \
	inc/sctag.h inc/scver.h inc/scregg.h inc/scmem.h inc/ro.h inc/rofio.h \
	inc/sctch.h inc/dg.h inc/scttr.h inc/mapio.h inc/mapfn.h \
	inc/mapstn.h inc/map.h inc/scproc.h inc/secsg.h
	cc $(CFLAGS) -c cmdser.c

cmdsp.o : cmdsp.c inc/cmdfns.h inc/cmdsp.h inc/scregg.h inc/ser.h inc/scstat.h \
	inc/scio.h inc/ro.h inc/gag.h inc/scmac.h inc/sctim.h \
	inc/scintr.h inc/fiog.h inc/scmem.h
	cc $(CFLAGS) -c cmdsp.c

cmdstn.o : cmdstn.c inc/cmdfns.h inc/cmdstn.h inc/cmdsp.h inc/scmac.h inc/scio.h \
	inc/sctim.h inc/gag.h inc/scstat.h inc/ser.h inc/fiog.h \
	inc/sctag.h inc/scver.h inc/scregg.h inc/scmem.h inc/ro.h inc/rofio.h \
	inc/sctch.h inc/dg.h inc/scttr.h inc/mapio.h inc/mapfn.h \
	inc/mapstn.h inc/map.h inc/scproc.h inc/secsg.h
	cc $(CFLAGS) -c cmdstn.c

cmdval.o : cmdval.c inc/cmdfns.h inc/cmdval.h inc/cmdsp.h inc/ser.h inc/scstat.h \
	inc/ro.h inc/fiog.h inc/scmem.h
	cc $(CFLAGS) -c cmdval.c

cmdxq.o : cmdxq.c inc/cmdfns.h inc/cmdga.h inc/cmdsp.h inc/scmac.h inc/scio.h \
	inc/sctim.h inc/gag.h inc/scstat.h inc/ser.h inc/fiog.h \
	inc/sctag.h inc/scver.h inc/scregg.h inc/scmem.h inc/ro.h inc/roga.h inc/rofio.h \
	inc/roloc.h inc/sctch.h inc/dg.h inc/scttr.h inc/mapio.h inc/mapfn.h \
	inc/mapstn.h inc/map.h inc/scproc.h inc/secsg.h inc/alstep.h
	cc $(CFLAGS) -c cmdxq.c

dg.o : dg.c inc/sck.h inc/ser.h inc/sctim.h inc/scttr.h inc/dgl.h inc/dg.h \
	inc/rodg.h inc/ro.h inc/gag.h inc/scstat.h inc/fiog.h inc/scmac.h \
	inc/mapio.h inc/scio.h inc/scproc.h inc/gatst.h inc/alisr.h
	cc $(CFLAGS) -c dg.c

fiog.o : fiog.c inc/fio.h inc/fiol.h inc/sctim.h inc/scttr.h inc/sck.h \
	inc/scstat.h inc/ser.h inc/romain.h inc/otf.h inc/scmem.h
	cc $(CFLAGS) -c fiog.c

fiol.o : fiol.c inc/sck.h inc/fio.h inc/scstat.h inc/alk.h inc/scmem.h
	cc $(CFLAGS) -c fiol.c

gafn.o : gafn.c inc/sck.h inc/fiog.h inc/gafn.h inc/gag.h inc/ser.h inc/sctim.h \
	inc/scintr.h 
	cc $(CFLAGS) -c gafn.c

gag.o : gag.c inc/sck.h inc/gafn.h inc/gag.h inc/scintr.h inc/scstat.h \
	inc/sctim.h inc/mapio.h inc/scio.h
	cc $(CFLAGS) -c gag.c

gamn.o : gamn.c inc/sck.h inc/gafn.h inc/gamn.h inc/gag.h inc/scintr.h \
	inc/scstat.h inc/sctim.h
	cc $(CFLAGS) -c gamn.c

gatst.o : gatst.c inc/sck.h inc/gafn.h inc/gag.h inc/gatst.h inc/romain.h \
	inc/roloc.h inc/scintr.h inc/fiog.h inc/sctim.h inc/ser.h inc/serm.h \
	inc/scio.h inc/scttr.h inc/ro.h
	cc $(CFLAGS) -c gatst.c

maclab.o : maclab.c inc/sck.h inc/maclab.h inc/ser.h inc/scmem.h
	cc $(CFLAGS) -c maclab.c

map.o : map.c inc/sck.h inc/ser.h inc/fiog.h inc/mapio.h inc/mapfn.h \
	inc/mapstn.h inc/map.h inc/scstat.h inc/otf.h
	cc $(CFLAGS) -c map.c

mapfn.o : mapfn.c inc/sck.h inc/ser.h inc/mapk.h inc/mapfn.h inc/scmem.h
	cc $(CFLAGS) -c mapfn.c

mapio.o : mapio.c inc/sck.h inc/ser.h inc/scintr.h inc/scio.h inc/gag.h \
	inc/sctim.h inc/mapfn.h inc/mapio.h inc/mapk.h inc/ro.h inc/rofio.h \
	inc/fiog.h inc/scttr.h inc/scstat.h
	cc $(CFLAGS) -c mapio.c

mapstn.o : mapstn.c inc/sck.h inc/mapstn.h inc/mapio.h inc/ser.h inc/scmem.h
	cc $(CFLAGS) -c mapstn.c

otf.o : otf.c inc/otf.h inc/sck.h inc/alk.h inc/mapk.h inc/sctim.h inc/fiog.h \
	inc/mapio.h inc/scintr.h inc/romain.h inc/ro.h inc/rofio.h inc/scio.h \
	inc/fiol.h inc/scmem.h
	cc $(CFLAGS) -c otf.c

pdiog.o : pdiog.c inc/pdio.h inc/cmdfns.h inc/scstat.h inc/otf.h inc/scmem.h \
	inc/gag.h inc/scio.h inc/sck.h inc/cmdsp.h inc/cmdal.h
	cc $(CFLAGS) -c pdiog.c

ro.o : ro.c inc/sck.h inc/ro.h inc/roga.h inc/rofn.h inc/rofio.h inc/roloc.h \
	inc/fiog.h inc/gag.h inc/gamn.h inc/ser.h inc/scstat.h \
	inc/mapio.h inc/sctim.h inc/scintr.h inc/scmac.h inc/scio.h inc/otf.h \
	inc/fiol.h inc/pdio.h
	cc $(CFLAGS) -c ro.c

rodg.o : rodg.c inc/sck.h inc/rodg.h inc/rofio.h inc/ro.h inc/roloc.h \
	inc/gag.h inc/fiog.h inc/ser.h inc/scttr.h \
	inc/scio.h inc/scintr.h inc/sctim.h 
	cc $(CFLAGS) -c rodg.c

rofio.o : rofio.c inc/sck.h inc/rofio.h inc/ro.h inc/roloc.h \
	inc/gag.h inc/fiog.h 
	cc $(CFLAGS) -c rofio.c

roga.o : roga.c inc/sck.h inc/ro.h inc/roga.h inc/roloc.h inc/rofio.h \
	inc/romain.h inc/gag.h inc/fiog.h inc/mapstn.h inc/sctch.h inc/ser.h
	cc $(CFLAGS) -c roga.c

scintr.o : scintr.c inc/sck.h inc/scintr.h inc/alk.h inc/alfio.h
	cc $(CFLAGS) -c scintr.c

scio.o : scio.c inc/sck.h inc/sctim.h inc/scstat.h inc/fiog.h inc/gag.h \
	inc/scmac.h inc/scintr.h inc/scio.h inc/mapio.h inc/mapstn.h \
	inc/ser.h inc/scttr.h inc/ro.h inc/otf.h inc/scmem.h
	cc $(CFLAGS) -c scio.c

scmac.o : scmac.c inc/sck.h inc/ser.h inc/scmac.h inc/maclab.h inc/sctag.h \
	inc/scstat.h inc/scregg.h inc/scver.h inc/cmdfns.h inc/cmdval.h \
	inc/cmdex.h inc/cmdmac.h inc/cmdsp.h inc/sctim.h inc/mapio.h \
	inc/fiog.h inc/scintr.h inc/scmem.h inc/secsg.h
	cc $(CFLAGS) -c scmac.c

scmem.o : scmem.c inc/ser.h inc/sck.h inc/scmem.h inc/scstat.h inc/scmac.h
	cc $(CFLAGS) -c scmem.c

scproc.o : scproc.c inc/scproc.h inc/cmdfns.h inc/sck.h \
	inc/sctag.h inc/scmac.h inc/ser.h inc/scstat.h inc/sctim.h \
	inc/ro.h inc/scintr.h inc/gag.h inc/secsg.h
	cc $(CFLAGS) -c scproc.c

screg.o : screg.c inc/scmac.h inc/screg.h inc/scregg.h inc/sck.h inc/ser.h inc/scmem.h
	cc $(CFLAGS) -c screg.c

scstat.o : scstat.c inc/sck.h inc/scstat.h inc/scintr.h inc/gag.h inc/ro.h \
	inc/scmac.h inc/scregg.h inc/fiog.h inc/mapio.h inc/scio.h inc/alk.h \
	inc/alfio.h inc/sctim.h
	cc $(CFLAGS) -c scstat.c

scta.o : scta.c inc/scta.h inc/sck.h inc/cmdfns.h inc/ser.h inc/scmac.h \
	inc/sctim.h inc/scintr.h inc/scmem.h
	cc $(CFLAGS) -c scta.c

sctch.o : sctch.c inc/sck.h inc/scintr.h inc/ser.h inc/sctim.h \
	inc/scttr.h inc/sctchl.h inc/sctch.h inc/scmac.h inc/scregg.h inc/gag.h \
	inc/ro.h inc/roga.h inc/rofio.h inc/fiog.h inc/scio.h \
	inc/scproc.h inc/alstep.h 
	cc $(CFLAGS) -c sctch.c

sctim.o : sctim.c inc/sck.h inc/sctim.h inc/sctim2.h inc/sctim3.h \
	inc/scintr.h inc/ser.h inc/gag.h inc/scio.h inc/ro.h \
	inc/fiog.h inc/scmem.h
	cc $(CFLAGS) -c sctim.c

scttr.o : scttr.c inc/sck.h inc/sctim.h inc/ser.h inc/scttr2.h inc/scttr.h
	cc $(CFLAGS) -c scttr.c

scver.o : scver.c inc/sck.h inc/scver.h inc/ser.h inc/gag.h inc/romain.h inc/fiog.h
	cc $(CFLAGS) -c scver.c

secsfndev.o : secsfndev.c inc/sck.h inc/mapk.h inc/secsg.h inc/secsl.h \
	inc/mapstn.h inc/fiol.h inc/cmdfns.h inc/scstat.h inc/scmac.h inc/ser.h
	cc $(CFLAGS) -c secsfndev.c

secsfnhost.o : secsfnhost.c inc/sck.h inc/secsg.h inc/secsl.h inc/sctim.h inc/screg.h
	cc $(CFLAGS) -c secsfnhost.c

secssxdev.o : secssxdev.c inc/sck.h inc/secsg.h inc/secsl.h 
	cc $(CFLAGS) -c secssxdev.c

secssxhost.o : secssxhost.c inc/sck.h inc/secsg.h inc/secsl.h \
	inc/serg.h inc/scmac.h inc/fiog.h inc/scmem.h
	cc $(CFLAGS) -c secssxhost.c

ser.o : ser.c inc/sck.h inc/ser.h inc/serm.h inc/serl.h inc/sctim.h inc/scstat.h \
	inc/scmac.h inc/secsg.h inc/secsl.h inc/scttr.h inc/scproc.h \
	inc/fiog.h inc/scmem.h
	cc ${CFLAGS} -c ser.c

eth.o : eth.c inc/sck.h inc/scver.h inc/scstat.h inc/fiog.h inc/fio.h \
	inc/scio.h inc/otf.h inc/roga.h inc/ro.h
	cc ${CFLAGS} -c eth.c

clean: 
	rm *.o scmain *~

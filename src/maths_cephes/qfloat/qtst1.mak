CC = gcc
CFLAGS = -g

qtst1:  qtst1.o qligami.o ligami.o
	$(CC) -o qtst1 qtst1.o qligami.o ligami.o libmq.a ../double/libmd.a

qtst1.o: qtst1.c qhead.h
	$(CC) $(CFLAGS) -c qtst1.c

ligami.o: ligami.c mconf.h
	$(CC) $(CFLAGS) -c ligami.c


hyperg.o: hyperg.c
	$(CC) -c hyperg.c

#deinit.obj: deinit.asm
#	load asm68k deinit.asm

#ieinit.obj: ieinit.asm
#	masm ieinit.asm;

#	link /MAP /NOE qtst1 hyp2f1 qgamma qhy2f1 ieinit,,,\lib\qlib100.lib \lib\fti.lib;

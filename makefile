c=gcc
op=-Wall -Wextra

all : BashorpionLib.o BashorpionClient.c BashorpionServer.c
	$(c) $(op)  BashorpionLib.o BashorpionClient.c -o BashorpionClient.exe
	$(c) $(op)  BashorpionLib.o BashorpionServer.c -o dames BashorpionServer.exe
BashorpionLib.o : BashorpionLib.c BashorpionLib.h
	$(c) $(op) -c BashorpionLib.c	

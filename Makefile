#Sabino San Juan.IV
#Makefile for recover
CC = gcc
#MT = -mtune=native
LDFLAGS = -lpthread 
CFLAGS = -g -Wall -ansi
EXECS =  recover 

OSTYPE= $(shell uname)
ifeq ($(OSTYPE),Linux)#if I have a linux box
   LDFLAGS+= -lrt
endif

all: $(EXECS)


recover:  interface.o fifo.o newList.o partTwo.o extra.o getPic.o getDoc.o getAvi.o getExe.o getWav.o
	$(CC) $(CFLAGS) $(LDFLAGS) interface.o  fifo.o newList.o partTwo.o extra.o getPic.o getDoc.o getAvi.o getExe.o getWav.o -o recover
#assembly files

interface.o: interface.c 
	$(CC)  $(CFLAGS) -c interface.c
fifo.o: fifo.c 
	$(CC)  $(CFLAGS) -c fifo.c
newList.o: newList.c 
	$(CC)   $(CFLAGS) -c newList.c
extra.o: extra.c 
	$(CC)   $(CFLAGS) -c extra.c
partTwo.o: partTwo.c 
	$(CC)   $(CFLAGS) -c partTwo.c
getPic.o: getPic.c 
	$(CC)   $(CFLAGS) -c getPic.c
getWav.o: getWav.c 
	$(CC)   $(CFLAGS) -c getWav.c
getDoc.o: getDoc.c 
	$(CC)   $(CFLAGS) -c getDoc.c
getAvi.o: getAvi.c 
	$(CC)   $(CFLAGS) -c getAvi.c
getExe.o: getExe.c 
	$(CC)   $(CFLAGS) -c getExe.c
#end argument
.PHONY: clean
clean:
	$(RM)  *.o $(EXECS)

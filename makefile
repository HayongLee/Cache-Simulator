# TCSS 372 - Computer Architecture
CC=gcc
I=lpthread

all: cacheSimulator

cacheSimulator: Main.o CacheSimulator.o AddressTrace.o Metrics.o
	$(CC) -o cacheSimulator Main.o CacheSimulator.o AddressTrace.o Metrics.o -$(I)

Main.o: Main.c
	$(CC) -c Main.c

CacheSimulator.o: CacheSimulator.c CacheSimulator.h
	$(CC) -c CacheSimulator.c

AddressTrace.o: AddressTrace.c AddressTrace.h
	$(CC) -c AddressTrace.c

Metrics.o: Metrics.c Metrics.h
	$(CC) -c Metrics.c

#Need dis
	


/*
 ============================================================================
 Name        : AddressTrace.h
 Author      : Jesse Bannon
 Class       : TCSS 372 - Computer Architecture
 School      : University of Washington Tacoma
 Copyright   : Use only for educational purposes and do not modify file.
 Description : Contains all enums address structs used throughout the program
               and conversions functions from unsigned integers to cache and
               trace addresses.
 ============================================================================
 */

#ifndef ADDRESSTRACE_H_
#define ADDRESSTRACE_H_

typedef enum MESI_Enum {I, E, S, M} MESI;
typedef enum INST_TYPE_Enum {R, W, F} INST_TYPE;
typedef enum ADDRESS_TYPE {INST, DATA} ADDR_TYPE;

typedef enum cache_block
{CPU0L1i_, CPU0L1d_, CPU0L2_, CPU1L1i_, CPU1L1d_, CPU1L2_, CPU2L1i_, CPU2L1d_, CPU2L2_, CPU3L1i_, CPU3L1d_, CPU3L2_, L3_, M1_, M2_, ALL_} CACHE;

typedef struct addressCache_node {

    unsigned int tag;
    unsigned int index;
    unsigned int time;

    MESI mesi;

} cacheAddress;

typedef struct addressTrace_node {
    unsigned int instructionAddress;
    unsigned int dataAddress;

    INST_TYPE inst_type;
} traceAddress;

unsigned int getTag(const unsigned int address, const int indexSize, const int offsetSize);
    /* Returns the tag of an address based on indexSize and offsetSize. */

unsigned int getIndex(const unsigned int address, const int indexSize, const int offsetSize);
    /* Returns the index of an address based on the indexSize and offsetSize. */

unsigned int getOffset(const unsigned int address, const int offsetSize);
    /* Returns the offset of an address based on the offsetSize. */

#endif

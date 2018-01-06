/*
 ============================================================================
 Name        : AddressTrace.c
 Author      : Jesse Bannon
 Class       : TCSS 372 - Computer Architecture
 School      : University of Washington Tacoma
 Copyright   : Use only for educational purposes and do not modify file.
 Description : Contains all enums address structs used throughout the program
               and conversions functions from unsigned integers to cache and
               trace addresses.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

unsigned int getTag(unsigned int address,  const int indexSize, const int offsetSize) {
    return (address >> (indexSize + offsetSize) & ~1);
}

unsigned int getIndex(unsigned int address, const int indexSize, const int offsetSize) {
    unsigned int tempAddress = address >> offsetSize;

    switch(indexSize) {
    case 0: return 0;
    case 1: return (tempAddress & 0x1);
    case 2: return (tempAddress & 0x3);
    case 3: return (tempAddress & 0x7);
    case 4: return (tempAddress & 0xF);

    case 5: return (tempAddress & 0x1F);
    case 6: return (tempAddress & 0x3F);
    case 7: return (tempAddress & 0x7F);
    case 8: return (tempAddress & 0xFF);

    case 9: return (tempAddress & 0x1FF);
    case 10: return (tempAddress & 0x3FF);
    case 11: return (tempAddress & 0x7FF);
    case 12: return (tempAddress & 0xFFF);

    case 13: return (tempAddress & 0x1FFF);
    case 14: return (tempAddress & 0x3FFF);
    case 15: return (tempAddress & 0x7FFF);
    case 16: return (tempAddress & 0xFFFF);


    case 17: return (tempAddress & 0x1FFFF);
    case 18: return (tempAddress & 0x3FFFF);
    case 19: return (tempAddress & 0x7FFFF);
    case 20: return (tempAddress & 0xFFFFF);

    default: printf("*** ERROR: Index should not be greater than 20 (Index size = %d). Returning 0.\n", indexSize);
    return 0;
    }
}

unsigned int getOffset(unsigned int address, const int offsetSize) {
    switch(offsetSize) {
    case 0: return 0;

    case 1: return (address & 0x1);
    case 2: return (address & 0x3);
    case 3: return (address & 0x7);
    case 4: return (address & 0xF);

    case 5: return (address & 0x1F);
    case 6: return (address & 0x3F);
    case 7: return (address & 0x7F);
    case 8: return (address & 0xFF);

    default: printf("*** Offset should not be greater than 8. Returning 0.\n");
    return 0;
    }
}

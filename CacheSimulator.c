/*
 ============================================================================
 Name        : AddressTrace.c
 Author      : Jesse Bannon
 Class       : TCSS 372 - Computer Architecture
 School      : University of Washington Tacoma
 Copyright   : Use only for educational purposes and do not modify file.
 Description : Reads in a .csv of address traces and simulates a
               five-level cache hierarchy and two CPUs.
 ============================================================================
 */
#pragma warning(disable:4996)

#include "CacheSimulator.h"
#include "AddressTrace.h"
#include "Metrics.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* Address pieces in bits */
#define OFFSET_SIZE (int)((log(CL_SIZE))/(log(2)))
#define INDEX_SIZE(entries) (int)((log(entries/CACHE_ASSOCIATIVITY))/(log(2)))
#define TAG_SIZE(entries) (int)(32-OFFSET_SIZE-(INDEX_SIZE(entries)))

/* Start of cache search within an array */
#define INDEX_START(index) (int)(CACHE_ASSOCIATIVITY * index)

typedef struct CPU_L_tag {
    cacheAddress L1i[L1_SIZE];
    cacheAddress L1d[L1_SIZE];
    cacheAddress L2[L2_SIZE];
} CPU;

static CPU CPU0;
static CPU CPU1;
static CPU CPU2;
static CPU CPU3;


static cacheAddress L3[L3_SIZE];
static cacheAddress M1[M1_SIZE];
static cacheAddress M2[M2_SIZE];

/*
 ============================================================================

        Function prototypes (and descriptions)

 ============================================================================
*/

CACHE getL2(const CPU* CPUnum); 
/*  Returns enum of the correct L2 based on the CPU. 
*/

CACHE getL1(const CPU* CPUnum, const ADDR_TYPE addr_type);
/*  Returns enum of the correct L1 based on the CPU and address type. 
*/

CPU* getOtherCPU(const CPU* CPU);
/*  Returns the other CPU address of the CPU parameter. 
*/

cacheAddress* getCache(const CACHE cache_block);    
/*  Returns the address of the cache block memory array
    based on the CACHE enum parameter.                  
*/

unsigned int getCacheIndex(const unsigned int tempAddress, const CACHE cache_block);
/*  Returns the index of an address trace based on the cache block.
*/

unsigned int getCacheTag(const unsigned int tempAddress, const CACHE cache_block);
/*  Returns the tag of an address trace based on the cache block.
*/

void writeToCache(const unsigned int tempAddress, const MESI mesi, const CACHE cache_block);
/*  Writes an address trace with the specified MESI state to the desired cache block.
*/

cacheAddress* checkCacheLevel(const unsigned int tempAddress, const CACHE cacheBlock);
/*  Checks if the specified cache block contains the address trace.
*/

void writeToLowerCaches(const unsigned int tempAddress, const ADDR_TYPE addr_type,
        cacheAddress* currentEntry, const CPU* CPUnum, const CACHE currentCache, 
        const MESI mesi);
/*  Writes to all caches under currentCache using writeToCache.
*/

void checkCache(const unsigned int tempAddress, const INST_TYPE instType, 
        const CPU *CPU, const ADDR_TYPE addr_type);
/*  Checks every cache starting at L1 if the trace address exists within a
    cache block.  

    If it exists and is a write, it will update the cache entry to modified 
    and write to lower cache entries (and check to invalide). 

    If it exists and is a read, it will write the entry to lower caches.  

    If it misses, it will write to every cache (L2 and L1 dependant of the CPU).
*/

void invalidateLowerCaches(const unsigned int tempAddress, 
        const ADDR_TYPE addr_type, const CPU *CPUnum);
/*  If L2 and L1i (or L1d) contain tempAddress, it sets its MESI state to invalid.
*/

void checkForShared(const unsigned int tempAddress, const ADDR_TYPE addr_type, 
        cacheAddress* currentEntry, const CPU* CPU);
/*  Checks the opposite CPU cache memory if it contains the same address. If it
    does it will set the currentEntry and the opposite CPU cache entry's MESI
    state to shared.
*/

void updateCacheEntries(const unsigned int tempAddress, cacheAddress* cacheHit,
        const CPU* CPU, const ADDR_TYPE addr_type, const CACHE currentCache);
/*  Updates a cache entry on a write depending on it's MESI state and writes
    to lower caches.
*/

void traceDecoder(traceAddress* tempAddress, const CPU* CPU);
/*  Interprets the trace address from the file and will check the cache for
    the instruction address and data address if it exists.
*/

int readAddressLine(FILE *inputFile, const CPU* CPU);
/*  Reads the entire file of address traces and sends each individual
    address trace to the traceDecoder of the specified CPU.
*/

/*
 ============================================================================

        Functions

 ============================================================================
*/

CACHE getL2(const CPU* CPUnum) {
    if (CPUnum == &CPU0)
        return CPU0L2_;
    else
        return CPU1L2_;

	if (CPUnum == &CPU1)
		return CPU1L2_;
	else
		return CPU2L2_;

	if (CPUnum == &CPU2)
		return CPU2L2_;
	else
		return CPU3L2_;
}

CACHE getL1(const CPU* CPUnum, const ADDR_TYPE addr_type) {
	{
		if (CPUnum == &CPU0) {
		if (addr_type == INST)
			return CPU0L1i_;
		else
			return CPU0L1d_;
	}
	else if (CPUnum == &CPU1) {
		if (addr_type == INST)
			return CPU1L1i_;
		else
			return CPU1L1d_;
	}
	else if (CPUnum == &CPU2) {
		if (addr_type == INST)
			return CPU2L1i_;
		else
			return CPU2L1d_;
	}
	else {
		if (addr_type == INST)
			return CPU3L1i_;
		else
			return CPU3L1d_;
	}
}
}

CPU* getOtherCPU(const CPU* CPU) {
	{
		if (CPU == &CPU0)
			return &CPU1;
		else
			return &CPU0;
	}
	{
		if (CPU == &CPU1)
			return &CPU2;
		else
			return &CPU1;
	}
	{
		if (CPU == &CPU2)
			return &CPU3;
		else
			return &CPU2;
	}
}

cacheAddress* getCache(const CACHE cache_block) {
    switch (cache_block) {
    case CPU0L1i_: return CPU0.L1i;
    case CPU0L1d_: return CPU0.L1d;
    case CPU1L1i_: return CPU1.L1i;
    case CPU1L1d_: return CPU1.L1d;
	case CPU2L1i_: return CPU2.L1i;
	case CPU2L1d_: return CPU2.L1d;
	case CPU3L1i_: return CPU3.L1i;
	case CPU3L1d_: return CPU3.L1d;
    case CPU0L2_: return CPU0.L2;
    case CPU1L2_: return CPU1.L2;
	case CPU2L2_: return CPU2.L2;
	case CPU3L2_: return CPU3.L2;
    case L3_: return L3;
    case M1_: return M1;
    case M2_: return M2;
    default: return NULL;
    }
}

unsigned int getCacheIndex(const unsigned int tempAddress, const CACHE cache_block) {
    switch (cache_block) {
    case CPU0L1i_: return getIndex(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
    case CPU0L1d_: return getIndex(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
    case CPU1L1i_: return getIndex(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
    case CPU1L1d_: return getIndex(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
	case CPU2L1i_: return getIndex(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
	case CPU2L1d_: return getIndex(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
	case CPU3L1i_: return getIndex(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
	case CPU3L1d_: return getIndex(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);

    case CPU0L2_: return getIndex(tempAddress, INDEX_SIZE(L2_SIZE), OFFSET_SIZE);
    case CPU1L2_: return getIndex(tempAddress, INDEX_SIZE(L2_SIZE), OFFSET_SIZE);
	case CPU2L2_: return getIndex(tempAddress, INDEX_SIZE(L2_SIZE), OFFSET_SIZE);
	case CPU3L2_: return getIndex(tempAddress, INDEX_SIZE(L2_SIZE), OFFSET_SIZE);

    case L3_: return getIndex(tempAddress, INDEX_SIZE(L3_SIZE), OFFSET_SIZE);
    case M1_: return getIndex(tempAddress, INDEX_SIZE(M1_SIZE), OFFSET_SIZE);
    case M2_: return getIndex(tempAddress, INDEX_SIZE(M2_SIZE), OFFSET_SIZE);
    default: return 0;
    }
}

unsigned int getCacheTag(const unsigned int tempAddress, const CACHE cache_block) {
    switch (cache_block) {
    case CPU0L1i_: return getTag(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
    case CPU0L1d_: return getTag(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
    case CPU1L1i_: return getTag(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
    case CPU1L1d_: return getTag(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
	case CPU2L1i_: return getTag(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
	case CPU2L1d_: return getTag(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
	case CPU3L1i_: return getTag(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);
	case CPU3L1d_: return getTag(tempAddress, INDEX_SIZE(L1_SIZE), OFFSET_SIZE);


    case CPU0L2_: return getTag(tempAddress, INDEX_SIZE(L2_SIZE), OFFSET_SIZE);
    case CPU1L2_: return getTag(tempAddress, INDEX_SIZE(L2_SIZE), OFFSET_SIZE);
	case CPU2L2_: return getTag(tempAddress, INDEX_SIZE(L2_SIZE), OFFSET_SIZE);
	case CPU3L2_: return getTag(tempAddress, INDEX_SIZE(L2_SIZE), OFFSET_SIZE);

    case L3_: return getTag(tempAddress, INDEX_SIZE(L3_SIZE), OFFSET_SIZE);
    case M1_: return getTag(tempAddress, INDEX_SIZE(M1_SIZE), OFFSET_SIZE);
    case M2_: return getTag(tempAddress, INDEX_SIZE(M2_SIZE), OFFSET_SIZE);
    default: return 0;
    }
}

void writeToCache(const unsigned int tempAddress, const MESI mesi, const CACHE cache_block) {
    cacheAddress* cache;
    cacheAddress* LRU;
    unsigned int tempIndex, tempTag;
    int indexStart, j;

    cache = getCache(cache_block);
    tempIndex = getCacheIndex(tempAddress, cache_block);
    tempTag = getCacheTag(tempAddress, cache_block);
    indexStart = INDEX_START(tempIndex);
    LRU = &cache[indexStart];

    for (j = indexStart; j < indexStart + CACHE_ASSOCIATIVITY; j++) {
        if (cache[j].mesi == I) {
            cache[j].index = tempIndex;
            cache[j].tag = tempTag;
            cache[j].time = time(NULL);
            cache[j].mesi = mesi;
            return;
        }

        if (cache[j].time < LRU->time)
            LRU = &cache[j];
    }

    LRU->index = tempIndex;
    LRU->tag = tempTag;
    LRU->time = time(NULL);
    LRU->mesi = mesi;


}

/* Returns latency of instruction */
cacheAddress* checkCacheLevel(const unsigned int tempAddress, const CACHE cacheBlock) {
    cacheAddress* cache = getCache(cacheBlock);
    unsigned int tempIndex = getCacheIndex(tempAddress, cacheBlock);
    unsigned int tempTag = getCacheTag(tempAddress, cacheBlock);
    int indexStart = INDEX_START(tempIndex);

    int i;
    for (i = indexStart; i < indexStart + CACHE_ASSOCIATIVITY; i++) {
        if (tempTag == cache[i].tag && cache[i].mesi != I)
            return &cache[i];
    }
    return NULL;
}

void writeToLowerCaches(const unsigned int tempAddress, const ADDR_TYPE addr_type,
        cacheAddress* currentEntry, const CPU* CPUnum, const CACHE currentCache, 
        const MESI mesi) {

    cacheAddress* cacheSearch;

    switch(currentCache) {
    case ALL_:
        writeToCache(tempAddress, mesi, M2_);
        writeToCache(tempAddress, mesi, M1_);
        writeToCache(tempAddress, mesi, L3_);
        writeToCache(tempAddress, mesi, getL2(CPUnum));
        writeToCache(tempAddress, mesi, getL1(CPUnum, addr_type));
        break;

    case M2_:
        writeToCache(tempAddress, mesi, M1_);
        writeToCache(tempAddress, mesi, L3_);
        writeToCache(tempAddress, mesi, getL2(CPUnum));
        writeToCache(tempAddress, mesi, getL1(CPUnum, addr_type));
    break;

    case M1_:
        writeToCache(tempAddress, mesi, L3_);
        writeToCache(tempAddress, mesi, getL2(CPUnum));
        writeToCache(tempAddress, mesi, getL1(CPUnum, addr_type));
        break;

    case L3_:
        if ((cacheSearch = checkCacheLevel(tempAddress, 
                getL2(getOtherCPU(CPUnum)))) != NULL) {

            mesiMetrics(currentEntry->mesi, S);
            currentEntry->mesi = S;

            mesiMetrics(cacheSearch->mesi, S);
            cacheSearch->mesi = S;
            if ((cacheSearch = checkCacheLevel(tempAddress, 
                    getL1(getOtherCPU(CPUnum),addr_type))) != NULL) {

                mesiMetrics(cacheSearch->mesi, S);
                cacheSearch->mesi = S;
            }

            writeToCache(tempAddress, S, getL2(CPUnum));
            writeToCache(tempAddress, S, getL1(CPUnum, addr_type));
        } else {
            writeToCache(tempAddress, mesi, getL2(CPUnum));
            writeToCache(tempAddress, mesi, getL1(CPUnum, addr_type));
        } break;

    case CPU0L2_:
        writeToCache(tempAddress, mesi, getL1(CPUnum, addr_type));
        break;
    case CPU1L2_:
        writeToCache(tempAddress, mesi, getL1(CPUnum, addr_type));
        break;
	case CPU2L2_:
		writeToCache(tempAddress, mesi, getL1(CPUnum, addr_type));
		break;
	case CPU3L2_:
		writeToCache(tempAddress, mesi, getL1(CPUnum, addr_type));
		break;
    default:
        break;
    }
}

void invalidateLowerCaches(const unsigned int tempAddress, 
        const ADDR_TYPE addr_type, const CPU *CPUnum) {

    cacheAddress* invalidateEntry;
    if ((invalidateEntry = checkCacheLevel(tempAddress, 
            getL2(getOtherCPU(CPUnum)))) != NULL) {

        invalidateEntry->mesi = I;
        mesiMetrics(S, I);
        if ((invalidateEntry = checkCacheLevel(tempAddress, 
                getL1(getOtherCPU(CPUnum), addr_type))) != NULL) {

            invalidateEntry->mesi = I;
            mesiMetrics(S, I);
        }
    }
}

void checkForShared(const unsigned int tempAddress, const ADDR_TYPE addr_type, 
        cacheAddress* currentEntry, const CPU* CPU) {

    cacheAddress* cacheSearch;
    if ((cacheSearch = checkCacheLevel(tempAddress, getL2(getOtherCPU(CPU)))) != NULL) {
        mesiMetrics(currentEntry->mesi, M);
        mesiMetrics(cacheSearch->mesi, I);
        currentEntry->mesi = M;
        cacheSearch->mesi = I;
        if ((cacheSearch = checkCacheLevel(tempAddress, 
                getL1(getOtherCPU(CPU), addr_type))) != NULL) {

            mesiMetrics(cacheSearch->mesi, I);
            cacheSearch->mesi = I;
        }
        writeToCache(tempAddress, M, getL2(CPU));
        writeToCache(tempAddress, M, getL1(CPU, addr_type));
    } else {
        writeToCache(tempAddress, currentEntry->mesi, getL2(CPU));
        writeToCache(tempAddress, currentEntry->mesi, getL1(CPU, addr_type));
    }
}

void updateCacheEntries(const unsigned int tempAddress, cacheAddress* cacheHit,
        const CPU* CPU, const ADDR_TYPE addr_type, const CACHE currentCache) {

    switch(cacheHit->mesi) {
    case M:
        writeToLowerCaches(tempAddress, addr_type, cacheHit, CPU, currentCache, M);
        break;
    case E: cacheHit->mesi = M;
        writeToLowerCaches(tempAddress, addr_type, cacheHit, CPU, currentCache, M);
        mesiMetrics(E, M);
        break;
    case S: cacheHit->mesi = M;
        mesiMetrics(S, M);
        invalidateLowerCaches(tempAddress, addr_type, CPU);
        writeToLowerCaches(tempAddress, addr_type, cacheHit, CPU, currentCache, M);

        break;
    case I:
    default: return;

    }
}

void checkCache(const unsigned int tempAddress, const INST_TYPE instType, 
        const CPU *CPU, const ADDR_TYPE addr_type) {

    cacheAddress* cacheHit;
    if ((cacheHit = checkCacheLevel(tempAddress, getL1(CPU, addr_type))) != NULL) {
        hitMetrics(getL1(CPU, addr_type), instType, addr_type);

        if (instType == W) {
            updateCacheEntries(tempAddress, cacheHit, CPU, addr_type, getL1(CPU, addr_type));
        }

    } else if ((cacheHit = checkCacheLevel(tempAddress, getL2(CPU))) != NULL) {
        hitMetrics(getL2(CPU), instType, addr_type);
        if (instType == W)
            updateCacheEntries(tempAddress, cacheHit, CPU, addr_type, getL2(CPU));
        else
            writeToLowerCaches(tempAddress, addr_type, cacheHit, 
                    CPU, getL2(CPU), cacheHit->mesi);

    } else if ((cacheHit = checkCacheLevel(tempAddress, L3_)) != NULL) {
        hitMetrics(L3_, instType, addr_type);
        if (instType == W)
            checkForShared(tempAddress, addr_type, cacheHit, CPU);

        else {
            writeToLowerCaches(tempAddress, addr_type, cacheHit, CPU, L3_, cacheHit->mesi);
            checkForShared(tempAddress, addr_type, cacheHit, CPU);
        }

    } else if ((cacheHit = checkCacheLevel(tempAddress, M1_)) != NULL) {
        hitMetrics(M1_, instType, addr_type);
        if (instType == W)
            updateCacheEntries(tempAddress, cacheHit, CPU, addr_type, M1_);
        else
            writeToLowerCaches(tempAddress, addr_type, cacheHit, CPU, M1_, cacheHit->mesi);

    } else if ((cacheHit = checkCacheLevel(tempAddress, M2_)) != NULL) {
        hitMetrics(M2_, instType, addr_type);
        if (instType == W) {
            updateCacheEntries(tempAddress, cacheHit, CPU, addr_type, M2_);
        } else
            writeToLowerCaches(tempAddress, addr_type, cacheHit, CPU, M2_, cacheHit->mesi);

    } else {
        hitMetrics(ALL_, instType, addr_type);
        writeToLowerCaches(tempAddress, addr_type, cacheHit, CPU, ALL_, E);
    }
}

void traceDecoder(traceAddress* tempAddress, const CPU* CPU) {

    if (tempAddress->inst_type == F) {
        checkCache(tempAddress->instructionAddress, R, CPU, INST);
        traceMetrics(F);

    } else if (tempAddress->inst_type == R){
        checkCache(tempAddress->instructionAddress, R, CPU, INST);
        traceMetrics(F);

        checkCache(tempAddress->dataAddress, R, CPU, DATA);
        traceMetrics(R);
    } else if (tempAddress->inst_type == W){
        checkCache(tempAddress->instructionAddress, R, CPU, INST);
        traceMetrics(F);

        checkCache(tempAddress->dataAddress, W, CPU, DATA);
        traceMetrics(W);
    }
}

int readAddressLine(FILE *inputFile, const CPU* CPU) {
    traceAddress* tempAddress = malloc(sizeof(traceAddress));
    int instructionRead, instructionType;
    char tempLine[30];

        while (fgets(tempLine, 30, inputFile)) {
        instructionMetrics();
        instructionRead = sscanf(tempLine, " %u%*c%d%*c%u",
                &tempAddress->instructionAddress,
                &instructionType,
                &tempAddress->dataAddress);

        if (instructionRead == 1) {
            tempAddress->inst_type = F;
            tempAddress->dataAddress = 0;
        } else {
            if (instructionType == 0)
                tempAddress->inst_type = R;
            else
                tempAddress->inst_type = W;
        }
        traceDecoder(tempAddress, CPU);
    }
    free(tempAddress);
    return 0;
}

unsigned WINAPI readAddressTraces(void* info) {
    FILE *inputFile;
    threadInfo* tInfo = (threadInfo*)info;
    CPU* theCpu1 = (!tInfo->cpu) ? &CPU0 : &CPU1;
	CPU* theCpu2 = (!tInfo->cpu) ? &CPU1 : &CPU2;
	CPU* theCpu3 = (!tInfo->cpu) ? &CPU2 : &CPU3;

    inputFile = fopen(FILE_NAME, "r");

    if (inputFile == NULL) {
        printf("* ERROR: Could not find %s", FILE_NAME);
        return;
    }

    while (readAddressLine(inputFile, theCpu1));
	while (readAddressLine(inputFile, theCpu2));
	while (readAddressLine(inputFile, theCpu3));
    fclose(inputFile);
}

void printCacheMetrics() {
    printMetrics();
}


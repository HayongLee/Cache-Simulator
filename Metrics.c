/*
 ============================================================================
 Name        : Main.c
 Author      : Jesse Bannon
 Class       : TCSS 372 - Computer Architecture
 School      : University of Washington Tacoma
 Copyright   : Use only for educational purposes and do not modify file.
 Description : Handles all metrics recorded during execution of the cache.
 ============================================================================
 */

#include "Metrics.h"
#include "CacheSimulator.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 ===========================================================================

    Static metric variables

 ===========================================================================
 */

static int InstructionCount = 0;
static int FetchCount = 0;
static int ReadCount = 0;
static int WriteCount = 0;

static int TotalLatency = 0;
static int L1iHit = 0, L1iMiss = 0;
static int L1dHit = 0, L1dMiss = 0;
static int L2Hit = 0, L2Miss = 0;
static int L3Hit = 0, L3Miss = 0;
static int M1Hit = 0, M1Miss = 0;
static int M2Hit = 0, M2Miss = 0;

static int MtoS = 0;
static int MtoI = 0;

static int EtoS = 0;
static int EtoM = 0;
static int EtoI = 0;

static int StoM = 0;
static int StoI = 0;

/*
 ===========================================================================

    Functions (descriptions in header)

 ===========================================================================
 */

void printMetrics() {
  printf("\n");
  printf("===========================================\n");
  printf("DUAL-CORE CACHE SIMULATOR\n");
  printf("by Jesse Bannon\n\n");
  printf("TCSS 372 - Computer Architecture\n");
  printf("University of Washington Tacoma\n");
  printf("===========================================\n\n");

  printf("Total Latency:\n");
  printf(" - In Nanoseconds:  %d\n", TotalLatency);
  printf(" - In Microseconds: %f\n", (float)TotalLatency/1000);
  printf(" - In Milliseconds: %f\n", (float)TotalLatency/1000000);
  printf(" - In Seconds:      %f\n", (float)TotalLatency/1000000000);
  printf("Total Instructions: %d\n", InstructionCount);
  printf(" - Fetches:     %d\n", FetchCount);
  printf(" - Data Reads:  %d\n", ReadCount);
  printf(" - Data Writes: %d\n", WriteCount);
  printf("L1i Hit Ratio: %f\n", (float)L1iHit/(L1iHit+L1iMiss));
  printf(" - L1i Hits:   %d\n", L1iHit);
  printf(" - L1i Misses: %d\n", L1iMiss);
  printf("L1d Hit Ratio: %f\n", (float)L1dHit/(L1dHit+L1dMiss));
  printf(" - L1d Hits:   %d\n", L1dHit);
  printf(" - L1d Misses: %d\n", L1dMiss);
  printf("L2 Hit Ratio:  %f\n", (float)L2Hit/(L2Hit+L2Miss));
  printf(" - L2 Hits:    %d\n", L2Hit);
  printf(" - L2 Misses:  %d\n", L2Miss);
  printf("L3 Hit Ratio:  %f\n", (float)L3Hit/(L3Hit+L3Miss));
  printf(" - L3 Hits:    %d\n", L3Hit);
  printf(" - L3 Misses:  %d\n", L3Miss);
  printf("M1 Hit Ratio:  %f\n", (float)M1Hit/(M1Hit+M1Miss));
  printf(" - M1 Hits:    %d\n", M1Hit);
  printf(" - M1 Misses:  %d\n", M1Miss);
  printf("M2 Hit Ratio:  %f\n", (float)M2Hit/(M2Hit+M2Miss));
  printf(" - M2 Hits:    %d\n", M2Hit);
  printf(" - M2 Misses:  %d\n", M2Miss);
  printf("MESI State Transitions:\n");
  printf("\tM\tE\tS\tI\n");
  printf("M\t-\t-\t%d\t%d\n", MtoS, MtoI);
  printf("E\t%d\t-\t%d\t%d\n", EtoM, EtoS, EtoI);
  printf("S\t%d\t-\t-\t%d\n", StoM, StoI);
  printf("I\t-\t-\t-\t-\n\n");
}

void L1miss(ADDR_TYPE type) {
    if (type == INST)
        ++L1iMiss;
    else
        ++L1dMiss;
}

void instructionMetrics() {
    ++InstructionCount;
}

void traceMetrics(INST_TYPE type) {

    switch(type) {
    case F: ++FetchCount; return;
    case R: ++ReadCount; return;
    case W: ++WriteCount; return;
    default: return;
    }
}

void hitMetrics(CACHE cacheHit, INST_TYPE inst_type, ADDR_TYPE addr_type) {
    switch(cacheHit) {
    case CPU0L1i_:
    case CPU1L1i_:
	case CPU2L1i_:
	case CPU3L1i_:
        TotalLatency += L1_LATENCY;
        ++L1iHit; return;


    case CPU0L1d_:
    case CPU1L1d_:
	case CPU2L1d_:
	case CPU3L1d_:
        TotalLatency += L1_LATENCY;
        ++L1dHit; return;

    case CPU0L2_:
    case CPU1L2_:
	case CPU2L2_:
	case CPU3L2_:
        TotalLatency += L2_LATENCY;
        L1miss(addr_type);
        ++L2Hit; return;

    case L3_:
        TotalLatency += L3_LATENCY;
        L1miss(addr_type);
        ++L2Miss;
        ++L3Hit; return;

    case M1_:
        TotalLatency += M1_LATENCY;
        L1miss(addr_type);
        ++L2Miss;
        ++L3Miss;
        ++M1Hit; return;

    case M2_:
        if (inst_type == W)
            TotalLatency += M2_WRITE_LATENCY;
        else
            TotalLatency += M2_READ_LATENCY;
        L1miss(addr_type);
        ++L2Miss;
        ++L3Miss;
        ++M1Miss;
        ++M2Hit; return;

    case ALL_:
        if (inst_type == W)
            TotalLatency += M2_WRITE_LATENCY;
        else
            TotalLatency += M2_READ_LATENCY;
        L1miss(addr_type);
        ++L2Miss;
        ++L3Miss;
        ++M1Miss;
        ++M2Miss;
        return;
    }
}

void mesiMetrics(MESI prev, MESI curr) {
    switch(prev) {
    case M:
        switch(curr) {
        case S: ++MtoS; return;
        case I: ++MtoI; return;
        default: return;
        } return;

    case E:
        switch(curr) {
        case M: ++EtoM; return;
        case S: ++EtoS; return;
        case I: ++EtoI; return;
        default: return;
        } return;

    case S:
        switch(curr) {
        case M: ++StoM; return;
        case I: ++StoI; return;
        default: return;
        } return;

    default: return;
    }
}

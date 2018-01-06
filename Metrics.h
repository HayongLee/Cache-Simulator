/*
 ============================================================================

 Name        : Metrics.h
 Author      : Jesse Bannon
 Class       : TCSS 372 - Computer Architecture
 School      : University of Washington Tacoma
 Copyright   : Use only for educational purposes and do not modify file.
 Description : Handles all metrics recorded during execution of the cache.

 ============================================================================
 */

#ifndef METRICS_H_
#define METRICS_H_
#include "AddressTrace.h"

/*
 ============================================================================

     Function prototypes (and descriptions)

 ============================================================================
 */

void printMetrics();
/* Prints all current metrics to console. */

void instructionMetrics();
void traceMetrics(INST_TYPE type);
void latencyMetrics(CACHE cache, INST_TYPE type);
void hitMetrics(CACHE cacheHit, INST_TYPE inst_type, ADDR_TYPE addr_type);
void mesiMetrics(MESI prev, MESI curr);

#endif

/*
 ============================================================================
 Name        : Main.c
 Author      : Jesse Bannon
 Class       : TCSS 372 - Computer Architecture
 School      : University of Washington Tacoma
 Copyright   : Use only for educational purposes and do not modify file.
 Description : Reads in a .csv of address traces and simulates a
               five-level cache hierarchy and two CPUs.
 ============================================================================
 */
#define HAVE_STRUCT_TIMESPEC
#define _CRT_SECURE_NO_WARNINGS
#define __STDC_FORMAT_MACROS

#pragma warning(disable:4996)

#include <stdlib.h>
#include <windows.h>
#include <process.h>
#include <stdio.h>

#include "CacheSimulator.h"

int main(void) {


    threadInfo thread1 = { 0 };
    threadInfo thread2 = { 1 };
	threadInfo thread3 = { 2 }; 
	threadInfo thread4 = { 3 };
	
	HANDLE hthread1, hthread2, hthread3, hthread4;


	unsigned int threadID1 = 0, threadID2 = 1, threadID3 = 2, threadID4 = 3;
	
	hthread1 = CreateThread(NULL, 0, (void*)readAddressTraces, &thread1, 0, &threadID1);
	hthread2 = CreateThread(NULL, 0, (void*)readAddressTraces, &thread2, 0, &threadID2);
	hthread3 = CreateThread(NULL, 0, (void*)readAddressTraces, &thread3, 0, &threadID3);
	hthread4 = CreateThread(NULL, 0, (void*)readAddressTraces, &thread4, 0, &threadID4);
	
	GetExitCodeThread(hthread1, &threadID1);
	GetExitCodeThread(hthread2, &threadID2);
	GetExitCodeThread(hthread3, &threadID3);
	GetExitCodeThread(hthread4, &threadID4);
	
	CloseHandle(hthread1);
	CloseHandle(hthread2);
	CloseHandle(hthread3);
	CloseHandle(hthread4);
	
    printCacheMetrics();

	system("pause");
    return 0;
}
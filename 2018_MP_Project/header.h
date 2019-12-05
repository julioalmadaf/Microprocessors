#ifndef _HEADER_H_
#define _HEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>


typedef unsigned int uint;
typedef unsigned char uchar;

#define NUM_OF_LOGICAL_BLOCK	256
#define NUM_OF_LOG_BLOCK	30 // set any value that you need (but it should be reasonable value)
#define NUM_OF_TOTAL_BLOCK		(NUM_OF_LOGICAL_BLOCK + NUM_OF_LOG_BLOCK)

#define PAGE_PER_BLOCK_BIT		7
#define PAGE_PER_BLOCK			(1 << PAGE_PER_BLOCK_BIT)

#define SECTOR_PER_PAGE_BIT		2
#define SECTOR_PER_PAGE			(1 << SECTOR_PER_PAGE_BIT)

#define MAX_LBA					(NUM_OF_LOGICAL_BLOCK)
#define MAX_LPA					(NUM_OF_LOGICAL_BLOCK * PAGE_PER_BLOCK)
#define MAX_LSA					(NUM_OF_LOGICAL_BLOCK * PAGE_PER_BLOCK * SECTOR_PER_PAGE)


// this is used to calculate the running time of NAND flash memory
#define tR		25
#define tPROG	200
#define tBERS	1500

#include "struct.h"

extern FTL_t* ftl; // modified 2011.11.11
extern FTL_log_t* ftl_log;

// erase_counter : Erase count of PBA
extern int erase_counter[NUM_OF_TOTAL_BLOCK];

void initialization(void);

#endif
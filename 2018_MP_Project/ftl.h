#ifndef _FTL_H_
#define _FTL_H_

#include "header.h"

void FTL_Write(uint LSA, int length, char* data);
void FTL_Read(uint LSA, int length, char* data);
void FTL_Run(uint LSA, uint length, uint r_w, char* data);

#endif
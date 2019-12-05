#ifndef _NAND_COMMAND_H_
#define _NAND_COMMAND_H_

#include "header.h"

void init_NAND();
void NAND_Command(uchar cmd, uint block, uint page, char* data);
void FindFilePosition(int block_addr, int page_addr);
void WritePage(char* data);
void ReadPage(char *data);
void EraseBlock();

#endif
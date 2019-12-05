#ifndef _ADDRESS_TRANS_H_
#define _ADDRESS_TRANS_H_

#include "header.h"

uint get_LPA_From_LSA(uint LSA);
uint get_LPA_Offset_From_LSA(uint LSA);
uint get_a_data_block(uint LPA);
int find_Block(uint LPA);
uint check_log_block(uint LPA);
uint get_a_log_block(uint LPA, uint& pagePrevLoc, uint& PrevPageNum);
uint get_a_free_page(uint LPA, uint& pagePrevLoc, uint& PrevPageNum);


void write_page(uint LPA, char* data);
void update_page(uint LPA, char* data);
#endif
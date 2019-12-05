#include "header.h"
#include "nand_command.h"

FILE *NAND_Storage;

void init_NAND(){
	NAND_Storage = fopen("nand_storage.txt", "w+");
}

void NAND_Command(uchar cmd, uint block, uint page, char* data)
{
	
	switch(cmd){
		case NAND_PROGRAM:
			count_nand_program++;

			FindFilePosition(block, page);

			WritePage(data);

			break;

		case NAND_READ:
			count_nand_read++;

			FindFilePosition(block, page);

			ReadPage(data);

			break;

		case NAND_ERASE:
			count_nand_erase++;

			//initalize all the pages in the block using write page function
			erase_counter[block]++;
			FindFilePosition(block, page);
			EraseBlock();
			break;
	}
}

void FindFilePosition(int block_addr, int page_addr)
{
	fseek(NAND_Storage,  block_addr * PAGE_PER_BLOCK * SECTOR_PER_PAGE + page_addr * SECTOR_PER_PAGE, SEEK_SET);
}

void EraseBlock()
{
	char data[513];
	for(int i=0; i<512; i++){
		data[i]='0';
	}
	data[512]='\0';
	fwrite(data, sizeof(char), PAGE_PER_BLOCK * SECTOR_PER_PAGE, NAND_Storage);
}

void WritePage(char* data)
{
	fwrite(data, sizeof(char), SECTOR_PER_PAGE, NAND_Storage);
}

void ReadPage(char *data)
{
	fread(data, sizeof(char), SECTOR_PER_PAGE, NAND_Storage);
}

#include "header.h"
#include "address_translation.h"
#include "nand_command.h"


void FTL_Write(uint LSA, int length, char* data){
	uint i,j, count=0;
	uint block, page;
	uchar location;
	char reg[5];

	while(length > 0){
		uint LPA = get_LPA_From_LSA(LSA);
		uchar LPA_offset = get_LPA_Offset_From_LSA(LSA);
		uchar LPA_length = length > (uint)SECTOR_PER_PAGE-LPA_offset ? SECTOR_PER_PAGE-LPA_offset : length;

		//initialize the register
		for(j=0;j<SECTOR_PER_PAGE;j++){
			reg[j] = '-';
		}
		//add the function to translate data to register 	
		
		for(j=0; j<LPA_length; j++){
			reg[LPA_offset + j] = data[j+count];
		}

		reg[4] = '\0';

		if(find_Block(LPA))
		{	//write the log block
			update_page(LPA, reg);
		}
		else
		{						//write the data block*/
			write_page(LPA, reg);
		}

		
		if(LPA_offset==0)
		{
			LSA = LSA + SECTOR_PER_PAGE;
			length = length - SECTOR_PER_PAGE;
		}
		else
		{
			LSA = LSA - LPA_offset + SECTOR_PER_PAGE;
			length = length + LPA_offset - SECTOR_PER_PAGE;
		}
		count += LPA_length;
	}
}

void FTL_Read(uint LSA, int length, char* data)
{
	uint i,j,k=0, count=0;
	uint block, page;
	uchar location;
	char reg[5];

	while(length > 0)
	{
		uint LPA = get_LPA_From_LSA(LSA);
		uint LPA_offset = get_LPA_Offset_From_LSA(LSA);
		uint LPA_length = length > SECTOR_PER_PAGE-LPA_offset ? SECTOR_PER_PAGE-LPA_offset : length;
		uint flag = 0;

		if(LPA >= NUM_OF_LOGICAL_BLOCK * PAGE_PER_BLOCK)
		{
			printf("error reading: inavailable address\n");
			system("pause");
			exit(1);
		}
		uint LBA = LPA / PAGE_PER_BLOCK;
		uint PageNm = LPA % PAGE_PER_BLOCK;
		if(ftl->Blk_Table[LBA].has_log_block == 0)
		{

			NAND_Command(NAND_READ, ftl->Blk_Table[LBA].pba, PageNm, (char*)reg);


		}
		else
		{
			uint log_addr = ftl->Blk_Table[LBA].log_block_address;

			for(i=0; i<ftl_log->Blk_Table[log_addr].top_page; i++)
			{
				if(ftl_log->Blk_Table[log_addr].page[i].page_num == PageNm && ftl_log->Blk_Table[log_addr].page[i].state == PAGE_VALID)
				{
						NAND_Command(NAND_READ, ftl_log->Blk_Table[log_addr].pba, i, (char*)reg);
						flag=1;
						break;
				}
			}
			if(flag==0)
			{
				NAND_Command(NAND_READ, ftl->Blk_Table[LBA].pba, PageNm, (char*)reg);
			}
		}
		reg[4]='\0';

		for(j=LPA_offset;j<(LPA_offset+LPA_length);j++)
		{
			data[(k++)] = reg[j];
		}

		if(LPA_offset==0)
		{
			LSA = LSA + SECTOR_PER_PAGE;
			length = length - SECTOR_PER_PAGE;
		}
		else
		{
			LSA = LSA - LPA_offset + SECTOR_PER_PAGE;
			length = length + LPA_offset - SECTOR_PER_PAGE;
		}
		count += LPA_length;
	}
	data[k]='\0';
}

void FTL_Run(uint LSA, uint length, uint r_w, char* data)
{
	if(r_w)
	{
		FTL_Read(LSA, length, data);
	}
	else
	{
		FTL_Write(LSA, length, data);
	}
}
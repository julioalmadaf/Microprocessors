#include "nand_command.h"
#include "address_translation.h"
#include "garbage_collection.h"

int find_Block(uint LPA)
{
	//Use the "is_free" and "top_page" of the block mapping table
	uint block_address = get_a_data_block(LPA); // LPA/128 = block number
	if(ftl->Blk_Table[block_address].top_page < (LPA % PAGE_PER_BLOCK))
		return 0; //found location
	else 
		return 1; //not found*/
}


uint get_LPA_From_LSA(uint LSA)
{
	return (LSA >> SECTOR_PER_PAGE_BIT) % MAX_LPA;
}

uint get_LPA_Offset_From_LSA(uint LSA)
{
	return LSA % SECTOR_PER_PAGE;
}

uint check_log_block(uint LPA)
{
	uint block_address = get_a_data_block(LPA);
	return ftl->Blk_Table[block_address].has_log_block;
}

void update_page(uint LPA, char* data)
{
	uint PPA; //physical page address
	uint pagePrevLoc, PrevPageNum, prevPageBlock;
	uint LBA = LPA / PAGE_PER_BLOCK;
	char temp_data[5];

	if(!check_log_block(LPA))
	{		//need to allocate the log block
		PPA = get_a_log_block(LPA, pagePrevLoc, PrevPageNum);
	}
	else
	{						//don't need to allocate the log block
		if(ftl_log->Blk_Table[ ftl->Blk_Table[LBA].log_block_address].num_free_page == 0)
		{
			GarbageCollection( ftl->Blk_Table[LBA].log_block_address );
			PPA = get_a_log_block(LPA, pagePrevLoc, PrevPageNum);
		}
		else
		{
			PPA = get_a_free_page(LPA, pagePrevLoc, PrevPageNum);
		}
	}

	uint NewPageBlock = get_a_data_block(PPA); //
	uint NewPageNum = PPA % PAGE_PER_BLOCK;
	uint i;
	
	if(pagePrevLoc == DATA_BLOCK)
	{
		prevPageBlock = ftl->Blk_Table[get_a_data_block(LPA)].pba;
	}
	else
	{
		prevPageBlock = NewPageBlock;
	}

	if(strstr(data,"-")==NULL)
	{
		NAND_Command(NAND_PROGRAM, NewPageBlock, NewPageNum, data);
	}
	else
	{	// when we need to modify the contents
		NAND_Command(NAND_READ, prevPageBlock, PrevPageNum, (char*)temp_data); // first, we need to read the data
		temp_data[4]='\0';
		// and we need to modify the contents "data"
		for(i=0; i<SECTOR_PER_PAGE; i++)
		{
			if(data[i]=='-') data[i] = temp_data[i];
		}

		NAND_Command(NAND_PROGRAM, NewPageBlock, NewPageNum, data); // write the new data
	}
}

void write_page(uint LPA, char* data)
{
	uint block_address = ftl->Blk_Table[ get_a_data_block(LPA) ].pba;
	uint page_address = LPA % PAGE_PER_BLOCK;

	ftl->Blk_Table[block_address].is_free = BLOCK_VALID;
	ftl->Blk_Table[block_address].top_page = page_address+1;
	NAND_Command(NAND_PROGRAM, block_address, page_address, data);
}

uint get_a_data_block(uint LPA)
{
	return LPA / PAGE_PER_BLOCK;
}

uint get_a_log_block(uint LPA, uint& pagePrevLoc, uint& PrevPageNum) // Get a new log block
{
	uint PPA; //physical page address
	uint log_block_address;
	uint i,j;
	uint LBA = get_a_data_block(LPA);

	if(number_of_free_block <= 1)
	{
	//check the number of free block
	//if there is not the free block, garbage collection is processed.
		GarbageCollection();
	}
	
	number_of_free_block--;//Use free log block

	//checks all log blocks
	for(i=0; i<NUM_OF_LOG_BLOCK; i++)
	{
		if(ftl_log->Blk_Table[i].is_free == BLOCK_FREE) // takes the first log block which is free
		{
			log_block_address = i;
			break;	
		}
	}

	ftl->Blk_Table[LBA].log_block_address = log_block_address;// Set log block address in Block table
	ftl->Blk_Table[LBA].has_log_block = 1;//its block table index has log block
	ftl->Blk_Table[LBA].num_invalid_page++; //Increments counter of invalid pages
	
	ftl_log->Blk_Table[log_block_address].is_free = BLOCK_VALID;//Block valid set
	ftl_log->Blk_Table[log_block_address].data_block_address = LBA; //Assigns LBA to the respective Data block
	ftl_log->Blk_Table[log_block_address].num_free_page = PAGE_PER_BLOCK-1; //Free page counter loses a free page
	ftl_log->Blk_Table[log_block_address].top_page = 1;
	
	ftl_log->Blk_Table[log_block_address].page = (page_t*)malloc(sizeof(page_t)*(PAGE_PER_BLOCK));//Memory allocation for page mapping
	for(j=0; j<PAGE_PER_BLOCK; j++)
	{//initiate
		ftl_log->Blk_Table[log_block_address].page[j].LPA = 0;
		ftl_log->Blk_Table[log_block_address].page[j].state = 0;
		ftl_log->Blk_Table[log_block_address].page[j].page_num = 0;
	}

	ftl_log->Blk_Table[log_block_address].page[0].state = PAGE_VALID;
	ftl_log->Blk_Table[log_block_address].page[0].page_num = LPA % PAGE_PER_BLOCK;

	pagePrevLoc = DATA_BLOCK;
	PrevPageNum = LPA % PAGE_PER_BLOCK;
	PPA = ftl_log->Blk_Table[log_block_address].pba * PAGE_PER_BLOCK;
	return PPA;
}


uint get_a_free_page(uint LPA, uint& pagePrevLoc, uint& PrevPageNum)
{
	uint PPA; //physical page address
	uint LBA = get_a_data_block(LPA);
	uint PageNum = LPA % PAGE_PER_BLOCK;
	uint log_block_address = ftl->Blk_Table[LBA].log_block_address;
	uint i,j, flag=0;
	
	
	for(i=0; i< ftl_log->Blk_Table[log_block_address].top_page; i++)
	{
		if(ftl_log->Blk_Table[log_block_address].page[i].page_num == PageNum && ftl_log->Blk_Table[log_block_address].page[i].state == PAGE_VALID)
		{
				pagePrevLoc = LOG_BLOCK;
				PrevPageNum=i;
				ftl_log->Blk_Table[log_block_address].page[i].state = PAGE_INVALID;
				ftl_log->Blk_Table[log_block_address].num_invalid_page++;
				flag = 1;
		}
	}
	if(flag == 0)
	{
		pagePrevLoc = DATA_BLOCK;
		PrevPageNum = PageNum;
		ftl->Blk_Table[LBA].num_invalid_page++;
	}
	
	ftl_log->Blk_Table[log_block_address].page[ ftl_log->Blk_Table[log_block_address].top_page ].page_num=PageNum;
	ftl_log->Blk_Table[log_block_address].page[ ftl_log->Blk_Table[log_block_address].top_page ].state = PAGE_VALID;

	PPA = ftl_log->Blk_Table[log_block_address].pba * PAGE_PER_BLOCK + ftl_log->Blk_Table[log_block_address].top_page;//새로 업데이트 될 로그블록페이지의 PPA반환
	ftl_log->Blk_Table[log_block_address].top_page++;
	ftl_log->Blk_Table[log_block_address].num_free_page--;

	return PPA;
}


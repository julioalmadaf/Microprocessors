#include "header.h"
#include "garbage_collection.h"
#include "nand_command.h"


void GarbageCollection( int selected_victim) //selected victim initialized in -1 from headers
{
	uint i,j;
	uint victim_block=-1, garbage_type=1, flag = 0, sw=0;
	
	//uses or searches for the last victim log block
	if(selected_victim==-1) // if selected victim is not set, by default searches for the last not free log block
	{
		for (i = 0; i < NUM_OF_LOGICAL_BLOCK; i++)
		{
			//Checks for victim block with ONLY invalid pages
			if (ftl->Blk_Table[i].num_invalid_page == 128 )
			{
				victim_block = i; //Assign victim block number
				garbage_type = 0; //Switch merge
			}
			else 
			{
				for (j = 0; j < NUM_OF_LOG_BLOCK; j++) //30 log blocks
				{
					if ((ftl_log->Blk_Table[j].is_free != BLOCK_FREE) && garbage_type == 1) victim_block = j; //Last log block that is not free
				}
			}
		}
	}
	else //this will happen if the not free log block is provided
	{
		victim_block = selected_victim; //selected victim set
	}


	if(garbage_type == 1) //initialized as 1
	{
			// Full block merge		
			uint free_log_block, LBA = ftl_log->Blk_Table[victim_block].data_block_address; //Address of matching data block
			char temp_data[5];
			int min=99999999; //not used

			//Choosing free log block to merge data and log block
			for(i=0; i<NUM_OF_LOG_BLOCK; i++) 
			{
				if(ftl_log->Blk_Table[i].is_free == BLOCK_FREE)
				{
					free_log_block = i;  //saves last number of log block that is free
				}
			}

			for(i=0; i<PAGE_PER_BLOCK; i++) //goes through each page, 128 pages
			{ 
				flag=0; //flag to specify if log block has valid data or not
				for(j=0; j<ftl_log->Blk_Table[victim_block].top_page; j++)  // Check log block, Valid page? checks if data on each page in a log block are valid or not
				{
					//stores each valid page from the victim log block into the free block ?
					if(ftl_log->Blk_Table[victim_block].page[j].page_num == i && ftl_log->Blk_Table[victim_block].page[j].state == PAGE_VALID) //checks page num and state
					{
							//i th Valid page in log block
							flag=1; //valid data in log block
							NAND_Command(NAND_READ, ftl_log->Blk_Table[victim_block].pba, j, (char*)temp_data); //cmd=read,physical block address, page, data
							temp_data[4]='\0'; //stores NULL on temp_data
							NAND_Command(NAND_PROGRAM, ftl_log->Blk_Table[free_log_block].pba, i, (char*)temp_data); // write the new data
							break;
					}
				}
				//stores each valid page from the data block into the free block ?
				if(flag == 0) // If not, valid data is in data block
				{
					NAND_Command(NAND_READ, ftl->Blk_Table[LBA].pba, i, (char*)temp_data);
					temp_data[4]='\0';
					NAND_Command(NAND_PROGRAM, ftl_log->Blk_Table[free_log_block].pba, i, (char*)temp_data); // write the new data

				}
			}

			uint temp_addr = ftl->Blk_Table[LBA].pba; //stores Physical block address into temp address
			ftl->Blk_Table[LBA].pba = ftl_log->Blk_Table[free_log_block].pba; //that same address = address from log block
			ftl_log->Blk_Table[free_log_block].pba = temp_addr; //address from log block= temp address
			// Change Merged block to data block


			ftl->Blk_Table[LBA].has_log_block = 0; //log block not allocated
			ftl->Blk_Table[LBA].log_block_address = 0;  //Log block address that matching with this data block
			ftl->Blk_Table[LBA].num_invalid_page = 0; //no. invalid pages
			ftl->Blk_Table[LBA].top_page = PAGE_PER_BLOCK; //top page number of free page
			ftl->Blk_Table[LBA].is_free = BLOCK_VALID; //block not free
			//initiation

			erase_Block(victim_block); //frees victim block
			erase_Block(free_log_block); //frees log block
			number_of_free_block ++; //increments the counter of free log blocks
	}
	else //switch merge
	{
		//Exchange of addresses between Data and Log block
		uint LBA = ftl->Blk_Table[victim_block].log_block_address;			//Address of matching log block
		uint temp_addr = ftl_log->Blk_Table[LBA].pba;						//Stores Physical block address into temp address
		ftl_log->Blk_Table[LBA].pba = ftl->Blk_Table[victim_block].pba;		//Assign Data block pba to log block pba
		ftl->Blk_Table[victim_block].pba = temp_addr;						//Address from data block= temp address
		
		//Free Data block
		ftl->Blk_Table[victim_block].has_log_block = 0;						//Log block not allocated
		ftl->Blk_Table[victim_block].log_block_address = 0;					//Log block address that matching with this data block
		ftl->Blk_Table[victim_block].num_invalid_page = 0;					//No. invalid pages
		ftl->Blk_Table[victim_block].top_page = PAGE_PER_BLOCK;				//Top page number of free page
		ftl->Blk_Table[victim_block].is_free = BLOCK_VALID;					//Block not free

		erase_Block(LBA); //Frees log block
		
		number_of_free_block++; //Increments the counter of free log blocks

	}
}


void erase_Block(uint block) //transform a block into a free block
{
	 // erase	
	free(ftl_log->Blk_Table[block].page);
	ftl_log->Blk_Table[block].is_free = BLOCK_FREE;
	ftl_log->Blk_Table[block].num_invalid_page = 0;
	ftl_log->Blk_Table[block].top_page = 0;
	ftl_log->Blk_Table[block].page = NULL;
	ftl_log->Blk_Table[block].data_block_address = 0;
	ftl_log->Blk_Table[block].num_free_page = PAGE_PER_BLOCK;
	// Initialize

	NAND_Command(NAND_ERASE, ftl_log->Blk_Table[block].pba, 0, 0);	
}


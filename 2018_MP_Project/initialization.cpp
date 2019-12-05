#include "header.h"
#include "struct.h"

void initialization()
{
	int i, j;

	ftl = (FTL_t*) malloc(sizeof(FTL_t));  // modified 2011.11.11

	//initialize the block FTL mapping
	for(i=0;i<NUM_OF_LOGICAL_BLOCK;i++){ //Initializes everything as free data blocks
		ftl->Blk_Table[i].is_free = BLOCK_FREE;
		ftl->Blk_Table[i].pba = i;
		ftl->Blk_Table[i].num_invalid_page = 0;
		ftl->Blk_Table[i].top_page = 0;
		ftl->Blk_Table[i].has_log_block = 0;
		ftl->Blk_Table[i].log_block_address = 0;
	}

	ftl_log = (FTL_log_t*) malloc(sizeof(FTL_log_t));

	for(i=0;i<NUM_OF_LOG_BLOCK;i++){//Initializes everything as free log blocks
		ftl_log->Blk_Table[i].is_free = BLOCK_FREE;
		ftl_log->Blk_Table[i].pba = NUM_OF_LOGICAL_BLOCK + i;
		ftl_log->Blk_Table[i].num_invalid_page = 0;
		ftl_log->Blk_Table[i].top_page = 0;
		ftl_log->Blk_Table[i].data_block_address = 0;
		ftl_log->Blk_Table[i].page = NULL;
		ftl_log->Blk_Table[i].num_free_page = 0;
	}

	//For result
	count_nand_program = 0;
	count_nand_read = 0;
	count_nand_erase = 0;

	for(i=0; i<NUM_OF_TOTAL_BLOCK; i++){
		erase_counter[i]=0;
	}
	// and other variables or arrays
}
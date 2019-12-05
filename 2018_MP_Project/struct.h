#ifndef _STRUCT_H_
#define _STRUCT_H_

enum PAGE_STATE{
	PAGE_FREE    = 1,
	PAGE_VALID   = 2,
	PAGE_INVALID = 3,
};

enum BLOCK_STATE {
	BLOCK_FREE	=	0,
	BLOCK_VALID	=	1,
};

enum PREV_PAGE_LOCATION {
	DATA_BLOCK	=	0,
	LOG_BLOCK	=	1,
};

enum NAND_CMD{
	NAND_PROGRAM			= 1,
	NAND_READ				= 2,
	NAND_ERASE				= 3,
	NAND_COPY_BACK_READ		= 4,
	NAND_COPY_BACK_PROGRAM	= 5,
	NAND_RANDOM_DATA_INPUT  = 6
};



typedef struct{
	uint	LPA;
	uchar	state;	// 0: free, 1: valid, 2: invalid
	// other variables //
	uint	page_num;
	//--other variables //
}page_t;   


typedef struct{
	uchar	is_free;	// 0: free, 1: not free
	uint    pba;		//physical block address
	uchar	top_page;	// Top page number of free page
	uchar	num_invalid_page; //
	uchar	has_log_block;  // 0 : log block is not allocated, 1 : log block is allocated
	uint	log_block_address;  //Log block address that matching with this data block
	// other variables //
	//--other variables //
}data_blk_table_t;   // Data block list struct

typedef struct{
	uchar	is_free;	// 0: free, 1: not free
	uint    pba;		//physical block address
	uchar	top_page;//Top page number of free page
	uchar	num_invalid_page;
	page_t	*page;		//Page data
	uint	data_block_address; //Data block address that matching with this log block
	uint	num_free_page;		//# of free page in that block
	// other variables //
	//--other variables //
}log_blk_table_t;  // Log block list struct

// FTL
typedef struct {
	data_blk_table_t		Blk_Table[NUM_OF_LOGICAL_BLOCK];
	// other global variable declaration

	//--other global variable declaration
}FTL_t;

typedef struct {
	log_blk_table_t		Blk_Table[NUM_OF_LOG_BLOCK];
	// other global variable declaration

	//--other global variable declaration
}FTL_log_t;

extern uint number_of_free_block;

//For result
extern uint count_nand_program;
extern uint count_nand_read;
extern uint count_nand_erase;

#endif
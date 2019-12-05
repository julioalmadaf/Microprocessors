/*
Julio Cesar Almada Fuerte
Mario Alfredo Donnadieu

BAST FTL Structure
BAST = block-level associative sector translation
FTLs are intermediate SW layer between file system and NAND flash memories
their main function are: 

Address translation: Uses LBAs to indicate the location of data to be accessed.
the BAST address translation is block-mapped based
They use LSBs of the LBAs as a page offset to indicate the page where data is stored of the corresponding LBA, with this they use a smaller mapping table

Garbage collection: Done when there is not a free block to be allocated for new data,
it erases previously allocated blocks by rearranging valid pages into a victim block, with this you can free blocks
Garbage collection determines the performance of an FTL
Garbage collection is done by merging blocks, which copies valid pages and erases blocks.

Bad block management: a bad block is a block that contains one or more pages with errors, 
bad blocks should not be used because they produce more errors.

Wear-leveling: Memory cells have a limited life, so wear leveling distribute these cycles between all the blocks on a device evenly.

BAST TRANSLATION SCHEME
Uses LSBs of LBAs as offsets for pages to indicate where the desired data is stored
MSBs used as index for block-level mapping table

BAST has two types of blocks
Data: flash blocks for newly written data
Log: write-buffer for updated data.
new data is written into it, log block corresponds to a data block
the original data on any type of block is invalidated
data in a data block is found by block-level address translation scheme
data in log blocks are found by page-level mapping table for log blocks in SRAM

Bast only supports switch and full merge operations
This operations are triggered by many log blocks, there is no free blocks to allocate and if all pages of a log block are consumed by previous updates

*/
#include "header.h"
#include "ftl.h"
#include "nand_command.h"

FTL_t* ftl; // modified 2011.11.11
FTL_log_t* ftl_log;
int erase_counter[NUM_OF_TOTAL_BLOCK];

uint count_nand_program;
uint count_nand_read;
uint count_nand_erase;
uint number_of_free_block=NUM_OF_LOG_BLOCK;
int main()
{
	uint LSA, length, operation;
	uint Operation_Count = 0;
	FILE *trace_in, *result_out;
	char* data; // modified 2011.11.11
	int i;

	LARGE_INTEGER freq, start, end;
	
	trace_in = fopen("trace_input.txt", "r");
	result_out = fopen("result.txt", "w");
	/////////////////////////////////////////////////////your input file
	
	initialization(); // modified 2011.11.11
	init_NAND();
	
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	int cnt=0;
	while(fscanf(trace_in, "%d\t%d\t%d", &LSA, &length, &operation) != EOF){
		cnt++;

		//if(cnt++ == 2) break;
		data = (char*)malloc(sizeof(char)*length+1);  // modified 2011.11.11
		//data = new char[length+1];
		if(operation == 0){ // if operation is write
			int i;
			fscanf(trace_in,"\t");
			for(i=0; i<length; i++){
				fscanf(trace_in,"%c",&data[i]);
			}
			data[length] = '\0';
			fscanf(trace_in,"\n");
		}
		else{
			fscanf(trace_in,"\n");
		}

		FTL_Run(LSA, length, operation, data);

		if(operation == 1){ // if operation is read
			printf( "%s\n",data);
			fprintf(result_out, "%s\n",data);
		}
		Operation_Count++;
		if(cnt%100==0)
			printf("%d th command \n",cnt);
		free(data);  // modified 2011.11.11
		//delete data;
	}

	QueryPerformanceCounter(&end);
	
	free(ftl);  // modified 2011.11.11

	fclose(trace_in);
	fclose(result_out);

	double run1 = (double)count_nand_program * (double)tPROG;
	double run2 = count_nand_read * tR;
	double run3 = count_nand_erase * tBERS;
	double total_run = run1+run2+run3;
	


	FILE *fp_er = fopen("erase_count.txt","w");
	
	double average_erase = count_nand_erase/NUM_OF_TOTAL_BLOCK;
	double variation=0;
	for(i=0; i<NUM_OF_TOTAL_BLOCK; i++){
		fprintf(fp_er,"pba : %d\t%d\n", i, erase_counter[i]);
		variation += ((erase_counter[i]-average_erase)*(erase_counter[i]-average_erase));
	}
	
	variation /= NUM_OF_TOTAL_BLOCK;
	FILE *fp_wear = fopen("wear-leveling.txt","w");

	fprintf(fp_wear,"Running Time: %4.3f ms\n", (double)(end.QuadPart - start.QuadPart) / freq.QuadPart * 1000);
	fprintf(fp_wear,"Variation of erase count : %lf \n",variation);
	fprintf(fp_wear,"Total # of program:	%d \t running time: %lf\n", count_nand_program, run1);
	fprintf(fp_wear,"Total # of read:	%d \t running time: %lf\n", count_nand_read, run2);
	fprintf(fp_wear,"Total # of erase:	%d \t running time: %lf\n", count_nand_erase, run3);

	printf("Running Time: %4.3f ms\n", (double)(end.QuadPart - start.QuadPart) / freq.QuadPart * 1000);
	printf("Variation of erase count : %lf \n",variation);
	printf("Total # of program:	%d \t running time: %lf\n", count_nand_program, run1);
	printf("Total # of read:	%d \t running time: %lf\n", count_nand_read, run2);
	printf("Total # of erase:	%d \t running time: %lf\n", count_nand_erase, run3);

	system("pause"); // modified 2011.11.11*/
	
	return 0;
}
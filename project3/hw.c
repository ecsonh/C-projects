#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int global_counter = 0;

struct page{//each page has 8 addresses, in both virtual and main memory
	int addresses[8];
};

struct PageTableEntries {
    int v_page_num, valid_bit, dirty_bit, page_num, time_stamp;
};

struct physical_memory{
	struct page main_mem[4];
};

struct virtual_memory{
	struct page virtual_mem[16];
};



void increment_counters(struct PageTableEntries pageTable[16]);
void increment_counters(struct PageTableEntries pageTable[16]){
	int i;
	for (i=0;i<16;i++){
		if(pageTable[i].valid_bit){pageTable[i].time_stamp++;}//all pages that are in main memory have their time stamp incremented after valid input
	}
	global_counter++; //global counter will always increase after every valid input
}

int main(int argc, char *argv[]){
	int use_LRU = 0; //boolean to know if we use LRU or FIFO
	if(argc == 2)
	{
		if(!(strcmp(argv[1],"LRU"))){use_LRU=1;}
	}
	
	//create virtual memory
	struct virtual_memory VM;
	int i;
	for(i=0;i<16;i++){
		int x;
		for(x=0;x<8;x++){
			VM.virtual_mem[i].addresses[x] = -1;//initilize all values to be -1
		}
	}
	
	//need to declare and initilize pageTable
	struct PageTableEntries pageTable[16];
	for (i=0;i<16;i++){
		pageTable[i].v_page_num = i;//virtual page num is same as index of this pageTable array
		pageTable[i].valid_bit =0; //no pages start in main memory
		pageTable[i].dirty_bit = 0;//no pages are dirty at the start
		pageTable[i].page_num = i; //page num is same as v_page_num when not valid, otherwise they are 0-3
		pageTable[i].time_stamp = 0;//counter that should increment for every page in main mem after commands
	}
	
	//create main memory
	struct physical_memory physical_mem;
	for(i=0;i<4;i++){
		int x;
		for(x=0;x<8;x++){
			physical_mem.main_mem[i].addresses[x] = -1;//initilize all values to be -1
		}
	}
	//physical_mem.main_mem[0].addresses[1] = 16;
	
	char input_str[50];
	char* input_array[4]; //need to declare only once
	
	while(1){
		printf("> ");
		fgets(input_str,sizeof(input_str),stdin);
		char* token = strtok(input_str," \n");//
		input_array[0] = token;
		int x = 1;
		while(token != NULL){//argument array will contain null at the end!
		token = strtok(NULL," \n");
		input_array[x]=token;
		x++;
		}
		if(!strcmp(input_array[0],"quit")){
			exit(0);
		}
		else if(!strcmp(input_array[0],"read"))
		{
			int read_address = atoi(input_array[1]);
			int page_number = read_address/8; //should return whole number
			int index_offset = read_address%8;//should return index of address in page
			if(use_LRU == 1){
				pageTable[page_number].time_stamp = 0;}
			//find out if page is in physical_mem using valid bit
			if(pageTable[page_number].valid_bit){//if page is in main memory
				// go to page in pageTable, get physical_mem page_num, and print value at specifc address of page 
				printf("%d\n",physical_mem.main_mem[pageTable[page_number].page_num].addresses[index_offset]); 
				increment_counters(pageTable);
			}
			else{
				printf("A Page Fault Has Occurred\n");//after this, move page into main memory either open spot or replace a victim page
				int need_to_replace_page = 1;
				int main_mem_spots[4] = {0,1,2,3};
				for(i=0;i<16;i++){//if any number 0-3 remains, that's an open slot in main memory for a page
					if(pageTable[i].valid_bit && pageTable[i].page_num==0){main_mem_spots[0] = -1;}
					else if(pageTable[i].valid_bit && pageTable[i].page_num==1){main_mem_spots[1] = -1;}
					else if(pageTable[i].valid_bit && pageTable[i].page_num==2){main_mem_spots[2] = -1;}
					else if(pageTable[i].valid_bit && pageTable[i].page_num==3){main_mem_spots[3] = -1;}
				}
				for (i=0;i<4;i++){
					if (main_mem_spots[i] != -1){ //if there was a spot remaining in main memory; set page num to this "i"
						pageTable[page_number].page_num = i;//set page_num
						pageTable[page_number].valid_bit = 1; //set valid bit to true
						physical_mem.main_mem[i] = VM.virtual_mem[page_number]; //copy page from VM into main memory
						//physical_mem.main_mem[i].addresses[1] = 16;
						need_to_replace_page = 0;//only if page could be placed into empty slot in main memory
						//printf("there was space in main memory\n");
						printf("%d\n",physical_mem.main_mem[pageTable[page_number].page_num].addresses[index_offset]);
						increment_counters(pageTable);
						break;
					}
				}
				if (need_to_replace_page){
					//FIFO or LRU
					
					int i;
					int virtual_page_num;
					int index_to_replace;//the index in main memory to replace (0,1,2,3)
					int longest_time_stamp = 0;
					for(i=0;i<16;i++){ //need to iterate through all pages in pageTable, see which valid page has the largest time
						if (pageTable[i].valid_bit && pageTable[i].time_stamp > longest_time_stamp){
							longest_time_stamp = pageTable[i].time_stamp;
							index_to_replace = pageTable[i].page_num;
							virtual_page_num = i; //the index of page in pageTable; ALSO MATCHES INDEX OF INT ARRAY IN VM.virtual_mem;
						}
					}
					if (pageTable[virtual_page_num].dirty_bit){ //IF page copy was dirtied in the main memory, need to copy it into virtual memory first
						pageTable[virtual_page_num].dirty_bit = 0;//we are gonna take care of it
						//set virtual_memory page to be equal to page in physical_mem, so that the changes are kept
						VM.virtual_mem[virtual_page_num]= physical_mem.main_mem[index_to_replace];  
					}
					pageTable[virtual_page_num].valid_bit = 0; //we are taking this page out of main memory
					pageTable[virtual_page_num].page_num = pageTable[virtual_page_num].v_page_num; //set this back to v_page_num
					pageTable[virtual_page_num].time_stamp = 0; //only pages in main memory have a time stamp greater than one (unless LRU and page just used)
					
					pageTable[page_number].valid_bit = 1; //we are putting this page into main memory
					pageTable[page_number].page_num = index_to_replace; //page_num is index of open spot in main memory (still needs to be set to the same data tho)
					physical_mem.main_mem[index_to_replace] = VM.virtual_mem[page_number]; //set main memory index to be this page
					printf("%d\n",physical_mem.main_mem[pageTable[page_number].page_num].addresses[index_offset]);
				}
					
					//else{}//Implemet LRU FIRST
				
			}
		}
		else if(!strcmp(input_array[0],"write")){
			int write_address = atoi(input_array[1]);
			int page_number = write_address/8; //should return whole number
			int index_offset = write_address%8;//should return index of address in page
			if(use_LRU == 1)
			{
				pageTable[page_number].time_stamp = 0;
			}
			if (pageTable[page_number].valid_bit){//if page of address in main memory
				physical_mem.main_mem[pageTable[page_number].page_num].addresses[index_offset] = atoi(input_array[2]); //alter the data here
				pageTable[page_number].dirty_bit = 1; //we changed a value in the page, must say its dirtied
				increment_counters(pageTable);
			}
			else{//if not in physical memory
				printf("A Page Fault Has Occurred\n");
				int need_to_replace_page = 1;
				int main_mem_spots[4] = {0,1,2,3};
				for(i=0;i<16;i++){//if any number 0-3 remains, that's an open slot in main memory for a page
					if(pageTable[i].valid_bit && pageTable[i].page_num==0){main_mem_spots[0] = -1;}
					else if(pageTable[i].valid_bit && pageTable[i].page_num==1){main_mem_spots[1] = -1;}
					else if(pageTable[i].valid_bit && pageTable[i].page_num==2){main_mem_spots[2] = -1;}
					else if(pageTable[i].valid_bit && pageTable[i].page_num==3){main_mem_spots[3] = -1;}
				}
				for (i=0;i<4;i++){
					if (main_mem_spots[i] != -1){ //if there was a spot remaining in main memory; set page num to this "i"
						pageTable[page_number].page_num = i;//set page_num
						pageTable[page_number].valid_bit = 1; //set valid bit to true
						physical_mem.main_mem[i] = VM.virtual_mem[page_number]; //copy page from VM into main memory
						physical_mem.main_mem[pageTable[page_number].page_num].addresses[index_offset] = atoi(input_array[2]);
						pageTable[page_number].dirty_bit = 1;
						need_to_replace_page = 0;//only if page could be placed into empty slot in main memory
						increment_counters(pageTable);
						break;
					}
				}
				if(need_to_replace_page){
					
					//FIFO_replace(pageTable,physical_mem,VM,page_number);
					int i;
					int virtual_page_num;
					int index_to_replace;//the index in main memory to replace (0,1,2,3)
					int longest_time_stamp = 0;
					for(i=0;i<16;i++){ //need to iterate through all pages in pageTable, see which valid page has the largest time
						if (pageTable[i].valid_bit && pageTable[i].time_stamp > longest_time_stamp){
							longest_time_stamp = pageTable[i].time_stamp;
							index_to_replace = pageTable[i].page_num;
							virtual_page_num = i; //the index of page in pageTable; ALSO MATCHES INDEX OF INT ARRAY IN VM.virtual_mem;
						}
					}
					if (pageTable[virtual_page_num].dirty_bit){ //IF page copy was dirtied in the main memory, need to copy it into virtual memory first
						pageTable[virtual_page_num].dirty_bit = 0;//we are gonna take care of it
						//set virtual_memory page to be equal to page in physical_mem, so that the changes are kept
						VM.virtual_mem[virtual_page_num]= physical_mem.main_mem[index_to_replace];  
					}
					pageTable[virtual_page_num].valid_bit = 0; //we are taking this page out of main memory
					pageTable[virtual_page_num].page_num = pageTable[virtual_page_num].v_page_num; //set this back to v_page_num
					pageTable[virtual_page_num].time_stamp = 0; //only pages in main memory have a time stamp greater than one (unless LRU and page just used)
					
					pageTable[page_number].valid_bit = 1; //we are putting this page into main memory
					pageTable[page_number].page_num = index_to_replace; //page_num is index of open spot in main memory (still needs to be set to the same data tho)
					physical_mem.main_mem[index_to_replace] = VM.virtual_mem[page_number]; //set main memory index to be this page
					physical_mem.main_mem[pageTable[page_number].page_num].addresses[index_offset] = atoi(input_array[2]);
					pageTable[page_number].dirty_bit = 1;
					increment_counters(pageTable);
					
				}
			}
		}
		else if(!strcmp(input_array[0],"showmain")){//need to print addresses(numbers) and their values of a specific page in main memory
			int ppn = atoi(input_array[1]);
			int virtual_page_number;
			int i;
			//for(i=0;i<16;i++){
				//if(pageTable[i].valid_bit && pageTable[i].page_num == ppn){virtual_page_number = i;}
			//}
			for(i=0;i<8;i++){
				//printf("%d: %d\n",(8*virtual_page_number)+i,physical_mem.main_mem[ppn].addresses[i]);
				printf("%d: %d\n",(8*ppn)+i,physical_mem.main_mem[ppn].addresses[i]);
			}
		}
		else if(!strcmp(input_array[0],"showptable")){
			int i;
			for(i=0;i<16;i++){
				printf("%d:%d:%d:%d\n",pageTable[i].v_page_num,pageTable[i].valid_bit,pageTable[i].dirty_bit,pageTable[i].page_num);
				
			}
		}
	}
}
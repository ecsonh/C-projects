#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

unsigned char heap[64]; //there will be 64 instances of 8-bit values in an array to represent the heap

int blockIsFree(unsigned char c); //check to see if the header says this block is free!
int getSize(unsigned char c);//get the size of this block using the first 7 bits
void allocateBlock(int sz);//allocate this block using this size if possible

int blockIsFree(unsigned char c)
{
	
	int temp = c & 1;
    if(temp ==0)
	{
		return 1;
	}
	return 0;
}
int getSize(unsigned char c)
{
	return c>>1;
}
void allocateBlock(int sz){
	int i = 0; // pointer to beginning of heap
	int found = 0; // a flag; if 0 then block wasnt found: might need to put error message in this case
	while (!found && i < 64){
			if (blockIsFree(heap[i]) && (getSize(heap[i])-2)>= sz){// block is free and fits the requested size
				found = 1;
				printf("%d\n",i+1);//print the first payload index
				//need to edit the header and footer bits, both the size (sz+2) and LSB( to be 1)
				heap[i] = sz+2; //set this index to this size
				heap[i] = heap[i] << 1; //logical left shift the size by 1 
				heap[i] |= 1; // set the last bit to be 1 to indicate this block is taken
				heap[i+sz+1] = heap[i]; //set this block to be same as header, since this will be the footer
				
				//now we need to loop through rest of heap, until next taken block is found or if the i is 63
				int new_header_index = i+sz+2;
				int new_block_size = 0;
				if ((heap[new_header_index] == 0) &&(heap[new_header_index+1] == 0) && (heap[new_header_index+2] == 0) && new_header_index < 64){
					//check to see if 3 spaces are available, then make new header and footer 
					int new_footer_index = new_header_index; //another iterator 
					while(heap[new_footer_index] == 0 && (new_footer_index < 63))//resize the next free block
					{
						/*if(new_footer_index==63)
						{
							break;
						}*/
						new_footer_index++;
						new_block_size++;
					}
					heap[new_footer_index] = new_block_size+1;//edit the size of the footer
					heap[new_footer_index] = heap[new_footer_index]<<1;
					heap[new_header_index] = heap[new_footer_index];
				}
				else
				{//if theres no room for header, 1 payload, and footer
					heap[new_header_index] = 0; //always make the new header just an empty byte
					if(((heap[new_header_index+1] & 1) == 0)&& (new_header_index+1 <= 63))
					{
						heap[new_header_index+1] = 0;
					}
					
				}
				
				break;
			}
			i += getSize(heap[i]); // go to header of next block
			
		}
}

int main(){
	char input_str[50];
	char* input_array[3];
	//need to initilize all values in array
	int i;
	for(i=0;i<64;i++){
		heap[i] = 0; //should initilize all 8 bits to be 0 for each of the 64 chars
	}
	//need to edit first 7 bits of header and footer to indicate the size of the fresh heap
	
	heap[0] |= 0x40;
	heap[0] = heap[0] << 1;
	heap[63] = heap[0];
	
	while(1)
    {
        printf("> ");
		fgets(input_str,sizeof(input_str),stdin);
        char* token = strtok(input_str," \n");//maximum of three argument
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
		else if(!strcmp(input_array[0],"malloc")){
			int allocate_size = atoi(input_array[1]);
			allocateBlock(allocate_size);
			
		}
		else if(!strcmp(input_array[0],"free")){
			int index = atoi(input_array[1]);
			int header_index = index-1;//header index of the block that we want to free;
			int block_size = heap[header_index]>>1;//size of the block
			int footer_index = header_index+block_size-1;//footer index of the block that we want to free;
			int i;
			for(i=1;i<block_size-1;i++)//free the allocated block as the user intended
			{
				heap[header_index+i] = 0;
			}
			heap[header_index] =heap[header_index]>>1; //set back to the bit with size and unallocated status
			heap[header_index] =heap[header_index]<<1; 
			heap[footer_index] =heap[header_index];
			
			if(heap[footer_index+1] ==0 && heap[footer_index+2] ==0 && footer_index<62)
			{//block_size +2 here 
				int new_size = heap[footer_index]>>1;
				heap[footer_index] = 0;
				heap[footer_index+1] = 0;
				heap[footer_index+2] = new_size+2;
				heap[footer_index+2] = heap[footer_index+2]<<1;
				heap[header_index] = heap[footer_index+2];
				footer_index = footer_index+2;
				block_size+=2;
			}
			else if(heap[footer_index+1] ==0 && footer_index<63)
			{//block_size +1 here
				int new_size = heap[footer_index]>>1;
				heap[footer_index] = 0;
				heap[footer_index+1] = new_size+1;
				heap[footer_index+1] = heap[footer_index+1]<<1;
				heap[header_index] = heap[footer_index+1];
				footer_index = footer_index+1;
				block_size+=1;
			}
			
			if(heap[header_index-1] ==0 && heap[header_index-2] ==0 && header_index > 1) //check header_index-1 is 0 and header_index-2 is 0 
			{//block_size +2 here 
				int new_size = heap[header_index]>>1;
				heap[header_index] = 0;
				heap[header_index-2] = new_size+2;
				heap[header_index-2] = heap[header_index-2]<<1;
				heap[footer_index] = heap[header_index-2];
				header_index = header_index-2;
				block_size+=2;
			}
			else if(heap[header_index-1] ==0 && header_index >0)
			{//block_size +1 here
				int new_size = heap[header_index]>>1;
				heap[header_index] = 0;
				heap[header_index-1] = new_size+1;
				heap[header_index-1] = heap[header_index-1]<<1;
				heap[footer_index] = heap[header_index-1];
				header_index = header_index-1;
				block_size+=1;
				
			}
			
			if(header_index>2 && (heap[header_index-1] & 1) == 0)//if the previous block is free(by checking the footer)
			{
				//we merge the previous block with the current block
				int previous_block_size = heap[header_index-1]>>1; // get the block size of the previous block
				block_size = previous_block_size + block_size; //merged of two block's size
				int new_header_index = header_index-previous_block_size;
				
				heap[header_index-1] = 0; // current header and previous footer to 0
				heap[header_index] = 0;
				
				heap[new_header_index] = block_size;
				int new_footer_index = footer_index; //the footer index of the new free block
				heap[new_header_index] = heap[new_header_index]<<1; //set the allocated status to 0 as free (SLL pushes bits to left, deletes MSB and makes LSB 0)
				heap[new_footer_index] = heap[new_header_index];
				header_index = new_header_index;
				
			}
			if(footer_index<61 && (heap[footer_index+1] & 1) == 0)//if the nexxt block is free(by checking the next header)
			{
				int next_block_size = heap[footer_index+1]>>1; // get the block size of the previous block, (do we need to check if in bounds?)
				block_size = next_block_size + block_size; //merged block sizes
				heap[footer_index+1] = 0; // next header and current footer to 0
				heap[footer_index] = 0;
				footer_index += next_block_size;
				 
				heap[header_index] = block_size;
				heap[header_index] = heap[header_index]<<1;
				heap[footer_index] = heap[header_index];
				
			}
		}
		else if (!strcmp(input_array[0],"blocklist")){
			//printing infinite for loop when trying to blocklist after coalescing free blocks together
			int i = 0;
			while (i<63){
				int size_of_payload = (heap[i] >> 1) -2; //get size of entire block (header and footer included) then minus 2 to account for header/footer
				if(heap[i] == 0){
					i++;
				}
				
				else if((heap[i] & 1) == 1){
					printf("%d,%d,allocated.\n",i+1,size_of_payload);
					i += (heap[i] >> 1); // increment the heap index counter to go past all of this block
				}
				else if((heap[i] & 1) == 0){
					printf("%d,%d, free.\n",i+1,size_of_payload);
					i += (heap[i] >> 1); // increment the heap index counter to go past all of this block
				}
			}
		}
		else if (!strcmp(input_array[0],"writemem")){
			int index = atoi(input_array[1]);
			char *input_string = input_array[2];
			int i =0;
			while(input_string[i] != '\0')
			{
				heap[index] = input_string[i];
				index++;
				i++;
			}
		}
		else if (!strcmp(input_array[0],"printmem")){
			int index = atoi(input_array[1]);
			int length = atoi(input_array[2]);
			int i = 0;
			for(;i<length;i++)
			{
				printf("%x ", heap[index+i]);
			}
			printf("\n");

		}
		else{
			continue;
		}
	}
	
}
//Sharvin Joshi, sharvimj, 49481100
//ShaoCheng Hsu, ecsonh, 79817999
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <sys/socket.h>
#include <unistd.h>
#define PORT 9990

/*
struct sockaddr_in  { 
uint16_t        sin_family; 
uint16_t        sin_port;     
struct in_addr  sin_addr;    
unsigned char   sin_zero[8]; 
}; */

int main(int argc, char **argv){ //grab arguments from cmd line (argv[1] should be txt file name, argv[2] should be server IP addy)
	int client_socket;
	struct sockaddr_in server_addr;
	
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	
	int addr_status = inet_pton(AF_INET,argv[2], &server_addr.sin_addr);
	int connect_status = connect(client_socket, (struct sockaddr*)&server_addr,sizeof(server_addr));
	if (connect_status < 0){return -1;} //if the connecting doesn't work
	
	char input_line [256];
	char* input_array[3];
	// char file_path[sizeof("Client Domain/")+sizeof(argv[1])];
	// strcpy(file_path, "Client Domain/");
	// strcat(file_path,argv[1]);
	//strcat("Local Directory/", argv[1]);
	FILE *client_input = fopen(argv[1],"r"); //through this, we'll read the client's input; dont forget to close at the end
	printf("Welcome to ICS53 Online Cloud Storage.\n");
	printf("> ");
	while(fgets(input_line,sizeof(input_line),client_input)){
		
		char* token = strtok(input_line," \n");//maximum of 2 arguments
		input_array[0] = token; //populate 0th index of the first argument (command)
		int input_length = 1;
		token = strtok(NULL," \n\r");
		while(token != NULL){//argument array will contain null at the end!
			input_array[input_length]=token;
			input_length++;
			token = strtok(NULL," \n\r");
		}
		if (!strcmp(input_line,"quit")&&input_length == 1){exit(0);}
		else if(!strcmp(input_array[0],"pause") && atoi(input_array[1])>0 && input_length == 2){
			sleep(atoi(input_array[1])); // sleep this process for this amount of time (client side)
		}
		else if(!strcmp(input_array[0],"append") && input_length == 2){
			//create a message for the server to understand what file to try and open for appending
			char append_header[256] = "a ";
			char server_response[256] = "";
			char what_to_append[256] = "";
			char what_to_copy[256] = "";
			strcat(append_header,input_array[1]); //example would be "a example.txt", needs null char at the end????
			strcat(append_header,"\0");
			send(client_socket,append_header,256,0); //tell server to start append mode with this file
			int connected_or_not = recv(client_socket,server_response,256,0); //see how server responded in "server_response";
			if(!strcmp(server_response,"failed appending")){//client notified that server couldn't acess file
				printf("File [%s] could not be found in remote directory.",input_array[1]);
				
				continue;
			}
			while(1){//otherwise the file is there and we are now appending
				
				printf("Appending> "); 
				fgets(what_to_append,sizeof(what_to_append),client_input); //get line to input to server and append to file
				
				//ASSUMING THAT PAUSE COMMAND IS ALWAYS CORRECTLY USED, like they only do "pause <int>".
				strcpy(what_to_copy,what_to_append);
				char *token_temp = strtok(what_to_copy," \n");
				//check to see if the command was pause, if so just pause the client side for some time
				if(!strcmp(token_temp,"pause")){
					printf("pause detected\n"); fflush(stdout);
					token_temp = strtok(NULL," \n"); //get the next argument
					int x = atoi(token_temp);
					token_temp = strtok(NULL," \n"); //get the next argument (should be NULL if its valid)
					sleep(x);
					continue;
				}
				
				send(client_socket, what_to_append, strlen(what_to_append), 0);
				recv(client_socket, server_response, 20, 0); //check to see if append success
				if(!strcmp(server_response,"close succeed")){
					printf("close detected\n"); fflush(stdout);
					break;
				}
				
				//recv(client_socket,server_response,256,0);//receive appending
				
			}
			
		}
		else if(!strcmp(input_array[0],"download") && input_length == 2){
			fflush(stdout);
			FILE * upload_file_descriptor;
			int received_size;
			int file_counter = 1;
			char destination_path[] = "./Local Directory/";  // Note how wedon't have the original file name.
			strcat(destination_path, input_array[1]);
			int chunk_size = 1000;
			char file_chunk[chunk_size];
	//    int chunk_counter = 0;
			char upload_header[256] = "d ";
			char server_response[256] = "";
			char what_to_append[256] = "";
			strcat(upload_header,input_array[1]); //example would be "a example.txt", needs null char at the end????
			strcat(upload_header,"\0");
			send(client_socket,upload_header,256,0);// send the file that I want to download from the remote directory
			recv(client_socket,server_response,256,0);
			if(!strcmp(server_response,"download mode"))//check if it's about to upload
			{
				upload_file_descriptor = fopen(destination_path,"wb");//read from the file
				int total_byte = 0;
				if(upload_file_descriptor==NULL)
				{
					printf("File %s can't be found in the remote directory",input_array[1]);
					continue;
				}
				
				fflush(stdout);
				bzero(file_chunk, chunk_size);
				// Receiving bytes from the socket.
				received_size = recv(client_socket, file_chunk, chunk_size, 0);
				total_byte = total_byte+received_size;
				// The server has closed the connection.
				// Note: the server will only close the connection when the application terminates.
				
				
				//Writing the received bytes into disk.
				fwrite(&file_chunk, sizeof(char), received_size, upload_file_descriptor);
				//printf("Client: file_chunk data is:\n%s\n\n", file_chunk); 
				fflush(stdout);
				printf("%d bytes download successfully.\n", total_byte); fflush(stdout);
				fclose(upload_file_descriptor);
				
				
			}
			else
			{
				printf("File %s can't be found in the remote directory\n",input_array[1]);
				continue;
			}

		}
		else if(!strcmp(input_array[0],"upload") && input_length == 2){
			
			FILE * upload_file_descriptor;
			int received_size;
			int file_counter = 1;
			char upload_header[256] = "u ";
			char server_response[256] = "";
			char what_to_append[256] = "";
			strcat(upload_header,input_array[1]); //example would be "a example.txt", needs null char at the end????
			strcat(upload_header,"\0");
			
			int sent_value = send(client_socket,upload_header,256,0);// send the file that I want to download from the remote directory
			recv(client_socket,server_response,256,0);
			fflush(stdout);
			int chunk_size = 1000;
			char file_chunk[chunk_size];
			if(!strcmp(server_response,"upload mode"))//check if it's about to upload
			{
				char filename[] = "./Local Directory/";
				strcat(filename,input_array[1]);
				upload_file_descriptor = fopen(filename,"rb");
				if (upload_file_descriptor != NULL)//read the file user asks for download
				{
					send(client_socket,"uploading",strlen("uploading")+1,0);
					fseek(upload_file_descriptor, 0L, SEEK_END);
					int file_size = ftell(upload_file_descriptor);  // Get file size.
					fflush(stdout);
					fseek(upload_file_descriptor, 0L, SEEK_SET);  // Sets the pointer back to the beginning of the file.
					int total_bytes = 0;  // Keep track of how many bytes we read so far.
					int current_chunk_size;  // Keep track of how many bytes we were able to read from file (helpful for the last chunk).
					ssize_t sent_bytes;
					while (total_bytes < file_size){
						bzero(file_chunk, chunk_size);
						current_chunk_size = fread(&file_chunk, sizeof(char), chunk_size, upload_file_descriptor);
						sent_bytes = send(client_socket, &file_chunk, current_chunk_size, 0);// Sending a chunk of file to the socket.
						// Keep track of how many bytes we read/sent so far.
						//        total_bytes = total_bytes + current_chunk_size;
						total_bytes = total_bytes + sent_bytes;
						

					}
					fclose(upload_file_descriptor);
					printf("%d bytes uploaded successfully.\n", total_bytes); fflush(stdout);
				}
				else
				{
					send(client_socket,"not upload",strlen("not upload")+1,0);
					printf("File %s could not be found in local directory.\n", input_array[1]);fflush(stdout);
				}
				
			}
			else
			{
				
				printf("File %s could not be found in local directory.", input_array[1]);fflush(stdout);
			}
	
		}
		else if(!strcmp(input_array[0],"delete") && input_length == 2){
			
			FILE * upload_file_descriptor;
			int received_size;
			int file_counter = 1;
			char upload_header[256] = "z ";
			char server_response[256] = "";
			char what_to_append[256] = "";
			strcat(upload_header,input_array[1]); //example would be "a example.txt", needs null char at the end????
			strcat(upload_header,"\0");
			int sent_value = send(client_socket,upload_header,256,0);// send the file that I want to download from the remote directory
			recv(client_socket,server_response,256,0);
			fflush(stdout);
			if(!strcmp(server_response,"delete mode"))//check if it's about to upload
			{
				printf("File deleted successfully.\n");
			}
			else
			{
				printf("File %s could not be found in remote directory.", input_array[1]);
			}
		}
		else{
            printf("Error Message: “Command [%s] is not recognized.”",input_array[0]); fflush(stdout);
        }
		sleep(1);
		printf("> ");fflush(stdout);

	}
	
	
}

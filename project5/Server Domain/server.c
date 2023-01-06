//Sharvin Joshi, sharvimj, 49481100
//ShaoCheng Hsu, ecsonh, 79817999
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 9990
/*
struct sockaddr_in  { 
uint16_t        sin_family; 
uint16_t        sin_port;     
struct in_addr  sin_addr;    
unsigned char   sin_zero[8]; 
}; */

int main(int argc, char **argv){
	int client_socket, server_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	int socket_status = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(argv[1]);
    address.sin_port = htons(PORT);
	int bind_status = bind(server_socket, (struct sockaddr*)&address, sizeof(address));
	char buffer[1024];
	char append_buffer[1024];
	char upload_buffer[1024];
	while(1)
	{
		int listen_status = listen(server_socket, 1);
		client_socket = accept(server_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		while (1){  // We go into an infinite loop because we don't know how many messages we are going to receive.
			
			int received_size = recv(client_socket, buffer, 1024, 0); //the message received from client should be in the buffer
			if (received_size == 0){  // Socket is closed by the other end.
				close(client_socket);
				
				break;
			}
			//Check to see what the command was using strtok. Create a file descriptor from the file name given by the client.
			//If the file can't be found, server sends a message back saying "failed appending"
			//Otherwise,tell the client that the file has been found, then in a inf loop ask the client for inputs for the appending mode.
			//If the input is or starts with "close ", tell the client that it has exited "appending mode"; otherwise, add the given input to file
			
			if(buffer[0]=='a'){ //if message was "a" then activate appending mode for given filename
				char* token = strtok(buffer," \n"); //break up the message by spaces and/or newlines (spaces are more important)
				token = strtok(NULL, " \n"); //this token should now be the filename of the file client wants to append to
				FILE * append_file_descriptor;
				append_file_descriptor = fopen(token,"a");
				if (append_file_descriptor != NULL){//returns NULL if error made
					int count = 0;
					while(1){
						if (count ==0)
						{
							send(client_socket,"appending",strlen("appending"),0); //tell client it could open the file in the server/ready to append
							count = 1;
						}
						int append_received_size = recv(client_socket, append_buffer, 30, 0); //put what client wants to append to file in append_buffer
						if(append_buffer[0] == 'c' && append_buffer[1] == 'l' && append_buffer[2] == 'o' && append_buffer[3] == 's'
						&& append_buffer[4] == 'e')
						{
							fclose(append_file_descriptor);//close the file, then tell the client the close worked.
							send(client_socket, "close succeed", strlen("close succeed")+1, 0);
							break;
						}
						else 
						{
							printf("%d\n", append_received_size);
							fwrite(&append_buffer,sizeof(char),append_received_size,append_file_descriptor);
							send(client_socket, "append succeed", strlen("append succeed")+1, 0);
						}
					}
				}
				else{
					send(client_socket,"failed appending",strlen("failed appending"),0); //tell client it couldn't open the file in the server
				}
			}
			if(buffer[0]=='d'){ //if message was "a" then activate appending mode for given filename
				char* token = strtok(buffer," \n"); //break up the message by spaces and/or newlines (spaces are more important)
				token = strtok(NULL, " \n"); //this token should now be the filename of the file client wants to append to
				char filename[] = "./Remote Directory/";
				strcat(filename,token);
				FILE *download_file_descriptor;
				int chunk_size = 1000;
				char file_chunk[chunk_size];
				download_file_descriptor = fopen(filename,"rb");
				if (download_file_descriptor != NULL)//read the file user asks for download
				{
					
					send(client_socket,"download mode",strlen("download mode")+1,0);
					fseek(download_file_descriptor, 0L, SEEK_END);
					int file_size = ftell(download_file_descriptor);  // Get file size.
					fseek(download_file_descriptor, 0L, SEEK_SET);  // Sets the pointer back to the beginning of the file.
					int total_bytes = 0;  // Keep track of how many bytes we read so far.
					int current_chunk_size;  // Keep track of how many bytes we were able to read from file (helpful for the last chunk).
					ssize_t sent_bytes;
					
					
					bzero(file_chunk, chunk_size);
					current_chunk_size = fread(&file_chunk, sizeof(char), chunk_size, download_file_descriptor);
					printf("%d\n",current_chunk_size);fflush(stdout);
					sent_bytes = send(client_socket, &file_chunk, current_chunk_size, 0);// Sending a chunk of file to the socket.
					// Keep track of how many bytes we read/sent so far.
					//        total_bytes = total_bytes + current_chunk_size;
					total_bytes = total_bytes + sent_bytes;
					fflush(stdout);
					
					fclose(download_file_descriptor);
					fflush(stdout);
				}
				else
				{
					send(client_socket,"download failed",strlen("download failed")+1,0);
					printf("File %s could not be found in local directory.", filename);
				}
			}
			if(buffer[0]=='u'){ //if message was "a" then activate appending mode for given filename
				char client_response[256] = "";
				char* token = strtok(buffer," \n"); //break up the message by spaces and/or newlines (spaces are more important)
				token = strtok(NULL, " \n"); //this token should now be the filename of the file client wants to append to
				FILE * upload_file_descriptor;
				int received_size;
				char destination_path[] = "./Remote Directory/";  // Note how wedon't have the original file name.
				strcat(destination_path, token);
				int chunk_size = 1000;
				char file_chunk[chunk_size];
				
				send(client_socket,"upload mode",strlen("upload mode")+1,0);
				recv(client_socket,client_response,256,0);
				printf("%s",client_response);fflush(stdout);
				if (strcmp(client_response, "uploading"))//read the file user asks for download
				{
					upload_file_descriptor = fopen(destination_path,"wb");
					//send(client_socket,"upload mode",strlen("upload mode")+1,0);
					
					bzero(file_chunk, chunk_size);
					// Receiving bytes from the socket.
					received_size = recv(client_socket, file_chunk, chunk_size, 0);
					// The server has closed the connection.
					// Note: the server will only close the connection when the application terminates.
					
						//printf("close");fflush(stdout);
						
						//break;
					
					//Writing the received bytes into disk.
					fwrite(&file_chunk, sizeof(char), received_size, upload_file_descriptor);
					fflush(stdout);
					fclose(upload_file_descriptor);
				}
				else
				{
					printf("File %s could not be found in local directory.", destination_path);fflush(stdout);
				}
			}
			if(buffer[0]=='z'){ //if message was "a" then activate appending mode for given filename
				char* token = strtok(buffer," \n"); //break up the message by spaces and/or newlines (spaces are more important)
				token = strtok(NULL, " \n"); //this token should now be the filename of the file client wants to append to
				char destination_path[] = "./Remote Directory/";  // Note how wedon't have the original file name.
				strcat(destination_path, token);
				int delete = remove(destination_path);
				if (delete == 0){//returns NULL if error made
					send(client_socket,"delete mode",strlen("delete mode")+1,0);
				}
				else
				{
					send(client_socket,"delete failed",strlen("delete failed")+1,0);
				}
			
			}
			// if (client_socket < 0) {
			// 	perror("accept");
			// 	exit(EXIT_FAILURE);
			
			// }
			
		}
	}
    return 0;
}
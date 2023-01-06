
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>

struct job{
	int job_id;
	int pid;
	int process_status; //-1 open space for job, 0=background(running), 1 = foreground(running), 2 is stopped(not running)
	char command_line [256];
};
//const int stdin_id = stdin;
//const int saved_stdout = dup(1);
//#define saved_stdout dup2(1)
void initilize_empty_jobs(int array_size,struct job * job_array);
void input_direction(char* argv);
int output_direction(char* argv);
void append_direction(char* argv);
struct job jobs_list[5];//contains all processes and their details (global), 5 processeses occurring max

//put signal handlers
void sigCHLD_handler(int signum);
void sigINT_handler(int signum);
void sigTSTP_handler(int signum);

int main(){
	//might need saved stdin too
	int outFileID = -1;
	int inFileID = -1;
	
	signal(SIGCHLD,sigCHLD_handler);
	signal(SIGINT,sigINT_handler);
	signal(SIGTSTP,sigTSTP_handler);
	int job_id_counter = 0; //to count number of job id
	initilize_empty_jobs(5,jobs_list);
	while(1){
		int input_file_bool = 0;
		int output_file_bool = 0;
		int append_file_bool = 0;
		char command[256];
		char command_copy[256];
		char command_copy1[256];
		char* argument_array[80];//string array(max 80 arguments)256 char long max per arg
		printf("prompt>");
		fgets(command,256,stdin); //get the input of the command
		if (command[0] == '\n'){continue;}//if nothing entered try again
		strcpy(command_copy,command);
		strcpy(command_copy1,command);
		//arguments are split by spaces and ended by newline
		char* token1 = strtok(command_copy1, " \n");
		argument_array[0]=token1;
		int arg_count = 1;
	
        while(token1!=NULL){//figure out if command has input from file or outputs to file
			token1 = strtok(NULL," \n");
            argument_array[arg_count] = token1;
            arg_count++;
			if(token1 != NULL)
			{
				if (!strcmp(token1,"<")) {input_file_bool = 1;}
				else if (!strcmp(token1,">")) {output_file_bool = 1;}
				else if (!strcmp(token1,">>")) {append_file_bool = 1;}
			}
        }
		char* token = strtok(command, " \n");
		if (!strcmp(token,"fg")||!strcmp(token,"bg")||
		!strcmp(token,"kill")||!strcmp(token,"cd")||!strcmp(token,"quit")||
		!strcmp(token,"jobs")){//IF built in command //NEED TO IMPLEMENT BUILT IN COMMANDS
			if(!strcmp(token,"quit")){//need to terminate all running processes, then you can exit
				int i;
				for(i=0;i<5;i++){
					if(jobs_list[i].process_status!=-1){//this job exists;needs to be terminated
						kill(jobs_list[i].pid,SIGINT);
					}
				}
				exit(0);
			}
			else if (!strcmp(token,"jobs")){
				//printf("printing jobs\n");
				int i;
				for (i=0;i<5;i++){
					if (jobs_list[i].process_status == -1){
						//printf("process_status is -1\n");
						continue;
					}
					else if (jobs_list[i].process_status == 0 || jobs_list[i].process_status == 1){
						printf("[%d] (%d) Running %s",jobs_list[i].job_id,jobs_list[i].pid,jobs_list[i].command_line);
					}
					else if (jobs_list[i].process_status == 2){
						printf("[%d] (%d) Stopped %s",jobs_list[i].job_id,jobs_list[i].pid,jobs_list[i].command_line);
					}
				}
			}
			else{
				char* second_argument = strtok(NULL," \n"); //get the intended second argument
				if (second_argument == NULL || !strcmp(second_argument,"\n"))
				{printf("wrong number of arguments(too little)\n"); continue;}
				else{
					char* temp_token = strtok(NULL," ");//see if there's a third argument
					if (temp_token != NULL && strcmp(temp_token,"\n"))
					{printf("wrong number of arguments(too much)\n"); continue;}
					else{//number of arguments was correct, must be a valid 2 argument built in command
						//printf("argument was %s, second argument was %s",token,second_argument);
						
						if(!strcmp(token,"fg")){
							//printf("fg activated");
							
							int child_status; //need this for the exec later on
							
							//move process to foreground using SIGCONT;second argument = job id / pid
							int i;
							if (second_argument[0]=='%'){//use the job id
								char temp_arr[80];
								memcpy(temp_arr,&second_argument[1],2*sizeof(*second_argument));
								for (i=0;i<5;i++){
									if (jobs_list[i].job_id == atoi(temp_arr)){
										//found process, continue this process
										//printf("fg with job id %d success\n",jobs_list[i].job_id);
										jobs_list[i].process_status = 1;
										kill(jobs_list[i].pid,SIGCONT);
										break;
									}
								}
							}
							else{//use the pid
								for (i=0;i<5;i++){
									if (jobs_list[i].pid == atoi(second_argument)){
										//found process, continue this process
										jobs_list[i].process_status = 1;
										kill(jobs_list[i].pid,SIGCONT);
										break;
									}
								}
							}
							pid_t pid = waitpid(jobs_list[i].pid,&child_status,WUNTRACED);
							//pid_t wpid = waitpid(pid_fork,&child_status,WUNTRACED);
						}
						else if(!strcmp(token,"bg")){
							//printf("bg activated");
							//move process to foreground using SIGCONT;second argument = job id / pid
							if (second_argument[0]=='%'){//use the job id
								char temp_arr[80];
								memcpy(temp_arr,&second_argument[1],2*sizeof(*second_argument));
								int i;
								for (i=0;i<5;i++){
									if (jobs_list[i].job_id == atoi(temp_arr)){
										//found process, continue this process
										//printf("fg with job id %d success\n",jobs_list[i].job_id);
										jobs_list[i].process_status = 0;
										kill(jobs_list[i].pid,SIGCONT);
										break;
									}
								}
							}
							else{
								int i;
								for (i=0;i<5;i++){
									if (jobs_list[i].pid == atoi(second_argument)){
										jobs_list[i].process_status = 0;
										kill(jobs_list[i].pid,SIGCONT);
										//printf("%d now running in background",jobs_list[i].pid);
										break;
									}
								}
							}
						}
						else if(!strcmp(token,"kill")){
							//move process to foreground using SIGCONT;second argument = job id / pid
							if (second_argument[0]=='%'){//use the job id
								char temp_arr[80];
								memcpy(temp_arr,&second_argument[1],2*sizeof(*second_argument));
								int i;
								for (i=0;i<5;i++){
									if (jobs_list[i].job_id == atoi(temp_arr)){
										kill(jobs_list[i].pid,SIGKILL);
										jobs_list[i].job_id = -1;
										jobs_list[i].pid = -1;
										jobs_list[i].process_status = -1;
										strcpy(jobs_list[i].command_line,"");
										break;
									}
								}
							}
							else{
								int i;
								for (i=0;i<5;i++){
									if (jobs_list[i].pid == atoi(second_argument)){
										//found process and terminated it
										kill(jobs_list[i].pid,SIGKILL);
										jobs_list[i].job_id = -1;
										jobs_list[i].pid = -1;
										jobs_list[i].process_status = -1;
										strcpy(jobs_list[i].command_line,"");
										//printf("%d terminated",jobs_list[i]);
										break;
									}
								}
							}
						}
						else if(!strcmp(token, "cd")){
							int change_directory = chdir(second_argument); // change to the input directory
							/*if(change_directory<0)
							{
								printf("fail to change directory");
							}
							else
							{
								printf("directory changed successfully.");
							}*/
						}
						continue;//jump back to start of while loop here
					}
				}
			}
		}
		
		else{
			char* input_arg;
			if((input_file_bool+output_file_bool+append_file_bool) ==2)
			{
				input_arg = argument_array[arg_count-4];
				
			}
			char* last_arg = argument_array[arg_count-2];

			
			
			//if it reaches here, it must be general command (executable file)
			//all arguments must be present in the array of strings
			//need to see if it's run in background or not
			int x = 1; //second argument starts here (first argument was the general or built-in command)
			int bg_flag = 0;
			
			
			while(token != NULL){
				token = strtok(NULL, " \n");
				//printf("%s xxx", token);
				//if this arg is &, its background process; null terminate the array here
				
				if(token != NULL && !strcmp(token,"&")){bg_flag = 1; argument_array[x]=NULL; break;}
				argument_array[x]=token;
				x++;
			}
			
			pid_t pid_fork= fork(); //create child process and parent process
			
			setpgid(0,0);
			int child_status; //need this for the exec later on
			//WHEN INPUT FILE GETS INVOVLE PID_FORK != 0
			
			if(pid_fork==0){//if process is child process
				mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
				if(input_file_bool == 0 && output_file_bool == 1 && append_file_bool == 0)
				{
					outFileID = open (last_arg, O_CREAT|O_WRONLY, mode);
				}
				else if(input_file_bool ==1 && output_file_bool == 0 && append_file_bool == 0) //JUST INPUT FILE
				{
					inFileID = open (last_arg, O_RDONLY, mode);
				}
				else if(input_file_bool == 0 && output_file_bool == 0 && append_file_bool == 1) //JUST APPEND
				{
					printf("append");
					outFileID = open(last_arg, O_CREAT|O_APPEND|O_WRONLY, mode);
				}
				else if(input_file_bool == 1 && output_file_bool == 1 && append_file_bool == 0) //input and output
				{
					outFileID = open (last_arg, O_CREAT|O_WRONLY, mode);
					inFileID = open (input_arg, O_RDONLY, mode);
				}
				if(outFileID> -1 && inFileID > -1){
					dup2(outFileID, STDOUT_FILENO);
					dup2(inFileID, STDIN_FILENO);
				}
				else if(outFileID > -1){
                    dup2(outFileID, STDOUT_FILENO);
					printf("dup outfile");
                }
                else if(inFileID > -1){
                    dup2(inFileID, STDIN_FILENO);
					printf("dup inputfile");
                }
                
                //dup2(saved_stdout, STDOUT_FILENO);
                //output_direction(last_arg);
                if(execv(argument_array[0],argument_array))
                {
                    execvp(argument_array[0],argument_array);
                } //to be able to run both?
					if(outFileID> -1 && inFileID > -1){
						close(inFileID);
					    inFileID = -1;
						close(outFileID);
					    outFileID = -1;
						printf("there");
					}
					else if(outFileID > -1){
					    close(outFileID);
					    outFileID = -1;
						printf("close outfile");
					}
					else if(inFileID > -1){
						close(inFileID);
					    inFileID = -1;
						printf("close intputfile");
					}

			}

			else{//parent process
				
				
				if(!bg_flag){//if child process is foreground
					//create a job
					struct job current_job;
					current_job.job_id = job_id_counter;//set this job's job_id and increment
					current_job.pid = pid_fork;//set the pid
					current_job.process_status = 1; //this process should be in the foreground
					strcpy(current_job.command_line,command_copy);
					job_id_counter++;
					//insert this job wherever a spot is available
					int i;
					for (i=0;i<5;i++){ //5 because five spots available only at a time
						if(jobs_list[i].job_id == -1){
							jobs_list[i] = current_job;
							break;
						}
					}
					//make the parent process wait for the child to finish, then reap dead child 
					pid_t wpid = waitpid(pid_fork,&child_status,WUNTRACED);
					//after child is done, get rid of the job from array
					/*
					for (i=0;i<5;i++){
						if(jobs_list[i].pid == wpid){
							jobs_list[i].job_id = -1;
							jobs_list[i].pid = -1;
							jobs_list[i].process_status = -1;
							strcpy(jobs_list[i].command_line,"");
							break;
						}
					}*/
				}
				else{//if child process is background
					//create a job
					/*struct job current_job;
					current_job.job_id = job_id_counter;//set this job's job_id and increment
					current_job.pid = pid_fork;//set the pid
					current_job.process_status = 0; //this process should be in the background
					strcpy(current_job.command_line,command_copy);*/
					//job_id_counter++;
					//insert this job wherever a spot is available
					int i;
					for (i=0;i<5;i++){ //5 because five spots available only at a time
						if(jobs_list[i].job_id == -1){
							jobs_list[i].job_id = job_id_counter;
							jobs_list[i].pid = pid_fork;
							jobs_list[i].process_status = 0;
							strcpy(jobs_list[i].command_line,command_copy);
							job_id_counter++;
							break;
						}
					}
					
					//sigCHLD should take care of it here
				}
			}
		}
		
	}
	return 0;
}

void input_direction(char* argv) //read from input file
{
	mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
	int inFileID = open (argv, O_RDONLY, mode);
	dup2(inFileID, STDIN_FILENO);
	//dup2(stdin, stdin_id);
}

int output_direction(char* argv)//write to output file
{
	int saved_stdout = dup(1);
	mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
	int outFileID = open (argv, O_CREAT|O_WRONLY, mode);
	printf("%d",outFileID);

	dup2(outFileID, STDOUT_FILENO);
	//dup2(saved_stdout, STDOUT_FILENO);
	return outFileID;
}

void append_direction(char* argv)//create or append to outputfile
{
	mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;
	int outFileID = open(argv, O_CREAT|O_APPEND, mode);
	dup2(outFileID, STDOUT_FILENO);
}

void initilize_empty_jobs(int array_size,struct job * job_array){
	int i;
	for(i=0;i<array_size;i++){
		job_array[i].job_id = -1;
		job_array[i].pid = -1;
		job_array[i].process_status = -1;
		strcpy(job_array[i].command_line,"");
	}
}

void sigCHLD_handler(int signum){
	int status,pid;
	while((pid = waitpid(-1,&status,WNOHANG | WUNTRACED))>0){
		if(WIFEXITED(status)){
			int i;
			for (i=0;i<5;i++){ //5 because five spots available only at a time
				if(jobs_list[i].pid == pid){
					jobs_list[i].job_id = -1;
					jobs_list[i].pid = -1;
					jobs_list[i].process_status = -1;
					strcpy(jobs_list[i].command_line,"");
					break;
				}
			}
		}/*
		else if (WIFSTOPPED(status)){ //if signal stops it 
			int i;
			for (i=0;i<5;i++){ //5 because five spots available only at a time
				if(jobs_list[i].pid == pid){
					//jobs_list[i].job_id = -1;
					//jobs_list[i].pid = -1;
					jobs_list[i].process_status = 2;
					//strcpy(jobs_list[i].command_line,"");
					break;
				}
			} 
		}*/
	}
}

void sigINT_handler(int signum){
	int i;
	for(i=0;i<5;i++){
		if(jobs_list[i].process_status == 1){//if instance of foreground process found
			//need to terminate this process and remove from the jobs list
			kill(jobs_list[i].pid,SIGINT); //kill this process
			jobs_list[i].job_id = -1;
			jobs_list[i].pid = -1;
			jobs_list[i].process_status = -1;
			strcpy(jobs_list[i].command_line,"");
			break;
		}
	}
}

void sigTSTP_handler(int signum){
	int i;
	for(i=0;i<5;i++){
		if (jobs_list[i].process_status == 1){//if process is foreground
			jobs_list[i].process_status = 2; //set this to be stopped in job_list
			kill(jobs_list[i].pid,SIGTSTP);
		}
	}
	return;//just return here?
}

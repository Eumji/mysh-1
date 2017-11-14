#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "commands.h"
#include "built_in.h"
#include "signal_handlers.h"

#define READ_END 0
#define WRITE_END 1

//int *childpid;
//struct single_command* child;
static struct built_in_command built_in_commands[] = {
	{ "cd", do_cd, validate_cd_argv },
	{ "pwd", do_pwd, validate_pwd_argv },
	{ "fg", do_fg, validate_fg_argv }
};

static int is_built_in_command(const char* command_name)
{
	static const int n_built_in_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);

	for (int i = 0; i < n_built_in_commands; ++i) {
		if (strcmp(command_name, built_in_commands[i].command_name) == 0) {
			return i;
		}
	}

	return -1; // Not found
}

/*
 * Description: Currently this function only handles single built_in commands. You should modify this structure to launch process and offer pipeline functionality.
 */
int evaluate_command(int n_commands, struct single_command (*commands)[512])
{

	if (n_commands > 0) {
		struct single_command* com = (*commands);

		assert(com->argc != 0);

		int built_in_pos = is_built_in_command(com->argv[0]);

		if (built_in_pos != -1) {
			if (built_in_commands[built_in_pos].command_validate(com->argc, com->argv)) {
				if (built_in_commands[built_in_pos].command_do(com->argc, com->argv) != 0) {
					fprintf(stderr, "%s: Error occurs\n", com->argv[0]);
				}
			} else {
				fprintf(stderr, "%s: Invalid arguments\n", com->argv[0]);
				return -1;	
			}
		} else if (strcmp(com->argv[0], "") == 0) {
			return 0;
		} else if (strcmp(com->argv[0], "exit") == 0) {
			return 1;
		} else {
			//pipe implemantation

			int subpid;

			int i,j;
			char **argv2 = (char **)malloc(16*sizeof(char*));

			int isbar=0;
			int whi=0;
			int fd[2];

			/*for(int i = 0 ; i <com->argc ; i++)
			  printf("%s\n", com->argv[i]);
			  */	


			for(whi=0;whi<n_commands;whi++){
				if(strcmp((com->argv[whi]), "|")==0)
				{
					isbar=1;
					break;
				}
			}



			//	if(isbar==1){
			//		pipeproc(com->argv);
			//	}




			//if bar exist make | before -> com_>argv, | after -> argv2
			if(isbar==1){
				com->argv[whi]=NULL;
				for(i=whi+1,j=0; i<n_commands;i++,j++){
					argv2[j]=(char *)malloc(strlen(com->argv[i]+1));
					strcpy(argv2[j], com->argv[i]);
					com->argv[i]=NULL;
				}
				argv2[j] = NULL;
			}
			else{
				argv2=com->argv;
			}


			/*if(isbar){
			  int pid=fork();
			  if(pid==0){
			  pipe(fds);

			  if((subpid = fork())<0){
			  perror("Can't fork process");
			  exit(1);
			  }

			  else if(subpid){
			  dup2(fds[1], STDOUT_FILENO);
			  close(fds[0]);
			  close(fds[1]);
			//redirectproc(com->argv);
			execv(com->argv[0],com->argv); 
			exit(1);

			}

			dup2(fds[0], STDIN_FILENO);
			close(fds[0]);
			close(fds[1]);
			//redirectproc(argv2);

			execv(argv2[0], argv2);
			exit(1);
			}
			else if (pid) {
			int status;
			wait(&status);
			return -1;
			}
			}*/
			//	else{

			//	printf("flag=%d\n", flag);

			//	int andpid=0;

			/*if(flag==2){
			//andpid=fork();
			}
			if(andpid()==0){}
			*/

			//save path variable
			char cargv[5][50]={"/usr/local/bin/", "/usr/bin/", "/bin/", "/usr/sbin/", "/sbin/"};
			char argvz[50]="";

			strcpy(argvz, com->argv[0]);

			int pid=fork();
			pipe(fd);
			int iswell=0;

			if(pid<0){ }
			else if(pid==0){
				printf("Is bar? %d", isbar);
				//if(isbar==0) pipeproc(com->argv);
				if(isbar==1){// if IPC
					dup2(fd[WRITE_END], STDOUT_FILENO);
					close(fd[READ_END]);
					close(fd[WRITE_END]);
					execv(com->argv[0], com->argv);

					exit(1);

				}
				else{// not IPC 
					if(execv(com->argv[0],com->argv)==-1){ iswell=-1; }

					//if execv fail then put path to com->argv[0] use for argument
					if(iswell==-1){
						for(int i=0;i<5;i++){
							if(iswell==-1){
								iswell=0;
								strcpy(com->argv[0], argvz);
								char s[100];
								strcpy(s, cargv[i]);
								strcat(s, com->argv[0]);
								strcpy(com->argv[0], s);
								if(execv(com->argv[0], com->argv)==-1){iswell=-1;}
							}
							else {break;}
						}
					}


					//finally execv fail 
					strcpy(com->argv[0], argvz);
					fprintf(stderr, "%s: command not found\n", com->argv[0]);
					return 1;//then kill child
				}

			}
			else{
				if(isbar==1){
					int ppid=fork();
					if(ppid==0){
						dup2(fd[READ_END], STDIN_FILENO);
						close(fd[WRITE_END]);
						close(fd[READ_END]);
						execv(argv2[0], argv2);
						exit(1);
					}
					else{
						int status;
						close(fd[READ_END]);
						close(fd[WRITE_END]);
						waitpid(ppid, &status, 0);
					}

				}
				else{
					int status;
					wait(&status); //parent wait child
					return -1;
				}
			}

		}



		return 0;
		}

	}

	void free_commands(int n_commands, struct single_command (*commands)[512])
	{
		for (int i = 0; i < n_commands; ++i) {
			struct single_command *com = (*commands) + i;
			int argc = com->argc;
			char** argv = com->argv;

			for (int j = 0; j < argc; ++j) {
				free(argv[j]);
			}

			free(argv);
		}

		memset((*commands), 0, sizeof(struct single_command) * n_commands);
	}

	void pipeproc(char **argvp)
	{
		int n,i,j;
		int subpid;
		int status;
		int exist =0;
		int fds[2];
		char **argvp2 = (char **)malloc(16*sizeof(char *));

		for(n=0;argvp[n]!=NULL;n++)
		{
			if(strcmp(argvp[n], "|") == 0)
			{
				exist =1;
				break;
			}
		}

		if(exist){
			argvp[n] = NULL;
			for(i=n+1, j=0; argvp[i]!=NULL; i++, j++){
				argvp2[j] = (char *) malloc (strlen(argvp[i]) +1);
				strcpy(argvp2[i], argvp[i]);
				argvp[i] = NULL;
			}
			argvp2[j]=NULL;
		}
		else { argvp2 = argvp;}

		if(exist){
			pipe(fds);

			if((subpid = fork())<0)
			{
				exit(1);
			}
			else if(subpid){
				dup2(fds[1], STDOUT_FILENO);
				close(fds[0]);
				close(fds[1]);
				execv(argvp[0], argvp);
				exit(1);
			}
			dup2(fds[0], STDIN_FILENO);
			close(fds[0]);
			close(fds[1]);
		}

		execv(argvp2[0], argvp2);
		exit(1);
	}


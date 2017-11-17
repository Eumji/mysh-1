#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include "commands.h"
#include "built_in.h"
#include "signal_handlers.h"

#define READ_END 0
#define WRITE_END 1

#define SOCK_PATH "tpf_unix_sock.server"
#define CLI_PATH "tpf_unix_sock.client"

pthread_t threads[2];

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

void *sendfunc(void *arg){
	char buf[26][2576];
	char buff[2566]={0};
	int pid=fork();
	int fd;
	int tt = dup(STDOUT_FILENO);

	if(pid<0){ }
	else if(pid==0){
		remove("temp");

		fd = open("temp", O_CREAT, 0644);
		fd = open("temp", O_WRONLY);

		dup2(fd, STDOUT_FILENO);

		char **data = arg;

		execv(data[0],data);

		close(fd);

		return NULL;
	}
	else{
		int twait;
		wait(&twait);
	}


	FILE * in = fopen("temp", "r");
	int n;

	for(n = 0 ; ; n++){
		if(fgets(buf[n], sizeof(buf[n]), in) == NULL) break;
	}

	for(int i = 0 ; i < n ; i++){
		strcat(buff, buf[i]);
	}

	int rc;
	int client_sock, len;
	struct sockaddr_un server_sockaddr;
	struct sockaddr_un client_sockaddr;
	memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
	memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

	client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(client_sock == -1){printf("c s err\n"); exit(1);}

	client_sockaddr.sun_family = AF_UNIX;
	strcpy(client_sockaddr.sun_path, CLI_PATH);
	len = sizeof(client_sockaddr);

	unlink(CLI_PATH);

	rc = bind(client_sock, (struct sockaddr *) &client_sockaddr, len);
	if(rc ==- 1){ printf("bind err"); exit(1);}

	server_sockaddr.sun_family = AF_UNIX;
	strcpy(server_sockaddr.sun_path, SOCK_PATH);
	rc = connect(client_sock, (struct sockaddr *) &server_sockaddr, len);
	if(rc ==- 1){ printf("conct err"); exit(1);}

	rc = send(client_sock, buff, strlen(buff), 0);
	if(rc == -1) {printf("send err");  exit(1);}

	close(client_sock);

	pthread_exit((void *)0);

}

void *recvfunc(void *arg){
	char buf[2560]={0};
	char **oper;
	char **dat = arg;
	int I;


	oper = (char**) malloc(sizeof(char*) * 15);


	for(I = 0 ; ; I++){
		if(dat[I] == NULL) break;
		oper[I] = (char*) malloc(sizeof(char) * (strlen(dat[I]) + 1));
		strcpy(oper[I], dat[I]);
	}


	int server_sock, client_sock;

	struct sockaddr_un server_sockaddr;
	struct sockaddr_un client_sockaddr;

	server_sock = socket(AF_UNIX, SOCK_STREAM, 0);

	server_sockaddr.sun_family = AF_UNIX;
	strcpy(server_sockaddr.sun_path , SOCK_PATH);
	int len = sizeof(server_sockaddr);
	unlink(SOCK_PATH);
	int rc = bind(server_sock, (struct sockaddr *) &server_sockaddr, len);

	rc = listen(server_sock, 10);

	client_sock = accept(server_sock, (struct sockaddr*) &client_sockaddr, &len);

	len = sizeof(client_sockaddr);
	rc = getpeername(client_sock, (struct sockaddr *) &client_sockaddr, &len);
	int bytes_rec = recv(client_sock, buf, sizeof(buf), 0);

	FILE * out = fopen("temp2", "w");
	fprintf(out, "%s", buf);
	fclose(out);

	oper[I] = (char*)malloc(sizeof(char) * 6);
	strcpy(oper[I++], "temp2");
	oper[I] = NULL;

	int pid=fork();

	if(pid<0){ }
	else if(pid==0){
		int xx = execv(oper[0],oper);
		printf("%d", xx);
	}
	else{
		int pst;
		wait(&pst);
		remove("temp2");
		close(server_sock);
		close(client_sock);
		free(oper);
		pthread_exit((void *)0);

	}
}


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
			int i,j;
			char **argv2 = (char **)malloc(16*sizeof(char*));//in a|b, store b part 

			int isbar=0;
			int whi=0;
			
			for(whi=0;whi<n_commands;whi++){
				if(strcmp((com->argv[whi]), "|")==0)
				{
					isbar=1;
					break;
				}
			}
			
			//if bar exist make | before -> com_>argv, | after -> argv2
			if(isbar==1){
				com->argv[whi]=NULL;
				for(i=whi+1,j=0; i<n_commands;i++,j++){
					argv2[j]=(char *)malloc(strlen(com->argv[i]+1));
					strcpy(argv2[j], com->argv[i]);
					com->argv[i]=NULL;
				}



				int status;

				argv2[j] = NULL;

				pthread_create(&threads[0], NULL, &sendfunc, (void *)com->argv);

				pthread_create(&threads[1], NULL, &recvfunc, (void *)argv2);

				pthread_join(threads[0], (void **)&status);

				pthread_join(threads[1], (void **)&status);

				return 0;
			}

			else{
				//save path variable
				char cargv[5][50]={"/usr/local/bin/", "/usr/bin/", 
					"/bin/", "/usr/sbin/", "/sbin/"};
				char**argvz = (char **)malloc(n_commands*sizeof(char*));
				char ar[500]="";

				for(int i = 0 ; i < n_commands ; i++){
					argvz[i] = (char*) malloc(sizeof(char) * (strlen(com->argv[i]) + 1));
					strcpy(argvz[i], com->argv[i]);
				}

				argvz[n_commands] = NULL;

				strcpy(ar, argvz[0]);

				int pid=fork();
				int iswell=0;

				if(pid<0){ }
				else if(pid==0){
					if(execv(argvz[0],argvz)==-1){ iswell=-1; }

					//if execv fail then put path to com->argv[0] use for argument
					if(iswell==-1){
						for(int i=0;i<5;i++){
							if(iswell==-1){
								iswell=0;
								strcpy(argvz[0], ar);
								char s[100];
								strcpy(s, cargv[i]);
								strcat(s, argvz[0]);
								strcpy(argvz[0], s);
								if(execv(argvz[0], argvz)==-1){iswell=-1;}
							}
							else {break;}
						}
					}

					//finally execv fail 
					strcpy(argvz[0], ar);
					fprintf(stderr, "%s: command not found\n", argvz[0]);
					return 1;//then kill child

				}
				else{
					int status;
					wait(&status); //parent wait child
					return -1;
				}
				for(int i = 0 ; i < n_commands; i++)
					free(argvz[i]);
				free(argvz);
			}
			for(int i = 0 ; i < n_commands ; i++){
				free(argv2[i]);
			}
			free(argv2);
		}
		return 0;
	}
}

void free_commands(int n_commands, struct single_command (*commands)[512])
{
	for (int i = 0; i <n_commands; ++i) {
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



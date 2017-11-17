#include "utils.h"

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*void mysh_parse_command(const char* command,
		int* n_command,
		struct single_command (*commands)[])
{
	char buf[4096];
	strcpy(buf, command);

	char *saveptr = NULL;
	char *tok = strtok_r(buf, " \n\t", &saveptr);//change / ->  \n\t

	int ti = 0;
	while (tok != NULL) {
		struct single_command* com = *commands + ti;
		//parse_single_command(tok, &com->argc, &com->argv);//flag |=

		++ti;


		tok = strtok_r(NULL, " \n\t", &saveptr);

	}

	*n_commands = ti;

}

void parse_single_command(const char* command,
		int *argc, char*** argv)
{
	const int kMaxArgc = 512;

	*argv = (char**)malloc(kMaxArgc * sizeof(char*));

	for (int i = 0; i < kMaxArgc; ++i)
		(*argv)[i] = NULL;

	char buf[4096];

	strcpy(buf, command);

	char *saveptr = NULL; 
	char *tok = strtok_r(buf, " \t", &saveptr);

	int ti = 0;

	while (tok != NULL) {
		(*argv)[ti] = (char*)malloc(strlen(tok));
		strcpy((*argv)[ti], tok);

		++ti;

		tok = strtok_r(NULL, " \t", &saveptr);
	}

	*argc = ti;

	if (*argc == 0) {
		*argc = 1;
		(*argv)[0] = (char*)malloc(1);
		(*argv)[0][0] = '\0';
	}
}
*/
void mysh_parse_command(const char* command, int *n_commands, struct single_command (*commands)[]){
	const int kMaxArgc = 512;
	
	(*commands)->argv = (char**)malloc(kMaxArgc);

	char buf[4096];
	strcpy(buf, command);

	char *tok = strtok(buf, " \n\t");

	int ti = 0;

	//struct single_command* com = *commands +ti;
	while(tok != NULL){
		(*commands)->argv[ti] = (char*)malloc(strlen(tok));
		strcpy(((*commands)->argv)[ti], tok);

		++ti;

		tok = strtok(NULL, " \n\t");
	}

	*n_commands = ti;
	(*commands)->argc = ti;

	if(*n_commands == 0){
		*n_commands =1;
		(*commands)->argv[0] = (char*)malloc(1);
		(*commands)->argv = '\0';
	}
}

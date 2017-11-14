#include "utils.h"

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void mysh_parse_command(const char* command,
		int* n_commands,
		struct single_command (*commands)[])
{
	//int flag = 0;//

	// (flag&1) != 0 : it has |
	// (flag&2) != 0 : it has &
	char buf[4096];
	strcpy(buf, command);

	char *saveptr = NULL;
	char *tok = strtok_r(buf, "\n", &saveptr);//change / ->  \n\t

//	struct single_command* com = *commands;

//	(com->argv) = (char**)malloc(512*sizeof(char*));
//	for(int i = 0 ; i < 512 ; i++) (com->argv)[i] = NULL;

	int ti = 0;
	//int isbar = 0;
	while (tok != NULL) {
		struct single_command* com = *commands + ti;
		parse_single_command(tok, &com->argc, &com->argv);//flag |=

//		com = *commands + ti;

//		com->argv[ti] = (char*)malloc(strlen(tok));

//		strcpy(com->argv[ti], tok);


		++ti;


		tok = strtok_r(NULL, "\n", &saveptr);

//		tok = strtok_r(NULL, "|", &saveptr);
		//if(tok != NULL && isbar == 0) flag |= 1;
	}

	*n_commands = ti;

	//return flag;
}

void parse_single_command(const char* command,
		int *argc, char*** argv)
{
	//int rval = 0;

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
		//if(strcmp(tok, "&") == 0){
		//	rval |= 2;
			// TODO: CHECK! I THINK IT NEEDS TO STRCPY
		//}
		//else{
			(*argv)[ti] = (char*)malloc(strlen(tok));
			strcpy((*argv)[ti], tok);

			++ti;
		//}

		tok = strtok_r(NULL, " \t", &saveptr);
	}

	*argc = ti;

	if (*argc == 0) {
		*argc = 1;
		(*argv)[0] = (char*)malloc(1);
		(*argv)[0][0] = '\0';
	}
	//return rval;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "built_in.h"
#include "utils.h"
#include "signal_handlers.h"

#define BUF_SIZE 8096

int main()
{
	signal(SIGINT, catch_sigint);
	signal(SIGTSTP, catch_sigtstp);

	int befoflag=0;

	while (1) {
		char buf[BUF_SIZE];
		//char buf2[BUF_SIZE];

		memset(buf, 0, sizeof(buf));
		//memset(buf2, 0, sizeof(buf));

		fgets(buf, sizeof(buf), stdin);

		//struct single_command andcom[512];
		struct single_command commands[512];
		//struct single_command commands2[512];
		//struct single_command commands3[512]; // for | operator


		int n_commands = 0;

		// (flag&1) != 0 : it has |
		// (flag&2) != 0 : it has &
		//int flag =
		mysh_parse_command(buf, &n_commands, &commands);

		// first command->0, second -> 1


		/*	if(flag==2){ 
			int ret, ret2;
			int mainpid=fork();

			if(mainpid==0){
			ret = evaluate_command(n_commands, &commands, flag);

			exit(0);
			}
			else{
		//waitpid(-1,
		fgets(buf2, 8096, stdin);
		int n_commands2=0;
		int flag2 = mysh_parse_command(buf2, &n_commands2, &commands2);
		ret2 = evaluate_command(n_commands2, commands2, flag2);
		//	int ppid = *childpid;
		//	printf("%d ", ppid);
		int status;
		wait(&status);

		if(ret==2){
		printf("done ");
		}
		else printf("running ");

		for(int i=0;i<n_commands-1; i++){
		printf("%s ", commands2->argv[i]);
		}
		printf("%s\n", commands2->argv[n_commands-1]);

		//exit(0);

		}


		}*/
		//	else{
		//

		int ret = evaluate_command(n_commands, &commands);

		free_commands(n_commands, &commands);
		n_commands = 0;

		if (ret == 1) {
			break;
			//		}
	}
	}

	return 0;
}

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

		memset(buf, 0, sizeof(buf));

		fgets(buf, sizeof(buf), stdin);

		struct single_command commands[512];

		int n_commands = 0;

		mysh_parse_command(buf, &n_commands, &commands);

		int ret = evaluate_command(n_commands, &commands);

		free_commands(n_commands, &commands);
		n_commands = 0;

		if (ret == 1) 
			break;

	}

	return 0;
}

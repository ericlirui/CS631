#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "sish.h"

/*extern area*/
extern char *optarg;

void
usage()
{
    errx(EXIT_FAILURE, "usage: %s [ -x ] [ -c command ]\n", getprogname());
}


void
init_sig()
{
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
        perror("reset SIGINT");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGQUIT, SIG_IGN) == SIG_ERR) {
        perror("reset SIGQUIT");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTSTP, SIG_IGN) == SIG_ERR) {
        perror("reset SIGTSTP");
        exit(EXIT_FAILURE);
    }
}

void
init()
{
    char *sish_path;
    char env[BUFSIZE];
    if ((sish_path = malloc(BUFSIZE * sizeof(char))) == NULL) {
        perror("malloc sish_path");
    }
    getcwd(sish_path, BUFSIZE);
    (void)snprintf(env, BUFSIZE, "SHELL=%s/sish", sish_path);
    if ((putenv(env)) != 0) {
        perror("putenv");
    }
    init_sig();
    init_task();
}

int
execute(char * cmd, int para_flag)
{
    char** tokbuf;

    init_task();
   if ((tokbuf = calloc(BUFSIZE, sizeof(char*))) == NULL){
       perror("tokens realloc");
       return -1;
   }

   if ( parse_cmd(cmd,tokbuf) == -1) {
        perror("parse fail");
       free(tokbuf);
       return -1;
    }

    if(execute_cmds(tokbuf,para_flag) != 0){
        perror("execute fail");
        free(tokbuf);
        return 0;
    }
    if(tokbuf != NULL)
        free_tokens(tokbuf);
    free_task();

    return 0;
}

void
enter_shell_mode(int para_flag)
{
    char * cmd = NULL;
    size_t len;
    int n;
    while(true) {
        (void)printf(PROMPT_STR);
        len = 0;
        if ((n = getline(&cmd, &len, stdin)) < 0)
            continue;
#ifdef DEBUG
        printf("shell:%s",cmd);
#endif
        cmd[n - 1] = '\0'; /* remove newline */
        if (execute(cmd, para_flag) != 0)
            break;
    }
    if(cmd != NULL )
        free(cmd);
}

int
main(int argc, char **argv)
{
    char* cmdline;
	int opt;
    int para_flag = 0;

	setprogname(argv[0]);

	/*init here*/
	init();

	while ((opt = getopt(argc, argv, "c:x")) != -1) {
		switch (opt) {
		case 'c':
            cmdline = optarg;
            para_flag |= PARAM_C;
			break;
		case 'x':
            para_flag |= PARAM_X;
            break;
		default:
			usage();
		}
	}

	if (IS_PARAM_CMD(para_flag))
        execute(cmdline,para_flag);
	else
        enter_shell_mode(para_flag);

    exit(EXIT_SUCCESS);
}


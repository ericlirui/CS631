#ifndef SISH_SISH_H
#define SISH_SISH_H

#define PARAM_C   0x1
#define PARAM_X   0x2

#define IS_PARAM_X_MODE(flag)       ((flag) & PARAM_X)
#define IS_PARAM_CMD(flag)          ((flag) & PARAM_C)
#define IS_OPTR(c)                  ((c) == '|' || (c) == '&' || (c) == '<' || (c) == '>')

#define BUFSIZE 1024
#define CD_PATHSIZE (BUFSIZE*4)
#define MAX_CD_PARA 2
#define PROMPT_STR "sish$ "
#define BG_STR "&"
#define CD "cd"
#define CURRENT_PID_STR "$$"
#define ECHO "echo"
#define EXIT "exit"
#define EXIT_NO_EXEC 127
#define EXIT_STATUS_STR "$?"
#define IO_IN "<"
#define IO_OUT ">"
#define IO_OUT_APPEND ">>"
#define IO_PIPE "|"
#define OUT_TO_STD 7
#define OUT_TO_FILE 8
#define APPEND_FILE 9
#define OUT_FILE_PERM 0644


struct task {
    char *command[BUFSIZE];
    int out_method;
    int bg;
    char *in_file;
    char *out_file;
    char *append_file;
    struct task *next;
};

void init_sig();
int init_task();
struct task * get_task_head();
int parse_cmd(char *,char**);
int execute_cmds(char** ,int);
int get_exitcode(void);
void set_exitcode(int );
void free_task();
struct task* generate_task(char** ,int ,int );
void free_tokens(char ** tokenbuf);
#endif

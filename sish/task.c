#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sish.h"

static struct task *taskhead;
static int exitcode;

int
get_exitcode(void)
{
    return exitcode;
}

void
set_exitcode(int code)
{
    exitcode = code;
}

int
init_task()
{
    if ((taskhead = calloc(1, sizeof(struct task))) == NULL) {
        perror("can't malloc");
        set_exitcode(EXIT_NO_EXEC);
        return -1;
    }
    return 0;
}

struct task *
get_task_head()
{
    return taskhead;
}

void
print_x_mode_task(struct task * cur, int para_flag)
{
    int i;
    while (IS_PARAM_X_MODE(para_flag) && cur) {
        fprintf(stderr, "+ ");
        for (i = 0; cur->command[i] != NULL; i++){
            fprintf(stderr, "%s ", cur->command[i]);
        }
#ifdef DEBUG
        fprintf(stderr, "outmthod: %d ,isbg:%d ", cur->out_method, cur->bg);
        if(cur->in_file !=NULL)
            fprintf(stderr, "infile %s ,",cur->in_file);
        if(cur->out_file !=NULL)
            fprintf(stderr, "out_file %s ,",cur->out_file);
        if(cur->append_file !=NULL)
            fprintf(stderr, "append_file %s ,",cur->append_file);
#endif
        fprintf(stderr, "\n");
        cur = cur->next;
    }
    return;
}
struct task*
generate_task(char** tokens,int tokcount,int para_flag)
{
    int command_pos = 0;
    int i;
    struct task *cur;
    int is_bg = 0;

    cur = get_task_head();
    cur->out_method = OUT_TO_STD;
    cur->next = NULL;
    for (i = 0; i < tokcount; i++) {
        if (tokens[i] == NULL){
            perror("memory fault");
            set_exitcode(EXIT_NO_EXEC);
            return NULL;
        }

        /*when encounter pipe line, create a new task node*/
        if(strncmp(tokens[i], IO_PIPE, strlen(IO_PIPE)) == 0) {
            cur->command[command_pos] = NULL;
            if (cur->command[0] == NULL) {
                printf("shell: syntax error near '|'\n");
                set_exitcode(EXIT_NO_EXEC);
                return NULL;
            }
            command_pos = 0;
            struct task *newnode;
            if ((newnode = (struct task *)malloc(sizeof(struct task))) == NULL) {
                perror("can't malloc");
                set_exitcode(EXIT_NO_EXEC);
                return  NULL;
            }
            newnode->out_method = OUT_TO_STD;
            newnode->next = NULL;
            cur->next = newnode;
            cur = cur->next;
        }
        else if (strncmp(tokens[i], IO_IN, strlen(IO_IN)) == 0) {
            cur->in_file = tokens[++i];
        }
        else if (strncmp(tokens[i], IO_OUT,strlen(IO_OUT)) == 0) {
            cur->out_method = OUT_TO_FILE;
            cur->out_file = tokens[++i];
        }
        else if (strncmp(tokens[i], IO_OUT_APPEND, strlen(IO_OUT_APPEND)) == 0) {
            cur->out_method = APPEND_FILE;
            cur->append_file = tokens[++i];
        }
        else if (strncmp(tokens[i], BG_STR, strlen(BG_STR)) == 0){
            if (i != tokcount - 1) {
                printf("shell: syntax error near '&'\n");
                set_exitcode(EXIT_NO_EXEC);
                return NULL;
            } else {
                is_bg = 1;
            }
        }
        else {
            cur->command[command_pos++] = tokens[i];
        }
    }
    cur->command[command_pos] = NULL;
    if (cur->command[0] == NULL) {
        printf("shell: syntax error\n");
        set_exitcode(EXIT_NO_EXEC);
        return NULL;
    }
    if (is_bg)
        taskhead->bg = 1;

    cur = taskhead;
    print_x_mode_task(cur,para_flag);
    return taskhead;
}

void
free_task()
{
    struct task * curr = taskhead;
    struct task * next;
    while (curr) {
        next = curr->next;
        curr->next = NULL;
        free(curr);
        curr = next;
    }
    taskhead = NULL;
    return;
}

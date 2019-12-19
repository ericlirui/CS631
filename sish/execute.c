#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include "sish.h"

void
reset_sig()
{
    if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
        perror("reset SIGINT");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) {
        perror("reset SIGQUIT");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGTSTP, SIG_DFL) == SIG_ERR) {
        perror("reset SIGTSTP");
        exit(EXIT_FAILURE);
    }
}

void
print_x_mode(char** tokenbuf, int para_flag, int tokcount)
{
    int i;
    if (IS_PARAM_X_MODE(para_flag)) {
        fprintf(stderr, "+ ");
        for (i = 0; i < tokcount; i++)
            fprintf(stderr, "%s ", tokenbuf[i]);
        fprintf(stderr, "\n");
    }
    return;
}

void
cd_builtin(char** tokbuf,int tokencount)
{
    struct passwd * passwd;
    char dir[CD_PATHSIZE];
    char path[CD_PATHSIZE];

    memset(dir,0,CD_PATHSIZE);
    if (tokencount > MAX_CD_PARA) {
        printf("cd: too many arguments\n");
        return;
    }

    passwd = getpwuid(getuid());
    if(passwd == NULL){
        set_exitcode(errno);
        return;
    }
    if (tokencount == 1)
        (void)snprintf(dir, CD_PATHSIZE, "%s", passwd->pw_dir);
    else if (tokencount == MAX_CD_PARA &&
            strncmp(tokbuf[tokencount - 1], "~", strlen("~")) == 0) {
        (void)snprintf(dir, CD_PATHSIZE, "%s%s", passwd->pw_dir, tokbuf[tokencount - 1] + 1);
    }
    else {
        strncpy(dir,tokbuf[tokencount - 1],strlen(tokbuf[tokencount - 1]));
    }

    if (dir[0] != '\0') {
        if(realpath(dir, path) == NULL){
            set_exitcode(errno);
            printf("cd: path %s is invalid \n",dir);
            return;
        }
        if (chdir(path) == -1) {
            set_exitcode(errno);
            printf("cd: no such directory \n");
            set_exitcode(errno);

        }
    }
    return;
}

void
echo_builtin(struct task * task)
{
    int i;
    if (task->command[1] != NULL)
    {
        if (!strcmp(task->command[1], CURRENT_PID_STR))
        {
            (void)printf("%d\n", getpid());
        }
        else if (!strcmp(task->command[1], EXIT_STATUS_STR))
        {
            (void)printf("%d\n", get_exitcode());
        }
        else
        {
            i = 1;
            while(task->command[i]){
                printf("%s ", task->command[i]);
                i++;
            }
            printf("\n");
        }
    }
    else
    {
        (void)printf("\n");
    }
    set_exitcode(0);
    return;
}

void
exit_builtin(int tokcount)
{
    if (tokcount == 1) {
        set_exitcode(EXIT_SUCCESS);
        exit(EXIT_SUCCESS);
    } else {
        printf("exit: too many arguments\n");
        set_exitcode(EXIT_NO_EXEC);
    }
    return;
}

int
redirect_io(int in, int out, struct task * task)
{
    int infile_fd;
    int outfile_fd = 0;
    /*deal with input file*/
    if (task->in_file != NULL) {
        if ((infile_fd = open(task->in_file, O_RDONLY)) == -1) {
            fprintf(stderr, "Unable to open %s: %s\n", task->in_file, strerror(errno));
            exit(EXIT_NO_EXEC);
        }
        if(dup2(infile_fd, STDIN_FILENO) == -1){
            fprintf(stderr, "Unable to dup2 %s: %s\n", task->in_file, strerror(errno));
            exit(EXIT_NO_EXEC);
        }
        (void)close(infile_fd);
    }
    else {
        if (in != STDIN_FILENO){
            if(dup2(in, STDIN_FILENO) == -1){
                fprintf(stderr, "Unable to dup2 %d: %s\n", in, strerror(errno));
                exit(EXIT_NO_EXEC);
            }
        }
    }
    /*deal with output and append file*/
    if (task->out_method != OUT_TO_STD) {
        switch (task->out_method) {
            case OUT_TO_FILE:
                if ((outfile_fd = open(task->out_file, O_CREAT | O_WRONLY | O_TRUNC, OUT_FILE_PERM)) == -1) {
                    fprintf(stderr, "can't create file %s: %s\n", task->out_file, strerror(errno));
                    exit(EXIT_NO_EXEC);
                }
                break;
            case APPEND_FILE:
                if ((outfile_fd = open(task->append_file, O_CREAT | O_WRONLY | O_APPEND, OUT_FILE_PERM)) == -1) {
                    fprintf(stderr, "can't create file %s: %s\n", task->out_file, strerror(errno));
                    exit(EXIT_NO_EXEC);
                }
                break;
            default:
                break;
        }
        if(dup2(outfile_fd, STDOUT_FILENO) == -1){
            fprintf(stderr, "Unable to dup2 %s: %s\n", task->out_file, strerror(errno));
            exit(EXIT_NO_EXEC);
        }
        (void)close(outfile_fd);
    } else { /*rediret to pipe write end*/
        if (out != STDOUT_FILENO) {
            if(dup2(out, STDOUT_FILENO) == -1){
                fprintf(stderr, "Unable to dup2 %d: %s\n", out, strerror(errno));
                exit(EXIT_NO_EXEC);
            }
            (void)close(out);
        }
    }
    return 0;
}

void
spawn_task(int in, int out, struct task *task)
{
    pid_t pid;
    if ((pid = fork()) == 0) {
        redirect_io(in,out,task);
        if (strncmp(task->command[0], ECHO, sizeof(ECHO)) == 0) {
            echo_builtin(task);
            exit(0);
        } else {
            if (execvp(task->command[0], task->command) == -1) {
                fprintf(stderr, "%s: command not found: %s\n", task->command[0],
                        strerror(errno));
                exit(EXIT_NO_EXEC);
            }
        }
    } else if (pid > 0) {
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid");
        }
        set_exitcode( WEXITSTATUS(status));
    } else {
        exit(EXIT_NO_EXEC);
    }
}

void
run_task(struct task *task)
{
    int pipe_fd[2];
    int in,out;

    in = dup(STDIN_FILENO);
    out = dup(STDOUT_FILENO);

    while (task->next != NULL) {
        if (pipe(pipe_fd)) {
            perror("pipe");
            exit(EXIT_NO_EXEC);
        }
        out = pipe_fd[1];
        spawn_task(in, out, task);
        close(pipe_fd[1]);
        in = pipe_fd[0];
        task = task->next;
    }
#ifdef DEBUG
    fprintf(stderr, "command:%s in: %d\n", task->command[0],in);
#endif
    redirect_io(in, STDOUT_FILENO, task);
    /*single command*/
    if (strncmp(task->command[0], ECHO, sizeof(ECHO)) == 0) {
        echo_builtin(task);
        exit(0);
    } else {
        if (execvp(task->command[0], task->command) == -1) {
            fprintf(stderr, "%s: command not found: %s\n", task->command[0],
                    strerror(errno));
            exit(EXIT_NO_EXEC);
        }
    }
}

void
execute_tasks(struct task * head)
{
    pid_t pid;
    if ((pid = fork()) == -1) {
        perror("fork error");
        set_exitcode(EXIT_NO_EXEC) ;
        return;
    }

    if (pid == 0) {
        reset_sig();
        if (head->bg)
            setpgid(0, 0);
        run_task(head);
    } else {
        int status;
        if (head->bg == 0) {
            if (waitpid(pid, &status, 0) < 0) {
                perror("waitpid");
            }
        }
        set_exitcode(WEXITSTATUS(status));
    }
    return;
}

int
execute_cmds(char ** tokenbuf,int para_flag)
{
    int tokcount = 0;
    struct task* head;

    while(tokenbuf[tokcount] != NULL) tokcount++;
    if (tokcount == 0)
        return 0;

    if (strncmp(tokenbuf[0], EXIT, sizeof(EXIT)) == 0) {
        print_x_mode(tokenbuf,para_flag,tokcount);
        exit_builtin(tokcount);
        return 0;
    }

    if (strncmp(tokenbuf[0], CD, sizeof(CD)) == 0) {
        print_x_mode(tokenbuf,para_flag,tokcount);
        cd_builtin(tokenbuf,tokcount);
        return 0;
    }

    if((head = generate_task(tokenbuf, tokcount, para_flag)) == NULL)
        return 0;

    execute_tasks(head);
    init_sig();
    return 0;
}

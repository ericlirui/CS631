#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <string.h>
#include <unistd.h>

#define	BUFSIZ	1024		/* size of buffer used by setbuf */

int
command(const char* string, char* outbuf, int outlen, char* errbuf,
int errlen){
    int fd1[2],pipe_stderr[2];
    pid_t pid;
    if (outlen > BUFSIZ){
        outlen = BUFSIZ;
    }
    if( errlen > BUFSIZ){
        errlen =BUFSIZ;
    }
    if (pipe(fd1) < 0) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipe_stderr) < 0) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        close(fd1[1]);
        close(pipe_stderr[1]);

        read(fd1[0], outbuf, outlen);
        read(pipe_stderr[0], errbuf, errlen);

        close(fd1[0]);
        close(pipe_stderr[0]);
        if (waitpid(pid, NULL, 0) < 0) {
            perror("waitpid error");
            exit(EXIT_FAILURE);
        }
    } else {
        close(fd1[0]);
        close(pipe_stderr[0]);

        dup2(fd1[1], STDOUT_FILENO);
        dup2(pipe_stderr[1], STDERR_FILENO);
        system(string);

        close(fd1[1]);
        close(pipe_stderr[1]);
    }
    return 0;
}

int
main() {
    char out[BUFSIZ], err[BUFSIZ];
    if (command("ls -l", out, BUFSIZ, err, BUFSIZ) == -1) {
        perror("command");
        exit(EXIT_FAILURE);
    }
    printf("stdout:\n%s\n", out);
    printf("stderr:\n%s\n", err);
    return EXIT_SUCCESS;
}

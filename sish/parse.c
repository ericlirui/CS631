#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "sish.h"


char* realloc_space(char* buf, int pos, int totalsize)
{
    if (pos >= totalsize) {
        totalsize += BUFSIZE;
        if ((buf = realloc(buf, totalsize)) == NULL) {
            perror("buf realloc");
            return NULL;
        }
    }
    return buf;
}
/* add whitespace for operators for later parse*/
char*
add_command_space(char *cmd)
{
    char *buf;
    int pos,bufsize;
    char ch;

    pos = 0;
    bufsize = 2 * BUFSIZE;
    if ((buf = calloc(bufsize,sizeof(char))) == NULL) {
        perror("malloc buf");
        return NULL;
    }
    while( (cmd != NULL) && (ch = (*cmd)) != '\0' )
    {
        if (IS_OPTR(ch)) {
            /* deal with ">>" */
            if (ch == '>' && *(cmd+1) == '>') {
                buf[pos++] = ' ';
                buf[pos++] = ch;
                buf[pos++] = ch;
                buf = realloc_space(buf,pos,bufsize);
                buf[pos++] = ' ';
                cmd++;
            }
            else {
                buf[pos++] = ' ';
                buf = realloc_space(buf,pos,bufsize);
                buf[pos++] = ch;
                buf = realloc_space(buf,pos,bufsize);
                buf[pos++] = ' ';
            }
        }
        else
            buf[pos++] = ch;
        buf = realloc_space(buf,pos,bufsize);
        cmd++;
    }
    return buf;
}

int
tokenize(char* cmd,char ** tokens)
{
    int tokcount,tokbufsize;
    tokcount = 0;
    tokbufsize = BUFSIZE;
    tokens[tokcount] = strtok(cmd, "\t ");
    while (tokens[tokcount] != NULL) {
        tokcount ++;
        if (tokcount >= BUFSIZE){
            tokbufsize += BUFSIZE;
            if ((tokens = realloc(tokens, sizeof(char*) * tokbufsize)) == NULL) {
                perror("tokens realloc");
                return -1;
            }
        }
        tokens[tokcount] = strtok(NULL, "\t ");
    }
    return 0;
}

int
parse_cmd(char *cmd,char** tokens)
{
    char *cmdsp;
    cmdsp = add_command_space(cmd);
    if (cmdsp == NULL)
       return -1;
#ifdef DEBUG
    printf("cmdspace:%s\n",cmdsp);
#endif
    if (tokenize(cmdsp,tokens) == -1)
        return -1;
    if (cmdsp != NULL)
        free(cmdsp);
    return 0;
}

void
free_tokens(char ** tokenbuf)
{

    free((void*)tokenbuf);
}

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

char**
tokenize(char* cmd)
{
    char** tokens;
    int tokcount,tokbufsize;

    tokcount = 0;
    tokbufsize = BUFSIZE * 2;
    if ((tokens = calloc(tokbufsize, sizeof(char*))) == NULL)
    {
        perror("tokens calloc");
        return NULL;
    }
    tokens[tokcount] = strtok(cmd, "\t ");
    while (tokens[tokcount] != NULL) {
        tokcount ++;
        if (tokcount >= BUFSIZE){
            tokbufsize += BUFSIZE;
            if ((tokens = realloc(tokens, sizeof(char*) * tokbufsize)) == NULL) {
                perror("tokens realloc");
                return NULL;
            }
        }
        tokens[tokcount] = strtok(NULL, "\t ");
    }
    return tokens;
}

char**
parse_cmd(char *cmd)
{
    char *cmdsp;
    char** tokens;

    cmdsp = add_command_space(cmd);
    if (cmdsp == NULL)
       return NULL;
#ifdef DEBUG
    printf("cmdspace:%s\n",cmdsp);
#endif
    tokens = tokenize(cmdsp);
    if (tokens == NULL)
        return NULL;
    free(cmdsp);
    return tokens;
}


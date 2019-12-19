#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "sish.h"


/* add whitespace for operators for later parse*/
void
add_command_space(char *cmd,char* buf)
{
    int pos;
    char ch;

    pos = 0;
    while( (cmd != NULL) && (ch = (*cmd)) != '\0' )
    {
        if (IS_OPTR(ch)) {
            /* deal with ">>" */
            if (ch == '>' && *(cmd+1) == '>') {
                buf[pos++] = ' ';
                buf[pos++] = ch;
                buf[pos++] = ch;
                buf[pos++] = ' ';
                cmd++;
            }
            else {
                buf[pos++] = ' ';
                buf[pos++] = ch;
                buf[pos++] = ' ';
            }
        }
        else
            buf[pos++] = ch;
        cmd++;
    }
    return;
}

int
tokenize(char* cmd,char ** tokens)
{
    char * tok;
    int tokcount,tokbufsize;

    tokcount = 0;
    tokbufsize = BUFSIZE;

    tok = strtok(cmd, "\t ");
    while (tok != NULL) {
        tokens[tokcount] = strdup(tok);
        tokcount ++;

        if (tokcount >= BUFSIZE){
            tokbufsize += BUFSIZE;
            if ((tokens = realloc(tokens, sizeof(char*) * tokbufsize)) == NULL) {
                perror("tokens realloc");
                return -1;
            }
        }
        tok = strtok(NULL, "\t ");
    }
    return 0;
}

int
parse_cmd(char *cmd,char** tokens)
{
    char *cmdsp;
    if ((cmdsp = calloc( 2 * BUFSIZE,sizeof(char))) == NULL) {
        perror("malloc buf");
        return -1;
    }
    add_command_space(cmd,cmdsp);
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
    int i = 0;
    while(tokenbuf[i]){
        free(*(tokenbuf+i));
        i++;
    }
    free(tokenbuf);
}

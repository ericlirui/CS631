
#include <stdio.h>
#include <sys/types.h>
#include <fts.h>
#include <string.h>
#include <stdlib.h>
#include "ls.h"

struct FILE_ENTITY g_fe;
int g_count = 0;

int fe_makeFileEntity(FTSENT *ftentryp, char *uname, char *gname){
    FILE_ENTITY *fe = NULL;

    if ((fe = calloc(1, sizeof(struct FILE_ENTITY))) == NULL) {
        return -1; /* Failed to allocate memory. */
    }
    if (strlen(uname) > (MAX_USER_NAME - 1)) {
        return -1; /* First name too long. */
    }
    if (strlen(gname) > (MAX_GROUP_NAME - 1)) {
        return -1; /* Last name too long. */
    }
    strcpy(fe->usrname, uname);
    strcpy(fe->grpname, gname);
    fe->pftsent = ftentryp;
    list_add_tail(&fe->listOfFile, &g_fe.listOfFile);
    g_count ++;
    return 0;
}

void fe_Initlist()
{
    memset(&g_fe,0, sizeof(FILE_ENTITY));
    INIT_LIST_HEAD(&g_fe.listOfFile);
    g_count = 0;
}

void fe_freelist(){
    struct list_head *pos,*q;
    struct FILE_ENTITY *tmp;
    list_for_each_safe(pos,q,&g_fe.listOfFile){
        tmp = list_entry(pos, struct FILE_ENTITY,listOfFile);
        list_del_init(pos);
        free(tmp);
    }
    return;
}

void fe_resetlist(){
    fe_freelist();
    fe_Initlist();
}

int fe_getListSize(){
    return  g_count;
}

void fe_iterateFiles(unsigned int para_flag){
    struct FILE_ENTITY *cur;
    list_for_each_entry(cur, &g_fe.listOfFile, listOfFile)
        print_entity(cur, para_flag);
}


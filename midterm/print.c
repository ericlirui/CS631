#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fts.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include "ls.h"

extern blksize_t g_blocksize;
char path[LS_PATH_MAX];

void print_err(char *fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
}

void print_nonprint_name(const char *pName) {
    const char *p = pName;
    int i;
    for (i = 0; p[i] != '\0'; i++) {
        if (isgraph(p[i])) {
            (void) putchar(p[i]);
        } else {
            (void) putchar('?');
        }
    }
}

void print_human_size(unsigned long bytes)
{
    char szbuf[5];
    (void)humanize_number(szbuf, sizeof(szbuf),
                         bytes,
                         "", HN_AUTOSCALE,
                         (HN_DECIMAL | HN_B | HN_NOSPACE));
    printf("%s ", szbuf);
}

void print_time(unsigned int para_flag,struct stat *pStat){
    time_t outputTime;
    char timeString[MAX_TIME_OUTPUT];

    if (IS_PARAM_FILE_CHANGETIME(para_flag)) {
        outputTime = pStat->st_ctime;
    } else if (IS_PARAM_FILE_ACCESSTIME(para_flag)) {
        outputTime = pStat->st_atime;
    } else {
        outputTime = pStat->st_mtime;
    }
    strftime(timeString, MAX_TIME_OUTPUT, "%b %e %H:%M", localtime(&outputTime));
    (void) printf("%s ", timeString);
}
void print_specialC_aftername(unsigned int para_flag,mode_t* pstMode ){
    char c = '\0';
    switch (*pstMode & S_IFMT) {
        case S_IFDIR:
            c = '/';
            break;
        case S_IFLNK:
            if (IS_PARAM_LIST(para_flag)||
                IS_PARAM_LIST_WITH_NUMERICALLY(para_flag)) {
                break;
            }
            c = '@';
            break;
        case S_IFSOCK:
            c = '=';
            break;
        case S_IFIFO:
            c = '|';
            break;
        case S_IFWHT:
            c = '%';
            break;
        case S_IFREG:
            if (*pstMode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                c = '*';
            }
            break;
    }
    (void) putchar(c);
}
void print_link(FTSENT *pFtsent){
    ssize_t npath;
    char buf[LS_PATH_MAX];
    if (snprintf(path, LS_PATH_MAX, "%s/%s", pFtsent->fts_path, pFtsent->fts_name) < 0) {
        print_err("snprintf error%d\n");
    }
    npath = readlink(path, buf, LS_PATH_MAX);
    buf[npath] = '\0';
    (void) printf(" -> %s ", buf);
}
void print_longformat(unsigned int para_flag,FILE_ENTITY *pfe){
    FTSENT *pFtsent = pfe->pftsent;
    struct stat *pStat = pFtsent->fts_statp;
    dev_t stRdev = pStat->st_rdev;
    mode_t stMode = pStat->st_mode;
    char modestr[MAX_MODE_STR];

    if (!IS_PARAM_LIST(para_flag)&&
        !IS_PARAM_LIST_WITH_NUMERICALLY(para_flag))
        return;
    strmode(stMode, modestr);
    modestr[MAX_MODE_STR-1] = '\0';

    (void) printf("%s ", modestr);

    (void) printf("%u ", pStat->st_nlink);

    (void) printf("%s ", pfe->usrname);

    (void) printf("%s ", pfe->grpname);

    if (S_ISBLK(stMode) || S_ISCHR(stMode))
        (void) printf("%d, %d ", major(stRdev), minor(stRdev));
    else {
        if (IS_PARAM_HUMANREADABLE(para_flag))
            print_human_size(pStat->st_size);
        else
            (void) printf("%ld ", pStat->st_size);
    }
    print_time(para_flag,pStat);
    return;
}
void print_entity(FILE_ENTITY *pfe, unsigned int para_flag){
    FTSENT *pFtsent = pfe->pftsent;
    struct stat *pStat = pFtsent->fts_statp;
    mode_t stMode = pStat->st_mode;

    if(pfe == NULL)
        return;

    if (IS_PARAM_INODE(para_flag)) {
        (void) printf("%lu ", pStat->st_ino);
    }

    if (IS_PARAM_BLOCKSIZE(para_flag)) {
        if (IS_PARAM_HUMANREADABLE(para_flag))
            print_human_size(pStat->st_size);
        else if (IS_PARAM_KILOBYTE(para_flag))
            (void) printf("%llu ",(long long unsigned int) ceil((double) (pStat->st_blocks * DEFAULT_BLOCKSIZE) / 1024));
        else
            (void) printf("%d ", (int)ceil((double) pStat->st_blocks * DEFAULT_BLOCKSIZE / (double) g_blocksize));
    }

    print_longformat(para_flag,pfe);

    if(IS_PARAM_FORCEPRINT(para_flag))
        print_nonprint_name(pFtsent->fts_name);
    else {
        printf("%s", pFtsent->fts_name);
    }
    if (IS_PARAM_CHARACTER_AFTER_PATHNAME(para_flag))
        print_specialC_aftername(para_flag,&stMode);

    if (S_ISLNK(stMode) && (IS_PARAM_LIST(para_flag) ||
                            IS_PARAM_LIST_WITH_NUMERICALLY(para_flag))) {
        print_link(pFtsent);
    }
    putchar('\n');
}


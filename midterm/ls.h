#ifndef _HW_LS_H_
#define _HW_LS_H_

#include <stdbool.h>
#include "dlist.h"

/*macro*/
/* _ means upper case ,no _ means lowercase*/
#define PARAM_NONE 0
#define PARAM_A_   0x1
#define PARAM_A    0x2
#define PARAM_D    0x4
#define PARAM_F_   0x8
#define PARAM_F    0x10
#define PARAM_H    0x20
#define PARAM_I    0x40
#define PARAM_K    0x80
#define PARAM_L    0x100
#define PARAM_N    0x200
#define PARAM_Q    0x400
#define PARAM_R_   0x800
#define PARAM_R    0x1000
#define PARAM_S_   0x2000
#define PARAM_S    0x4000
#define PARAM_T    0x8000
#define PARAM_U    0x10000
#define PARAM_W    0x20000
#define PARAM_C    0x40000

#define LS_PATH_MAX  4096

#ifndef MAX_USER_NAME
#define MAX_USER_NAME (32)
#endif

#ifndef MAX_GROUP_NAME
#define MAX_GROUP_NAME (32)
#endif

#define DEFAULT_BLOCKSIZE 512

#define MAX_MODE_STR 11
#define MAX_TIME_OUTPUT 13

#define IS_PARAM_RECURSIVE(flag)           ((flag) & PARAM_R_)
#define IS_PARAM_REVERSE_SORTED(flag)    ((flag) & PARAM_R)
#define IS_PARAM_INCLUDE_ENTRIES(flag)     ((flag) & PARAM_A)
#define IS_PARAM_ENTRIES_WITHOUT_DOT(flag)  ((flag) & PARAM_A_)
#define IS_PARAM_F_NOT_SORTED(flag)         ((flag) & PARAM_F)
#define IS_PARAM_CHARACTER_AFTER_PATHNAME(flag)         ((flag) & PARAM_F_)
#define IS_PARAM_DIR_NOT_DISPLAY(flag)        ((flag) & PARAM_D)
#define IS_PARAM_INODE(flag)                ((flag)& PARAM_I)
#define IS_PARAM_BLOCKSIZE(flag)            ((flag) & PARAM_S)
#define IS_PARAM_LIST_WITH_NUMERICALLY(flag)      ((flag) & PARAM_N)
#define IS_PARAM_LIST(flag)                       ((flag) & PARAM_L)
#define IS_PARAM_FILE_CHANGETIME(flag)                       ((flag) & PARAM_C)
#define IS_PARAM_FILE_ACCESSTIME(flag)                       ((flag) & PARAM_U)
#define IS_PARAM_FORCEPRINT(flag)                       ((flag) & PARAM_Q)
#define IS_PARAM_KILOBYTE(flag)    ((flag) & PARAM_K)
#define IS_PARAM_HUMANREADABLE(flag)    ((flag) & PARAM_H)
#define IS_PARAM_FORCE_RAWPRINT(flag)    ((flag) & PARAM_W)
#define IS_PARAM_SORTBY_MODTIME(flag)    ((flag) & PARAM_T)
#define IS_PARAM_SORTBY_FILESIZE(flag)    ((flag) & PARAM_S_)


/*structure declaration*/
typedef struct FILE_ENTITY {
    char usrname[MAX_USER_NAME];
    char grpname[MAX_GROUP_NAME];
    FTSENT *pftsent;
    struct list_head listOfFile;
} FILE_ENTITY;

/*function declaration*/

int fe_makeFileEntity(FTSENT *ftentryp, char *uname, char *gname);
void fe_Initlist();
void fe_freelist();
int fe_getListSize();
void fe_resetlist();
void fe_iterateFiles(unsigned int);

void print_err(char *fmt, ...);
void print_entity(FILE_ENTITY*, unsigned int );
void print_human_size(unsigned long bytes);

int sortByName(const FTSENT **a, const FTSENT **b);
int sortBySize(const FTSENT **a, const FTSENT **b);
int sortByChTime(const FTSENT **a, const FTSENT **b);
int sortByModTime(const FTSENT **a, const FTSENT **b);
int sortByAccTime(const FTSENT **a, const FTSENT **b);
int sortByRName(const FTSENT **a, const FTSENT **b);
int sortByRSize(const FTSENT **a, const FTSENT **b);
int sortByRChTime(const FTSENT **a, const FTSENT **b);
int sortByRModTime(const FTSENT **a, const FTSENT **b);
int sortByRAccTime(const FTSENT **a, const FTSENT **b);
#endif






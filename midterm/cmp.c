#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/stat.h>
#include <fts.h>

int sortByName(const FTSENT **a, const FTSENT **b) {
    int a_info, b_info;
    a_info = (*a)->fts_info;
    if (a_info == FTS_ERR)
        return 0;
    b_info = (*b)->fts_info;
    if (b_info == FTS_ERR)
        return 0;

    if (a_info == FTS_NS || b_info == FTS_NS) {
        if (b_info != FTS_NS)
            return 1;
        else if (a_info != FTS_NS)
            return -1;
    }
    return strcmp((*a)->fts_name, (*b)->fts_name);
}

int sortBySize(const FTSENT **a, const FTSENT **b) {
    int a_info, b_info;
    struct stat *pStata;
    struct stat *pStatb;

    a_info = (*a)->fts_info;
    if (a_info == FTS_ERR)
        return 0;
    b_info = (*b)->fts_info;
    if (b_info == FTS_ERR)
        return 0;

    if (a_info == FTS_NS || b_info == FTS_NS) {
        if (b_info != FTS_NS)
            return 1;
        else if (a_info != FTS_NS)
            return -1;
    }

    pStata = (*a)->fts_statp;
    if (pStata == NULL)
        return 0;
    pStatb = (*b)->fts_statp;
    if (pStatb == NULL)
        return 0;

    if ( pStata->st_size > pStatb->st_size) {
        return -1;
    } else if ( pStata->st_size < pStatb->st_size) {
        return 1;
    }
    return sortByName(a, b);
}

int sortByChTime(const FTSENT **a, const FTSENT **b) {
    int a_info, b_info;
    struct stat *pStata;
    struct stat *pStatb;

    a_info = (*a)->fts_info;
    if (a_info == FTS_ERR)
        return 0;
    b_info = (*b)->fts_info;
    if (b_info == FTS_ERR)
        return 0;
    if (a_info == FTS_NS || b_info == FTS_NS) {
        if (b_info != FTS_NS)
            return 1;
        else if (a_info != FTS_NS)
            return -1;
    }

    pStata = (*a)->fts_statp;
    if (pStata == NULL)
        return 0;
    pStatb = (*b)->fts_statp;
    if (pStatb == NULL)
        return 0;

    if(pStata->st_ctime < pStatb->st_ctime){
        return 1;
    } else if (pStata->st_ctime > pStatb->st_ctime){
        return -1;
    }
    return sortByName(a, b);

}


int sortByModTime(const FTSENT **a, const FTSENT **b) {
    int a_info, b_info;
    struct stat *pStata;
    struct stat *pStatb;

    a_info = (*a)->fts_info;
    if (a_info == FTS_ERR)
        return 0;
    b_info = (*b)->fts_info;
    if (b_info == FTS_ERR)
        return 0;
    if (a_info == FTS_NS || b_info == FTS_NS) {
        if (b_info != FTS_NS)
            return 1;
        else if (a_info != FTS_NS)
            return -1;
    }

    pStata = (*a)->fts_statp;
    if (pStata == NULL)
        return 0;
    pStatb = (*b)->fts_statp;
    if (pStatb == NULL)
        return 0;

    if(pStata->st_mtime < pStatb->st_mtime){
        return 1;
    } else if (pStata->st_mtime > pStatb->st_mtime){
        return -1;
    }
    return sortByName(a, b);

}

int sortByAccTime(const FTSENT **a, const FTSENT **b) {
    int a_info, b_info;
    struct stat *pStata;
    struct stat *pStatb;

    a_info = (*a)->fts_info;
    if (a_info == FTS_ERR)
        return 0;
    b_info = (*b)->fts_info;
    if (b_info == FTS_ERR)
        return 0;
    if (a_info == FTS_NS || b_info == FTS_NS) {
        if (b_info != FTS_NS)
            return 1;
        else if (a_info != FTS_NS)
            return -1;
    }

    pStata = (*a)->fts_statp;
    if (pStata == NULL)
        return 0;
    pStatb = (*b)->fts_statp;
    if (pStatb == NULL)
        return 0;

    if(pStata->st_atime < pStatb->st_atime){
        return 1;
    } else if (pStata->st_atime > pStatb->st_atime){
        return -1;
    }
    return sortByName(a, b);

}
int sortByRName(const FTSENT **a, const FTSENT **b) {
    return -sortByName(a,b);
}

int sortByRSize(const FTSENT **a, const FTSENT **b) {
    return -sortBySize(a,b);
}

int sortByRAccTime(const FTSENT **a, const FTSENT **b) {
    return  -sortByAccTime(a,b);
}
int sortByRChTime(const FTSENT **a, const FTSENT **b) {
    return  -sortByChTime(a,b);
}
int sortByRModTime(const FTSENT **a, const FTSENT **b) {
    return  -sortByModTime(a,b);
}

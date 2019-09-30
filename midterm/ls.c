#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "ls.h"

void show(char filename[]){
    DIR *dr;
    struct dirent* direntp;
    if(( dr = opendir(filename)) == NULL)
        fprintf(stderr,"ls: cannot open%s \n",filename);
    while((direntp = readdir(dr)) != NULL)
        printf("%-10s",direntp->d_name);
    closedir(dr);
}
int main(int argc, char* argv[]){
    if(argc == 1){
        show(".");
    }

    while(--argc){
        printf("%s: \n", *++argv);
        show(*argv);
        printf("\n");
    }
    return 0;
}

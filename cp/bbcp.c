#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <errno.h>
#define BUFFSIZE 1024

void print_err(char *fmt, ...)
{
	va_list args;
	va_start(args,fmt);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(1);
}


int main(int argc, char **argv){
	
	int fp[2];
	int buff_to_write;
	char buff[BUFFSIZE];
	DIR *dir = NULL;
	char *src = argv[1];
	char *dest = argv[2];
	char *ts1, *ts2, *src_filename, *dest_dirname;	
	struct stat s_src = {0};
        struct stat s_dest = {0};
	
	if(argc < 3)
		print_err("Usage: bbcp source target");
	if((fp[0] = open(src, O_RDONLY)) == -1){
		print_err("bbcp:Unable to open %s",src);
	}
	dir = opendir(src);
	if(dir){
		print_err("bbcp:source should be a file");
	}
	dir = opendir(dest);
	if(dir){
		/* if dest is a directory , then attach the source file name after the directory*/
		strcat(dest, "/");
		ts1 = strdup(src);
		src_filename =basename(ts1);
		fprintf(stderr, "\n");
		strcat(dest, src_filename);
		closedir(dir);
	}
	else{
		/* if not, get the directory name before the final '/',check if that is a directory*/ 
		ts2 = strdup(dest);
		dest_dirname = dirname(ts2);
		if(dest_dirname){
			DIR *subdir = opendir(dest_dirname);
			if(!subdir){
				print_err("bbcp: target should be a valid path or file name %s",dest);
			}
			closedir(subdir);
		}
	}

	if((fp[1] = open(dest, O_WRONLY | O_CREAT , S_IRUSR | S_IWUSR)) == -1){
		print_err("bbcp:Unable to create %s ", dest);
	}
	
	if(-1 == fstat(fp[0],&s_src)){
		print_err("bbcp:Get file info error");
	}
	if(-1 == fstat(fp[1],&s_dest)){
		print_err("bbcp:Get file info error");
	}
	if(s_src.st_size == s_dest.st_size && 
	   s_src.st_ino == s_dest.st_ino){
		print_err("bbcp: %s and %s are identical,(not copied)",src,dest);
	}
	/*clean the dest file*/
	if(0 != ftruncate(fp[1],0)){
		print_err("bbcp: unable to truncate file, errno:%d",errno);
	}

	if(-1 == lseek(fp[1],0,SEEK_SET)){
		print_err("bbcp: clean file error, errno:%d",errno);
	}
	while((buff_to_write = read(fp[0],buff,BUFFSIZE)) > 0){
		if(buff_to_write != write(fp[1],buff,buff_to_write)){
			print_err("bbcp:Write error to file %s", dest);
		}
	}

	return 0;
}


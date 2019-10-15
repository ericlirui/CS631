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
#include <sys/types.h>
#include <sys/mman.h>

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
	char *buff_to_read = NULL;
	char *buff_to_write = NULL;
	size_t src_filelength = 0;
	DIR *dir = NULL;
	char *src = argv[1];
	char *dest = argv[2];
	char *ts1, *ts2, *src_filename, *dest_dirname;
	struct stat s_src = {0};
        struct stat s_dest = {0};

	if(argc < 3)
                print_err("Usage: bbcpm source target");
	if((fp[0] = open(src, O_RDONLY)) == -1){
		print_err("bbcpm:Unable to open %s",src);
	}
	dir = opendir(src);
	if(dir){
		print_err("bbcpm:source should be a file");
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
				print_err("bbcpm: target should be a valid path or file name %s",dest);
			}
			closedir(subdir);
		}
	}

	if((fp[1] = open(dest, O_RDWR | O_CREAT , S_IRUSR | S_IWUSR)) == -1){
		print_err("bbcpm:Unable to create %s ", dest);
	}
	
	
	if(-1 == fstat(fp[0],&s_src)){
		print_err("bbcpm:Get file info error");
	}
	if(-1 == fstat(fp[1],&s_dest)){
		print_err("bbcpm:Get file info error");
	}
	if(s_src.st_size == s_dest.st_size && 
	   s_src.st_ino == s_dest.st_ino){
		print_err("bbcpm: %s and %s are identical,(not copied)", src, dest);
	}
	/*clean the dest file*/
	if(0 != ftruncate(fp[1],0)){
		print_err("bbcpm: unable to truncate file, errno:%d", errno);
	}

	if(-1 == lseek(fp[1],0,SEEK_SET)){
		print_err("bbcpm: clean file error, errno:%d", errno);
	}
	
	/*get the source file length*/
	src_filelength = lseek(fp[0],0,SEEK_END);
	if(0xFFFFFFFF == src_filelength){
		print_err("bbcpm: get source file length err, errno:%d", errno);
	}
	if(0 != src_filelength){
		buff_to_read = mmap(NULL, src_filelength, PROT_READ, MAP_PRIVATE, fp[0], 0);
		if(buff_to_read == MAP_FAILED){
			print_err("bbcpm: mmap1 error,errno%d,%d", errno,src_filelength);
		}
	
		if(0 !=	ftruncate(fp[1], src_filelength)){
			print_err("bbcpm: unable to truncate file, errno:%d", errno);
		}
	
		buff_to_write = mmap(NULL,src_filelength, PROT_READ | PROT_WRITE, MAP_SHARED, fp[1], 0 );
		if(buff_to_write == MAP_FAILED){
			print_err("bbcpm: mmap error,errno:%d", errno);
		}
	
		(void)memcpy(buff_to_write, buff_to_read, src_filelength);
		if(-1 == munmap(buff_to_read, src_filelength)){
			print_err("bbcpm: munmap error,errno%d", errno);
		}
		if(-1 == munmap(buff_to_write, src_filelength)){
			print_err("bbcpm: munmap error,errno%d", errno);
		}
	}
	(void)close(fp[0]);
	(void)close(fp[1]);
	return 0;
}


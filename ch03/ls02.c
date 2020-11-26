/** ls1.c
 **   purpose  list contents of directory or directories
 **   action   if no args, use .  else list files in args
 **/

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>

void do_ls(char dirname[]);
// ls 
int main(int argc, char** argv) 
{
    if(argc == 1) {
        do_ls(".");
    } else {
        while(--argc) {
            printf("%s:\n", *++argv);
            do_ls(*argv);
        }
    }
    return 0;
} 

// list files in directory called dirname
// 就是循环调用 readdir ，对 通过  opendir 打开 dirname 的 dir_ptr 不断进行读出 
void do_ls(char dirname[]) {
    DIR *dir_ptr;
    struct dirent * dirent_pt;// each entry

    if((dir_ptr = opendir(dirname)) == NULL) {
        fprintf(stderr, "ls1: cannot open %s\n", dirname);
    } else {// open sucessful
        while((dirent_pt = readdir(dir_ptr)) != NULL) {
            printf("%s\n", dirent_pt->d_name);
        }
        closedir(dir_ptr);
    }
}
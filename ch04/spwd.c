/* spwd.c: a simplified version of pwd
 *
 *	starts in current directory and recursively
 *	climbs up to root of filesystem, prints top part
 *	then prints current part
 *
 *	uses readdir() to get info about each thing
 *
 *      bug: prints an empty string if run from "/"
 **/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

ino_t get_inode(char *fname);
void inum_to_name(ino_t, char *, int);
void printpathto(ino_t);


int main() 
{
    printpathto(get_inode("."));    
    putchar('\n');  // add new line
    return 0;
}


// return inode num of the file
ino_t get_inode(char *fname) {
    struct stat info;

    if(stat(fname, &info) == -1) {
        perror(fname);
        fprintf(stderr, " Cannot stat ");
        exit(EXIT_FAILURE);
    }
    return info.st_ino;
}

// 通过当前 inode 所在目录，得到 .. 的inode
// 并打印当前目录的 name
// 再进行递归调用
void printpathto(ino_t cur_inode) {
    

    char name[BUFSIZ];
    if(get_inode("..") != cur_inode) {// 递归终止条件
        chdir("..");// get parent path, 改为当前目录

        inum_to_name(cur_inode, name, BUFSIZ);

        printpathto(get_inode("."));// 递归调用
        // 从顶向下打印
        printf("/%s", name);
    }// 至此 说明终止, 结束即可。

}

// inode -> name
// copies its name into namebuf
void inum_to_name(ino_t inode, char *name_buf, int buf_len) {
    DIR *dir_ptr;
    struct dirent *direntp;

    if( (dir_ptr = opendir(".")) == NULL) {
        perror(".");
        exit(EXIT_FAILURE);
    }
    // 至此 成功打开
    // 开始读当前所在的目录中目录项， 找到相应的名字 
    while((direntp = readdir(dir_ptr)) != NULL) {
        if(direntp->d_ino == inode) {//说明找到
            strncpy(name_buf, direntp->d_name, buf_len);
            name_buf[buf_len - 1] = '\0';
            closedir(dir_ptr);// 关闭
            return;
        }
    }
    // 至此说明查找失败
    fprintf(stderr, "error looking for inum %d\n", inode);
    exit(EXIT_FAILURE);
}
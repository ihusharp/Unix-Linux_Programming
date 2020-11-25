#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFSIZE 4096
#define COPMODE 0644// 文件模式
// A- 0， 表示十进制
// B－用户
// C－组用户
// D－其他用户
// 0644->即用户具有读写权限，组用户和其它用户具有只读权限；

void oops(char*, char*);

int main(int argc, char** argv) {

    char buf[BUFSIZE];// 缓冲区

    if(argc != 3) {
        fprintf(stderr, "usage:%s source destination\n", *argv);
        exit(1);
    }

    int in_fd, out_fd;
    if((in_fd = open(argv[1], O_RDONLY)) == -1) {
        oops("Can't open ", argv[1]);// 不能打开第一个
    }
    if((out_fd = creat(argv[2], COPMODE)) == -1) {
        oops("Can't create ", argv[2]);// 不能 写入
    }

    int n_chars;
    while((n_chars = read(in_fd, buf, BUFSIZE)) > 0) {//说明读入
        if(write(out_fd, buf, n_chars) != n_chars) {// 写入不同
            oops("Write error to ", argv[2]);
        }
    }
    if(n_chars == -1) {
        oops("Read error from ", argv[1]);
    }
    if(close(in_fd) == -1 || close(out_fd) == -1) {
        oops("Close file error!","");
    }

    return 0;
}

// 输出错误
void oops(char* err, char* err2) {
    fprintf(stderr, "Error: %s", err);
    perror(err2);
    exit(1);
}
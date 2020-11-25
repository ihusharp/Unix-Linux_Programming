#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <utmp.h>
#include <stdlib.h>
#include <time.h>
// #include "utmplib.h"

#define SHOWHOST    // include remote machine on output
void show_time(long time) ;
void show_info(struct utmp* buf);

int main()
{
    int fd;
    struct utmp current_record;
    int reclen = sizeof(struct utmp);

    // #define UTMP_FILE	_PATH_UTMP
    // #define	_PATH_UTMP	"/var/run/utmp"
    fd = open(UTMP_FILE, O_RDONLY);// 只读
    if(fd == -1) {// 0 表示成功， -1 表示失败
        perror("who0");
        exit(1);// 退出
    }
    // 返回 读入长度
    while(read(fd, &current_record, reclen) == reclen) {
        show_info(&current_record);
    }
    close(fd);

    return 0;
}

// husharp  :0           2020-11-25 18:59 (:0)
void show_info(struct utmp* buf) {
    if(buf->ut_type != USER_PROCESS) {// 
        return;
    }
    printf("%-8.8s ", buf->ut_user);// 左对齐
    printf("%-8.8s ", buf->ut_line);
    // printf("%10ld",                          buf->ut_time);
    show_time(buf->ut_time);
    // char* cp = ctime(&(buf->ut_time));
    

#ifdef SHOWHOST
    if(buf->ut_host[0] != '\0') {
        printf("(%s)", buf->ut_host);
    }
#endif
    printf("\n");
}

void show_time(long time) {
    char* timeVal;
    timeVal = ctime(&time);

    /*原 time 输出为  printf("%.20s", timeVal); 
        Wed Nov 25 21:59:12 
        现只需要
        Nov 25 21:59
        因此输出格式如下
    */
    printf("%.12s", timeVal + 4);
}
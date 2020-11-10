/* more01.c  - version 0.1 of more
 *	read and print 24 lines then pause for a few special commands
 */

#include <stdio.h>
#include <stdlib.h>

#define PAGELEN 24
#define LINELEN 512

int see_more();
void do_more(FILE*);

int main(int argc, char* argv[])
{
    FILE* fp;

    if(argc == 1) {// 如果没有参数， 那便从标准输入中读入
                    // eg  who | more 将 who 重定向到 more 的输入中 
        do_more(stdin);
    } else {
        while(--argc) {
            if((fp = fopen(*++argv, "r")) != NULL) {
                do_more(fp);
                fclose(fp);
            } else {
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}

// read PAGELEN lines, then call see_more() for further instructions
void do_more(FILE* fp)
{
    char line[LINELEN];
    int num_of_lines = 0;
    int see_more(), reply;// 声明

    while(fgets(line, LINELEN, fp)) {
        if(num_of_lines == PAGELEN) {
            reply = see_more();
            if(reply == 0) {// 说明为 q
                break;
            }
            num_of_lines -= reply;// 以通过 while 向下继续读取 line
        }
        if(fputs(line, stdout) == EOF) {
            exit(EXIT_FAILURE);
        }
        num_of_lines++;// 计数
    }

}

// 读取 输入字符， 返回下移行数
int see_more()
{
    // response 分为三种情况
    /*
    1、 space: next line
    2、 CR： next page
    3、 q： exit
    */
    int c;
    // echo -e "\033[7m more? \033[m" 显示 [more?]的提示方格
    printf("\033[7m more? \033[m");
    while ((c = getchar()) != EOF) {
        if (c == 'q') {
            return 0;
        }
        if (c == ' ') {// 空格为 1 面 即 为行 
            return PAGELEN;
        }
        if (c == '\n') {// 回车为 1 行
            return 1;
        }
    }
    return 0;
}
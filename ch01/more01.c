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

    if(argc == 1) {
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
    int num_of_lines = 0, reply;
    int see_more();

    // 不断读入
    while (fgets(line, LINELEN, fp)) {
        if(num_of_lines == PAGELEN) {
            reply = see_more();// ask 
            if(reply == 0) {
                break;
            }
            num_of_lines -= reply;// reset cnt
        }
        if(fputs(line, stdout) == EOF) {
            exit(EXIT_FAILURE);
        }
        num_of_lines++;
    }
}

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
        if (c == ' ') {
            return PAGELEN;
        }
        if (c == 'q') {
            return 1;
        }
    }
    return 0;
}
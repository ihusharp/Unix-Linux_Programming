/*
添加键入字符立即响应程序
*/
#include<stdio.h>
#include<stdlib.h>

#define	PAGELEN	24
#define	LINELEN	512

void do_more(FILE *);
int see_more(FILE *);

int main(int argc, char ** argv)
{
    FILE *fp;
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

void do_more(FILE *fp) {
    char line[LINELEN];
    int num_of_lines = 0;

    int see_more(FILE *), reply;// 声明

    // tty 文件夹是键盘和显示器 的设备描述文件
    // 向这个文件写相当于在用户的屏幕上
    // 向这个文件读， 相当于从键盘获取输入
    FILE *fp_tty;
    if((fp_tty = fopen("/dev/tty", "r")) == NULL) {
        exit(EXIT_FAILURE);
    }

    while(fgets(line, LINELEN, fp)) {
        if(num_of_lines == PAGELEN) {//到达最低端
            reply = see_more(fp_tty);
            if(reply == 0) {
                break;
            }
            num_of_lines -= reply;
        }
        if(fputs(line, stdout) == EOF) {
            exit(EXIT_FAILURE);
        } 
        num_of_lines++;
    }
}

int see_more(FILE *cmd) {
    
	int	c;

	printf("\033[7m more? \033[m");		/* reverse on a vt100	*/
	while( (c=getc(cmd)) != EOF )		/* NEW: reads from tty  */
	{
		if ( c == 'q' )			/* q -> N		*/
			return 0;
		if ( c == ' ' )			/* ' ' => next page	*/
			return PAGELEN;		/* how many to show	*/
		if ( c == '\n' )		/* Enter key => 1 line	*/
			return 1;		
	}
	return 0;
}


/* play_again4.c
 *	purpose: ask if user wants another transaction
 *	 method: set tty into chr-by-chr, no-echo mode
 *		 set tty into no-delay mode
 *		 read char, return result
 *		 resets terminal modes on SIGINT, ignores SIGQUIT
 *	returns: 0=>yes, 1=>no, 2=>timeout
 *	 better: reset terminal mode on Interrupt
 */
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define	ASK	"Do you want another transaction"
#define	TRIES   3  /* max tries */
#define SLEEPTIME  2 /* time per try */
#define	BEEP       printf("\naaaaa!");// alert user

int get_response(char* , int);// 给出回应
int tty_mode(int how);// 修改或者保持 mode 
int set_cr_noecho_mode();// 设置为无回显， char_by_char
int set_nodelay_mode(); // 设置为非阻塞
int get_ok_char();
int set_delay_mode();// 设置为阻塞
void ctrl_c_handler(int);// 对 Ctrl+C 的信号捕获

int main() {

    tty_mode(0);    // 设置为 mode 此时为 char_by_char 
    set_cr_noecho_mode();// 设置为无回显， char_by_char
    set_nodelay_mode();// 设置为非阻塞
    // set_delay_mode();
    signal(SIGINT, ctrl_c_handler);// handler int
    signal(SIGQUIT, SIG_IGN);// 忽略 quit
    int response = get_response(ASK, TRIES);
    tty_mode(1);// reload tty mode
    printf("\nresponse is %d\n", response);
    return response;
}

/*
 * purpose: ask a question and wait for a y/n answer or maxtries
 *  method: use getchar and complain about non-y/n input
 * returns: 0=>yes, 1=>no, 2=>timeout
 */
int get_response(char* question, int maxTries) {
    printf("%s (y/n)?", question);
    int input;
    while(1) {
        sleep(SLEEPTIME);
        input = tolower(get_ok_char());
        if ( input == 'y' )
			return 0;
		if ( input == 'n' )
			return 1;
		if ( maxTries-- == 0 )			/* outatime?	*/
			return 2;			/* sayso	*/
        BEEP;// 提示
    }
}

/*
 *  skip over non-legal chars and return y,Y,n,N or EOF
 */
int get_ok_char() {
    int c;
    // strchr 查找第一个匹配处, 并返回地址
    // 对于不匹配的直接返回
    while( (c = getchar()) != EOF && strchr("yYnN", c) == NULL);
    return c;
}

/* how == 0 => save current mode,  how == 1 => restore mode */
/* this version handles termios and fcntl flags             */
int tty_mode(int how) {
    static struct termios original_mode;// 保存原状态
    static int original_flags;  // 保存
    static int changed = 0;// 表示之前是否改变

    if(how == 0) {
        tcgetattr(0, &original_mode);
        original_flags = fcntl(0, F_GETFL);
        changed = 1;// 表示 change
    } else if(changed) {// 进行恢复
        return tcsetattr(0, TCSANOW, &original_mode);
        fcntl(0, F_SETFL, original_flags);
    }
}

/* 
 * purpose: put file descriptor 0 into chr-by-chr mode and noecho mode
 *  method: use bits in termios
 */
int set_cr_noecho_mode(){
    struct termios ttystate;

    tcgetattr(0, &ttystate);// read cur
    ttystate.c_lflag &= ~ICANON;/* no buffering		*/
    ttystate.c_lflag &= ~ECHO;  // no echo
    // VMIN定义了要求等待的最小字节数，
    // 下面表示将这个字节数 置为 1。
    ttystate.c_cc[VMIN] = 1;/* get 1 char at a time	*/
    tcsetattr(0, TCSANOW, &ttystate);
}


/*
 * purpose: put file descriptor 0 into no-delay mode
 *  method: use fcntl to set bits
 *   notes: tcsetattr() will do something similar, but it is complicated
 */
int set_nodelay_mode() {
    int flags;
    flags = fcntl(0, F_GETFL);// get
    flags |= O_NDELAY;// 非阻塞
    fcntl(0, F_SETFL, flags);// set
}

int set_delay_mode() {
    int flags;
    flags = fcntl(0, F_GETFL);// get
    flags &= ~O_NDELAY;// 非阻塞
    fcntl(0, F_SETFL, flags);// set
}


/*
 * purpose: called if SIGINT is detected
 *  action: reset tty and scram
 */
void ctrl_c_handler(int signum) {
    tty_mode(1);// 恢复
    exit(2);
}
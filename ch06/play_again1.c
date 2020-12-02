/* play_again1.c
 *	purpose: ask if user wants another transaction
 *	 method: set tty into char-by-char mode, read char, return result
 *	returns: 0=>yes, 1=>no
 *	 better: do no echo inappropriate input
 */

#include <stdio.h>
#include <termios.h>

#define	QUESTION	"Do you want another transaction"
int get_response(char* );// 给出回应
int tty_mode(int how);// 修改或者保持 mode 
int set_crmode();

int main() {

    // static struct termios original_mode;
    tty_mode(0);    // 设置为 mode 此时为 char_by_char 
    set_crmode();
    int response = get_response(QUESTION);
    // printf("response is %d\n", response);
    tty_mode(1);
    printf("\nresponse is %d\n", response);
    return response;
}

/*
 * purpose: ask a question and wait for a y/n answer
 *  method: use getchar and ignore non y/n answers
 * returns: 0=>yes, 1=>no
 */
int get_response(char* question) {
    printf("%s (y/n)?", question);
    int input;
    while(1) {
        switch(input = getchar()) {
            case 'y':
            case 'Y':
                return 0;
            case 'n':
            case 'N':
            case EOF:
                return 1;
            default:
				printf("\ncannot understand %c, ", input);
				printf("Please type y or no\n");
        }
    }
}

/* how == 0 => save current mode,  how == 1 => restore mode */
int tty_mode(int how) {
    static struct termios original_mode;// 保存原状态
    if(how == 0) {
        tcgetattr(0, &original_mode);
    } else {
        return tcsetattr(0, TCSANOW, &original_mode);
    }
}

/* 
 * purpose: put file descriptor 0 (i.e. stdin) into chr-by-chr mode
 *  method: use bits in termios
 */
int set_crmode(){
    struct termios ttystate;

    tcgetattr(0, &ttystate);// read cur
    ttystate.c_lflag &= ~ICANON;/* no buffering		*/
    // VMIN定义了要求等待的最小字节数，
    // 下面表示将这个字节数 置为 1。
    ttystate.c_cc[VMIN] = 1;/* get 1 char at a time	*/
    tcsetattr(0, TCSANOW, &ttystate);
}
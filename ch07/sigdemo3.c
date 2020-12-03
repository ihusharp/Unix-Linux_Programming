/* sigdemo3.c
 *	purpose:   show answers to signal questions
 *	question1: does the handler stay in effect after a signal arrives?
 *	question2: what if a signalX arrives while handling signalX?
 *      question3: what if a signalX arrives while handling signalY?
 *      question4: what happens to read() when a signal arrives?
 */
// 测试多个信号
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define INPUTLEN 100

void inthandler(int);// 中断信号 Ctrl + C
void quithandler(int);// quit 信号 Ctrl + '\'

int main(int argc, char** argv) {

    signal( SIGINT,  inthandler );		/* set handler */
    signal( SIGQUIT, quithandler );		/* set handler */

    int nchars;
    char input[INPUTLEN];
    do// 不为 quit 便循环
    {
        printf("\nType a message\n");
        if( (nchars = read(0, input, (INPUTLEN - 1))) == -1) {
            perror("read error!\n");
        } else {
            input[nchars] = '\0';// 终结符
            printf("You typed: %s", input);
        }
    } while (strncmp(input, "quit", 4) != 0);
    
}

// 中断信号 Ctrl + C
void inthandler(int s) {
    printf(" Received signal %d .. waiting\n", s );
    sleep(2);
    printf("    Leaving inthandler \n");
}

// quit 信号 Ctrl + '\'
void quithandler(int s) {
	printf(" Received signal %d .. waiting\n", s );
	sleep(3);
	printf("  Leaving quithandler \n");    
}
/* sigactdemo.c
 *               purpose: shows use of sigaction()
 *               feature: blocks ^\ while handling ^C
 *              does not reset ^C handler, so two kill
 */

// 注意是如何处理 SIGINT 时阻塞 SIGQUIT 的

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#define INPUTLEN 100
void intHandler(int);

int main(int argc, char** argv) {
    struct sigaction newHandler;    // new setting
    sigset_t blocked;      // 将一部分阻塞
    char x[INPUTLEN];

/*
struct sigaction {
	 // 下面两个选择一个
	__sighandler_t sa_handler; // SIG_DFL、SIG_IGN、或者函数处理
	void (*sa_sigaction) (int, siginfo_t *, void *);	// new handler 
		
	unsigned long sa_flags;
	__sigrestore_t sa_restorer;
	sigset_t sa_mask;		// mask last for extensibility
};
*/
    /* load these two members first */  
    newHandler.sa_handler = intHandler;
    // 设置为 捕鼠器模式 + 被中断的系统调用会被自动重新启动
    // 捕鼠器模式指：当再次调用 Ctrl + C 时不会再起作用（只会处理用一次）
    // newHandler.sa_flags = SA_RESETHAND | SA_RESTART;//捕鼠器模式 + 被中断的系统调用会被自动重新启动
    newHandler.sa_flags = SA_RESTART;// 被中断的系统调用会被自动重新启动

    /* then build the list of blocked signals */
    sigemptyset(&blocked); /* clear all bits      */
    // sigaddset()用来将参数signum 代表的信号加入至参数set 信号集里.
    // 信号集表示哪些要被阻塞，此处便是指 Ctrl+\ 被 Ctrl+C 阻塞
    sigaddset(&blocked, SIGQUIT);/* add SIGQUIT to list */

    newHandler.sa_mask = blocked;// 置为 设置的模式

    if(sigaction(SIGINT, &newHandler, NULL) == -1) {
        perror("sigaction");
    } else {
        while(1) {
            fgets(x, INPUTLEN, stdin);
            printf("input: %s", x);
        }
    }

}

void intHandler(int s) {
    printf("Called with signal %d\n", s);
    sleep(2);
    printf("done handling signal %d\n", s);
}
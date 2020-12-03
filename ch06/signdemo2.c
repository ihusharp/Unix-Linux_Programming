/* sigdemo2.c - shows how to ignore a signal
 *            - press Ctrl-\ to kill this one
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main() {

    signal(SIGINT, SIG_IGN);// 忽略信号 
    printf("you can't stop me!\n");
    printf("try Ctrl+C?\n");

    while(1) {
        sleep(1);
        // 在UNIX类系统，换行\n就表现为光标下一行并回到行首
        printf("hahha(´∀`)σ)Д`)");
        fflush(0);
    }
    
}

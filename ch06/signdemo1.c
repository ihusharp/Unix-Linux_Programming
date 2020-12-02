/* sigdemo1.c - shows how a signal handler works.
 *            - run this and press Ctrl-C a few times
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void f(int);// handler 声明

int main() {
    int i;

    signal(SIGINT, f);
    for (i = 0; i < 5; i++)
    {
        printf("hello\n");
        sleep(1);
    }
    
}

void f(int signum) {
    printf("O!\n");
}
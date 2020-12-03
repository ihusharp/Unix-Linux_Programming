/* waitdemo1.c - shows how parent pauses until child finishes
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define	DELAY	2

void child_code(int delay);
void parent_code(int childpid);

int main(int argc, char** argv) {
    int newPid;

    printf("before: mypid is %d\n", getpid());

    if((newPid = fork()) == -1) {
        perror("fork");
    } else if(newPid == 0) {
        // 子进程
        child_code(3);
    } else {
        // 父进程
        parent_code(newPid);
    }
}

// new process takes a nap and then exits
void child_code(int delay) {
    printf("child %d here. will sleep for %d seconds\n", getpid(), delay);
    sleep(delay);
    printf("child done.about to exit\n");
    exit(100);
}
void parent_code(int childpid) {
    int wait_rv;		/* return value from wait() */
	wait_rv = wait(NULL);// 返回子进程 pid
	printf("done waiting for %d. Wait returned: %d\n", childpid, wait_rv);
}
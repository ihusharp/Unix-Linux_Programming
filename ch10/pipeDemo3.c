// 实现进程间的通信

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define	CHILD_MESS	"I want a cookie\n"
#define	PAR_MESS	"testing..\n"
#define	oops(m,x)	{ perror(m); exit(x); }


int main()
{
	int	pipefd[2];		/* the pipe	*/
	int	len;			/* for write	*/
	char	buf[BUFSIZ];		/* for read	*/
	int	read_len;

	if ( pipe( pipefd ) == -1 )
		oops("cannot get a pipe", 1);

    int pid = fork();
    if(pid == -1) {
        perror("fork error!");
    } else if(pid == 0) {// child
    // 不再指向标准输出，而是指向创建的管道文件，
    // 那么以后往标准输出写入的任何东西，都会写入管道文件。
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execlp("ps", "ps", "-ef", NULL);
        exit(0);
    } else {// parent
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execlp("grep", "grep", "systemd", NULL);
    }

    return 0;
}
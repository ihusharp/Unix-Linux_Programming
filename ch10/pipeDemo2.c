/* pipedemo2.c	* Demonstrates how pipe is duplicated in fork()
 *		* Parent continues to write and read pipe,
 *		  but child also writes to the pipe
 */
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
        close(pipefd[0]);// 关闭读
        char msg[] = "hello world";
        write(pipefd[1], msg, strlen(msg) + 1);
        close(pipefd[1]);
        exit(0);
    } else {// parent
        close(pipefd[1]);// 关闭写
        char msg[128];
        read(pipefd[0], msg, 128);
        close(pipefd[0]);
        printf("message: %s\n", msg);
        return 0;
    }
}
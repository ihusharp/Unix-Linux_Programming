/* selectdemo.c : watch for input on two devices AND timeout
 *         usage: selectdemo dev1 dev2 timeout
 *        action: reports on input from each file, and 
 *                reports timeouts
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define oops(m, x)  {perror(m);exit(x);}
void showdata(char *fname, int fd);

int main(int argc, char** argv) {
	int    fd1, fd2;           /* the fds to watch      */
	struct timeval timeout;    /* how long to wait      */
	fd_set readfds;            /* watch these for input */
	int    maxfd;		   /* max fd plus 1	    */
	int    retval;		   /* return from select    */

	if ( argc != 4 ){
		fprintf(stderr,"usage: %s file file timeout", *argv);
		exit(1);
	}

	/** open files **/
	if ( (fd1 = open(argv[1],O_RDONLY)) == -1 )
		oops(argv[1], 2);
	if ( (fd2 = open(argv[2],O_RDONLY)) == -1 )
		oops(argv[2], 3);
    maxfd = 1 + (fd1 < fd2 ? fd2 : fd1);// 监听的最大文件描述符 + 1

    while(1) {
        //获得所需要的文件描述符列表
        FD_ZERO(&readfds);
        FD_SET(fd1, &readfds);
        FD_SET(fd2, &readfds);
        // atoi (表示 ascii to integer)是把字符串转换成整型数的一个函数
        timeout.tv_sec = atoi(argv[3]);// 标记超时时间
        timeout.tv_usec = 0;

        // wait for input
        // 返回满足需求的文件描述符 数目
        retval = select(maxfd, &readfds, NULL, NULL, &timeout);
        if(retval == -1)
            oops("select", 4);


        if(retval > 0) {
            // FD_ISSET是为了检查在select函数返回后，某个描述符是否准备好
            if(FD_ISSET(fd1, &readfds))// 进行监听
                showdata(argv[1], fd1);
            if(FD_ISSET(fd1, &readfds))
                showdata(argv[1], fd1);
        } else {
            printf("no input after %d seconds\n", atoi(argv[3]));
        }


    }

    
}

void showdata(char *fname, int fd) {
    char buf[BUFSIZ];


    printf("%s: ", fname);
    fflush(stdout);
    int n = read(fd, buf, BUFSIZ);
    if ( n == -1 )
		oops(fname,5);
    write(1, buf, n);
    write(1, "\n", 1);

}
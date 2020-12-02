// #include <error.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/types.h>
// #include <sys/wait.h>
// int main()
// {
//     pid_t id = fork();
//     if (id < 0) {
//         perror("fork");
//         exit(1);
//     } else if (id == 0) {
//         //child
//         printf("I am child\n");
//         sleep(5);
//         exit(20);
//     } else {
//         int status = 0;
//         pid_t ret = waitpid(-1, &status, 0); //   阻塞式等待，等待5s
//         printf("proc wait...\n");
//         // WIFEXITED(status) 这个宏用来指出子进程是否为正常退出的，
//         // 如果是，它会返回一个非零值。
//         if (WIFEXITED(status) && ret == id) {
//             printf("wait success,child return code is :%d.\n", WEXITSTATUS(status));

//         } else {
//             printf("wait fail,return.\n");
//             return 1;
//         }
//     }
//     return 0;
// }

//非阻塞的等待方式
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
int main()
{
    pid_t id = fork();
    if (id < 0) {
        perror("fork");
        exit(1);
    } else if (id == 0) {
        //child
        printf("I am child\n");
        sleep(5);
        exit(20);
    } else {
        int status = 0;
        pid_t ret = 0;
        do {
            // 如果pid指定的子进程没有结束，则waitpid()函数立即返回0，
            // 而不是阻塞在这个函数上等待
            ret = waitpid(-1, &status, WNOHANG);// 非阻塞
            if (ret == 0) {
                printf("child is running\n");
            }
            sleep(1);
        } while (ret == 0);
        if (WIFEXITED(status) && ret == id) {
            printf("wait success,child return code is %d\n", WEXITSTATUS(status));
        } else {
            printf("wait child fail,return\n");
            return 1;
        }
    }
    return 0;
}

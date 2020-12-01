/* echostate.c
 *   reports current state of echo bit in tty driver for fd 0
 *   shows how to read attributes from driver and test a bit
 */

#include <stdio.h>
#include <termios.h>
#include <stdlib.h>

int main() {
    struct termios info;

    // 此处 0 表示从标准输入读入
    int readVal = tcgetattr(0, &info);

    if(readVal == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    if(info.c_cflag & ECHO) {
        printf(" echo is on , since its bit is 1\n");
    } else {
        printf(" echo if OFF, since its bit is 0\n");
    }

    return 0;
}
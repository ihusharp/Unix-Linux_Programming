/* setecho.c
 *   usage:  setecho [y|n]
 *   shows:  how to read, change, reset tty attributes
 */

#include <stdio.h>
#include <termios.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    struct termios info;

    if (argc == 1) {
        exit(EXIT_FAILURE);
    }
    if(tcgetattr(0, &info) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    if(argv[1][0] == 'y') {
        info.c_cflag |= ECHO;   // turn on
        printf("open sucess!\n");
    } else {
        printf("close sucess!\n");
        info.c_cflag &= ~ECHO;  // turn off
    }

    if(tcsetattr(0, TCSANOW, &info) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
    return 0;
}
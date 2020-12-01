// write0.c
//
// purpose: send messages to another terminal
//  method: open the other terminal for output then copy from stdin to that
//          terminal
//    show: a terminal is just a file supporting regular i/o
//   usage: write0 ttyname

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s [ttyname]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int tty_fd = open(argv[1], O_WRONLY);
    if(tty_fd == -1) {
        fprintf(stderr, "Cannot open!\n");
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    char buf[BUFSIZ];
    int size;
    while((size = read(tty_fd, buf, BUFSIZ)) > 0) {
        if(size != write(tty_fd, buf, size)) {//说明未完全写入
            fprintf(stderr, "Cannot write to %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;

}

#include <termios.h>
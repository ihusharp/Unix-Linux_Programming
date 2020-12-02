

#include<stdio.h>

int
main(int argc, char *argv[])
{
    int ch;
    while(1)
    {
        ch = fgetc(stdin);
        if (ch == 10) {
            fprintf(stdout, "You have pressed the enter key\n");
        }
    }
}

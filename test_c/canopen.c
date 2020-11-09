#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    FILE* fp;
    if (argc != 2) {
        printf("can't open!\n");
        exit(EXIT_FAILURE);
    }
    if ((fp = fopen(argv[1], "r")) == NULL) {
        printf("can't open!\n");
        exit(EXIT_FAILURE);    
    }
    printf("can open!\n");
    exit(EXIT_SUCCESS);
    fclose(fp);
    return 0;
}
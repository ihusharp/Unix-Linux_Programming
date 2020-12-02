#include <stdio.h>

int main() {
    int c;
    int n = 0;
    printf("aaa\rbbb\n");
    printf("aaa\nbbb");
    while((c = getchar()) != 'Q') {
        printf("char %3d is %c code %d\n", n++, c, c);
    }
}
/* twordcount3.c - threaded word counter for two files.	
 *		 - Version 3: one counter per file
 */
// 分别运行， 每个线程给予私有参数， 最后一起加起来
#include <stdio.h>
#include <pthread.h>
#include <ctype.h>

// 当创建线程需要多个参数时， 采用构造一个结构体
struct arg_set{
    char *fname;
    int cnt;
};

void countWords(void *);

int main(int argc, char** argv) {
    pthread_t t1, t2;
    struct arg_set args1, args2;

    if ( argc != 3 ){
		printf("usage: %s file1 file2\n", argv[0]);
		exit(1);
	}

    args1.fname = argv[1];
    args1.cnt = 0;
    pthread_create(&t1, NULL, countWords, (void*)&args1);
    
    args2.fname = argv[2];
    args2.cnt = 0;
    pthread_create(&t2, NULL, countWords, (void*)&args2);
    
    // 进行等待
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("%5d: %s\n", args1.cnt, argv[1]);
	printf("%5d: %s\n", args2.cnt, argv[2]);
	printf("%5d: total words\n", args1.cnt + args2.cnt);
}


void countWords(void *a) {
    struct arg_set *args = a;
    FILE* fp;

    int c, prevc = '\0';
    if(( fp = fopen(args->fname, "r")) != NULL) {
        while( (c = getc(fp)) != EOF) {
            // 如果 c 是一个数字或一个字母，则该函数返回非零值，否则返回 0。
            // 此处判断， 凡是一个非字母或数字跟在字母或数字后面， 那么就是单词结尾
            if(!isalnum(c) && isalnum(prevc)) {
                args->cnt++;
            }
            prevc = c;
        }
        fclose(fp);
    } else {
        perror(args->fname);
    }
    return NULL;
}
/* utmplib.c  - functions to buffer reads from utmp file 
 *
 *      functions are
 *              utmp_open( filename )   - open file
 *                      returns -1 on error
 *              utmp_next( )            - return pointer to next struct
 *                      returns NULL on eof
 *              utmp_close()            - close file
 *
 *      reads NRECS per read and then doles them out from the buffer
 */
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <utmp.h>

// 缓冲区可以存 16 个记录
#define NRECS 16
#define NULLUT ((struct utmp*)NULL)
#define UTSIZE (sizeof(struct utmp))

static char utmpbuf[NRECS*UTSIZE];
static int num_recs;// 缓冲区目前数量
static int cur_recs;// 缓冲区中已被使用数量

static int fd_utmp = -1;


// open utmp 
int utmp_open(char* filename) {
    fd_utmp = open(filename, O_RDONLY);
    cur_recs = num_recs = 0;
    return fd_utmp;
}


// read next bunch of records into buffer
// 可以使 read 次数减少到 1 / NRECS 
int utmp_reload() {
    int amr_read = read(fd_utmp, utmpbuf, NRECS * UTSIZE);

    num_recs = amr_read / UTSIZE;// 读入记录数量
    cur_recs = 0;

    return num_recs;
}

// utmp_next 会调用 read 通过内核获取 16 条充满缓冲区
struct utmp* utmp_next() {
    struct utmp *recp;

    if(fd_utmp == -1) {
        return NULLUT;
    }
    // 至此成功打开
    // 若缓冲区已经被使用数量， 和 缓冲区中存入的数量（不一定为默认值，因为可能最后没有剩下那么多）
    if((cur_recs == num_recs) && (utmp_reload() == 0)) {
        return NULLUT;
    }
    // 至此在 已经使用完缓冲区所有数量后， 调用 utmp_reload 返回非 0
    recp = (struct utmp*)&utmpbuf[cur_recs * UTSIZE];// 取该地址的指针
    cur_recs++;
    return recp;
}


void utmp_close() {
    if(fd_utmp != -1) {// 表示成功打开了
        close(fd_utmp);
    }
}
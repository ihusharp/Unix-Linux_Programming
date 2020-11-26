/* statinfo.c - demonstrates using stat() to obtain
 *              file information.
 *            - some members are just numbers...
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

// printf info
void show_stat_info(char* filename, struct stat* buf);

int main(int argc, char** argv)
{
    struct stat info; // buf for return file information

    if (argc < 1) {
        perror(argv[1]);
    } else {
        // stat 返回到 第二个参数 info（即 buf）中
        if (stat(argv[1], &info) != -1) {
            show_stat_info(argv[1], &info);
            return 0;
        }
    }

    return 1;
}

void show_stat_info(char* filename, struct stat* buf)
{
    printf("   mode: %o\n", buf->st_mode); /* type + mode */
    printf("  links: %ld\n", buf->st_nlink); /* # links     */
    printf("   user: %d\n", buf->st_uid); /* user id     */
    printf("  group: %d\n", buf->st_gid); /* group id    */
    printf("   size: %ld\n", buf->st_size); /* file size   */
    printf("modtime: %ld\n", buf->st_mtime); /* modified    */
    printf("   name: %s\n", filename); /* filename    */
}

/*
husharp@hjh-Ubuntu:~/CS/Unix-Linux/Unix&Linux_Programming/ch03$ ./fileinfo fileinfo.c    mode: 100664
  links: 1
   user: 1000
  group: 1000
   size: 1461
modtime: 1606406362
   name: fileinfo.c


husharp@hjh-Ubuntu:~/CS/Unix-Linux/Unix&Linux_Programming/ch03$ ls -l fileinfo.c 
-rw-rw-r-- 1 husharp husharp 1461 11月 26 23:59 fileinfo.c
*/
// ls02.c
//    purpose  list contents of directory or directories
//    action   if no args, use .  else list files in args
//    note     uses stat and pwd.h and grp.h

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

void do_ls(char dirname[]);
void do_stat(char*);
void mod_to_letters(int, char*);
char* uid_to_name(uid_t);
char* gid_to_name(gid_t);
void show_stat_info(char* filename, struct stat* buf);
// ls
int main(int argc, char** argv)
{
    if (argc == 1) {
        do_ls(".");
    } else {
        while (--argc) {
            printf("%s:\n", *++argv);
            do_ls(*argv);
        }
    }
    return EXIT_SUCCESS;
}

// list files in directory called dirname
// 就是循环调用 readdir ，对 通过  opendir 打开 dirname 的 dir_ptr 不断进行读出
void do_ls(char dirname[])
{
    DIR* dir_ptr;
    struct dirent* dirent_pt; // each entry

    if ((dir_ptr = opendir(dirname)) == NULL) {
        fprintf(stderr, "ls2: cannot open %s\n", dirname);
    } else { // open sucessful
        while ((dirent_pt = readdir(dir_ptr)) != NULL) {
            do_stat(dirent_pt->d_name);
            // printf("%s\n", dirent_pt->d_name);
        }
        closedir(dir_ptr);
    }
    return;
}

void do_stat(char* filename)
{
    struct stat stat_buf;

    if (stat(filename, &stat_buf) == -1) {
        perror(filename);
    } else {
        show_stat_info(filename, &stat_buf);
    }
}

/*
总用量 16
-rw-r--r-- 1 husharp husharp 1182 9月  27 00:24 git需知
drwxr-xr-x 8 husharp husharp 4096 11月 26 10:46 Linux
*/
void show_stat_info(char* filename, struct stat* buf)
{
    char modeStr[11];
    mod_to_letters(buf->st_mode, modeStr); // mode change

    printf("%s ", modeStr);
    printf("%4d ", (int)buf->st_nlink);
    printf("%-8s ", uid_to_name(buf->st_uid)); // 靠左对齐
    printf("%-8s ", gid_to_name(buf->st_uid));
    printf("%8ld ", (long)buf->st_size); // long int
    // ctime   Thu Nov 26 23:27:56
    printf("%.12s ", ctime(&buf->st_mtim) + 4); // 星期
    // printf("%.12s ", 4 + ctime(&buf->st_mtim));// 星期

    printf("%s\n", filename);
}

// chansform mode to char array the file type
void mod_to_letters(int mode, char* str)
{
    strcpy(str, "----------"); // default
    // 3 bits for user
    if (S_ISDIR(mode)) // 对应 reg 文件就直接 default 为 - 了
        str[0] = 'd';
    if (S_ISCHR(mode))
        str[0] = 'c';
    if (S_ISBLK(mode))
        str[0] = 'b';
    // 3 bits for group
    if (mode & S_IRUSR)
        str[1] = 'r';
    if (mode & S_IWUSR)
        str[2] = 'w';
    if (mode & S_IXUSR)
        str[3] = 'x';
    // 3 bits for other
    if (mode & S_IRGRP)
        str[4] = 'r';
    if (mode & S_IWGRP)
        str[5] = 'w';
    if (mode & S_IXGRP)
        str[6] = 'x';
}

//  The passwd structure.
// struct passwd
// {
//   char *pw_name;		/* Username.  */
//   char *pw_passwd;		/* Password.  */
//   __uid_t pw_uid;		/* User ID.  */
//   __gid_t pw_gid;		/* Group ID.  */
//   char *pw_gecos;		/* Real name.  */
//   char *pw_dir;			/* Home directory.  */
//   char *pw_shell;		/* Shell program.  */
// };

#include <pwd.h>
// 通过 stat 返回的 uid 得到 uid name
// 为了健壮性， 进行扩展
char* uid_to_name(uid_t uid)
{
    struct passwd* pwd = getpwuid(uid); // 获取 struct

    static char str[11];
    if (pwd == NULL) {
        // sprintf()函数用于将格式化的数据写入字符串
        sprintf(str, "%d", uid);
        return str;
    } else {
        return pwd->pw_name;
    }
}

// 通过 stat 返回的 gid 得到 gid name
#include <grp.h>
char* gid_to_name(gid_t gid)
{
    struct group* grp = getgrgid(gid);
    static char str[10];

    if (grp == NULL) {
        sprintf(str, "%d", gid);
        return str;
    } else {
        return grp->gr_name;
    }
}

#include <utime.h>
utime()
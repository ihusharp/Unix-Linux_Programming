



ls 工作

![image-20201126233222678](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch03/think.assets/image-20201126233222678.png)

## 1、ls01

首先通过 

由于 ls 等同于 ls .  ，因此主函数列出如下

```
    if(argc == 1) {
        do_ls(".");
    } else {
        while(--argc) {
            printf("%s:\n", *++argv);
            do_ls(*argv);
        }
    }
```

就是循环调用 readdir ，对 通过  opendir 打开 dirname 的 dir_ptr 不断进行读出

### 目录项

```
struct dirent {
    ino_t          d_ino;       /* Inode number */
    off_t          d_off;       /* Not an offset; see below */
    unsigned short d_reclen;    /* Length of this record */
    unsigned char  d_type;      /* Type of file; not supported
                                    by all filesystem types */
    char           d_name[256]; /* Null-terminated filename */
};

```



## 2、ls 扩展研究

### ls -l

```
总用量 28
drwxr-xr-x 2 husharp husharp 4096 11月 24 15:08 ch01
drwxr-xr-x 2 husharp husharp 4096 11月 26 01:15 ch02
drwxrwxr-x 3 husharp husharp 4096 11月 26 23:36 ch03
drwxr-xr-x 3 husharp husharp 4096 11月 24 15:35 OS_By_Geek
-rw-r--r-- 1 husharp husharp   82 11月  8 00:20 README.md
drwxr-xr-x 3 husharp husharp 4096 11月 26 23:28 some_think
drwxrwxr-x 2 husharp husharp 4096 11月 10 08:15 test_c

```

![image-20201126234508975](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch03/think.assets/image-20201126234508975.png)





### stat 获取文件属性

stat (2)             - get file status , return information about a file, in the buffer pointed
       to by statbuf. 

```
int stat(const char *pathname, struct stat *statbuf);
```



```
struct stat {
    __dev_t st_dev;		/* Device.  */
    __ino_t st_ino;		/* File serial number.	*/
    __mode_t st_mode;			/* File mode.  */
    __nlink_t st_nlink;			/* Link count.  */
    __uid_t st_uid;		/* User ID of the file's owner.	*/
    __gid_t st_gid;		/* Group ID of the file's group.*/
    __off_t st_size;			/* Size of file, in bytes.  */
	struct timespec st_atim;		/* Time of last access.  */
    struct timespec st_mtim;		/* Time of last modification.  */
    struct timespec st_ctim;		/* Time of last status change.  */
}
```









![image-20201126233316127](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch03/think.assets/image-20201126233316127.png)
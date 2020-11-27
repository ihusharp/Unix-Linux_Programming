



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

实现完  fileinfo  之后，目前对比如下

```
husharp@hjh-Ubuntu:~/CS/Unix-Linux/Unix&Linux_Programming/ch03$ ./fileinfo fileinfo.c    mode: 100664
  links: 1
   user: 1000
  group: 1000
   size: 1461
modtime: 1606406362
   name: fileinfo.c


husharp@hjh-Ubuntu:~/CS/Unix-Linux/Unix&Linux_Programming/ch03$ ls -l fileinfo.c 
-rw-rw-r-- 1 husharp husharp 1461 11月 26 23:59 fileinfo.c
```

发现 links、size、name 没问题，ctime可以转换 modtime

### 现在需要做的是：

![image-20201126233316127](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch03/think.assets/image-20201126233316127.png)

mode 的转换 和 uid gid 的数字--> 用户名

100664 与 -rw-rw-r-- 的关系：

![image-20201127170133305](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch03/think.assets/image-20201127170133305.png)

st_mode 长 16 位，按照每三位进行 100664 = 001,000,000,110,110,100

因此 type 就是 1000，其余依次

在 sys/stats.h 中，发现是通过掩码的方式进行文件类型分类

```
/* File types.  */
#define	__S_IFDIR	0040000	/* Directory.  */
#define	__S_IFCHR	0020000	/* Character device.  */
#define	__S_IFBLK	0060000	/* Block device.  */
#define	__S_IFREG	0100000	/* Regular file.  */
#define	__S_IFIFO	0010000	/* FIFO.  */
#define	__S_IFLNK	0120000	/* Symbolic link.  */
#define	__S_IFSOCK	0140000	/* Socket.  */
```

因此上面的 100664 明显为 regular 

#### 1、mode 转换为 str

现在实现转换 mode

```c
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
```



#### 2、uid

联机查看 username

```
getlogin (3)         - get username
```

查看  more /etc/passwd

```
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
```

但是 /etc/passwd 并没有包含所有用户，而是通过 NIS 保存所有用户信息

man 3 getpwuid 可以访问用户信息得到 passwd 指针

```c
   The passwd structure is defined in <pwd.h> as follows:

       struct passwd {
           char   *pw_name;       /* username */
           char   *pw_passwd;     /* user password */
           uid_t   pw_uid;        /* user ID */
           gid_t   pw_gid;        /* group ID */
           char   *pw_gecos;      /* user information */
           char   *pw_dir;        /* home directory */
           char   *pw_shell;      /* shell program */
       };
```
因此可以通过该结构得到 name

```c
#include <pwd.h>
// 通过 stat 返回的 uid 得到 uid name
char *uid_to_name( uid_t uid) {
    return getpwuid(uid)->pw_name;
}
```



#### 3、gid

```
man 5 group
-------------------------------
group - user group file

DESCRIPTION
       The  /etc/group file is a text file that defines the groups on the system.  There is one entry per line, with the following format:
           group_name:password:GID:user_list
```

cat /etc/group

```
root:x:0:
daemon:x:1:
bin:x:2:
sys:x:3:
cdrom:x:24:husharp
plugdev:x:46:husharp
```

最后一个是组中的成员列表， 一个用户可以同时属于多个组

**getgrgid 函数可以得到组名，而不用操心实现的细节。**

```c
#include <grp.h>
char *gid_to_name( gid_t gid) {
    return getgrgid(gid)->gr_name;
}
```

综合可得具体函数见 ls02.c






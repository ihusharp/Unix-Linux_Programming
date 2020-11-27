



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



## 2、ls02

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

### 综合可得具体函数见 ls02.c

其中 uid_to_name 为了健壮性， 进行扩展：

因为可能 uid 为 NULL，此时需要返回输出 uid

gid_to_name 同理

```c
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

```



### 现在问题：

1、没有 按照文件名排序输出

2、没有显示记录总数

3、ls /tmp 显示错误，不能显示指定目录的信息





## 3、三个特殊的位

![image-20201127170133305](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch03/think.assets/image-20201127170133305.png)

suid、sgid、sticky

```c
#define	__S_ISUID	04000	/* Set user ID on execution.  */
#define	__S_ISGID	02000	/* Set group ID on execution.  */
#define	__S_ISVTX	01000	/* Save swapped text after use (sticky).  */
```

### suid、sgid：

#### eg 修改密码 passwd

passwd 是怎么修改——只有 root 权限才能修改文件 /etc/passwd 的呢？

```shell
husharp@hjh-Ubuntu:~/CS/Unix-Linux/Unix&Linux_Programming$ ls -l /etc/passwd
-rw-r--r-- 1 root root 2496 9月  21 22:01 /etc/passwd
husharp@hjh-Ubuntu:~/CS/Unix-Linux/Unix&Linux_Programming$ ls -l /usr/bin/passwd 
-rwsr-xr-x 1 root root 59640 3月  23  2019 /usr/bin/passwd
```

我们发现，位于 /etc/passwd 的文件 passwd 没有 s 位，只有 root 才能进行更改。而位于 /usr/bin/passwd  的 passwd 的命令具有 s 位，运行该程序（即 passwd）命令对 /etc/passwd 进行操作时，认为是由 /etc/passwd 文件所有者在运行该程序，因此能够修改 /etc/passwd 中的内容。

当然不能修改其他人的文件，因为 passwd 命令会通过 getuid 知道此时是谁在运行这个程序，只能修改该用户 ID 所对应的密码。

**sgid 与 suid 相似**

**sticky ：**

1、对于文件：用于早期的 swap 分区，告诉内核即使没有人在使用程序，也要放在 swap（就像 “粘”sticky 在 swap一样）。

2、对于目录：使目录里的文件只能被删除者删除，如 /tmp 谁都可以删除。



### 补充

现在说明为什么每个文件有 12 个属性，但是只显示 9 个字符?

![image-20201127204159990](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch03/think.assets/image-20201127204159990.png)





## 4、设置和修改文件属性

### 1、文件属性

创建文件时建立（比如 reg 采用 creat ），不能修改。

### 2、许可位与特殊属性位

1.建立：通过 fd= creat("newfile", 0744);  -> umask(0744)【调用内核函数】

2.修改：

```c
chmod("/tmp/file1", 04764);
chmod("/tmp/file1", S_ISUID | ...| S_IROTH);
```

3.Shell 进行修改：

Shell 命令 chmod 可以通过 八进制模式和 符号模式。

### 3、链接数

link 和 unlink（改变别名数量）

### 4、文件所有者与组

chown 修改文件所有者和组

```
chown("file1", 200, 40);	// 用户 ID 为 200， 组 ID 为 40
```

Shell 命令 chown 和 chrgp 

### 5、文件大小

占用存储空间的字节大小。

向文件增加内容时，会自动增加，没有直接减少文件占用空间的函数（除了 creat 直接置为 0）。

### 6、时间

最后修改时间，最后访问时间，属性最后修改时间。

utime函数修改最后修改时间，最后访问时间。

utime 函数采用以下这个存放两个时间的结构体

```c
struct utimbuf
  {
    __time_t actime;		/* Access time.  */
    __time_t modtime;		/* Modification time.  */
  };
```

Shell 可以通过 touch 进行修改。

### 7、文件名

creat 建立文件名

rename 修改文件名








































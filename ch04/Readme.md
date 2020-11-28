```
             +---- file content
             |
file system -+---- file attributes: stored in i-node. 
             |                      Every file has a i-node struct,
             |                      store file attributes, blocks the file uses, etc.
             |
             +---- directory: dir is a kind of file(yes it has a i-node). 
                              Its content is a list of (i-node, filename) entries.

              i-node table: The list of all i-node. 
+----------+     ↓----↓      CONTENT: real "files"
|disk block|:   □□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□
+----------+    ↑
           super block: store info of file system.
```

## 1、相关命令

mkdir, rmdir, rm, ln, mv, cd, chdir

### 1、ln

ln x xlink   硬链接 x->xlink

 

/ 的父目录指向自己，有个功能可以在实现 pwd 循环向上找 / 时， 若 .. 指向自己，那便说明到达顶端了。

目录中包含的是文件的引用，每个引用称为链接。



### 2、mkdir

```c
int mkdir(const char *pathname, mode_t mode);
```

mode 为权限掩码

mkdir 创建该目录的 inode 点，并赋名 pathname ，放在当前目录（通过  . 获取 inode 号）中增加目录项



### 3、rmdir

从目录树中删除一个目录节点

```c
int rmdir(const char *pathname);
```

该目录必须为空，即只有 . 和  ..， 删除也是通过 .. 获取其父目录，在其中删除该目录的链接。



### 4、rm

从一个目录文件中删除一个记录

```c
int unlink (const char * path)
```

unlink 不能用来删除目录，减少链接数





### 5、ln

创建一个文件的链接

```c
int link(const char *oldpath, const char *newpath);
```

 不能生成目录的链接



6、mv

改变文件和目录的名字或位置

```c
int rename(const char *oldpath, const char *newpath);
```

其实就是修改链接从一个目录到另一个目录

![image-20201128221131381](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch04/Readme.assets/image-20201128221131381.png)

即 对于 `rename("x", "z");`

```c
if(link("x", "z") != -1) {
	unlink("x");
}
```



### 6、cd

```c
int chdir(const char *path);
```

进程中有一个存放当前目录 inode 节点的变量，从一个目录进入另一个目录，只是改变那个变量的值。





## ferror 和 perror

**1.ferror**

- 函数原型： int ferror(FILE *fp)
- 功能：测试文件是否出现错误
- 返回值：未出错是0；出错为非0。

- 每次调用文件输入输出函数，均产生一个新的ferror函数值
- fopen打开文件时，ferror函数处置自动置为0
- 对同一个文件（文件指针或文件描述符）每一次调用输入输出函数，均产生一个新的ferror函数值，因此，应当在调用一个输入输出函数后立即检查ferror函数的值，否则信息会丢失。因此，应当在调用一个输入输出函数后立即检 查ferror函数的值，否则信息会丢失。
- 在执行fopen函数时，ferror函数的初始值自动置为0。

```
void main2()
{
    FILE *pf = fopen("C:\\51job.txt", "r");
    if (pf==NULL) {
        printf("\nopen  fail");
    }  else {
        printf("\nopen  sucess");
    }
    if (ferror(pf)==0) {//文件打开失败的情况，不让你继续，提示异常  
        printf("\n正常");
    } 
```

**2.perror()**

- 原函数：

　　　　void perror(const char *s);

- 功能：
- perror ( )用 来 将 上 一 个 函 数 发 生 错 误 的 原 因 输 出 到 标 准 设备 (stderr) 。参数 s  所指的字符串会先打印出,后面再加上错误原因字符串。此错误原因依照全局变量error  的值来决定要输出的字符串。在库函数中有个error变量，每个error值对应着以字符串表示的错误类型。当你调用"某些"函数出错时，该函数已经 重新设置了error的值。perror函数只是将你输入的一些信息和现在的error所对应的错误一起输出。
- eg:　

```
  #include <stdio.h>
  int main(void)
  {
FILE *fp ;
  fp = fopen( "/root/noexitfile", "r+" );
  if ( NULL == fp ) {
  perror("/root/noexitfile");
  }
  return 0;
  }
  运行结果：
  [root@localhost io]# gcc perror.c
  [root@localhost io]# ./a.out
  /root/noexitfile: No such file or directory
```

## 2、编写 pwd

调用 getcwd 查看

```c
int main()
{
	char wd[4096];
	getcwd(wd, 4096);
	printf("%s\n", wd);
	return 0;
}
```

**output：**      `/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch04`



### 现在开始实现

![image-20201128231333764](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch04/Readme.assets/image-20201128231333764.png)

递归调用获取当前所在目录的 inode ，通过递归到达最顶部再返回时输出 path 路径名

### 步骤如下

1、首先获取当前文件所在路径下 " . " 的inode（通过此 inode 向上找 其 " . "）

```c
// return inode num of the file
ino_t get_inode(char *fname) {
    struct stat info;

    if(stat(fname, &info) == -1) {
        perror(fname);
        fprintf(stderr, " Cannot stat ");
        exit(EXIT_FAILURE);
    }
    return info.st_ino;
}
```

2、得到当前文件所在路径的 " .. " 的 name（通过 遍历所在目录的所有目录项）

```c
// inode -> name
// copies its name into namebuf
void inum_to_name(ino_t inode, char *name_buf, int buf_len) {
    DIR *dir_ptr;
    struct dirent *direntp;

    if( (dir_ptr = opendir(".")) == NULL) {
        perror(".");
        exit(EXIT_FAILURE);
    }
    // 至此 成功打开
    // 开始读当前所在的目录中目录项， 找到相应的名字 
    while((direntp = readdir(dir_ptr)) != NULL) {
        if(direntp->d_ino == inode) {//说明找到
            strncpy(name_buf, direntp->d_name, buf_len);
            name_buf[buf_len - 1] = '\0';
            closedir(dir_ptr);// 关闭
            return;
        }
    }
    // 至此说明查找失败
    fprintf(stderr, "error looking for inum %d\n", inode);
    exit(EXIT_FAILURE);
}
```

3、递归调用 1、 进行打印name

```c
// 通过当前 inode 所在目录，得到 .. 的inode
// 并打印当前目录的 name
// 再进行递归调用
void printpathto(ino_t cur_inode) {
    

    char name[BUFSIZ];
    if(get_inode("..") != cur_inode) {// 递归终止条件
        chdir("..");// get parent path, 改为当前目录

        inum_to_name(cur_inode, name, BUFSIZ);

        printpathto(get_inode("."));// 递归调用
        // 从顶向下打印
        printf("/%s", name);
    }// 至此 说明终止, 结束即可。

}
```





代码实现后输出

```
/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch04
```

与 pwd 对比

```
/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch04
```

发现没有 home 



这是由于————这个文件系统的根，并非整棵树的根。



## 3、问题探究

### 1、mount 

![image-20201128232308016](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch04/Readme.assets/image-20201128232308016.png)

是由拼接将多棵树构成一棵树。

由 mount 查看可知，/dev/sda6   被装载到  /home 下

```shell
husharp@hjh-Ubuntu:~$ mount | grep home
/dev/sda6 on /home type ext4 (rw,relatime)
```

因此之前的程序，正是由于其根节点实则为 /home 所以便停止了。



### 2、多重 inode 节点号

更值得注意的是：

如下图，当进行不同文件系统连到同一棵树时， 会可能存在 相同 inode 号的文件。因此 link 拒绝跨设备的链接， rename 拒绝不同文件系统间 inode 的转移。

![image-20201128233149299](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/ch04/Readme.assets/image-20201128233149299.png)



### 3、ln -s 符号链接

符号链接创办不同节点号，但是内容相同

```shell
husharp@hjh-Ubuntu:~/test$ ln a.c b
husharp@hjh-Ubuntu:~/test$ ls
a.c    b
husharp@hjh-Ubuntu:~/test$ ln -s a.c c
husharp@hjh-Ubuntu:~/test$ ls
a.c    b  c
husharp@hjh-Ubuntu:~/test$ ls -ail
总用量 28
6291723 drwxr-xr-x  2 husharp husharp 4096 11月 28 23:39 .
6291457 drwxr-xr-x 47 husharp husharp 4096 11月 28 22:44 ..
6297197 -rw-r--r--  2 husharp husharp   61 9月  21 13:32 a.c						<<<<---------6297197 
6297197 -rw-r--r--  2 husharp husharp   61 9月  21 13:32 b						 <<<<---------6297197 
6328505 lrwxrwxrwx  1 husharp husharp    3 11月 28 23:39 c -> a.c  	  <<<<---------6328505
```



### diff 和 wc 比较内容和行数












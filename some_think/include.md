







系统调用

- 对于已经有的文件，可以使用open打开这个文件，close关闭这个文件；
- 对于没有的文件，可以使用creat创建文件；
- 打开文件以后，可以使用lseek跳到文件的某个位置；
- 可以对文件的内容进行读写，读的系统调用是read，写是write。

读或写操作时， 文件位置会自动推进。



## 中介与 Glibc

[gnu的glibc网址](http://www.gnu.org/software/libc/sources.html)

如果你做过开发，你会觉得刚才讲的和平时咱们调用的函数不太一样。这是因为，平时你并没有直接使用系统调用。虽然咱们的办事大厅已经很方便了，但是为了对用户更友好，我们还可以使用中介**Glibc**，有事情找它就行，它会转换成为系统调用，帮你调用。

Glibc 是 Linux 下使用的开源的标准 C 库，它是 GNU 发布的 libc 库。**Glibc 为程序员提供丰富的 API，除了例如字符串处理、数学运算等用户态服务之外，最重要的是封装了操作系统提供的系统服务，即系统调用的封装**。

每个特定的系统调用对应了至少一个 Glibc 封装的库函数，比如说，系统提供的打开文件系统调用 sys_open 对应的是 Glibc 中的 open 函数。

有时候，Glibc 一个单独的 API 可能调用多个系统调用，比如说，Glibc 提供的 printf 函数就会调用如 sys_open、sys_mmap、sys_write、sys_close 等等系统调用。

也有时候，多个 API 也可能只对应同一个系统调用，如 Glibc 下实现的 malloc、calloc、free 等函数用来分配和释放内存，都利用了内核的 sys_brk 的系统调用。

### syscalls.list

 在源码中，有个 syscalls.list ，里面列着所有 glibc 的函数对应的系统调用

```
# File name       Caller          Syscall name         Args        Strong name        Weak names

close		-	close		Ci:i	__libc_close	__close close
open		-	open		Ci:siv	__libc_open __open open
write		-	write		Ci:ibn	__libc_write	__write write
```

### make-syscall.sh

然后 glibc 还有一个脚本 make-syscall.sh，可以根据上面的配置文件，对于每一个封装好的系统调用，生成一个文件。

这个文件里面定义了一些宏，例如 #define SYSCALL_NAME open。

make-syscalls.sh中对应的代码为`echo '#define SYSCALL_NAME $syscall'`

```
  echo "\
	\$(make-target-directory)
	(echo '#define SYSCALL_NAME $syscall'; \\
	 echo '#define SYSCALL_NARGS $nargs'; \\
	 echo '#define SYSCALL_SYMBOL $strong'; \\
	 echo '#define SYSCALL_CANCELLABLE $cancellable'; \\
	 echo '#define SYSCALL_NOERRNO $noerrno'; \\
	 echo '#define SYSCALL_ERRVAL $errval'; \\
	 echo '#include <syscall-template.S>'; \\"
	 

/************************注释
   defining a few macros:
	SYSCALL_NAME		syscall name
	SYSCALL_NARGS		number of arguments this call takes
	SYSCALL_SYMBOL		primary symbol name
	SYSCALL_CANCELLABLE	1 if the call is a cancelation point
	SYSCALL_NOERRNO		1 to define a no-errno version (see below)
	SYSCALL_ERRVAL		1 to define an error-value version (see below)
**********************************/
```

### syscall-template.S

glibc 还有一个文件 syscall-template.S，使用上面这个宏，定义了这个系统调用的调用方式。

```
// PSEUDO是伪代码的意思
#define T_PSEUDO(SYMBOL, NAME, N)		PSEUDO (SYMBOL, NAME, N)

------------------------------------------------------
T_PSEUDO (SYSCALL_SYMBOL, SYSCALL_NAME, SYSCALL_NARGS)
	ret
T_PSEUDO_END (SYSCALL_SYMBOL)
--------------------------------------------------------
```

### 在 sysdep.h 中：

sysdeps/unix/sysv/linux/i386/sysdep.h

```
#define	PSEUDO(name, syscall_name, args)				      \
  .text;								      \
  ENTRY (name)								      \
    DO_CALL (syscall_name, args);					      \
    cmpl $-4095, %eax;							      \
    jae SYSCALL_ERROR_LABEL

```

里面对于任何一个系统调用，会调用 DO_CALL。这也是一个宏，这个宏 32 位和 64 位的定义是不一样的。

### 32位

sysdeps/unix/sysv/linux/i386/sysdep.h



```
/* Linux takes system call arguments in registers:

	syscall number	%eax	     call-clobbered
	arg 1		%ebx	     call-saved
	arg 2		%ecx	     call-clobbered
	arg 3		%edx	     call-clobbered
	arg 4		%esi	     call-saved
	arg 5		%edi	     call-saved
	arg 6		%ebp	     call-saved

   The stack layout upon entering the function is:

	24(%esp)	Arg# 6
	20(%esp)	Arg# 5
	16(%esp)	Arg# 4
	12(%esp)	Arg# 3
	 8(%esp)	Arg# 2
	 4(%esp)	Arg# 1
	  (%esp)	Return address
*/
#define DO_CALL(syscall_name, args)			      		      \
    PUSHARGS_##args							      \
    DOARGS_##args							      \
    movl $SYS_ify (syscall_name), %eax;					      \
    ENTER_KERNEL							      \
    POPARGS_##args
```

1. 将请求参数放在**寄存器**里面（PUSHARGS）
2. 根据**系统调用的名称**，得到**系统调用号**（SYS_ify (syscall_name)），***放在寄存器`%eax`里面***
3. 然后执行`ENTER_KERNEL`

```
# define ENTER_KERNEL int $0x80
```

int 就是 interrupt，也就是“中断”的意思。int $0x80 就是触发一个软中断，通过它就可以陷入（trap）内核。

> 中断分为
>
> - 外中断（按照是否导致宕机来划分）——必须是某个硬件导致
>   - 可屏蔽中断
>   - 不可屏蔽中断
> - 内中断（按中断是否正常来划分）
>   - 软中断——软件主动发起
>   - 异常

在内核启动的时候，还记得有一个 trap_init()，其中有这样的代码：

```
set_system_intr_gate(IA32_SYSCALL_VECTOR, entry_INT80_32);
```

这是一个软中断的陷入门。当接收到一个系统调用的时候，entry_INT80_32 就被调用了。

```
ENTRY(entry_INT80_32)
	ASM_CLAC
	pushl	%eax			/* pt_regs->orig_ax */
	SAVE_ALL pt_regs_ax=$-ENOSYS switch_stacks=1	/* save rest */
	movl	%esp, %eax
	call	do_int80_syscall_32
.Lsyscall_32_done:
...
.Lirq_return:
	INTERRUPT_RETURN
...
ENDPROC(entry_INT80_32)


#define INTERRUPT_RETURN		iret
```

entry_INT80_32 -> do_int80_syscall_32  -> do_syscall_32_irqs_on -> INTERRUPT_RETURN(iret)

```
/* Handles int $0x80 */
__visible void do_int80_syscall_32(struct pt_regs *regs)
{
	do_syscall_32_irqs_on(regs);
}
```

通过 push 和 SAVE_ALL 将当前用户态的寄存器，保存在 pt_regs 结构里面。

进入内核之前，保存所有的寄存器，然后调用 do_syscall_32_irqs_on。它的实现如下：

```
static __always_inline void do_syscall_32_irqs_on(struct pt_regs *regs)
{
	struct thread_info *ti = current_thread_info();
	unsigned int nr = (unsigned int)regs->orig_ax;
    ...
	if (likely(nr < IA32_NR_syscalls)) {
		regs->ax = ia32_sys_call_table[nr](
			(unsigned int)regs->bx, (unsigned int)regs->cx,
			(unsigned int)regs->dx, (unsigned int)regs->si,
			(unsigned int)regs->di, (unsigned int)regs->bp);
	}
	syscall_return_slowpath(regs);
}
```

1. 将**系统调用号**从寄存器`%eax`中取出，然后根据系统调用号，在**系统调用表**中找到相应的函数进行调用
2. 将寄存器中保存的参数取出来，作为函数参数
3. 根据宏定义，`#define ia32_sys_call_table sys_call_table`，系统调用就放在这个表里面

INTERRUPT_RETURN

```
// Linux源码
#define INTERRUPT_RETURN		iret
```

iret指令将原来**用户态**保存的现场恢复回来，包括代码段、指令指针寄存器等，此时用户态进程**恢复执行**



![image-20201126123614814](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/some_think/include.assets/image-20201126123614814.png)



### 系统调用表

系统调用表 sys_call_table 是怎么形成的呢？

32 位的系统调用表定义在面 arch/x86/entry/syscalls/syscall_32.tbl 文件里。例如 open 是这样定义的：

```
5	i386	open			sys_open			compat_sys_open
```

![image-20201126124456579](/home/husharp/CS/Unix-Linux/Unix&Linux_Programming/some_think/include.assets/image-20201126124456579.png)

1. 第1列的数字是**系统调用号**，32位和64位的系统调用号是不一样的
2. 第3列是**系统调用名称**
3. 第4列是系统调用在***内核中的实现函数***

系统调用在内核中的实现函数要有一个声明。声明往往在 include/linux/syscalls.h 文件中。例如 sys_open 是这样声明的：



### 1、fprintf

int fprintf(FILE * stream, const char * format, ...);

**函数说明：fprintf()会根据参数format 字符串来转换并格式化数据, 然后将结果输出到参数stream 指定的文件中, 直到出现字符串结束('\0')为止。**

与printf 的区别在于， fprintf 向第一个参数指定的流写入内容，而 printf 向 stdout 写入内容。

```c
fprintf(stderr, "Error: %s", err);
```


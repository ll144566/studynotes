# gcc常用参数功能说明。
以编译程序test.c为例

~~~markdown
-E: 预处理，主要是进行宏展开等步骤，生成的文件微test.i

gcc -E test.c

-S: 编译，生成汇编代码，生成的文件为test.S

gcc -S test.c

-c: 汇编：生成机器码，生成的文件未test.o

gcc -c test.c

(-o): 链接：生成可执行文件

gcc test.c (-o test)

gcc -D test.c：在编译的时候定义宏
gcc -I test.c：指定头文件的路径
gcc -g test.c：gdb调试的时候需要添加该参数
gcc -O test.c：编译优化，3个等级（1-3）
gcc -Wall test.c: 编译期间输出警告信息
~~~

[Linux位置无关代码实现,浅谈位置无关代码](https://blog.csdn.net/weixin_39521520/article/details/116880453)


静态库的制作和使用

~~~shell
# 制作
gcc -c *.c -I ../include/
ar rcs libMyCalc.a *.o  #打包

# 使用（已知 静态库 libMyCalc.a）
# 方式1
gcc main.c lib/libMyCalc.a -o sum -Iinclude

# 方式2
gcc main.c -I include -L lib -l MyCalc -o myapp
~~~

动态库的制作和使用

~~~shell
gcc -fPIC -c *.c -I ../include/
gcc -shared -o libMyCal.so *.o -I ../include/

# 第一种方式
gcc main.c lib/libMyCal.so -o app -I include

# 第二种方式
gcc main.c  -Iinclude -L./lib -lMyCal -o myapp

#ldd ./myapp 查看myapp依赖的库
~~~

使用（第二种方式生产的可执行文件）

`./myapp: error while loading shared libraries: libMyCal.so: cannot open shared object file: No such file or directory`

解决方案

1. `sudo cp lib/libMyCal.so /lib`不推荐；
2. `export LD_LIBRARY_PATH=./lib` ./lib指动态链接库的路径，仅对当前终端有效，可用于临时测试；
3. 在家目录的`.bashrc`文件中添加`export LD_LIBRARY_PATH=./lib`；
4. 修改动态链接器的配置文件
   - `sudo vim /etc/ld.so.conf`,
   - 添加动态链接库路径，如`/home/apricity/CLearn/Calc/lib`
   - `sudo ldconfig -v`（-v 提示信息，可不加）。



# gdb

[LINUX下GDB的使用方法](https://blog.csdn.net/awm_kar98/article/details/82840811)

~~~markdown
（gdb）help：查看命令帮助，具体命令查询在gdb中输入help + 命令,简写h

（gdb）run：重新开始运行文件（run-text：加载文本文件，run-bin：加载二进制文件）,简写r

（gdb）start：单步执行，运行程序，停在第一执行语句

（gdb）list：查看原代码（list-n,从第n行开始查看代码。list+ 函数名：查看具体函数）,简写l, (l 文件名:行号  查看其它文件） 

（gdb）set：设置变量的值

（gdb）next：单步调试（逐过程，函数直接执行）,简写n

（gdb）step：单步调试（逐语句：跳入自定义函数内部执行）,简写s

（gdb）frame：切换函数的栈帧,简写f

（gdb）info：查看函数内部局部变量的数值,简写i

（gdb）finish：结束当前函数，返回到函数调用点

（gdb）u：退出当前循环

（gdb）continue：继续运行,简写c

（gdb）print：打印值及地址,简写p

（gdb）quit：退出gdb,简写q

（gdb）break+num：在第num行设置断点,简写b

（gdb）info breakpoints：查看当前设置的所有断点

（gdb）delete breakpoints num：删除第num个断点,简写d

（gdb）display：追踪查看具体变量值

（gdb）undisplay：取消追踪观察变量

（gdb）watch：被设置观察点的变量发生修改时，打印显示

（gdb）i watch：显示观察点

（gdb）enable breakpoints：启用断点

（gdb）disable breakpoints：禁用断点

（gdb）x：查看内存x/20xw 显示20个单元，16进制，4字节每单元

（gdb）run argv[1] argv[2]：调试时命令行传参

（gdb）backtrace：查看函数的调用的栈帧和层级关系,简写bt

~~~

使用gdb调试的时候，gdb只能跟踪一个进程，可以在fork函数调用之前，通过指令设置gdb调试工具跟踪父进程或者是跟踪子进程。默认是跟踪父进程。

set follow-fork-mode child

set follow-fork-mode parent

**注意，一定要在fork函数调用之前设置才有效**

# makefile

= 和 := 的区别

~~~makefile
1、“=”

make会将整个makefile展开后，再决定变量的值。也就是说，变量的值将会是整个makefile中最后被指定的值。看例子：

x = foo
y = $(x) bar
x = xyz

在上例中，y的值将会是 xyz bar ，而不是 foo bar 。

2、“:=”

“:=”表示变量的值决定于它在makefile中的位置，而不是整个makefile展开后的最终值。

x := foo
y := $(x) bar
x := xyz

在上例中，y的值将会是 foo bar ，而不是 xyz bar 了。
~~~



makefile中的变量

~~~makefile

$< #规则中的第一个依赖, 如果依赖目标是多个, 逐个表示依赖目标
$@ #规则中的目标
$^ #规则中的所有依赖


CPPFLAGS #预处理器需要的选项 如：-I
CFAGS    #编译的时候使用的参数 -Wall -g -c
LDFLAGS -L -l
~~~

例：

~~~makefile
obj=main.o del.o add.o
target=app
CC=gcc
$(target):$(obj)
        $(CC) $(obj) -o $(target)

%.o:%.c
        $(CC) -c $< -o $@

~~~



makefile中的函数

~~~makefile
target=app
src=$(wildcard ./*.c)
obj=$(patsubst ./%.c,./%.o,$(src))
CC=gcc
$(target):$(obj)
        $(CC) $(obj) -o $(target)

%.o:%.c
        $(CC) -c $< -o $@

.PHONY:clean #声明clean为伪目标，不做更新比较
clean:
        rm $(obj) $(target) -f

~~~

权限问题

~~~makefile
target=app
src=$(wildcard ./*.c)
obj=$(patsubst ./%.c,./%.o,$(src))
CC=gcc
$(target):$(obj)
        $(CC) $(obj) -o $(target)

%.o:%.c
        $(CC) -c $< -o $@

.PHONY:clean
clean:
        -mkdir /new  # -执行失败，继续下一个命令
        rm $(obj) $(target) -f
~~~

简提Makefile中打印日志信息
打印用info，Makefile提供了三个命令

~~~makefile
$(warning "the value of LOCAL_PATH is$(FILE)")
$(info "the value of LOCAL_PATH is$(FILE)")
$(eror "the value of LOCAL_PATH is$(FILE)")
~~~



[Makefile中的调试打印方法](https://blog.csdn.net/guozhongwei1/article/details/88693221?utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-1.no_search_link&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-1.no_search_link)

[Makefile 使用总结](https://www.cnblogs.com/wang_yb/p/3990952.html)

cpu为什么要使用虚拟地址空间与物理地址空间映射？解决了什么样的问题？



1. 方便编译器与操作系统安排程序的地址分布。
   程序可以使用一系列相邻的虚拟地址来访问物理内存中不相邻的大内存缓冲区。
2. 方便进程之间隔离
   不同进程使用的虚拟地址彼此隔离，一个进程中的代码无法更改正在由另一进程使用的物理内存。
3. 方便OS使用内存
   程序可以使用一系列的虚拟地址来访问大于可用物理内存的内存缓冲区。当物理内存的供应量变小时，内存管理器会将物理内存页（通常大小为4KB）保存到磁盘文件。数据或代码页会根据需要在物理内存和磁盘之间移动







# Linux文件操作函数

1. stat函数

   穿透（追踪）函数 -- 软连接

2. lstat函数
   不穿透（追踪）函数 

~~~java

~~~





# 进程控制块

task_struct

- 进程id：系统中每个进程都有唯一的id，在C语言中用pid_t类型表示，其实就是一个非负整数。
- 进程的状态，有就绪、允许、挂起、停止等状态。
- 进程切换时需要保存和恢复的一下CPU寄存器。
- 描述虚拟地址空间的信息。
- 描述控制终端的信息。
- 当前工作目录。
- umask掩码。
- 文件描述符，包含很多指向fil结构体的指针。
- 和信号相关的信息。
- 用户id和组id。
- 会话和进程组。
- 进程可使用的资源上限。



# exec函数族语法
实际上，在Linux中并没有exec函数，而是有6个以exec开头的函数族，其中只有execve是真正意义上的系统调用，其它都是在此基础上经过包装的库函数。

下表列举了exec函数族的6个成员函数的语法。


~~~c
/*
	头文件： #include <unistd.h>
*/
int execl(const char *path, const char *arg, ...)
int execv(const char *path, char *const argv[])
int execle(const char *path, const char *arg, ..., char *const envp[])
int execve(const char *path, char *const argv[], char *const envp[])
int execlp(const char *file, const char *arg, ...)
int execvp(const char *file, char *const argv[])
~~~

exec函数族一般规律

exec函数一旦调用成功即执行新的程序，不返回。只有失败才返回，错误值-1。所以通常我们直接在exec函数调用后直接调用perror()和exit()，无需if判断。

exec函数族名字很相近，使用起来也很相近，它们的一般规律如下：

l (list)                           命令行参数列表

p (path)                       搜素file时使用path变量

v (vector)                    使用命令行参数数组

e (environment)       使用环境变量数组,不使用进程原有的环境变量，设置新加载程序运行的环境变量

**重点**

int execl(const char *path, const char *arg, ...); //执行自定义程序、执行系统命令

int execlp(const char *file, const char *arg, ...); //执行系统命令

# wait

# 1、wait()

pid_t wait(int *  status)：成功：返回清理掉的子进程的ID； 失败：返回-1（没有子进程);

当进程终止时，操作系统的隐式回收机制会：

1. 关闭所有文件描述符；
2. 释放用户空间分配的内存，内核的pcb仍存在。其中保存该进程的退出状态。

**正常退出 -> 退出值；异常退出 -> 终止信号**

可使用wait函数传出参数status来保持进程的退出状态，借助宏函数来进一步判断进程终止的具体原因。宏函数可分为以下三组：

1. WIFEXITED(status)为非0 -> 进程正常结束,
   WEXITSTATUS(status) 如上宏为真，使用此宏 -> 获取进程退出状态(exit或return的参数);
2. WIFSIGNED(status)为非0 -> 进程异常终止,
   WTERMSIG(status) 如上宏为真，使用此宏 -> 获取使进程终止的那个信号的编号;
3. WIFSTOPPED(status)为非0 -> 进程进入暂停状态,
   WSTOPSIG(status) 如上宏为真，使用此宏 ->  获取使进程暂停止的那个信号的编号;
   WIFCONTINUED(status)为真 -> 进程暂停后已继续运行。

# 2、waitpid()

作用同wait()，但可指定pid进程清理，可以不阻塞

pid_t waitpid(pid_t pid, int *  status, int options)；成功：返回清理掉的子进程；失败：-1（无子进程）

特殊参数（pid）和返回情况：

1. **> 0 回收指定ID的子进程；**
2. **-1 回收任意子进程（相当于wait）；**
3. 0 回收和当前waitpid一个组的所有子进程；
4. < -1 回收指定进程组内的任意子进程。

返回 0：参数3为WNOHANG，且子进程正在运行。



# 进程状态



R 正在运行或将要运行；
S 休眠可中断；
D 休眠不可中断；（通常 IO 的进程）；
T 停止或被追踪；
Z 僵尸状态；
X 死亡状态 ；





# 进程间通信

## 1、管道

### 1.1、匿名管道

#### 1.1.1、概念

[linux管道pipe详解](https://blog.csdn.net/oguro/article/details/53841949)

[Linux系统编程pipe()](https://blog.csdn.net/qq_35433716/article/details/86171936)

管道是一种最基本的IPC机制，作用于有血缘关系的进程之间，完成数据传递。调用pipe系统函数即可创建一个管道。有如下特质：

1. 其本质是一个伪文件(实为内核缓冲区)

2. 由两个文件描述符引用，一个表示读端，一个表示写端。

3. 规定数据从管道的写端流入管道，从读端流出。

管道的原理: 管道实为内核使用环形队列机制，借助内核缓冲区(4k)实现。

**管道的局限性：**

1. 数据自己读不能自己写。
2. 数据一旦被读走，便不在管道中存在，不可反复读取。
3. 由于管道采用半双工通信方式。因此，数据只能在一个方向上流动。
4. 只能在有公共祖先的进程间使用管道。

常见的通信方式有，单工通信、半双工通信、全双工通信。

**函数**

int pipe[int pipefd[2]]; 成功返回0； 失败返回 -1

函数调用成功返回r/w两个文件描述符。无open，但需手动close， 规定：fd[0] -> r; fd[1] -> w，就像0对于标准输入，1对应标准输出一样。向管道文件读写数据其实就是在读写内核区。



例如父进程写，子进程读

父进程关闭读文件描述符(为了不浪费FD并正确检测死亡读者)

子进程关闭写文件描述符(为了可以检测到EOF条件)



**管道的读写行为**

  使用管道需要注意以下4种特殊情况（假设都是阻塞I/O操作，没有设置O_NONBLOCK标志）：

1. 如果所有指向管道写端的文件描述符都关闭了（管道写端引用计数为0），而仍然有进程从管道的读端读数据，那么管道中剩余的数据都被读取后，再次read会返回0，就像读到文件末尾一样。

2. 如果有指向管道写端的文件描述符没关闭（管道写端引用计数大于0），而持有管道写端的进程也没有向管道中写数据，这时有进程从管道读端读数据，那么管道中剩余的数据都被读取后，再次read会阻塞，直到管道中有数据可读了才读取数据并返回。

3. 如果所有指向管道读端的文件描述符都关闭了（管道读端引用计数为0），这时有进程向管道的写端write，那么该进程会收到信号SIGPIPE，通常会导致进程异常终止。当然也可以对SIGPIPE信号实施捕捉，不终止进程。具体方法信号章节详细介绍。

4. 如果有指向管道读端的文件描述符没关闭（管道读端引用计数大于0），而持有管道读端的进程也没有从管道中读数据，这时有进程向管道写端写数据，那么在管道被写满时再次write会阻塞，直到管道中有空位置了才写入数据并返回。





wc

常见参数如下：

- -c 统计字节数。
- -l 统计行数。
- -m 统计字符数。这个标志不能与 -c 标志一起使用。
- -w 统计字数。注意，这里的字指的是由空格，换行符等分隔的字符串。



**文件类型**

"d" 目录directory
"-" 文件file
"l" 链接文件link file
"s" 通讯文件socket
"p" 管道文件
"b" 区块设备文件，文件里可供存储的接口设备block
"c" 字符设备文件，串行端口设备，如键盘、鼠标character

#### 1.1.2、示例

##### 1.1.2.1、父子进程

~~~c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

 
int main(int argc, char *argv[])
{
	int fd_pipe[2] = {0};
	pid_t pid;

	if( pipe(fd_pipe) < 0 ){

		perror("pipe");

	}

	pid = fork();

	if( pid < 0 ){
		perror("fork");
		exit(-1);
	}

	if( pid == 0 ){
		sleep(3);
		char buf[] = "hello, mike";
		write(fd_pipe[1], buf, strlen(buf)); 
		_exit(0);

	}else if( pid > 0){
		fcntl(fd_pipe[0], F_SETFL, O_NONBLOCK); 
		while(1){
			char str[50] = {0};
			read( fd_pipe[0], str, sizeof(str) );
			printf("str=[%s]\n", str);
			sleep(1);
		}
	}
	return 0;
}

~~~

##### 1.1.2.2、实现“ls | wc -l”命令

参数及含义

| 参数 | 含义                                                         |
| ---- | ------------------------------------------------------------ |
| -c   | print the byte counts统计字节数                              |
| -i   | print the newline counts：统计行数                           |
| -m   | print the character counts：将每个文件的字符数及文件名输出到屏幕上，如果当前系统不支持多字节字符其将显示与-c参数相同的结果 |
| -w   | print the word counts：统计字数                              |




~~~c
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void sys_err(const char* str)
{
    perror(str);
    exit(1);
}

int main(int argc,char* argv[])
{
    int ret;
    int fd[2];
    ret = pipe(fd); // 创建进程
    if(ret==-1)
    {
        sys_err("pipe error");
    }

    pid_t p = fork();
    if(p>0)
    {
        close(fd[0]);   //关闭读端
        // 将输出到屏幕的文件描述符，重定向到管道写端
        dup2(fd[1],STDOUT_FILENO);
        // 默认ls命令是，写到屏幕，由于上面的重定向，现在写道管道了
        execlp("ls","ls",NULL);
        // 上面成功，这条不会执行，这里我们只能先依赖系统回收，后面使用信号回收
        close(fd[1]);
    }
    else if(p==0)
    {
        close(fd[1]);   //关闭写端
        // 将输入文件描述符，重定向到读端
        dup2(fd[0],STDIN_FILENO);
        // wc默认是读取屏幕内容，现在从管道的读端读取
        execlp("wc","wc","-l",NULL);
        
        // 上面成功，这条不会执行，这里我们只能先依赖系统回收，后面使用信号回收
        close(fd[0]);
    }

    return 0;
}

~~~



##### 1.1.2.3、兄弟进程

~~~c

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define BUFSIZE PIPE_BUF

void err_quit(char* msg)
{
	perror( msg);
	exit(1);
}

int main()
{
	int fd[2];
	char buf[BUFSIZE] = "hello child";
	pid_t pid;
	if(pipe(fd) < 0)
	{
		err_quit("fork fail");
	}
	write(fd[1],buf,sizeof(buf));
	if((pid = fork()) < 0)
	{
		err_quit("fork fail");
	}

	if(pid == 0)
	{
		close(fd[0]);
		char *buff="hello child\n";
		write(fd[1], buff, strlen(buff));
		sleep(1);
		exit(1);
	}

	if((pid = fork()) < 0)
	{
		err_quit("fork fail");
	}

	else if(pid == 0)
	{
		close(fd[1]);
		char buf[1024] = {0};
		int ret = read(fd[0], buf, sizeof(buf));
		write(STDOUT_FILENO, buf, ret);
		printf("recv message %s\n", buf);
		exit(0);
	}
	close(fd[0]);
	close(fd[1]);
	wait(NULL);
	wait(NULL);
	return 0;
}


~~~



### 1.2、有名管道

#### 1.2.1、概念

[Linux进程间通信（一）之无名管道（PIPE）和有名管道（FIFO  代码参考）](https://blog.csdn.net/yishizuofei/article/details/78301414)



[以下参考自](https://blog.csdn.net/firefoxbug/article/details/8137762)

无名管道应用的一个重大限制是它没有名字，因此，只能用于具有亲缘关系的进程间通信，有名管道又称为FIFO，是进程间通信的一种方式。

在有名管道（named pipe或FIFO）提出后，该限制得到了克服。FIFO不同于管道之处在于它提供一个路径名与之关联，以FIFO的文件形式存在于文件系统中。这样，即使与FIFO的创建进程不存在亲缘关系的进程，只要可以访问该路径，就能够彼此通过FIFO相互通信（能够访问该路径的进程以及FIFO的创建进程之间），因此，通过FIFO不相关的进程也能交换数据。值得注意的是，FIFO严格遵循先进先出（first in first out），对管道及FIFO的读总是从开始处返回数据，对它们的写则把数据添加到末尾。它们不支持诸如lseek()等文件定位操作。
管道的缓冲区是有限的（管道制存在于内存中，在管道创建时，为缓冲区分配一个页面大小）
管道所传送的是无格式字节流，这就要求管道的读出方和写入方必须事先约定好数据的格式，比如多少字节算作一个消息（或命令、或记录）等等
**FIFO的打开规则：**

1. 如果当前打开操作是为读而打开FIFO时，若已经有相应进程为写而打开该FIFO，则当前打开操作将成功返回；否则，可能阻塞直到有相应进程为写而打开该FIFO（当前打开操作设置了阻塞标志）；或者，成功返回（当前打开操作没有设置阻塞标志）。
2. 如果当前打开操作是为写而打开FIFO时，如果已经有相应进程为读而打开该FIFO，则当前打开操作将成功返回；否则，可能阻塞直到有相应进程为读而打开该FIFO（当前打开操作设置了阻塞标志）；或者，返回ENXIO错误（当前打开操作没有设置阻塞标志）。

**总之就是一句话，一旦设置了阻塞标志，调用mkfifo建立好之后，那么管道的两端读写必须分别打开，有任何一方未打开，则在调用open的时候就阻塞。**

[以上参考自](https://blog.csdn.net/firefoxbug/article/details/8137762)

创建有名管道的函数有两个：mknod和mkfifo

int mkfifo(const char *pathname, mode_t mode);

int mknod(const char *pathname, mode_t mode, dev_t dev);

有名管道的总体操作：

~~~markdown
创建管道mkfifo
打开管道open
读管道read
写管道write
关闭管道close
删除管道unlink
~~~

#### 1.2.2、示例

~~~c
read
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
	const char*FIFO = "/tmp/fifo";
	if(access(FIFO, F_OK) != 0)
	{
		if(mkfifo(FIFO, 0777) < 0)
		{
			perror("mkfifo error");
			exit(1);
		}
	}

	int fd = open(FIFO, O_RDONLY);
	assert(fd != -1);

	char buf[128] = {0};
	int n = 0;
	while((n = read(fd, buf, 127)) != 0)
	{
		printf("revc message: %s\n", buf);
		memset(buf, 0, 128);
	}
	close(fd);
	unlink(FIFO);//删除文件
	return 0;
}

//write
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

int main()
{
	const char*FIFO = "/tmp/fifo";
	printf("read to open\n");
	int fd = open (FIFO, O_WRONLY);
	printf("open success\n");

	assert(fd != -1);

	char buf[128] = {0};
	while(1)
	{
		printf("input: ");
		fgets(buf, sizeof(buf), stdin);

		write(fd, buf, strlen(buf));

		if(strncmp(buf, "bye", 3) == 0)
		{
			break;
		}
	}
	close(fd);
	return 0;
}

~~~









#include<unistd.h>

int access(const char* pathname, int mode);

参数介绍：

    pathname 是文件的路径名+文件名
    
    mode：指定access的作用，取值如下

F_OK 值为0，判断文件是否存在

X_OK 值为1，判断对文件是可执行权限

W_OK 值为2，判断对文件是否有写权限

R_OK 值为4，判断对文件是否有读权限



## 2、mmap

### 2.1、

```c
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, off_t offset);
int munmap(void *addr, size_t length);
```

返回值：成功返回创建的映射区首地址；失败返回 MAP_FAILED宏。

参数

- addr：建立映射区的首地址，由Linux内核指定。使用时，直接传递NULL。

- length：欲创建映射区的大小。

- prot：映射区权限PROT_READ、PROT_WRITE、PROT_READ|PROT_WRITE；

- flags：标志位参数（常用于设定更新物理区域。设置共享、创建匿名映射区）
  MAP_SHARED:会将映射区所做的操作反映到物理设备上；

  MAP_PRIVATE：映射区所做的修改不会反应到物理设备。

- fd：用来建立映射区的文件描述符。

- offset：映射文件的偏移（必须是4K的整数倍）。

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

int var = 100;
int main()
{
	int *p;
	pid_t pid;

	int fd;
	fd = open("tmp", O_RDWR | O_CREAT | O_TRUNC, 0644);

	if(fd < 0)
	{
		perror("open error");
		exit(1);
	}

	unlink("tmp");
	ftruncate(fd, 4);

	p = (int*)mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(p == MAP_FAILED)
	{
		perror("mmap failed");
		exit(1);
	}

	close(fd);

	pid = fork();

	if(pid == 0)
	{
		*p = 2000;
		var = 1000;
		printf("child, *p = %d, var = %d\n", *p, var);
	}
	else
	{
		sleep(1);
		printf("parent, *p = %d, var = %d\n", *p, var);

		wait(NULL);

		int ret = munmap(p, 4);
		if(ret == -1)
		{
			perror("munmap error");
			exit(1);
		}
	}
	return 0;
}

~~~



**思考**

1. 可以open的时候O_CREAT一个新的文件创建映射区吗？
2. 如果对mmap的返回值执行++操作，munmap可否成功？
3. 如果open时是O_RDONLY,mmap时prot参数指定PROT_READ|PROT_WRITE会怎样？
4. 如果文件偏移量为1000会怎样？
5. 如果不检测mmap的返回值会怎样？
6. 对mmap返回值越界操作会怎样？
7. 文件描述符先关闭，对mmap映射有没有影响？



**总结**

1. 创建映射区的过程，隐含着一次对映射文件的读操作。
2. 当MAP_SHARED时，要求映射区的权限应 <= 文件打开的权限（出于对映射区的保护）。而MAP_SHARED则无所谓，因为mmap中的权限是对内存的限制。
3. 映射区的释放与文件关闭无关。只有映射建立成功，文件可以立即关闭。
4. 特别注意，当映射文件大小为0时，不能建立映射区。所以映射的文件必须要有实际大小！mmap使用时常常会出现总线错误，通常是由于共享为念存储空间大小引起的。
5. munmap传入的地址一定是mmap的返回地址。坚决杜绝指针++操作。
6. 文件偏移量必须为4K的整数倍。
7. mmap创建的映射区出错率非常高，一定要检查返回值，确保映射区建立成功后再进行后续操作。



mmap父子进程通信

父子灯有血缘关系的进程之间也可以通过mmap建立的映射区来完成数据通信，但相应的要在创建映射区的时候指定对应的标志位参数flags：

- MAP_PRIVATE（私有映射）父子进程各自独占映射区。
- MAP_SHARED（共享内存）父子进程共享映射区

***结论：***父子进程共享：1. 打开的文件；2. mmap建立的映射区（但必须要使用MAP_SHARED）。

### 2.2、匿名映射区

#### 2.2.1、概念

 匿名映射区的建立，不需要打开一个文件就可以进行映射，并用来进行父子进程间通信。

   Linux系统中，有一个宏定义： MAP_ANONYNOUS或者MAP_ANON可以创建匿名映射，而不需要使用文件。

mmap(NULL,4,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,-1,0);

#### 2.2.2、示例

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

int var = 100;
int main()
{
	int *p;
	pid_t pid;


	p = (int*)mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
	if(p == MAP_FAILED)
	{
		perror("mmap failed");
		exit(1);
	}

	pid = fork();

	if(pid == 0)
	{
		*p = 2000;
		var = 1000;
		printf("child, *p = %d, var = %d\n", *p, var);
	}
	else
	{
		sleep(1);
		printf("parent, *p = %d, var = %d\n", *p, var);

		wait(NULL);

		int ret = munmap(p, 4);
		if(ret == -1)
		{
			perror("munmap error");
			exit(1);
		}
	}
	return 0;
}
[a
~~~



但是以上两个宏定义仅仅适用于linux中。在UNIX及其他类UNIX系统中是不存在这两个宏定义的。

那么匿名映射的创建就需要使用其他方法：

int fd = open("/dev/zero",O_RDWR);

char *p = (char*)mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

close(fd);

pid_t pid =fork();

.......

.......

munmap(p,size);

### 2.3、非血缘关系进程通信

#### 2.3.1、示例

~~~c
//write

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

struct STU {
	int id;
	char name[20];
	char sex;
};

void sys_err(char *str)
{
	perror(str);
	exit(-1);
};

int main(int argc, char *argv[])
{
	int fd;
	struct STU student = {0, "小明", 'm'};
	struct STU *mm;
	if(argc < 2)
	{
		printf("./* file_shared\n");
		exit(-1);
	}

	fd = open(argv[1], O_RDWR | O_CREAT);
	ftruncate(fd, sizeof(student));
	mm = mmap(NULL, sizeof(student), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(mm == MAP_FAILED)
	{
		sys_err("mmap error");
	}
	close(fd);

	while(student.id < 20)
	{
		memcpy(mm, &student, sizeof(student));
		student.id++;
		sleep(1);
	}
	munmap(mm, sizeof(student));
	return 0;
}

//read

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

struct STU {
	int id;
	char name[20];
	char sex;
};

void sys_err(char *str)
{
	perror(str);
	exit(-1);
}

int main(int argc, char *argv[])
{
	int fd;
	struct STU student;
	struct STU *mm;
	if(argc < 2)
	{
		printf("./* file_shared\n");
		exit(-1);
	}

	fd = open(argv[1], O_RDONLY);
	mm = mmap(NULL, sizeof(student), PROT_READ, MAP_SHARED, fd, 0);
	unlink(argv[1]);	
	ftruncate(fd, sizeof(student));
	if(mm == MAP_FAILED)
	{
		sys_err("mmap error");
	}
	close(fd);

	while(1)
	{
		printf("id = %d\tname = %s\tsex = %c\n", mm->id, mm->name, mm->sex);
		sleep(2);
	}
	munmap(mm, sizeof(student));
	return 0;
}

~~~



# 信号

## 1、概念

**产生信号：**

1. 按键产生，如调用Ctrk+c、Ctrl+z、Ctrl+\
2. 系统调用产生，如kill、raise、abort
3. 软件条件产生，如定时器alarm
4. 硬件异常产生，如非法访问内存（段错误）、除0（浮点数例外）、内存对齐错误（总线错误）
5. 命令产生，如kill命令

**递达：** 递送并且到达

**未决：**产生和递达之间的状态，主要由于阻塞（屏蔽）导致该状态。

**信号的处理方式：**

1. 默认执行方式
2. 忽略（丢弃）
3. 捕捉（调用户处理函数）

<font color = "red">注意：9）SIGKILL和19）SIGSTOP，不允许忽略和捕捉，只能执行默认。甚至不能阻塞</font>

Linux内核的进程控制块PCB是一个结构体，task_struct，除了包含进程id，状态，工作目录，用户id，组id，文件描述符，还包含了信号相关的信息，主要指阻塞信号集和未决信号集。

**阻塞信号集（信号屏蔽字）**：将某些信号加入集合，对他们设置屏蔽，当屏蔽x信号后，再收到该信号将推后（解除屏蔽线后）

**未决信号集：**

1. 信号产生，未决信号集中描述该信号的位立刻翻转为1，表信号处于未决状态。当信号处理对应位翻转回为0.这一刻往往非常短暂。
2. 信号产生后由于某些原因（主要是阻塞）不能抵达。这类信号的集合称之为未决信号集。在屏蔽解除前，信号一直处于未决状态。

**信号4要素**

1. 编号；
2. 名称；
3. 事件；
4. 默认处理动作。

 **Signal dispositions**

参考man 7 signal帮助文档

​       Each signal has a current disposition, which determines how the process behaves when it is delivered the signal.

```markdown
   The entries in the "Action" column of the tables below specify the default disposition for each signal, as follows:

   Term   Default action is to terminate the process.

   Ign    Default action is to ignore the signal.

   Core   Default action is to terminate the process and dump core (see core(5)).

   Stop   Default action is to stop the process.

   Cont   Default action is to continue the process if it is currently stopped.
```

<font color = "red">注意：9）SIGKILL和19）SIGSTOP，不允许忽略和捕捉，只能执行默认。甚至不能阻塞</font>

**快捷键产生的信号**

ctrl-c 发送 SIGINT 信号给前台进程组中的所有进程。常用于终止正在运行的程序。
ctrl-z 发送 SIGTSTP 信号给前台进程组中的所有进程，常用于挂起一个进程。
ctrl-d 不是发送信号，而是表示一个特殊的二进制值，表示 EOF。
ctrl-\ 发送 SIGQUIT 信号给前台进程组中的所有进程，终止前台进程并生成 core 文件。

kill -l查看消息类型

## 2、kill函数（发送信号）

### 2.1.函数介绍
功能描述：
用于向任何进程组或进程发送信号。

头文件用法：

~~~c
#include <sys/types.h>

#include <signal.h>

int kill(pid_t pid, int sig);
~~~





**参数：**
pid：可能选择有以下四种

1. pid > 0：pid是信号欲送往的进程的标识。
2. pid = 0：信号将送往所有与调用kill()的那个进程属同一个使用组的进程。
3. pid = -1：信号将发送给进程有权限发送的系统中的所有进程，无权限进程：进程1(init)。
4. pid < -1：信号将送往以-pid为组标识的进程。

sig：准备发送的信号代码，假如其值为零则没有任何信号送出，但是系统会执行错误检查，通常会利用sig值为零来检验某个进程是否仍在执行。

**返回值说明：** 成功执行时，返回0。失败返回-1，errno被设为以下的某个值 EINVAL：指定的信号码无效（参数 sig 不合法） EPERM；权限不够无法传送信号给指定进程 ESRCH：参数 pid 所指定的进程或进程组不存在



查看进程组ps -ejH

### 2.1.2、示例

~~~c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define N 5

#if 0
int main()
{
	int ret = kill(getpid(), SIGKILL);
	if(ret = -1);
	{
		printf("kill failed\n");
		exit(1);
	}
	while(1);
	return 0;
}
#endif

int main()
{
	pid_t pid, pid_killed;
	int i;
	for(i = 0; i < 3; i++)
	{
		pid = fork();
		if(pid == 0)
		{
			break;
		}
		if(i == 1)
		{
			pid_killed = pid;
		}
	}
	if(i < 3)
	{
		while(1)
		{
			sleep(1);
			printf("I'm %d child\n",i);
		}
	}else
	{
		sleep(2);
		int ret = kill(pid_killed, SIGKILL);
		if(ret == -1)
		{
			printf("kill failed!\n");
		}
	}
	return 0;
}

~~~

## 3、raise和abort函数（发送信号）

### 3.1、函数介绍

man 2 raise|man 2 abort

**raise函数：**

#include <signal.h>

raise - send a signal to the caller

给当前进程发送指定的信号（自己给自己发）raise(signo) == kill(getpid(), signo)

int raise(int sig); 成功0 ，失败非0值

**abort函数：**

#include <stdlib.h>

abort - cause abnormal process termination

给自己发送异常终止信号6) SIGABRT信号，终止并产生core文件。

void abort(void)；该函数无返回值和参数

### 3.2、示例

~~~c
#include <signal.h>
#include <stdlib.h>

int main()
{
	//abort();
	raise(SIGQUIT);
	return 0;
}

~~~

## 4、软条件产生信号（发送信号）

### 4.1、alarm函数

#### 4.1.1、函数介绍

  alarm - set an alarm clock for delivery of a signal

    alarm也称为闹钟函数，alarm()用来设置信号SIGALRM在经过参数seconds指定的秒数后传送给目前的进程。如果参数seconds为0，则之前设置的闹钟会被取消，并将剩下的时间返回。要注意的是，一个进程只能有一个闹钟时间，如果在调用alarm之前已设置过闹钟时间，则任何以前的闹钟时间都被新值所代替。

<font color = "red">每个进程都有且只有唯一一个定时器</font>

~~~c
#include<unistd.h>

unsigned int alarm（unsigned int seconds)
~~~

**函数参数**
　　seconds:指定秒数

**函数返回值**
　　成功：如果调用此alarm（）前，进程已经设置了闹钟时间，则返回上一个闹钟时间的剩余时间，否则返回0。
　　出错：-1

**取消定时器**

alarm(0)，返回旧闹钟余下秒数



定时与进程状态无关（自然定时法）！就绪、运行、挂起（阻塞、暂停）、终止、僵尸... 无论进程处于何种状态，alarm都计时



<font color = "red">time ./alarm 获取程序执行时间</font>

实际执行时间 = 系统时间 + 用户时间 + 等待时间 

打印 IO操作耗时

#### 4.1.2、示例

~~~c
#include <stdio.h>
#include <unistd.h>

int main()
{
	int i = 0;
	alarm(1);
	while(1)
	{
		printf("count = %d\n", ++i);
	}
	return 0;
}

~~~

### 4.2、settitimer函数

#### 4.2.1、函数介绍

getitimer, setitimer - get or set value of an interval timer

[以下参考](https://blog.csdn.net/wanxuexiang/article/details/86657190)

设置定时器（闹钟）。可替代alarm函数。精度微妙us，可以实现周期定时。

一、setitimer()函数的使用
通过man手册可以查看详细的用法 见文末

~~~c
//头文件
#include <sys/time.h>
//函数原型
int getitimer(int which, struct itimerval *curr_value);
int setitimer(int which, const struct itimerval *new_value,
                     struct itimerval *old_value);
~~~


函数使用
1. which
which参数表示类型，可选的值有：

~~~markdown
1. ITIMER_REAL：以系统真实的时间来计算，它送出SIGALRM信号。（计算自然时间）
2. ITIMER_VIRTUAL：以该进程在用户态下花费的时间来计算，它送出SIGVTALRM信号。（只计算进程占用CPU时间）
3. ITIMER_PROF：以该进程在用户态下和内核态下所费的时间来计算，它送出SIGPROF信号（计算占用cpu及执行系统调用的时间）
~~~

（2）时间结构体

~~~c
struct itimerval 
{
	/* Interval for periodic timer */
	struct timeval it_interval;
	/* Time until next expiration */ 
	struct timeval it_value;    
};
struct timeval 
{
	time_t      tv_sec;         /* seconds */
	suseconds_t tv_usec;        /* microseconds 1/1000000 seconds */
};
~~~

new_value： 参数用来对计时器进行设置，it_interval为计时间隔，it_value为延时时长，表示的是在setitimer方法调用成功后，延时it_value后触发一次SIGALRM信号，以后每隔it_interval触发一次SIGALRM信号。

settimer工作机制是，先对it_value倒计时，当it_value为零时触发信号，然后重置为it_interval，继续对it_value倒计时，一直这样循环下去。

基于此机制，setitimer既可以用来延时执行，也可定时执行。

假如it_value为0是不会触发信号的，所以要能触发信号，it_value得大于0；如果it_interval为零，只会延时，不会定时（也就是说只会触发一次信号)。
old_value： 参数，通常用不上，设置为NULL，它是用来存储上一次setitimer调用时设置的new_value值。

[以上参考](https://blog.csdn.net/wanxuexiang/article/details/86657190)

#### 4.2.2、示例

~~~c
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int my_alarm(unsigned int sec)
{
	struct itimerval it, oldit;
	int ret;
	it.it_value.tv_sec = sec;
	it.it_value.tv_usec = 0;
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 0;

	ret = setitimer(ITIMER_REAL, &it, &oldit);
	if(ret == -1)
	{
		perror("setitimer");
		exit(1);
	}
	return oldit.it_value.tv_sec;
}

int main()
{
	int i;
	my_alarm(1);
	for(i = 0;; i++)
	{
		printf("count = %d\n", i);
	}
	
	return 0;
}

~~~

## 5、信号的捕捉

### 5.1、函数介绍

 signal - ANSI C signal handling

设置某一信号的对应动作

```cpp
#include <signal.h>
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);

#define SIG_ERR (void (*)())-1  //(void (*)())-1：表示将-1强制转换成void (*)()类型的函数指针
#define SIG_DEL (void (*)())0
#define SIG_IGN (void (*)())1

```

**参数说明**　

第一个参数signum：指明了所要处理的信号类型，它可以取除了SIGKILL和SIGSTOP外的任何一种信号。 　 
第二个参数handler：描述了与信号关联的动作，它可以取以下三种值： SIG_IGN、SIG_DFL、sighandler_t类型的函数指针 

```markdown
   signal() sets the disposition of the signal signum to handler, which is either SIG_IGN, SIG_DFL, or the address of a programmer-defined function (a "signal handler").

   If the signal signum is delivered to the process, then one of the following happens:

   *  If the disposition is set to SIG_IGN, then the signal is ignored.

   *  If the disposition is set to SIG_DFL, then the default action associated with the signal (see signal(7)) occurs.这个符号表示恢复对信号的系统默认处理。不写此处理函数默认也是执行系统默认操作。 

   *  If the disposition is set to a function, then first either the disposition is reset to SIG_DFL, or the signal is blocked (see Portability below), and then handler is called
      with argument signum.  If invocation of the handler caused the signal to be blocked, then the signal is unblocked upon return from the handler.

   The signals SIGKILL and SIGSTOP cannot be caught or ignored.
```

### 5.2、示例

~~~c
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

unsigned int my_alarm(unsigned int sec, unsigned interv_sec)
{
	struct itimerval it, oldit;
	int ret;
	it.it_value.tv_sec = sec;
	it.it_value.tv_usec = 0;
	it.it_interval.tv_sec = interv_sec;
	it.it_interval.tv_usec = 0;

	ret = setitimer(ITIMER_REAL, &it, &oldit);
	if(ret == -1)
	{
		perror("setitimer");
		exit(1);
	}
	return oldit.it_value.tv_sec;
}

void myfunc(int sigmno)
{
	printf("hello world \n");
}

int main()
{
	int i;
	my_alarm(5, 3);
	signal(SIGALRM, myfunc);
	while(1);
	return 0;

}

~~~

man文档中typedef void (*sighandler_t)(int); 但实际使用过程中会报错。

~~~markdown
*.c:11:2: error: unknown type name ‘sighandler_t’
  sighandler_t handler;
~~~

解决方案

1. 在使用的函数中重定义typedef void (*sighandler_t)(int);
2. 实际定义的或与man文档不同，可以尝试使用sig_t

~~~c
#include <signal.h>
#include <stdio.h>

//typedef void (*sighandler_t)(int);
void catchsignal(int signo)
{
	printf("%d----------has been catched!\n", signo);
}

void main()
{
//	sighandler_t handler;
	sig_t handler;
	handler = signal(SIGINT, catchsignal);

	if(handler == SIG_ERR)
	{
		perror("siganl error");
	}
	while(1);
}

~~~



## 6、信号集操作函数

内核通过读取未决信号集来判断信号是否应被处理。信号屏蔽字mask可以影响未决信号集。而我们可以在应用程序中自定义set来改变mask。已达到屏蔽指定信号的目的。

### 6.1、信号集设定

sigset_t set; //typedef unsigned long sigset;

```c
#include <signal.h>

int sigemptyset(sigset_t *set);                          // 将某个信号集清0             成功：0；失败：-1

int sigfillset(sigset_t *set);                           // 将某个信号集置1             成功：0；失败：-1

int sigaddset(sigset_t *set, int signum);                // 将某个信号加入信号集         成功：0；失败：-1

int sigdelset(sigset_t *set, int signum);                // 将某个信号清出信号集         成功：0；失败：-1

int sigismember(const sigset_t *set, int signum);        // 判断某个信号是否在信号集中    成功：0；失败：-1
```

sigset_t类型的本质是位图。但不应该直接使用位操作，而应该使用上述函数，保证跨系统操作有效。

对比认知select函数





### 6.2、sigprocmask函数

用来屏蔽信号、解除信号也使用该函数。其本质，读取或修改进程的信号屏蔽字（PCB中）

严格注意，屏蔽信号：只是将信号延后执行（延至解除屏蔽）；而忽略表示将信号丢弃处理。

返回值：成功0；失败-1，设置errmo

~~~c
#include <signal.h>

int sigprocmask(ubt how,const sigset_t*set,sigset_t *oldset);     
~~~

**参数：**

1. set：传入参数，是一个位图，set中哪设置1，就表示当前进程屏蔽哪个信号。
2. oldset：传出参数，保存旧的信号屏蔽字为mask。
3. how参数取值：假设当前的信号屏蔽字为mask
   - SIG_BLOCK：当how设置此值，set表示需要屏蔽的信号。相当于mask = mask|set。
   - SIG_UNBLOCK：当how设置此值，set表示需要解除屏蔽的信号。相当于mask = mask&~set。
   - SIG_SETMASK：当how设置此值，set表示用与替代原始屏蔽集的新屏蔽集，相当于mask = set；若调用sigprocmask解除了对当前若干个信号的阻塞，则在sigprocmask返回前，至少将其中一个信号递达。

### 6.3、sigpending

读取当前进程的未决信号集

~~~c
#include <signal.h>     

int sigpending(const sigset_t*sigmask);
~~~

返回值：成功0；失败-1，设置errmo

#### 6.3.1、示例

**打印未决信号集**

~~~c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void print(sigset_t *ped)
{
	int i;
	for(i = 1; i < 32; i++)
	{
		if(sigismember(ped, i) == 1)
		{
			putchar('1');
		}
		else
		{
			putchar('0');
		}
	}
	printf("\n");
}

int main()
{
	sigset_t myset, oldset, ped;
	sigemptyset(&myset);
	sigaddset(&myset, SIGQUIT); //屏蔽信号3
	sigaddset(&myset, SIGINT);
    sigaddset(&myset, SIGTSTP);
    sigaddset(&myset, SIGSGEV);
    sigaddset(&myset, SIGTSTP);
	int ret = sigprocmask(SIG_BLOCK, &myset, &oldset);
	if(ret == -1)
	{
		perror("sigpromask error\n");
	}
    while(1)
	{
		sleep(1);
		sigpending(&ped);
		print(&ped);
	}
}

~~~



### 6.4、sigaction（信号的捕获2）

#### 6.4.1、概念
 signal 函数的使用方法简单，但并不属于 POSIX 标准，在各类 UNIX 平台上的实现不尽相同，因此其用途受

到了一定的限制。而 POSIX 标准定义的信号处理接口是 sigaction 函数，其接口头文件及原型如下：

```c
 #include <signal.h>
 int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

 struct sigaction
 {
    void     (*sa_handler)(int);
    void     (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t  sa_mask;
    int       sa_flags;
    void     (*sa_restorer)(void);//该参数废弃
 };
```

**参数**

1. signum：要操作的信号。
2. act：要设置的对信号的新处理方式。
3. oldact：原来对信号的处理方式。
4. 返回值：0 表示成功，-1 表示有错误发生。

**sigaction结构体**

sa_restorer：该元素是过时的，不应该使用，POSIX.1标准将不指定该元素。（弃用）

sa_sigaction：当sa_flags被指定为SA_SIGINFO标志时，使用该信号处理程序（很少使用）

**重点掌握**

1. sa_handler：指定信号捕捉后的处理函数名（即注册函数）。也可赋值为SIG_IGN表忽略或SIG_DFL表执行默认动作。
2. sa_mask：调用信号处理函数时，所需要屏蔽的信号集合（信号屏蔽字）。注意：仅在处理函数被调用期间屏蔽生效，是临时性装置。
3. sa_flags：通常设置为0，表示使用默认属性。

#### 6.4.2、示例

~~~c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void docatch(int signo)
{
	sleep(2);
	printf("%d signal is catched\n", signo);
}

int main()
{
	struct sigaction act;
	act.sa_handler = docatch;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGQUIT);
	act.sa_flags = 0; //默认属性 信号捕捉函数执行期间，自动屏蔽本信号
	int ret = sigaction(SIGINT, &act, NULL);
	if(ret < 0)
	{
		perror("sigaction error");
		exit(1);
	}
	while(1);
	return 0;
}

~~~



## 6.5、信号捕捉特性

1. 进程正常运行时，默认PCB中有一个信号屏蔽字，假定为☆，它决定了自动屏蔽哪些信号。当注册了某个信号捕捉函数，捕捉到该信号后，要调用该函数，而该函数可能执行很长时间，在这期间所屏蔽的信号不由☆决定。而是由sa_mask来指定。调用完信号处理函数，再恢复为☆。
2. xxx信号捕捉函数在执行期间，xxx信号自动被屏蔽。
3. 阻塞的常规信号不支持排队，产生多次只记录一次。（后32个实时信号支持排队）。

## 6.6、内核实现信号捕捉过程

![image][内核实现信号捕捉过程]

<font color = "red">signal、signal不能完成捕捉信号的任务，只是用来注册信号捕捉函数，真正实施捕捉信号的是内核</font>



# 静态条件（时序静态）

## 1、pause函数

### 1.1、概念

 pause - wait for signal

调用该函数可以造成进程主动挂起，等待信号唤醒。调用该系统调用的进程将处于阻塞状态（主动放弃CPU）直到有信号递达将其唤醒。

```c
#include <unistd.h>
int pause(void);
//pause() returns only when a signal was caught and the signal-catching function returned.  In this case, pause() returns -1, and errno is set to EINTR.
```

返回值：-1 并设置errno为EINTR

返回值：

1. 如果信号的默认处理动作是终止进程，则进程终止，pause函数没有机会返回。
2. 如果信号的默认处理动作是忽略，进程继续处于挂起状态，pause函数不反悔。
3. 如果信号的处理动作是捕捉，则调用完信号处理函数后，pause返回-1。
4. pause收到的信号不能被屏蔽，如果屏蔽，那么pause就不能被唤醒。

### 1.2、示例

**用信号机制实现sleep**

~~~c
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>

void catch_signal(int signo)
{
}
unsigned int mysleep(unsigned int seconds)
{
	struct sigaction act, oldact;
	act.sa_handler = catch_signal;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	int ret = sigaction(SIGALRM, &act, &oldact);
	if(ret == -1)
	{
		perror("sigaction error");
		exit(1);
	}
	alarm(seconds);
	ret = pause();
	if(ret == -1 && errno == EINTR)
	{
		printf("pause success\n");
	}
	ret = alarm(0);  //取消闹钟 （由于其它原因被提前唤醒时，取消闹钟）
	sigaction(SIGALRM, &oldact, NULL); //当前处理方式是调用catch_signal，现在恢复SIGALRM信号以前的处理动作 
	return ret;
}



int main()
{
	while(1)
	{
		mysleep(3);
		printf("------------------\n");
	}
	return 0;
}

~~~

## 2、时序竟态

**设想如下 场景**

欲睡觉，定闹钟10分钟，希望10分钟后闹铃将自己唤醒。

- 正常：定时，睡觉，10分钟后被闹钟唤醒。
- 异常：闹钟定好后，被唤走，外出劳动，20分钟后劳动结束。回来继续睡觉计划，但劳动期间闹钟已经响过，不会再将自己唤醒。

### 2.1、时序问题分析

回顾，借助pause和alarm实现的mysleep函数。设想如下时序：

1. 注册SIGALRM信号处理函数（sigaction）。
2. 调用alarm(1)函数设定闹钟1秒。
3. 函数调用刚结束，开机倒计时1秒。当前进程失去CPU，内核调度优先级高的进程（有多个）取代当前进程。当前进程无法获得CPU，进入就绪态等待CPU。
4. 1秒后，闹钟超时，向内核发起SIGALRM信号（自然定时法，与进程状态无关），高优先级进程尚未执行完，当前进程仍处于就绪态，信号无法处理（未决）。
5. 优先级高的进程执行完，当前进程获得CPU资源，内核调度回到当前进程执行。SIGALRM信号递达，信号设置捕捉，执行处理函数sig_alarm。
6. 信号处理函数执行结束，返回当前进程主控进程，pause()被挂起等待。（欲等待alarm函数发送的SIGALARM信号将自己唤醒）。
7. SIGALARM信号已经处理完毕，pause不会等到。



### 2.2、解决时序问题

可以通过设置屏蔽SIGALARM的方法来控制程序执行逻辑，但无论如何设置，程序都有可能再“解除信号屏蔽”与“挂起等待信号”这两个操作间隙失去CPU资源。除非将这两步骤合并成一个“原子操作”。sigsuspend函数湖北这个功能。在对时许要求严格的场合下都应该使用sigsuspend替换pause。

~~~c
//NAME
//       sigsuspend, rt_sigsuspend - wait for a signal

//SYNOPSIS
       #include <signal.h>
       int sigsuspend(const sigset_t *mask);
~~~

sigsuspend函数使用期间，进程信号屏蔽字由其参数mask指定。

可将某个信号（如SIGALARM）从临时信号屏蔽字mask中删除，这样在调用sigsuspend时将解除对 该信号的屏蔽，然后挂起等待，当sigsuspend返回时，进程的信号屏蔽字恢复为原来的值。如果原来对该信号是屏蔽态，sigsuspend返回后仍然屏蔽盖该信号。

### 2.3、示例

改进版mysleep

~~~c
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>

void sig_alarm(int signo)
{
	printf("catch signed\n");
}
unsigned int mysleep(unsigned int seconds)
{
	struct sigaction newact, oldact;
	sigset_t newmask, oldmask, suspmask;
	unsigned int unslept;

	//1.为SIGALRM设置捕捉函数，一个空函数
	newact.sa_handler = sig_alarm;
	sigemptyset(&newact.sa_mask);
	newact.sa_flags = 0;
	int ret = sigaction(SIGALRM, &newact, &oldact);
	if(ret == -1)
	{
		perror("sigaction error");
		exit(1);
	}

	//2.设置阻塞信号集，阻塞SIGALRM信号
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGALRM);
	sigprocmask(SIG_BLOCK, &newmask, &oldmask);

	//定时n秒
	alarm(seconds);

	//4.构造一个调用sigsuspend临时有效的阻塞信号集，在临时阻塞信号集里解除SIGALRM的阻塞
	suspmask = oldmask;
	sigdelset(&suspmask, SIGALRM);

	//5.sigsuspeng调用期间，采用临时阻塞信号集suspmask替换原阻塞信号集，这个信号中不包含SIGALRM信号，同时挂起等待，当这个sigsuspend被信号唤醒时，恢复原有的阻塞信号集

	sigsuspend(&suspmask);

	ret = alarm(0);
	//6.恢复SIGARM原有的处理动作，呼应前面注释1
	sigaction(SIGALRM, &oldact, NULL);

	//7.解除对SIGARM的阻塞，呼应前面注释2
	sigprocmask(SIG_SETMASK, &oldmask, NULL);
	return ret;
}



int main()
{
	while(1)
	{
		mysleep(3);
		printf("------------------\n");
	}
	return 0;
}

~~~

### 2.4、总结

竟态条件跟系统负载有很紧密的关系，体现出信号的不可靠。系统负载越严重，信号不可靠性越强。

不可靠由其实现原理导致。信号是通过软件方式实现（跟内核调度高度依赖，延时性强），每次系统调用结束后，或中断处理结束后，需通过扫描PCB中的未决信号集，来判断是否应该处理某个信号。当系统负载过重时，会出现时序混录。

这种意外情况只能在编写程过程中提前预见，主动规避，而无法通过gdb程序调试等其它手段弥补。且由于该错误不具有规律性，后期捕捉和重新十分困难。











# 报错



Bus error(coredump)



stray ‘\274’ in program

原因：在程序中打入了全角字符，也就是不是英文输入法





# 头文件

使用open前需要先包含头文件

\#include <sys/types.h>

\#include <sys/stat.h>

\#include <fcntl.h>





#include <errno.h>

头文件errno.h定义了一个全局的宏errn





一、头文件
#include<unistd.h>

二、函数原型
int unlink(const char *pathname);

三、函数介绍
unlink()函数功能即为删除文件。执行unlink()函数会删除所给参数指定的文件。

注意：

        执行unlink()函数并不一定会真正的删除文件，它先会检查文件系统中此文件的连接数是否为1，如果不是1说明此文件还有其他链接对象，因此只对此文件的连接数进行减1操作。若连接数为1，并且在此时没有任何进程打开该文件，此内容才会真正地被删除掉。在有进程打开此文件的情况下，则暂时不会删除，直到所有打开该文件的进程都结束时文件就会被删除。

返回值：成功返回0，失败返回 -1

[fcntl函数的用法总结](https://blog.csdn.net/fengxinlinux/article/details/51980837)





























# base64图片(不要动)

[内核实现信号捕捉过程]:data:image/png;base64,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


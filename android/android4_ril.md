#   1. RILC运行机制

## 1.1 RILC启动过程

### 1.1.1. 寻找rilc的加载入口

Android手机在开机过程中，Linux Kernal会运行rild可执行文件加载和启动LibRIL；可找到system/core/rootdir/init.rc，Linux系统在启动过程中，回家载此配置文件中所配置的系统服务

~~~makefile
# @init.rc
# path system/core/rootdir/init.rc
service ril-daemon /system/bin/rild
    class main
    socket rild stream 660 root radio
    socket rild-debug stream 660 radio system
    user root
    group radio cache inet misc audio sdcard_rw log
~~~



通过上面的配置信息，发现Android Linux内核加载过程中，会执行rild可执行文件启动ril-daemon的系统服务，可总结出以下三点

- Android手机启动过程中会同时启动基于rild的后台服务，如果此服务发生异常退出，系统会进行重新加载，可以说是RILC运行的守护进程。

- 建立了两个socket连接，其端口号分别是rild和rild-debug；ril比较熟悉，我们在学习RILJ建立的socket连接时，其端口号就是rild；而rild-debug用作测试，在Android手机发布时，可将此端口屏蔽。

- 基于安全考虑，启动ril-daemon系统Service服务的用户为radio，进入Android虚拟设备的控制台可查看其进程详情。

  

### 1.1.2. 解析RILC的加载方法

前面已将找到了RILC的加载入口：运行rild可执行文件，启动ril-daemon系统的Service服务。找到hardware/ril/rild/rild.c程序文件中的main函数，其中的处理逻辑最关键的是

~~~c++
//@rild.c
//path hardware/ril/rild/rild.c

int main(int argc, char **argv)
{
    //第一步调用ril.cpp中的RIL_startEventLoop函数，LibRIL开始循环监听Socket事件
    RIL_startEventLoop();

    //通过reference-ril.so动态链接库，获取指向RIL_init函数的指针rilInit
    rilInit = (const RIL_RadioFunctions *(*)(const struct RIL_Env *, int, char **))dlsym(dlHandle, "RIL_Init");

    if (rilInit == NULL) {
        fprintf(stderr, "RIL_Init not defined or exported in %s\n", rilLibPath);
        exit(-1);
    }

    //第二步，调用reference-ril.so动态链接库的RIL_init函数，传递s_rilEnv给reference-ril.so，并返回funcs
    funcs = rilInit(&s_rilEnv, argc, rilArgv);
    //第三步，调用libril.so的RIL_register函数，将funcs传递和libril.so
    RIL_register(funcs);

}
~~~

通过上面的代码，rild.c的main方法加载RILC，其处理逻辑可分为三大步，而每一步都会对应一个关键的函数调用，这些步骤及函数分别是

1. 调用RIL_startEventLoop函数，LibRIL开启循环监听Socket连接，即可开始接收RILJ发起的Socket连接请求和RIL Solicited消息请求。
2. 调用RIL_Init函数，这里非常关键，首先，通过dlsym方法获取指向reference-ril.so动态链接库中RIL_Init函数的指针；其次，调用reference-ril.so动态链接库中reference-ril.so动态链接库中的RIL_Init函数，完成RIL Stub的初始化，即reference-ril.so动态链接库。重点关注RIL_Init函数调用的参数s_rilEnv以及返回参数funcs，s_rilEnv即Runtime，funcs即Functions。
3. 调用RIL_register函数，即reference-ril.so动态链接库，提供其Functions给LibRIL。

> **注意** rild.c中main函数负责启动RILC，其中最关键的就是将LibRIL与Reference-RIL之间建立起一种互相调用的能力，LibRIL中有指向Reference-RIL中的funcs结构体的指针，而References-RIL中有指向LibRIL的s_rilEnv结构体的指针，这种关系一旦建立，LibRIL与Reference-RIL之间便会抛开RILD，两者直接进行对方提供的函数调用，从而完成RIL消息的交互。
>
> ​	另外，funcs是通过调用Reference-RIL中的RIL_Init函数获取的，而s_rilEnv函数指针在什么地方获取呢？RILC启动过程中的3个关键函数中都没有完成其初始化的操作；原来，s_rilEnv的初始化在rild.c代码中的静态代码块中完成。其处理详情且看后面的章节。

## 1.2 RILC运行过程

​	通过前面的学习可知，RILC启动完成后，LibRIL具有Reference-RIL返回的指向RIL-RadioFunctions类型的funcs指针，而Reference-RIL具有LibRIL的RIL-Env类型的s_rilEnv结构体的指针不论是Solicited消息还是Unsolicited消息，根据其消息的流向，可分为两种不同的消息。

- 下行消息：LibRIL接收到RILJ发起的Solicited消息后，LibRIL使用funcs调用Reference-RIL的onRequset、onStateRequest等方法。
- 上行消息：Modem中相关通信状态发生变换或者执行完Solicited请求消息后，Reference-RIL通过s_rilEnv结构体指针调用LibRIL的OnRequestComplete、OnUnsolicitedRequestComplete等方法完成上行消息的发送。

> 上行消息和下行消息的划分，与Unsolicited和Solicited消息的划分不同，其参考点是：RILJ处于上层，RILC处于下层。这两种消息的对应关系是：下行消息可对应Solicited Request消息，而上行消息可对应与Solicited Response和Unsolicited Response这两个消息。

# 2. 初识RILC中的运行环境LibRIL

## 2.1 LibRIL主要文件的作用

​	LibRIL代码路径为hardware/ril/libril，此目录下共有5个代码文件以及一个Android编译配置文件，其代码详情如下。

- ril.cpp：作为LibRIL运行环境的核心代码，负责建立RUntime运行环境的框架。
- ril_commands.h和ril_unsolcommands.h：ril_commands.h头文件定义了LibRIL接收到RILJ发出的Solicited请求消息所对应的调用函数和返回函数；而ril_unsolcommands.h定义了Unsolicited消息饭后调用的函数。
- ril_event.h和ril_event.cpp：ril_event事件的结构体定义以及事件基于ril_event双向链表的操作函数。

  LibRIL中以ril.cpp代码为核心，其它代码协助它完成LibRIL运行环境的启动和运行，LibRIL运行环境有两大作用。

- 与RILJ基于Socket的交互；
- 与Reference-RIL基于函数调用的交互。

## 2.2 重点结构体说明

RILD加载和启动LibRIL运行环境的过程中，涉及两个非常关键的指向struct结构体指针：s_rilEnv和funcs，它们分别对应于RIL_Env和RIL_RadioFunctions结构体。

### 2.2.1. RIL_Env结构体

~~~c++
//@ril.h
//path hardware/ril/include/ril.h
struct RIL_Env {
    /**
     * "t" is parameter passed in on previous call to RIL_Notification
     * routine.
     *
     * If "e" != SUCCESS, then response can be null/is ignored
     *
     * "response" is owned by caller, and should not be modified or
     * freed by callee
     *
     * RIL_onRequestComplete will return as soon as possible
     */
    void (*OnRequestComplete)(RIL_Token t, RIL_Errno e,
                           void *response, size_t responselen);

    /**
     * "unsolResponse" is one of RIL_UNSOL_RESPONSE_*
     * "data" is pointer to data defined for that RIL_UNSOL_RESPONSE_*
     *
     * "data" is owned by caller, and should not be modified or freed by callee
     */

    void (*OnUnsolicitedResponse)(int unsolResponse, const void *data,
                                    size_t datalen);

    /**
     * Call user-specifed "callback" function on on the same thread that
     * RIL_RequestFunc is called. If "relativeTime" is specified, then it specifies
     * a relative time value at which the callback is invoked. If relativeTime is
     * NULL or points to a 0-filled structure, the callback will be invoked as
     * soon as possible
     */

    void (*RequestTimedCallback) (RIL_TimedCallback callback,
                                   void *param, const struct timeval *relativeTime);
};

~~~

前面有个疑问，在RILD中启动LibRIL的过程中，s_rilEnv如何初始化？答案就是在rild.c中的静态方法中完成初始化。

~~~c++
//@rild.c
//path hardware/ril/rild/rild.c
static struct RIL_Env s_rilEnv = {
    RIL_onRequestComplete,
    RIL_onUnsolicitedResponse,
    RIL_requestTimedCallback
};
~~~

那么是如何实现的呢？答案就是在rild.c代码中用extern修饰了libril.so动态链接库中的相关函数声明。

~~~c++
//@rild.c
//path hardware/ril/rild/rild.c
extern void RIL_onRequestComplete(RIL_Token t, RIL_Errno e,
                           void *response, size_t responselen);

extern void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
                                size_t datalen);

extern void RIL_requestTimedCallback (RIL_TimedCallback callback,
                               void *param, const struct timeval *relativeTime);

~~~

rild.c代码中的main函数会调用ril.cpp中的RIL_startEventLoop和RIL_register这两个函数，在rild.c代码中,同样可找到extern修饰的函数声明。

接着，在ril.cpp代码文件中，可找到这5个方法的具体逻辑实现。在RILD需要完成动态加载libril.so动态链接库中的函数，还需要在编译过程中加入相关的编译参数，在hardware/ril/rild/Android.mk编译配置文件中定义它们之间的依赖关系，详情如下：

~~~makefile
# @Android.mk
# path hardware/ril/rild/Android.mk
LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libril
~~~

因此，rild.c编译出来的可执行文件rild在运行过程中，可正常调用和访问LibRIL编译出来的libril.so动态链接库中的函数。

### 2.2.2. RIL_RadioFunctions结构体

其代码详情如下

~~~c++
//@ril.h
//path hardware/ril/include/ril.h
typedef struct {
    int version;        /* set to RIL_VERSION */
    RIL_RequestFunc onRequest;
    RIL_RadioStateRequest onStateRequest;
    RIL_Supports supports;
    RIL_Cancel onCancel;
    RIL_GetVersion getVersion;
} RIL_RadioFunctions;
~~~

RIL_RadioFunctions结构体中定义了一个int类型字段version，用来标志Reference-RIL版本号；LibRIL在接收到RILJ发起的Solicited请求消息后，其它5个指向函数的指针会调用Reference-RIL提供的funcs中对应请求函数，我们需要重点关注onRequest和getVersion这两个指针函数。

## 2.3 LibRIL运行环境加载过程

LibRIL运行环境的加载过程，体现在rild.c代码的main函数中RIL_startEventLoop和RIL_register两个函数的调用，共同完成LibRIL运行环境的初始化工作。

- RIL_startEventLoop函数开启ril_event事件监听。
- RIL_register函数引入三方RIL_RadioFunctions。

### 2.3.1 开启ril_event事件监听入口RIL_startEventLoop

RILC在启动过程中，首先调用LibRIL的RIL_startEventLoop函数完成LibRIL运行环境的准备，然后开始循环监听socket相关RIL事件

**1. RIL_startEventLoop**

~~~c++
//@ril.cpp
//path hardware/ril/libril/ril.cpp
extern "C" void
RIL_startEventLoop(void) {
    int ret;
    pthread_attr_t attr;

    /* spin up eventLoop thread and wait for it to get started */
    s_started = 0;//启动标准
    pthread_mutex_lock(&s_startupMutex);//怎加pthread的同步锁

    pthread_attr_init (&attr);//初始化pthread参数
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&s_tid_dispatch, &attr, eventLoop, NULL);

    while (s_started == 0) {
        pthread_cond_wait(&s_startupCond, &s_startupMutex);
    }

    pthread_mutex_unlock(&s_startupMutex);

    if (ret < 0) {
        LOGE("Failed to create dispatch thread errno:%d", errno);
        return;
    }
}
~~~

RIL_startEventLoop函数主要逻辑是：创建基于eventLoop函数调用的子线程，这里会使用到pthread。Linux系统下的多线程遵循POSIX线程接口，称为pthread。

**2. eventLoop函数**

进入ril.cpp代码中的eventLoop函数

~~~c++
//@ril.cpp
//path hardware/ril/libril/ril.cpp
static void *
eventLoop(void *param) {
    int ret;
    int filedes[2];

    ril_event_init();//初始化ril_event双向链表

    pthread_mutex_lock(&s_startupMutex);//增加pthread同步锁

    s_started = 1;//修改启动状态为1
    pthread_cond_broadcast(&s_startupCond);//发出s_startupCond通知

    pthread_mutex_unlock(&s_startupMutex);//修改pthread同步锁

    ret = pipe(filedes);//创建管道通信

    if (ret < 0) {
        LOGE("Error in pipe() errno:%d", errno);
        return NULL;
    }

    s_fdWakeupRead = filedes[0];//输入的文件描述符
    s_fdWakeupWrite = filedes[1];//输出的文件描述符

    fcntl(s_fdWakeupRead, F_SETFL, O_NONBLOCK);

    ril_event_set (&s_wakeupfd_event, s_fdWakeupRead, true,
                processWakeupCallback, NULL);//设置并创建新的ril_event参数，关注s_wakeupfd_event文件描述符以及RIL事件回调方法processWakeupCallback

    rilEventAddWakeup (&s_wakeupfd_event);

    // Only returns on error
    ril_event_loop();//增加ril_event节点并激活
    LOGE ("error in event_loop_base errno:%d", errno);
    // kill self to restart on error
    kill(0, SIGKILL);

    return NULL;
}
~~~

eventLoop函数处理逻辑主要分为以下几个部分：

- 修改s_started启动状态，取值为1，并发出修改通知，而此通知会在ril.cpp代码中的RIL_startEventLoop函数中响应。
- 创建并激活s_wakeupfd_event的事件处理，此事件的发送和接收实现的方式基于pipe管道通信。
- 调用ril_event.cpp代码中的ril_event_loop函数，循环接收和处理ril_event事件，重点分析s_wakeupfd_event的事件处理和代码中的ril_event_loop函数。

3.s_wakeupfd_event事件的处理

s_wakeupfd_event事件处理主要分为三大块，详情如下：

- 创建管道获取其输入和输出文件描述符s_fdWakeupRead 、s_fdWakeupWrite。
- 使用s_fdWakeupRead和processWakeupCallback创建s_wakeupfd_event事件。
- 增加并激活s_wakeupfd_event事件。

ril_event双向链表此时仅有一个节点，那就是s_wakeupfd_event，此节点的fd文件描述符为s_fdWakeupRead ，RIL事件的回调函数为：processWakeupCallback。

**4. ril_event_loop函数**

ril_event.cpp代码中的ril_event_loop函数中，处理逻辑的核心是for(;;)，只要循环中的处理不发生变化，ril_event_loop函数调用是不会返回的。

至此，rild.c代码中的main函数，调用ril.cpp代码中的RIL_startEventLoop函数初始化了LibRIL运行环境，它主要完成以下工作：

- 启动子线程，调用ril_event_loop函数，进入for循环监听ril_event事件。
- 完成s_wakeupfd_event事件的节点初始化和激活

~~~c++
//@ril_event.cpp
//path hardware/ril/libril/ril_event.cpp
void ril_event_loop()
{
    int n;
    fd_set rfds;
    struct timeval tv;
    struct timeval * ptv;


    for (;;) {

        // make local copy of read fd_set
        memcpy(&rfds, &readFds, sizeof(fd_set));
        if (-1 == calcNextTimeout(&tv)) {
            // no pending timers; block indefinitely
            dlog("~~~~ no timers; blocking indefinitely ~~~~");
            ptv = NULL;
        } else {
            dlog("~~~~ blocking for %ds + %dus ~~~~", (int)tv.tv_sec, (int)tv.tv_usec);
            ptv = &tv;
        }
        printReadies(&rfds);
        n = select(nfds, &rfds, NULL, NULL, ptv);
        printReadies(&rfds);
        dlog("~~~~ %d events fired ~~~~", n);
        if (n < 0) {
            if (errno == EINTR) continue;

            LOGE("ril_event: select error (%d)", errno);
            // bail?
            return;
        }

        // Check for timeouts
        processTimeouts();
        // Check for read-ready
        processReadReadies(&rfds, n);
        // Fire away
        firePending();
    }
}
~~~

### 2.3.2 RIL_register函数引入RIL_RadioFunctions

rild.c代码中的main函数会调用RIL_register函数完成第三方RIL_RadioFunctions，同时完成s_listen_event和s_debug_event两个RIL事件节点的创建和加载。

**1. RIL_register函数**

首先分析RIL_register函数是如何引入第三方RIL_RadioFunctions的，其处理逻辑如下：

~~~c++
//@ril.cpp
//path hardware/ril/libril/ril.cpp
extern "C" void
RIL_register (const RIL_RadioFunctions *callbacks) {
    int ret;
    int flags;

    if (callbacks == NULL) {
        LOGE("RIL_register: RIL_RadioFunctions * null");
        return;
    }
    if (callbacks->version < RIL_VERSION_MIN) {
        LOGE("RIL_register: version %d is to old, min version is %d",
             callbacks->version, RIL_VERSION_MIN);
        return;
    }
    if (callbacks->version > RIL_VERSION) {
        LOGE("RIL_register: version %d is too new, max version is %d",
             callbacks->version, RIL_VERSION);
        return;
    }
    LOGE("RIL_register: RIL version %d", callbacks->version);

    if (s_registerCalled > 0) {
        LOGE("RIL_register has been called more than once. "
                "Subsequent call ignored");
        return;
    }

    memcpy(&s_callbacks, callbacks, sizeof (RIL_RadioFunctions));//拷贝callbacks到全局变量s_callbacks变量中，这样其他地方也可以使用到

    s_registerCalled = 1;//修改是否调用此函数的标志

    // Little self-check

    for (int i = 0; i < (int)NUM_ELEMS(s_commands); i++) {
        assert(i == s_commands[i].requestNumber);
    }

    for (int i = 0; i < (int)NUM_ELEMS(s_unsolResponses); i++) {
        assert(i + RIL_UNSOL_RESPONSE_BASE
                == s_unsolResponses[i].requestNumber);
    }

    // New rild impl calls RIL_startEventLoop() first
    // old standalone impl wants it here.

    if (s_started == 0) {
        RIL_startEventLoop();
    }

    // start listen socket

#if 0
    ret = socket_local_server (SOCKET_NAME_RIL,
            ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);

    if (ret < 0) {
        LOGE("Unable to bind socket errno:%d", errno);
        exit (-1);
    }
    s_fdListen = ret;

#else
    s_fdListen = android_get_control_socket(SOCKET_NAME_RIL);
    if (s_fdListen < 0) {
        LOGE("Failed to get socket '" SOCKET_NAME_RIL "'");
        exit(-1);
    }

    ret = listen(s_fdListen, 4);

    if (ret < 0) {
        LOGE("Failed to listen on control socket '%d': %s",
             s_fdListen, strerror(errno));
        exit(-1);
    }
#endif


    /* note: non-persistent so we can accept only one connection at a time */
    ril_event_set (&s_listen_event, s_fdListen, false,
                listenCallback, NULL);

    rilEventAddWakeup (&s_listen_event);

#if 1
    // start debug interface socket

    s_fdDebug = android_get_control_socket(SOCKET_NAME_RIL_DEBUG);
    if (s_fdDebug < 0) {
        LOGE("Failed to get socket '" SOCKET_NAME_RIL_DEBUG "' errno:%d", errno);
        exit(-1);
    }

    ret = listen(s_fdDebug, 4);

    if (ret < 0) {
        LOGE("Failed to listen on ril debug socket '%d': %s",
             s_fdDebug, strerror(errno));
        exit(-1);
    }

    ril_event_set (&s_debug_event, s_fdDebug, true,
                debugCallback, NULL);

    rilEventAddWakeup (&s_debug_event);
#endif

}
~~~

上面的代码最重要的是对第三方Reference-RIL提供的callbacks参数，即funcs进行是否为空、版本号等方面的验证，通过验证后，拷贝保存此指向RIL_RadioFunctions结构体指针：这样，在LibRIL中就可以调用第三方Reference-RIL提供的RIL请求相关函数，而第三方提供的Reference-RIL可以用so动态链接库的方式提供，保障了第三方厂家代码的安全性、保密性。

**s_listen_event**

继续分析s_listen_event和s_debug_event两个RIL事件节点处理逻辑，它们的处理逻辑非常相似，省略RIL调试事件s_debug_event，重点解析RIL监听事件s_listen_event的创建和激活，其处理逻辑如下：

~~~c++
//@ril.cpp
//path hardware/ril/libril/ril.cpp
#if 0
    ret = socket_local_server (SOCKET_NAME_RIL,
            ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);

    if (ret < 0) {
        LOGE("Unable to bind socket errno:%d", errno);
        exit (-1);
    }
    s_fdListen = ret;

#else
    s_fdListen = android_get_control_socket(SOCKET_NAME_RIL);
    if (s_fdListen < 0) {
        LOGE("Failed to get socket '" SOCKET_NAME_RIL "'");
        exit(-1);
    }

    ret = listen(s_fdListen, 4);

    if (ret < 0) {
        LOGE("Failed to listen on control socket '%d': %s",
             s_fdListen, strerror(errno));
        exit(-1);
    }
#endif
~~~

在创建和激活s_listen_event事件的过程中重点关注以下几点：

- 创建端口号为rild的Socket的网络服务端并获取其文件描述符，在此过程中请关注if 0 else的条件判断，程序默认会进入if 0的条件判断中的处理逻辑。
- 创建和激活的s_listen_event与s_debug_event事件不同的是，创建的Socket服务端其端口号是：rild-debug；而RIL事件的回调方法是：debugCallback。
- 至此共创建了3个ril_event结构体，它们分别是s_wakeupfd_event、_listen_event和s_debug_event。

# 3. 运行状态中的ril_event事件处理机制

LibRIL在启动完成后进入运行状态，将围绕ril_event结构体处理RIL相关事件；RIL事件相关的数据都会封装在ril_event结构体中，一个RIL事件会对应一个ril_event结构体。

## 3.1 认识ril_event结构体

其代码详情如下：

~~~c++
//@ril_event.h
//path hardware/ril/libril/ril_event.h


typedef void (*ril_event_cb)(int fd, short events, void *userdata);//定义指向RIL事件Callback回调函数的指针

struct ril_event {
    struct ril_event *next;
    struct ril_event *prev;

    int fd; //文件描述符
    int index;//当前RIL事件索引
    bool persist;//保留当前RIL事件标志
    struct timeval timeout;//事件回调函数的指针
    ril_event_cb func;
    void *param;
};
~~~

通过上面的结构体定义，ril_event结构体支持双向链表，每一个节点都有指向上一个节点和下一个节点的指针；并且在此头文件中，定义了针对ril_event双向链表的函数

- void ril_event_init();// Initialize internal data structs
- void ril_event_set(struct ril_event * ev, int fd, bool persist, ril_event_cb func, void * param);// Initialize an event

- void ril_event_add(struct ril_event * ev);// Add event to watch list

- void ril_timer_add(struct ril_event * ev, struct timeval * tv);// Add timer event

- void ril_event_del(struct ril_event * ev);// Remove event from watch list

- void ril_event_loop();// Event loop

上面ril_event双向链表的6个操作函数，其处理逻辑全部在ril_event.cpp文件中实现。

ril_event.cpp中实现了ril_event.h头文件事件的多个处理函数，其作用是配合ril.cpp完成RIL事件的封装和处理。

## 3.2 RIL事件声明周期控制的处理函数

观察ril_event.h头文件中定义的ril_event事件处理不难发现，RIL事件的处理最重要的就是ril_event_init、ril_event_set、ril_event_add和ril_event_del（ril_event_del基本不会使用，因此不多做介绍），这4个函数包括了对RIL事件的生命周期的控制。

**1. ril_event双向链表初始化操作ril_event_init**

在启动LibRIL运行环境的时候，调用eventLoop函数时首先会调用ril_event_init方法，初始化ril_event；他会完成两个ril_event链表的初始化操作：timer_list和pending_list。

timer_list链表中保存RIL的定时事件，而pending_list链表中保存RIL的请求事件以及初始化ril_event数组watch_table。其处理逻辑如下：

~~~c++
//@ril_event.cpp
//path hardware/ril/libril/ril_event.cpp

// Initialize internal data structs
void ril_event_init()
{
    MUTEX_INIT();

    FD_ZERO(&readFds);
    init_list(&timer_list);
    init_list(&pending_list);
    memset(watch_table, 0, sizeof(watch_table));
}
~~~

上面的代码处理逻辑完成4个变量的初始化操作：readFds、timer_list、pending_list和watch_table，其中3个是ril_event结构体相关，而readFds则是和fd_set相关。

> **注意** 什么是fd_set呢？它是select多端口复用机制中提供的一种数据结构，是long类型数组，其中每一个数组元素都能与其打开的文件描述符关联，从而使用文件描述符进行I/O操作。因此，在处理多个ril_event事件的过程中，会使用到select多端口复用机制。

**2. 设置新的ril_event事件参数ril_event_set**

在创建ril_event后，需要调用ril_event_set函数设置其相关的参数，否则ril_event将不能正常工作。其代码处理逻辑如下：

~~~c++
//@ril_event.cpp
//path hardware/ril/libril/ril_event.cpp


// Initialize an event
void ril_event_set(struct ril_event * ev, int fd, bool persist, ril_event_cb func, void * param)
{
    dlog("~~~~ ril_event_set %x ~~~~", (unsigned int)ev);
    memset(ev, 0, sizeof(struct ril_event));
    ev->fd = fd;//设备文件描述符
    ev->index = -1;//设置索引默认为-1
    ev->persist = persist;//RIL事件持久化标志
    ev->func = func;//事件的Callback回调函数
    ev->param = param;//参数
    fcntl(fd, F_SETFL, O_NONBLOCK);//设置文件描述符状态O_NONBLOCK非阻塞I/O
}
~~~

这段代码中非常关键的就是设置文件描述符状态，参数决定了对I/O处理采用什么方式，这里采用了O_NONBLOCK非阻塞I/O处理方式。

**3. 增加ril_event事件ril_event_add**

前面的准备工作已经完成，最后一步就是调用ril_event_add函数增加ril_event事件监听，此方法将前面已经准备好的ril_event保存到watch_table数组中，并根据ril_event->fd文件描述符设置FD_SET。其代码处理逻辑如下：

~~~c++
//@ril_event.cpp
//path hardware/ril/libril/ril_event.cpp


// Add event to watch list
void ril_event_add(struct ril_event * ev)
{
    dlog("~~~~ +ril_event_add ~~~~");
    MUTEX_ACQUIRE();
    for (int i = 0; i < MAX_FD_EVENTS; i++) {
        if (watch_table[i] == NULL) {
            watch_table[i] = ev;//将ev保存到watch_table数组中
            ev->index = i;//设置索引与数组下标对应
            dlog("~~~~ added at %d ~~~~", i);
            dump_event(ev);
            FD_SET(ev->fd, &readFds);//设置FD_SET,只需要监听readFds即可获取对应文件描述符的I/O读写
            if (ev->fd >= nfds) nfds = ev->fd+1;
            dlog("~~~~ nfds = %d ~~~~", nfds);//更新nfds
            break;
        }
    }
    MUTEX_RELEASE();
    dlog("~~~~ -ril_event_add ~~~~");
}
~~~

在LibRIL运行环境的加载过程中，共创建了3个ril_event结构体，分别是：s_wakeupfs_event、s_listen_event、和s_debug_event,通过本小节的学习，可以总结出以下几点：

- 3个ril_event结构体全部保存在watch_table数组中；
- 每个结构体的文件描述符都设置到readFds的FD_SET数据类型里，完成了select多端口复用的准备工作。

> **注意** LilRIL运行环境加载完成后，timer_list和pending_list双向链表中仍然没有ril_event事件节点；那么，timer_list和pending_list什么时候才会有节点呢？请看后面的分析。

## 3.3 ril_event_loop处理机制

LibRIL运行环境的加载过程中，最后会调用ril_event_loop函数，开始监听RIL事件；ril_event_loop函数通过for循环select多端口复用，监听设置的前三个文件描述符s_wakeupfd_event->fd、s_listen_event->fd、s_debug_event->fd，一旦产生I/O事件，select便会响应，找到对应的ril_event结构体，最后通过结构体的func发起RIL事件响应的回调函数的调用，其处理逻辑如下：

~~~c++
//@ril_event.cpp
//path hardware/ril/libril/ril_event.cpp
void ril_event_loop()
{
    int n;
    fd_set rfds;
    struct timeval tv;
    struct timeval * ptv;


    for (;;) {

        // make local copy of read fd_set
        memcpy(&rfds, &readFds, sizeof(fd_set));
        if (-1 == calcNextTimeout(&tv)) {
            // no pending timers; block indefinitely
            dlog("~~~~ no timers; blocking indefinitely ~~~~");
            ptv = NULL;
        } else {
            dlog("~~~~ blocking for %ds + %dus ~~~~", (int)tv.tv_sec, (int)tv.tv_usec);
            ptv = &tv;
        }
        printReadies(&rfds);
        n = select(nfds, &rfds, NULL, NULL, ptv);
        printReadies(&rfds);
        dlog("~~~~ %d events fired ~~~~", n);
        if (n < 0) {
            if (errno == EINTR) continue;

            LOGE("ril_event: select error (%d)", errno);
            // bail?
            return;
        }

        // Check for timeouts
        processTimeouts();
        // Check for read-ready
        processReadReadies(&rfds, n);
        // Fire away
        firePending();
    }
~~~

通过上面的代码可知，其中最重要的是processTimeouts、processReadReadies和firePending3个函数的调用，可分为两步操作。

1. 增加pending_list双向链表；其中包括了两种类型的RIL事件：已超时的定时RIL事件和接收到的RIL相关请求事件；这里可以回答前面提出的pending_list双向链表什么时候增加RIL事件的节点问题。
2. 调用firePending函数遍历pending_list双向链表ril_event，调用其func回调函数，完成对应的RIL事件回调。

**1. 增加pending_list双向链表中的RIL事件节点**

向pending_list双向链表中增加RIL事件节点的入口有两个地方，processTimeouts函数和

processReadReadies函数。processTimeouts函数处理已超时的RIL定时事件，其处理逻辑如下：

~~~c++
//@ril_event.cpp
//path hardware/ril/libril/ril_event.cpp
static void processTimeouts()
{
    dlog("~~~~ +processTimeouts ~~~~");
    MUTEX_ACQUIRE();
    struct timeval now;
    struct ril_event * tev = timer_list.next;
    struct ril_event * next;

    getNow(&now);
    // walk list, see if now >= ev->timeout for any events
	
    //遍历双向链表，并判断其中的节点是否有超市的RIL事件
    dlog("~~~~ Looking for timers <= %ds + %dus ~~~~", (int)now.tv_sec, (int)now.tv_usec);
    while ((tev != &timer_list) && (timercmp(&now, &tev->timeout, >))) {
        // Timer expired
        dlog("~~~~ firing timer ~~~~");
        next = tev->next;
        removeFromList(tev);
        //关键点，增加RIL事件节点到pending_list
        addToList(tev, &pending_list);
        tev = next;
    }
    MUTEX_RELEASE();
    dlog("~~~~ -processTimeouts ~~~~");
}
~~~

由上面的代码可知，其中有两个关键点：RIL事件超时的时间判断和将已经超时的RIL事件增加到pending_list链表中。它们的处理逻辑非常简单，感兴趣的读者请自行阅读相关代码。processReadReadies函数处理接收到RIL请求事件，其处理逻辑如下：

~~~c++
//@ril_event.cpp
//path hardware/ril/libril/ril_event.cpp
static void processReadReadies(fd_set * rfds, int n)
{
    dlog("~~~~ +processReadReadies (%d) ~~~~", n);
    MUTEX_ACQUIRE();

    for (int i = 0; (i < MAX_FD_EVENTS) && (n > 0); i++) {
        struct ril_event * rev = watch_table[i];
        if (rev != NULL && FD_ISSET(rev->fd, rfds)) {
            addToList(rev, &pending_list);
            if (rev->persist == false) {
                removeWatch(rev, i);
            }
            n--;
        }
    }

    MUTEX_RELEASE();
    dlog("~~~~ -processReadReadies (%d) ~~~~", n);
}
~~~

通过上面的代码，可以总结出三点：

- FD_ISSET判断select事件中的文件描述符，根据watch_table数组中对应的文件描述符，找到对应的ril_event事件。
- 调用addToList函数，将ril_Event事件增加到pending_list链表中，等待RIL事件的回调。
- 删除非持久的RIL事件。

**2. RIL事件回调处理**

前面已经完成了pending_list链表中需要触发的RIL事件的准备工作，最后是调用firePending函数完成RIL事件的回调。其处理逻辑如下：

~~~c++
//@ril_event.cpp
//path hardware/ril/libril/ril_event.cpp
static void firePending()
{
    dlog("~~~~ +firePending ~~~~");
    struct ril_event * ev = pending_list.next;
    while (ev != &pending_list) {
        struct ril_event * next = ev->next;
        removeFromList(ev);
        ev->func(ev->fd, 0, ev->param);//会将fd文件描述符作为参数进行回调处理
        ev = next;
    }
    dlog("~~~~ -firePending ~~~~");
}
~~~

# 4. 详解LibRIL运行机制

前面已经重点分析了LibRIL运行环境的加载过程，以及ril_event事件的处理机制，接着重点解析LibRIL运行环境的运行机制，主要以下两个方面展开讲述：

- RILJ建立与RIL的Socket连接过程；
- RILJ向RIL发起的Solicited消息的交互流程和处理机制。

## 4.1 RILJ与LibRIL建立Socket连接过程

Android手机在启动过程中会加载Phone应用，同时会去构造RILJ对象，在RILJ的构造方法中，主动发起rild的Socket链接；这时LibRIL运行环境中，在ril_event_loop函数中的select会响应s_listen_event的RIL请求事件。通过前面的学习可知，ril_event_loop函数中接收到RIL请求事件后，通过processTimeouts、processReadReadies和firePending函数的连续调用，最后通过ril_event-func函数发起Callback回调操作，而s_listen_event->func为listenCallback函数的处理逻辑如下：

~~~c++
//@ril.cpp
//path hardware/ril/libril/ril.cpp
static void listenCallback (int fd, short flags, void *param) {
    int ret;
    int err;
    int is_phone_socket;
    RecordStream *p_rs;

    struct sockaddr_un peeraddr;
    socklen_t socklen = sizeof (peeraddr);

    struct ucred creds;
    socklen_t szCreds = sizeof(creds);

    struct passwd *pwd = NULL;

    assert (s_fdCommand < 0);
    assert (fd == s_fdListen);

    //接收Scoket连接请求，s_fdListen和fd必须保持一致，否则直接退出
    s_fdCommand = accept(s_fdListen, (sockaddr *) &peeraddr, &socklen);

    if (s_fdCommand < 0 ) {//建立socket异常处理
        LOGE("Error on accept() errno:%d", errno);
        /* start listening for new connections again */
        rilEventAddWakeup(&s_listen_event);
	      return;
    }
    //基于安全考虑，只接受Phone应用发起的Socket连接，否则直接关闭

    /* check the credential of the other side and only accept socket from
     * phone process
     */
    errno = 0;
    is_phone_socket = 0;

    err = getsockopt(s_fdCommand, SOL_SOCKET, SO_PEERCRED, &creds, &szCreds);

    if (err == 0 && szCreds > 0) {
        errno = 0;
        pwd = getpwuid(creds.uid);
        if (pwd != NULL) {
            if (strcmp(pwd->pw_name, PHONE_PROCESS) == 0) {
                is_phone_socket = 1;
            } else {
                LOGE("RILD can't accept socket from process %s", pwd->pw_name);
            }
        } else {
            LOGE("Error on getpwuid() errno: %d", errno);
        }
    } else {
        LOGD("Error on getsockopt() errno: %d", errno);
    }

    if ( !is_phone_socket ) {
      LOGE("RILD must accept socket from %s", PHONE_PROCESS);

      close(s_fdCommand);
      s_fdCommand = -1;

      onCommandsSocketClosed();

      /* start listening for new connections again */
      rilEventAddWakeup(&s_listen_event);

      return;
    }
	
    //设置文件描述符状态，O_NONBLOCK非阻塞I/O
    
    ret = fcntl(s_fdCommand, F_SETFL, O_NONBLOCK);

    if (ret < 0) {
        LOGE ("Error setting O_NONBLOCK errno:%d", errno);
    }

    LOGI("libril: new connection");

    p_rs = record_stream_new(s_fdCommand, MAX_COMMAND_BYTES);

    //设置s_commands_event相关参数，Callback为processCommandsCallback函数
    ril_event_set (&s_commands_event, s_fdCommand, 1,
        processCommandsCallback, p_rs);

    rilEventAddWakeup (&s_commands_event);

    onNewCommandConnect();//Scoket连接后的处理，主要发起一下unsolicited消息
}
~~~

通过上面的代码可知，listenCallback函数有两个主要的处理逻辑，分别如下：

- 接受Socket连接请求，建立与RILJ的Socket连接；
- 增加s_commands_event监听。

## 4.2 Solicited消息的交互流程和处理机制

RILJ建立起与LibRIL的Socket连接后，LibRIL运行环境中的watch_table数组增加了一个事件监听s_commands_event。即，RILJ基于rild端口的Socket向LibRIL发起Solicited Request消息请求时，s_commands_event会通过func发起RIL事件的Callback函数调用，即调用processCommandsCallback函数处理RILJ发起的RILSolicited Request消息请求。

**1. processCommandsCallback函数**

其处理逻辑非常简单，主要是调用processCommandBuffer函数，负责处理RILJ发起的RIL请求；其中会涉及RequestInfo结构体，因此，首先看看RequestInfo的结构体定义。

~~~c++
//@ril_event.cpp
//path hardware/ril/libril/ril_event.cpp
typedef struct RequestInfo {
    int32_t token;      //this is not RIL_Token
    CommandInfo *pCI;
    struct RequestInfo *p_next;
    char cancelled;
    char local;         // responses to local commands do not go back to command process
} RequestInfo;
~~~

通过上面的代码可知，RequestInfo是一个单向链表，每一个节点都有指向下一个节点的指针p_next；另外，CommandInfo是什么呢？请看下面的结构体定义：

~~~c++
//@ril_event.cpp
//path hardware/ril/libril/ril_event.cpp
typedef struct {
    int requestNumber;//请求编号
    void (*dispatchFunction) (Parcel &p, struct RequestInfo *pRI);//指向请求调用的函数
    int(*responseFunction) (Parcel &p, void *response, size_t responselen);
} CommandInfo;
~~~

通过这两个结构体定义的解析可知，RequestInfo和RILJ中的RILRequest类非常类似，都有请求编号、请求方法调用和返回调用。

**2. processCommandBuffer函数**

~~~c++
//@ril_event.cpp
//path hardware/ril/libril/ril_event.cpp
static int
processCommandBuffer(void *buffer, size_t buflen) {
    Parcel p;
    status_t status;
    int32_t request;
    int32_t token;
    RequestInfo *pRI;
    int ret;

    p.setData((uint8_t *) buffer, buflen);

    // status checked at end
    status = p.readInt32(&request);//获取RIL请求类型
    status = p.readInt32 (&token);//

    if (status != NO_ERROR) {
        LOGE("invalid request block");
        return 0;
    }

    if (request < 1 || request >= (int32_t)NUM_ELEMS(s_commands)) {
        LOGE("unsupported request code %d token %d", request, token);
        // FIXME this should perhaps return a response
        return 0;
    }


    //创建RequestInfo
    pRI = (RequestInfo *)calloc(1, sizeof(RequestInfo));

    pRI->token = token;
    pRI->pCI = &(s_commands[request]);//通过RIL请求类型，获取CommandInfo

    ret = pthread_mutex_lock(&s_pendingRequestsMutex);
    assert (ret == 0);

    pRI->p_next = s_pendingRequests;//在s_pendingRequests链表中增加节点
    s_pendingRequests = pRI;

    ret = pthread_mutex_unlock(&s_pendingRequestsMutex);
    assert (ret == 0);

/*    sLastDispatchedToken = token; */

    pRI->pCI->dispatchFunction(p, pRI);//根据CommandInfo发起RIL请求对应函数的调用

    return 0;
}
~~~

其中s_commands[request]根据RIL请求类型获取CommandInfo比较关键，它是如何实现的呢，请看s_commands的定义，详情如下：

~~~c++
//@ril.cpp
//path hardware/ril/libril/ril.cpp
static CommandInfo s_commands[] = {
#include "ril_commands.h"
};
~~~

在ril_commands.h头文件中定义了107个RIL请求的处理函数和回调函数。

~~~c++
//@ril_commands.h
//path hardware/ril/libril/ril_commands.h
	{0, NULL, NULL},                   //none
    {RIL_REQUEST_GET_SIM_STATUS, dispatchVoid, responseSimStatus},
    {RIL_REQUEST_ENTER_SIM_PIN, dispatchStrings, responseInts},
    {RIL_REQUEST_ENTER_SIM_PUK, dispatchStrings, responseInts},
...
~~~

通过上面两段代码可知，在ril.cpp中的静态变量ril_commands在首次进入此代码时，就已经创建了107个CommandInfo，保存在ril_commands数组中；RILJ在发起RIL请求后，在这里通过RIL请求类型获取请求处理和返回处理函数，并且RILJ中定义的RIL请求类型与RILC中ril.h头文件中的定义保持一致。

比如，在发起拨号请求时，LibRIL中匹配的CommandInfo的请求调用函数为dispatchDial，而返回调用的函数为responseVoid；当LibRIL发起pRI->pCI->dispatchFunction函数调用时，实际调用的函数是dispatchDial。

~~~c++
//@ril.cpp
//path hardware/ril/libril/ril.cpp
static void
dispatchDial (Parcel &p, RequestInfo *pRI) {
    RIL_Dial dial;
    RIL_UUS_Info uusInfo;
    int32_t sizeOfDial;
    int32_t t;
    int32_t uusPresent;
    status_t status;

    memset (&dial, 0, sizeof(dial));

    dial.address = strdupReadString(p);

    status = p.readInt32(&t);
    dial.clir = (int)t;

    if (status != NO_ERROR || dial.address == NULL) {
        goto invalid;
    }

    if (s_callbacks.version < 3) { // Remove when partners upgrade to version 3
        uusPresent = 0;
        sizeOfDial = sizeof(dial) - sizeof(RIL_UUS_Info *);
    } else {
        status = p.readInt32(&uusPresent);

        if (status != NO_ERROR) {
            goto invalid;
        }

        if (uusPresent == 0) {
            dial.uusInfo = NULL;
        } else {
            int32_t len;

            memset(&uusInfo, 0, sizeof(RIL_UUS_Info));

            status = p.readInt32(&t);
            uusInfo.uusType = (RIL_UUS_Type) t;

            status = p.readInt32(&t);
            uusInfo.uusDcs = (RIL_UUS_DCS) t;

            status = p.readInt32(&len);
            if (status != NO_ERROR) {
                goto invalid;
            }

            // The java code writes -1 for null arrays
            if (((int) len) == -1) {
                uusInfo.uusData = NULL;
                len = 0;
            } else {
                uusInfo.uusData = (char*) p.readInplace(len);
            }

            uusInfo.uusLength = len;
            dial.uusInfo = &uusInfo;
        }
        sizeOfDial = sizeof(dial);
    }

    startRequest;
    appendPrintBuf("%snum=%s,clir=%d", printBuf, dial.address, dial.clir);
    if (uusPresent) {
        appendPrintBuf("%s,uusType=%d,uusDcs=%d,uusLen=%d", printBuf,
                dial.uusInfo->uusType, dial.uusInfo->uusDcs,
                dial.uusInfo->uusLength);
    }
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    //这里非常关键，s_callbacks即Reference-RIL提供的函数，关注requestNumber、pRI等参数
    s_callbacks.onRequest(pRI->pCI->requestNumber, &dial, sizeOfDial, pRI);

#ifdef MEMSET_FREED
    memsetString (dial.address);
#endif

    free (dial.address);

#ifdef MEMSET_FREED
    memset(&uusInfo, 0, sizeof(RIL_UUS_Info));
    memset(&dial, 0, sizeof(dial));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}
~~~

通过上面的代码，结合其它LibRIL请求调用函数，如dispatchStrings、dispatchVoid、dispatchInts等函数，可总结出以下两点：

- LibRIL请求调用函数，最终目的是通过Reference-RIL提供的s_callbacks发起onRequest函数的调用。
- onRequest函数调用的参数都会有requestNumber和pRI。

> **注意** Solicited消息可细分为Solicited Request请求消息和Solicited Response返回消息，它们都是成对出现的。Solicited Request请求消息的处理逻辑我们已经做了详细解析和学习，那么，Solicited Response消息是如何完成的呢？

Reference-RIL在接收到LinRIL发起的onRequest函数请求调用时，首先会根据RIL请求类型转换成对应的AT命令，并向Modem发起AT命令，接着便会调用LibRIL提供的RIL_onRequestComplete函数，完成RIL请求处理完成后的回调。因此，我们接着解析RIL_onRequsetComplete函数的处理逻辑。

~~~c++
//@ril.cpp
//path hardware/ril/libril/ril.cpp
extern "C" void
RIL_onRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responselen) {
    RequestInfo *pRI;
    int ret;
    size_t errorOffset;

    /*强制转换成RequestInfo，还记得Reference-RIL中的onRequest函数吗？其中的参数就有RequestInfo。原来的RIL请求调用和返回Callback的两个过程中，RequestInfo都会以参数新式传递，因此，与RILJ中的消息处理相同Solicited消息的Request和Response通过RequsetInfo产生一一对应的关系*/
    pRI = (RequestInfo *)t;

    if (!checkAndDequeueRequestInfo(pRI)) {
        LOGE ("RIL_onRequestComplete: invalid RIL_Token");
        return;
    }

    if (pRI->local > 0) {
        // Locally issued command...void only!
        // response does not go back up the command socket
        LOGD("C[locl]< %s", requestToString(pRI->pCI->requestNumber));

        goto done;
    }

    appendPrintBuf("[%04d]< %s",
        pRI->token, requestToString(pRI->pCI->requestNumber));

    if (pRI->cancelled == 0) {
        Parcel p;

        p.writeInt32 (RESPONSE_SOLICITED);
        p.writeInt32 (pRI->token);
        errorOffset = p.dataPosition();

        p.writeInt32 (e);

        if (response != NULL) {
            // there is a response payload, no matter success or not.
            ret = pRI->pCI->responseFunction(p, response, responselen);

            /* if an error occurred, rewind and mark it */
            if (ret != 0) {
                p.setDataPosition(errorOffset);
                p.writeInt32 (ret);
            }
        }

        if (e != RIL_E_SUCCESS) {
            appendPrintBuf("%s fails by %s", printBuf, failCauseToString(e));
        }

        if (s_fdCommand < 0) {
            LOGD ("RIL onRequestComplete: Command channel closed");
        }
        sendResponse(p);//通过Socket连接发送Parcel数据，即RILJ接收此数据
    }

done:
    free(pRI);
}
~~~

上面的代码需要重点关注两点，分别为

- responseFunction函数调用，完成不用返回的Parcel数据设置；
- sendResponse函数调用，通过Socket连接发送Parcel数据，即RILJ接收此数据。

同样，在发起拨号请求后pRI->pCI->responseFunction函数第哦啊有实际上是responseCallList函数调用，其处理逻辑如下：

~~~c++
//@ril.cpp
//path hardware/ril/libril/ril.cpp
static int responseCallList(Parcel &p, void *response, size_t responselen) {
    int num;

    if (response == NULL && responselen != 0) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof (RIL_Call *) != 0) {
        LOGE("invalid response length %d expected multiple of %d\n",
            (int)responselen, (int)sizeof (RIL_Call *));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;
    /* number of call info's */
    num = responselen / sizeof(RIL_Call *);
    p.writeInt32(num);

    for (int i = 0 ; i < num ; i++) {
        RIL_Call *p_cur = ((RIL_Call **) response)[i];
        /* each call info */
        p.writeInt32(p_cur->state);
        p.writeInt32(p_cur->index);
        p.writeInt32(p_cur->toa);
        p.writeInt32(p_cur->isMpty);
        p.writeInt32(p_cur->isMT);
        p.writeInt32(p_cur->als);
        p.writeInt32(p_cur->isVoice);
        p.writeInt32(p_cur->isVoicePrivacy);
        writeStringToParcel(p, p_cur->number);
        p.writeInt32(p_cur->numberPresentation);
        writeStringToParcel(p, p_cur->name);
        p.writeInt32(p_cur->namePresentation);
        // Remove when partners upgrade to version 3
        if ((s_callbacks.version < 3) || (p_cur->uusInfo == NULL || p_cur->uusInfo->uusData == NULL)) {
            p.writeInt32(0); /* UUS Information is absent */
        } else {
            RIL_UUS_Info *uusInfo = p_cur->uusInfo;
            p.writeInt32(1); /* UUS Information is present */
            p.writeInt32(uusInfo->uusType);
            p.writeInt32(uusInfo->uusDcs);
            p.writeInt32(uusInfo->uusLength);
            p.write(uusInfo->uusData, uusInfo->uusLength);
        }
        appendPrintBuf("%s[id=%d,%s,toa=%d,",
            printBuf,
            p_cur->index,
            callStateToString(p_cur->state),
            p_cur->toa);
        appendPrintBuf("%s%s,%s,als=%d,%s,%s,",
            printBuf,
            (p_cur->isMpty)?"conf":"norm",
            (p_cur->isMT)?"mt":"mo",
            p_cur->als,
            (p_cur->isVoice)?"voc":"nonvoc",
            (p_cur->isVoicePrivacy)?"evp":"noevp");
        appendPrintBuf("%s%s,cli=%d,name='%s',%d]",
            printBuf,
            p_cur->number,
            p_cur->numberPresentation,
            p_cur->name,
            p_cur->namePresentation);
    }
    removeLastChar;
    closeResponse;

    return 0;
}
~~~

responseCallList函数与其它responseXXX函数的处理逻辑都非常类似，都是对Parcel数据的设置，不同之处在于Parcel数据的内容和顺序；Parcel数据完成以后，最后就是调用sendResponse函数发起Parcel数据基于Socket的发送，sendResponse函数中的处理逻辑

sendResponseRaw函数的处理逻辑如下：

~~~c++
//@ril.cpp
//path hardware/ril/libril/ril.cpp
static int
sendResponseRaw (const void *data, size_t dataSize) {
    int fd = s_fdCommand;//获取s_commands_event的文件描述符即可操作rild端口号的Socket连接
    int ret;
    uint32_t header;

    if (s_fdCommand < 0) {
        return -1;
    }

    if (dataSize > MAX_COMMAND_BYTES) {
        LOGE("RIL: packet larger than %u (%u)",
                MAX_COMMAND_BYTES, (unsigned int )dataSize);

        return -1;
    }

    pthread_mutex_lock(&s_writeMutex);

    header = htonl(dataSize);

    ret = blockingWrite(fd, (void *)&header, sizeof(header));

    if (ret < 0) {
        pthread_mutex_unlock(&s_writeMutex);
        return ret;
    }

    ret = blockingWrite(fd, data, dataSize);

    if (ret < 0) {
        pthread_mutex_unlock(&s_writeMutex);
        return ret;
    }

    pthread_mutex_unlock(&s_writeMutex);

    return 0;
}
~~~

至此，Solicited Request和Solicited Response消息成对处理逻辑和流程已经解析完成，其关键流程可总结出以下几点。

- RILJ建立与LibRIL的Socket连接时，s_listen_event会响应Socket连接事件，同时会增加s_commands_event事件的监听。
- RILJ向LibRIL发起RIL请求后，s_commands_event响应RIL请求，同时根据请求响应类型，获取对应的RequestInfo并调用其请求函数，最终调用Reference-RIL中的onRequest函数。
- Reference-RIL在函数调用过程中，将RIL请求转化为AT命令，完成调用LibRIL中的RIL_onRequestComplete函数。
- LibRIL中的RIL_onRequestComplete函数，根据RequestInfo中的信息调用返回处理函数，其目的是Parcel相关数据，最后通过sendResponse函数基于Socket连接发起的Parcel数据。

# 5. Reference-RIL运行机制

## 5.1 RIL_Init函数初始化Reference-RIL

RILC启动的三大步骤中，第二个步骤就是调用Reference-RIL中的RIL_Init函数，其处理逻辑如下：

~~~c
//@reference-ril.c
//path hardware/ril/reference-ril/reference-ril.c
const RIL_RadioFunctions *RIL_Init(const struct RIL_Env *env, int argc, char **argv)
{
    int ret;
    int fd = -1;
    int opt;
    pthread_attr_t attr;

    s_rilenv = env;

    while ( -1 != (opt = getopt(argc, argv, "p:d:s:"))) {
        switch (opt) {
            case 'p':
                s_port = atoi(optarg);
                if (s_port == 0) {
                    usage(argv[0]);
                    return NULL;
                }
                LOGI("Opening loopback port %d\n", s_port);
            break;

            case 'd':
                s_device_path = optarg;
                LOGI("Opening tty device %s\n", s_device_path);
            break;

            case 's':
                s_device_path   = optarg;
                s_device_socket = 1;
                LOGI("Opening socket %s\n", s_device_path);
            break;

            default:
                usage(argv[0]);
                return NULL;
        }
    }

    if (s_port < 0 && s_device_path == NULL) {
        usage(argv[0]);
        return NULL;
    }

    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&s_tid_mainloop, &attr, mainLoop, NULL);//创建基于mainLoop函数运行的子进程

    return &s_callbacks;//返回RIL_RadioFunctions
}
~~~

RIL_Init函数完成的Reference-RIL初始化工作，包括三个主要步骤：

1. 记录LibRIL提供的RIL_Env指针，通过它可以调用LibRIL提供的相应函数；
2. 启动基于mainLoop函数运行的子进程，mainLoop主要负责监听和接收Modem主动上报的Unsolicited消息；
3. 返回Reference-RIL提供的指向RIL_RadioFunctions指针s_callbacks。

这里可能有一个疑问，s_callbacks是在什么时候初始化呢？原来他说一个static静态变量，其初始化逻辑如下：

~~~c
//@reference-ril.c
//path hardware/ril/reference-ril/reference-ril.c
static const RIL_RadioFunctions s_callbacks = {
    RIL_VERSION,
    onRequest,
    currentState,
    onSupports,
    onCancel,
    getVersion
};
~~~

s_callbacks作为一个静态变量，在首次访问reference-ril.c时就会完成其初始化操作。另外，还有五个指向函数的指针非常重要，在LibRIL中通过这些指向Reference-RIL中函数的指针，便能非常方便地调用Reference-RIL提供的对应函数。其中，在前面解析和学习LibRIL的过程中，多次涉及onRequest和getVersion这两个函数，也是在RILC中使用最多的函数。

我们接下来解析Reference-RIL提供的onRequest函数。

## 5.2 onRequest接收LibRIL的请求调用

通过查找Reference-RIL提供的onRequest函数调用逻辑，发现它仅在一个地方被调用，那就是在LibRIL中。LibRIL接收到RILJ的请求后，通过onRequest函数调用，向Reference-RIL发起对应的RIL请求。在onRequest函数中，其处理逻辑主要完成了两件事：

- 将RIL请求转化成对应的AT命令，并向Modem发出AT命令。
- 调用LibRIL的RIL_onRequestCpmplete函数，完成RIL请求处理结果的返回。

reference-ril.c代码中，已经完成的onRequest函数其代码量比较大，处理逻辑框架详情如下：

~~~c
//@reference-ril.c
//path hardware/ril/reference-ril/reference-ril.c
static void
onRequest (int request, void *data, size_t datalen, RIL_Token t)
{
    ATResponse *p_response;
    int err;

    LOGD("onRequest: %s", requestToString(request));

    /* Ignore all requests except RIL_REQUEST_GET_SIM_STATUS
     * when RADIO_STATE_UNAVAILABLE.
     */
    //Radio无线通信模块状态不可用，直接返回RIL_E_RADIO_NOT_AVAILABLE消息
    if (sState == RADIO_STATE_UNAVAILABLE
        && request != RIL_REQUEST_GET_SIM_STATUS
    ) {
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }

    /* Ignore all non-power requests when RADIO_STATE_OFF
     * (except RIL_REQUEST_GET_SIM_STATUS)
     */
    //Radio无线通信模块已经关闭或者获取SIM卡状态，直接返回RIL_E_RADIO_NOT_AVAILABLE消息
    if (sState == RADIO_STATE_OFF
        && !(request == RIL_REQUEST_RADIO_POWER
            || request == RIL_REQUEST_GET_SIM_STATUS)
    ) {
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }

    //根据RIL请求类型做不同的操作
    switch (request) {
        ...
        case RIL_REQUEST_GET_CURRENT_CALLS:
            requestGetCurrentCalls(data, datalen, t);
            break;
        case RIL_REQUEST_DIAL:
            requestDial(data, datalen, t);
            break;
        case RIL_REQUEST_HANGUP:
            requestHangup(data, datalen, t);
            break;
        ...
        default:
            RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
            break;
    }
}
~~~

上述代码最关键的就是根据RIL请求类型调用requestGetCurrentCalls、requestDial、requestHangup等不同函数，完成相应的逻辑处理，或是直接在switch case分支中不调用函数而直接完成对应的处理逻辑。

在switch case分支中的处理逻辑不论以什么方式，都会完成两件事：

- 向Modem发起执行AT命令请求；
- 调用RIL_onRequestComplete函数完成RIL请求的返回。

举个实际，比如在发起拨号请求时，RILJ会向LibRIL发出RIL_REQUEST_DIAL的RIL消息，LibRIL会调用Reference-RIL的onRequest方法继续传递RIL请求，onRequest方法根据RIL消息类型，会调用RequestDial函数向Modem、发起的拨号的AT命令，并将AT命令的执行结果反馈给LibRIL。requestDial函数的处理逻辑如下：

~~~c
//@reference-ril.c
//path hardware/ril/reference-ril/reference-ril.c
static void requestDial(void *data, size_t datalen, RIL_Token t)
{
    RIL_Dial *p_dial;
    char *cmd;
    const char *clir;
    int ret;

    p_dial = (RIL_Dial *)data;

    switch (p_dial->clir) {
        case 1: clir = "I"; break;  /*invocation*/
        case 2: clir = "i"; break;  /*suppression*/
        default:
        case 0: clir = ""; break;   /*subscription default*/
    }

    //生产ATD电话拨号请求AT命令字符串
    asprintf(&cmd, "ATD%s%s;", p_dial->address, clir);

    ret = at_send_command(cmd, NULL);

    free(cmd);

    /* success or failure is ignored by the upper layer here.
       it will call GET_CURRENT_CALLS and determine success that way */
    RIL_onRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
}
~~~

上面的代码也验证了onRequest函数中的处理逻辑非常简单，我们记住两点即可：将RIL请求转化为对应AT命令的执行和调用RIL_onRequestComplete函数完成请求处理结果的返回。另外RIL_onRequestComplete的函数调用其中还有一个宏定义的转换过程，相关的宏定义详情如下：

~~~c
//@reference-ril.c
//path hardware/ril/reference-ril/reference-ril.c
#define RIL_onRequestComplete(t, e, response, responselen) s_rilenv->OnRequestComplete(t,e, response, responselen)
#define RIL_onUnsolicitedResponse(a,b,c) s_rilenv->OnUnsolicitedResponse(a,b,c)
#define RIL_requestTimedCallback(a,b,c) s_rilenv->RequestTimedCallback(a,b,c)
~~~

原来是LibRIL提供的s_rilenv结构体中指向函数的指针，通过这些指针去调用LibRIL中对应的函数。

onRequest函数调用的requestXXX函数共有12个，这些函数中的处理逻辑与requestDial函数非常类似。步骤如下：

1. 首先将请求相关信息组合成AT命令；
2. 调用at_send_command_sms函数通过AT命令通道向Modem发起AT命令；
3. 调用LibRIL提供的函数onReuqestComplete完成RIL请求返回处理结果的回调操作。

 ## 5.3 Unsolicited消息处理流程

在RILC启动过程中，rild.c代码中的main函数通过动态链接库调用Reference-RIL中的RIL_Init函数，从而完成Reference-RIL初始化工作。在这个过程中有一步非常重要，那就是在RIL_Init函数中启动基于mainLoop函数运行的子进程。

**1. mainLoop函数**

mainLoop函数主要负责监听和处理Modem上报的Unsolicited消息，其处理逻辑如下：

~~~c
//@reference-ril.c
//path hardware/ril/reference-ril/reference-ril.c
static void *
mainLoop(void *param)
{
    int fd;
    int ret;

    AT_DUMP("== ", "entering mainLoop()", -1 );
    at_set_on_reader_closed(onATReaderClosed);
    at_set_on_timeout(onATTimeout);

    for (;;) {
        fd = -1;
        
        /*一共有三个处理分支，主要是与Modem建立基于串口的通信连接；在Android4.0源代码中，会建立/dev/socket/qemud端口的		Socket连接，并返回其文件描述符fd*/
        while  (fd < 0) {
            if (s_port > 0) {
                fd = socket_loopback_client(s_port, SOCK_STREAM);
            } else if (s_device_socket) {
                if (!strcmp(s_device_path, "/dev/socket/qemud")) {
                    /* Before trying to connect to /dev/socket/qemud (which is
                     * now another "legacy" way of communicating with the
                     * emulator), we will try to connecto to gsm service via
                     * qemu pipe. */
                    fd = qemu_pipe_open("qemud:gsm");
                    if (fd < 0) {
                        /* Qemu-specific control socket */
                        fd = socket_local_client( "qemud",
                                                  ANDROID_SOCKET_NAMESPACE_RESERVED,
                                                  SOCK_STREAM );
                        if (fd >= 0 ) {
                            char  answer[2];

                            if ( write(fd, "gsm", 3) != 3 ||
                                 read(fd, answer, 2) != 2 ||
                                 memcmp(answer, "OK", 2) != 0)
                            {
                                close(fd);
                                fd = -1;
                            }
                       }
                    }
                }
                else
                    fd = socket_local_client( s_device_path,
                                            ANDROID_SOCKET_NAMESPACE_FILESYSTEM,
                                            SOCK_STREAM );
            } else if (s_device_path != NULL) {
                fd = open (s_device_path, O_RDWR);
                if ( fd >= 0 && !memcmp( s_device_path, "/dev/ttyS", 9 ) ) {
                    /* disable echo on serial ports */
                    struct termios  ios;
                    tcgetattr( fd, &ios );
                    ios.c_lflag = 0;  /* disable ECHO, ICANON, etc... */
                    tcsetattr( fd, TCSANOW, &ios );
                }
            }

            if (fd < 0) {
                perror ("opening AT interface. retrying...");
                sleep(10);
                /* never returns */
            }
        }

        s_closed = 0;
        ret = at_open(fd, onUnsolicited);//开启AT命令通道，at_open函数由atchannel.c提供

        if (ret < 0) {
            LOGE ("AT error %d on at_open\n", ret);
            return 0;
        }

        /*调用RILC的RequestTimedCallback函数，增加定时RIL任务，且回调的函数是initilaizeCallback，因此，RILC直接发起			initilaizeCallback函数调用，从而进行ATE0Q0V1、ATS0等17个AT命令的执行，用于初始化Modem*/
        RIL_requestTimedCallback(initializeCallback, NULL, &TIMEVAL_0);

        // Give initializeCallback a chance to dispatched, since
        // we don't presently have a cancellation mechanism
        sleep(1);

        waitForClose();
        LOGI("Re-opening after close");
    }
}
~~~

上面代码有两个关键点：

- 与Modem建立基于串口的通信连接，同时获取连接的文件描述符fd；
- 调用at_open函数开启AT命令通道。

**2. at_open函数**

进入atchannel.c代码中的at_open函数，此函数的处理逻辑非常简单，可总结出以下几点：

- 保持与Modem建立连接的文件描述符fd，以及接收到Modem发出的AT命令后的回调函数onUnsolicited。
- 启动基于readerLoop函数运行的子进程，readerLoop函数会for循环监听并接收Modem发出的AT命令，其处理逻辑如下：

~~~c
//@atchannel.c
//path hardware/ril/reference-ril/atchannel.c
static void *readerLoop(void *arg)
{
    for (;;) {
        const char * line;

        line = readline();//读取modem发出的AT命令

        if (line == NULL) {
            break;
        }

        if(isSMSUnsolicited(line)) {//接收到新的短信
            char *line1;
            const char *line2;

            // The scope of string returned by 'readline()' is valid only
            // till next call to 'readline()' hence making a copy of line
            // before calling readline again.
            line1 = strdup(line);
            line2 = readline();

            if (line2 == NULL) {
                break;
            }

            if (s_unsolHandler != NULL) {
                s_unsolHandler (line1, line2);//调用onUnsolicited函数
            }
            free(line1);
        } else {
            processLine(line);//处理Modem发出的命令行
        }

#ifdef HAVE_ANDROID_OS
        if (s_ackPowerIoctl > 0) {
            /* acknowledge that bytes have been read and processed */
            ioctl(s_fd, OMAP_CSMI_TTY_ACK, &s_readCount);
            s_readCount = 0;
        }
#endif /*HAVE_ANDROID_OS*/
    }

    onReaderClosed();

    return NULL;
}
~~~

readerLoop函数的处理逻辑可分为两大部分：

- 读取Modem发出的AT命令；
- 根据AT命令进行处理。

AT命令的处理这里分为了两个处理逻辑分支：短信相关AT命令处理逻辑和普通AT命令处理逻辑。

Android手机接收到Modem发出有新短信的AT命令，其文本格式非常固定，共有两行line1和line2，因此它们可以在第一个分支做特殊处理；而调用processLine函数处理普通AT命令，其中也包括了多种处理方式，最典型的如SINGLELINE、MULITILINE和NO_RESULT等。虽然处理方式不同，但它们最终会调用handleUnsolicited函数，而此函数只完成了一种工作，就是调用onUnsolicited函数。

> **注意** readerLoop函数中的两个AT命令处理分支，在最后都会调用onUnsolicited函数。这里做一个猜想，onUnsolicited函数会调用LibRIL提供的函数完成Unsolicited消息的发送。
>
> 通过前面的学习，我们只知道RIL向Modem发起AT命令，在Reference-RIL中也会接收到Modem发出的AT命令：如何区分是谁发给谁的AT命令，要通过Radio日志。
>
> 其中，>符号表示RIL发给Modem的AT命令，而<符号则是Modem发送给RIL的AT命令。

**3. onUnsolicited**

为验证前面的猜想，继续解析onUnsolicited函数的处理逻辑，详情如下：

~~~c
//@reference-ril.c
//path hardware/ril/reference-ril/reference-ril.c
static void onUnsolicited (const char *s, const char *sms_pdu)
{
    char *line = NULL;
    int err;

    /* Ignore unsolicited responses until we're initialized.
     * This is OK because the RIL library will poll for initial state
     */
    if (sState == RADIO_STATE_UNAVAILABLE) {//Radio状态不对，直接返回
        return;
    }

    if (strStartsWith(s, "%CTZV:")) {
        /* TI specific -- NITZ time */
        char *response;

        line = strdup(s);
        at_tok_start(&line);

        err = at_tok_nextstr(&line, &response);

        if (err != 0) {
            LOGE("invalid NITZ line %s\n", s);
        } else {
            /*调用LibRIL提供的OnUnsolicited函数，主动发起RIL_UNSOL_NITZ_TIME_RECEIVED类型的Unsolicited消息通知*/
            RIL_onUnsolicitedResponse (
                RIL_UNSOL_NITZ_TIME_RECEIVED,
                response, strlen(response));
        }
    } else if (strStartsWith(s,"+CRING:")
                || strStartsWith(s,"RING")
                || strStartsWith(s,"NO CARRIER")
                || strStartsWith(s,"+CCWA")
    ) {/*来电、没有Call List等命令，也就是通话状态发生变化，调用RILC提供的OnUnsolicited-Response函数，主动发起				RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED类型的Unsolicited消息通知*/
        RIL_onUnsolicitedResponse (
            RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,
            NULL, 0);
#ifdef WORKAROUND_FAKE_CGEV
        RIL_requestTimedCallback (onDataCallListChanged, NULL, NULL); //TODO use new function
#endif /* WORKAROUND_FAKE_CGEV */
    } else if (strStartsWith(s,"+CREG:")
                || strStartsWith(s,"+CGREG:")
    ) {
        RIL_onUnsolicitedResponse (
            RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED,
            NULL, 0);
#ifdef WORKAROUND_FAKE_CGEV
        RIL_requestTimedCallback (onDataCallListChanged, NULL, NULL);
#endif /* WORKAROUND_FAKE_CGEV */
    } else if (strStartsWith(s, "+CMT:")) {
        RIL_onUnsolicitedResponse (
            RIL_UNSOL_RESPONSE_NEW_SMS,
            sms_pdu, strlen(sms_pdu));
    } else if (strStartsWith(s, "+CDS:")) {
        RIL_onUnsolicitedResponse (
            RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT,
            sms_pdu, strlen(sms_pdu));
    } else if (strStartsWith(s, "+CGEV:")) {
        /* Really, we can ignore NW CLASS and ME CLASS events here,
         * but right now we don't since extranous
         * RIL_UNSOL_DATA_CALL_LIST_CHANGED calls are tolerated
         */
        /* can't issue AT commands here -- call on main thread */
        RIL_requestTimedCallback (onDataCallListChanged, NULL, NULL);
#ifdef WORKAROUND_FAKE_CGEV
    } else if (strStartsWith(s, "+CME ERROR: 150")) {
        RIL_requestTimedCallback (onDataCallListChanged, NULL, NULL);
#endif /* WORKAROUND_FAKE_CGEV */
    }
}
~~~

**4. RIL_onUnsolicitedResponse函数**

进入LibRIL中的RIL_onUnsolicitedResponse函数，此函数的代码量较大，其程序处理框架如下：

~~~c++
//@ril_event.cpp
//path hardware/ril/libril/ril_event.cpp
extern "C"
void RIL_onUnsolicitedResponse(int unsolResponse, void *data,
                                size_t datalen)
{
    int unsolResponseIndex;
    int ret;
    int64_t timeReceived = 0;
    bool shouldScheduleTimeout = false;

    if (s_registerCalled == 0) {
        // Ignore RIL_onUnsolicitedResponse before RIL_register
        LOGW("RIL_onUnsolicitedResponse called before RIL_register");
        return;
    }

    unsolResponseIndex = unsolResponse - RIL_UNSOL_RESPONSE_BASE;

    if ((unsolResponseIndex < 0)
        || (unsolResponseIndex >= (int32_t)NUM_ELEMS(s_unsolResponses))) {
        LOGE("unsupported unsolicited response code %d", unsolResponse);
        return;
    }

    // Grab a wake lock if needed for this reponse,
    // as we exit we'll either release it immediately
    // or set a timer to release it later.
    switch (s_unsolResponses[unsolResponseIndex].wakeType) {
        case WAKE_PARTIAL:
            grabPartialWakeLock();
            shouldScheduleTimeout = true;
        break;

        case DONT_WAKE:
        default:
            // No wake lock is grabed so don't set timeout
            shouldScheduleTimeout = false;
            break;
    }

    // Mark the time this was received, doing this
    // after grabing the wakelock incase getting
    // the elapsedRealTime might cause us to goto
    // sleep.
    if (unsolResponse == RIL_UNSOL_NITZ_TIME_RECEIVED) {
        timeReceived = elapsedRealtime();
    }

    appendPrintBuf("[UNSL]< %s", requestToString(unsolResponse));

    Parcel p;

    p.writeInt32 (RESPONSE_UNSOLICITED);
    p.writeInt32 (unsolResponse);

    ret = s_unsolResponses[unsolResponseIndex]
                .responseFunction(p, data, datalen);
    if (ret != 0) {
        // Problem with the response. Don't continue;
        goto error_exit;
    }

    // some things get more payload
    switch(unsolResponse) {
        case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED:
            p.writeInt32(s_callbacks.onStateRequest());
            appendPrintBuf("%s {%s}", printBuf,
                radioStateToString(s_callbacks.onStateRequest()));
        break;


        case RIL_UNSOL_NITZ_TIME_RECEIVED:
            // Store the time that this was received so the
            // handler of this message can account for
            // the time it takes to arrive and process. In
            // particular the system has been known to sleep
            // before this message can be processed.
            p.writeInt64(timeReceived);
        break;
    }

    ret = sendResponse(p);
    if (ret != 0 && unsolResponse == RIL_UNSOL_NITZ_TIME_RECEIVED) {

        // Unfortunately, NITZ time is not poll/update like everything
        // else in the system. So, if the upstream client isn't connected,
        // keep a copy of the last NITZ response (with receive time noted
        // above) around so we can deliver it when it is connected

        if (s_lastNITZTimeData != NULL) {
            free (s_lastNITZTimeData);
            s_lastNITZTimeData = NULL;
        }

        s_lastNITZTimeData = malloc(p.dataSize());
        s_lastNITZTimeDataSize = p.dataSize();
        memcpy(s_lastNITZTimeData, p.data(), p.dataSize());
    }

    // For now, we automatically go back to sleep after TIMEVAL_WAKE_TIMEOUT
    // FIXME The java code should handshake here to release wake lock

    if (shouldScheduleTimeout) {
        // Cancel the previous request
        if (s_last_wake_timeout_info != NULL) {
            s_last_wake_timeout_info->userParam = (void *)1;
        }

        s_last_wake_timeout_info
            = internalRequestTimedCallback(wakeTimeoutCallback, NULL,
                                            &TIMEVAL_WAKE_TIMEOUT);
    }

    // Normal exit
    return;

error_exit:
    if (shouldScheduleTimeout) {
        releaseWakeLock();
    }
}
~~~

RIL_onUnsolicitedResponse函数的处理逻辑重点关注以下几点：

- 根据Unsolicited Response消息类型获取s_unsolResponse数组中对应的UnsolResponse结构体对象，其中包括此消息电源唤醒策略和Parcel数据处理函数。
- 应用UnsolResponseInfo中的电源管理唤醒策略，进行电源唤醒操作。
- 调用UnsolResponseInfo的Parcel数据处理函数，完成Parcel数据的组织和设置。
- 调用sendResponse函数，通过Socket连接发送Parcel数据给RILJ。




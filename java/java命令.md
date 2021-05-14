# 1 java命令

~~~markdown
1. javap -s *.class //查看方法签名

2. javac *.java //编译生成class文件

3. javah [option] com.example.test
	javah [options] <classes>
	其中, [options] 包括:
	  -o <file>                输出文件 (只能使用 -d 或 -o 之一)
	  -d <dir>                 输出目录
	  -v  -verbose             启用详细输出
	  -h  --help  -?           输出此消息
      -version                 输出版本信息
      -jni                     生成 JNI 样式的标头文件 (默认值)
      -force                   始终写入输出文件
      -classpath <path>        从中加载类的路径
      -cp <path>               从中加载类的路径
      -bootclasspath <path>    从中加载引导类的路径
      <classes> 是使用其全限定名称指定的

~~~

# 2 使用Android.mk

## 2.1 基本配置

~~~groovy
//build.gradle
defaultConfig {
        ...
        ndk{
            abiFilters "arm64-v8a", "armeabi-v7a", "x86","x86_64"
        }
    	...
}
buildTypes {
        externalNativeBuild {
            ndkBuild {
                path 'src/main/jni/Android.mk'
            }
        }
        ...
    }
~~~

~~~markdown
//Android.mk
LOCAL_PATH :=$(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := ndkdemotest-jni
LOCAL_SRC_FILES :=ndkdemotest.c
include $(BUILD_SHARED_LIBRARY)
~~~



## 2.2 Log打印

~~~markdown
//Android.mk
LOCAL_LDLIBS += -llog
~~~

~~~c
#define LOG_TAG "10086 from c"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
~~~

2.3 java对象获取

~~~c
Java_com_example_helloworld_NDKTools_callback(JNIEnv *env, jobject clazz) {
    jclass jclazz = (*env)->FindClass(env,"com/example/helloworld/NDKTools");//获取反射对象
    jobject obj =(*env)->AllocObject(env,jclazz);//获取类的实例对象
    jmethodID jmethodId=(*env)->GetMethodID(env,jclazz,"callFromC", "(Ljava/lang/String;I)V");//获取对象方法
    jmethodID jstaticmethod=(*env)->GetStaticMethodID(env,jclazz,"callFromC2",

    jstring fromC=CStr2Jstring(env,"Yes");
    jstring fromc2=(*env)->NewStringUTF(env,"YES");

    (*env)->CallVoidMethod(env,obj,jmethodId,fromc2,666);//调用对象方法
    (*env)->CallStaticVoidMethod(env,jclazz,jstaticmethod);//调用类方法

}
~~~

~~~markdown
1. native函数（c/c++中的实现）都至少有两个参数，JNEIEnv 和jclass或jobject

2. 第二个参数通常为jclass/jobject类型

	2.1 当native（java中的声明）方法为静态（static）时，第二个参数为jclass代表native方法所属的class对象

	2.2 当native方法为非静态时，第二个参数为jobject代表native方法所属的对象
~~~

## 3 使用cmake

~~~markdown
创建project时选择native C++需要的文件都会自动创建
~~~




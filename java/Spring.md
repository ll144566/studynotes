# Spring动态代理



## 1. 接口

* MethodInterceptor

~~~java
public class Around implements MethodInterceptor {
    @Override
    public Object invoke(MethodInvocation methodInvocation) throws Throwable {
        System.out.println("---MethodInterceptor method before---");
        Object ret = methodInvocation.proceed();
        System.out.println("---MethodInterceptor method after---");
        return false;
    }
}
~~~

* MethodBeforeAdvice

~~~java
public class Before implements MethodBeforeAdvice {
    @Override
    public void before(Method method, Object[] objects, Object o) throws Throwable {
        System.out.println("----method before advice log------");
    }
}
~~~

* 使用

  ~~~xml
      <aop:config>
  <!--        所有的方法都作为切入点，加入额外的功能-->
  <!--        <aop:pointcut id="pc" expression="execution(* *(..))"/>-->
  
  <!--        <aop:pointcut id="pc" expression="execution(* login(..))"/>-->
  
  <!-- -->
  <!--        <aop:pointcut id="pc" expression="execution(* *(com.example.proxy.User))"/>-->
          <aop:pointcut id="pc" expression="args((com.example.proxy.User))"/>
  
  <!--        或者com.example.proxy.UserService.login(..)指定某个类的某个方法-->
  <!--        <aop:pointcut id="pc" expression="execution(* com.example.proxy.UserServiceImpl.login(..))"/>-->
  
  <!--        指定某个类的所有方法-->
  <!--        <aop:pointcut id="pc" expression="execution(* com.example.proxy.UserServiceImpl.*(..))"/>-->
  
  <!--        * 一级包 *. 一级包乃至多级包-->
  <!--        <aop:pointcut id="pc" expression="execution(* *..UserServiceImpl.*(..))"/>-->
  <!--            组装：目的吧切入点与额外的功能整合-->
  <!--        <aop:advisor advice-ref="before" pointcut-ref="pc"/>-->
  
  <!--        两种切入点都可-->
  <!--        <aop:advisor advice-ref="before" pointcut-ref="pc"/>-->
          <aop:advisor advice-ref="around" pointcut-ref="pc"/>
      </aop:config>
  ~~~

  

## 2.切入点函数

* args： 主要用于函数（方法）参数的匹配

  ```xml
  <aop:pointcut id="pc" expression="execution(* *(com.example.proxy.User))"/>
  <aop:pointcut id="pc" expression="args((com.example.proxy.User))"/>
  ```

  

* within 主要用于类，包切入点表达式的匹配

  ~~~xml
  <!--        * 一级包 *. 一级包乃至多级包-->
          <aop:pointcut id="pc" expression="execution(* *..UserServiceImpl.*(..))"/>
          <aop:pointcut id="pc" expression="execution(* com.example.proxy..*(..))"/>
          <aop:pointcut id="pc" expression="within(com.example.proxy..*)"/>
  ~~~

* @annotation 为具有特殊注解的方法添加额外功能

  ~~~xml
  <aop:pointcut id="pc2" expression="@annotation(com.example.proxy.Log)"/>
   <aop:advisor advice-ref="around" pointcut-ref="pc2"/>
  ~~~

* 切入点函数的逻辑运算

  * and与操作

    ```xml
    案例：方法名为login，且同时两个参数
    1. execute(* login(String,String))
    2. execute(* login(..) and args(String,String)
    注意：与操作一般不用于不同类型的切入点函数
    ```

  * or或操作

    ~~~markdown
    案例：register方法和login方法作为切入点
    1. execute(* login(..) or execute(* register(..)
    ~~~

    

# AOP的底层实现原理

## 1.JDK动态代理

~~~markdown
代理创建三要素
1. 原始对象
2. 额外功能
2. 代理对象和原始对象实现相同接口
如果原始对象未实现接口，则代理对象除继承方法外只包含以下三个函数：
1. public final boolean com.sun.proxy.$Proxy0.equals(java.lang.Object)
2. public final java.lang.String com.sun.proxy.$Proxy0.toString()
3. public final int com.sun.proxy.$Proxy0.hashCode()
~~~

~~~java
public class TestJdkProxy {
    public static void main(String[] args) {
        UserService userService=new UserServiceImpl();
        InvocationHandler handler=new InvocationHandler() {
            @Override
            //o为代理对象，一般不用
            public Object invoke(Object o, Method method, Object[] objects) throws Throwable {
                System.out.println("----------proxy log----------");
                Object ret =  method.invoke(userService,objects);
                return ret;
            }
        };
        UserService serviceProxy = (UserService) Proxy.newProxyInstance(TestJdkProxy.class.getClassLoader(), userService.getClass().getInterfaces(), handler);
        serviceProxy.logout();
    }
}
~~~

## 2.CGlib动态代理

~~~markdown
通过继承创建代理对象
~~~

~~~java
public class TestCglib {
    public static void main(String[] args) {
        UserService service=new UserService();
        Enhancer enhancer=new Enhancer();
        enhancer.setClassLoader(TestCglib.class.getClassLoader());
        enhancer.setSuperclass(UserService.class);
        MethodInterceptor interceptor=new MethodInterceptor() {
            @Override
            public Object intercept(Object o, Method method, Object[] objects, MethodProxy methodProxy) throws Throwable {
                System.out.println("------cglib proxy------");
                Object ret=method.invoke(service,objects);
                return ret;
            }
        };
        enhancer.setCallback(interceptor);
        UserService userService = (UserService) enhancer.create();
        userService.logout();
    }
}
~~~

## 3.BeanPostProcessor与动态代理

~~~java
public class ProxyBeanPostProcessor implements BeanPostProcessor {
    @Override
    public Object postProcessBeforeInitialization(Object bean, String beanName) throws BeansException {
        return bean;
    }

    @Override
    public Object postProcessAfterInitialization(Object bean, String beanName) throws BeansException {
        InvocationHandler handler=new InvocationHandler() {
            @Override
            public Object invoke(Object o, Method method, Object[] objects) throws Throwable {
                System.out.println("------ProxyBeanPostProcessor------");
                Object ret =  method.invoke(bean, objects);
                return ret;
            }
        };
        Object o = Proxy.newProxyInstance(ProxyBeanPostProcessor.class.getClassLoader(), bean.getClass().getInterfaces(), handler);
        return o;
    }
}
~~~

~~~xml
   <bean class="com.example.factory.ProxyBeanPostProcessor"/>
   <bean id="userService" class="com.example.proxy.UserServiceImpl"/>
~~~

# 基于注解的AOP编程

## 1.开发步骤

~~~markdown
1. 原始对象
2. 额外功能
3. 切入点
4. 组装切面
~~~

~~~java
@Aspect
public class MyAspect {
    @Around("execution(* login(..))")
    public Object aroundProceedingJoinPoint(ProceedingJoinPoint joinPoint) throws Throwable {
        System.out.println("----aspect log ----");
        return joinPoint.proceed();

    }
}
~~~

~~~xml
   <bean id="userService" class="com.example.aspect.UserServiceImpl"/>
    <bean id="around" class="com.example.aspect.MyAspect"/>
<!--    告知spring基于注解进行AOP编程-->
    <aop:aspectj-autoproxy/>
~~~

## 2.细节

1. 切入点复用

~~~java
@Aspect
public class MyAspect {
    @Pointcut("execution(* login(..))")
    public void myPointCut(){}

    @Around(value = "myPointCut()")
    public Object aroundProceedingJoinPoint(ProceedingJoinPoint joinPoint) throws Throwable {
        System.out.println("----aspect log ----");
        return joinPoint.proceed();

    }
    @Around(value = "myPointCut()")
    public Object around2ProceedingJoinPoint(ProceedingJoinPoint joinPoint) throws Throwable {
        System.out.println("----aspect log2 ----");
        return joinPoint.proceed();

    }
}

~~~

2. 动态代理的创建方式

   

~~~markdown
AOP底层实现又两种代理方式
1. JDK
2. CGlib
默认清空AOP编程底层使用JDK动态代理技术
	<aop:aspectj-autoproxy proxy-target-class="true"/> 注解开发
	<aop:config proxy-target-class="true"> 传统开发
proxy-target-class默认为false
如果切换Cglib proxy-target-class改为true
~~~

## 3.AOP开发中的坑

在同一个业务类中，进行业务间的方法调用，只有代理对象的方法加入了额外功能，原始方法间的互相调用没有额外功能，可以通过实现ApplicationContextAware接口避免这个问题。

~~~java
public class UserServiceImpl implements UserService , ApplicationContextAware {

    private ApplicationContext ctx;
    @Override
    public void register(User user) {
        System.out.println("UserServiceImpl.register 业务运算 +DAO");
        UserService userService = (UserService) ctx.getBean("userService");
        userService.login("","");
    }

    @Override
    public boolean login(String name, String password) {
        System.out.println("UserServiceImpl.login");
        return true;
    }

    @Override
    public void logout() {
        System.out.println("UserServiceImpl.logout");
    }

    @Override
    public void setApplicationContext(ApplicationContext applicationContext) throws BeansException {
        //获取代理对象
        ctx=applicationContext;
    }
}
~~~


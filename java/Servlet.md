~~~markdown
javac -d . -encoding utf-8 -classpath D:\soft\tomcat\apache-tomcat-10.0.5\lib\servlet-api.jar MyServlet.java

javac -encoding utf-8 -classpath D:\soft\tomcat\apache-tomcat-10.0.5\lib\servlet-api.jar MyServlet.java
~~~





~~~xml
<?xml version="1.0" encoding="UTF-8"?>
<!--
 Licensed to the Apache Software Foundation (ASF) under one or more
  contributor license agreements.  See the NOTICE file distributed with
  this work for additional information regarding copyright ownership.
  The ASF licenses this file to You under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with
  the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
-->
<web-app xmlns="https://jakarta.ee/xml/ns/jakartaee"
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xsi:schemaLocation="https://jakarta.ee/xml/ns/jakartaee
                      https://jakarta.ee/xml/ns/jakartaee/web-app_5_0.xsd"
  version="5.0"
  metadata-complete="true">

  <!--创建Servlet标签-->
  <servlet>
  	<servlet-name>my1</servlet-name>
  	<servlet-class>MyServlet</servlet-class>
  </servlet>
  
  <!--创建Servlet映射-->
  <servlet-mapping>
  	<servlet-name>my1</servlet-name>
  	<!--/客户端访问路径 http://localhost:8080/项目名称-->
  	<url-pattern>/my1</url-pattern>
  </servlet-mapping>
</web-app>

~~~





## 1、基本配置-起步



![image-20210512222624771](..\typora-user-images\image-20210512222624771.png)





![image-20210512222846596](..\typora-user-images\image-20210512222846596.png)





![image-20210512223015317](..\typora-user-images\image-20210512223015317.png)



中文乱码

~~~javascript
    public void service(ServletRequest servletRequest, ServletResponse servletResponse) throws ServletException, IOException {
        servletResponse.setContentType("text/html;charset=utf-8");
        PrintWriter writer = servletResponse.getWriter();
        writer.println("这是我的第一个Servlet页面");
    }

~~~





打包war包

![image-20210512225315587](..\typora-user-images\image-20210512225315587.png)





![image-20210512225409914](..\typora-user-images\image-20210512225409914.png)





![image-20210512225454633](..\typora-user-images\image-20210512225454633.png)



![image-20210512225551793](..\typora-user-images\image-20210512225551793.png)

可放在apache-tomcat-xxx\webapps下 然后启动tomcat，war包会自动解压





如果控制台输出乱码

Java -Dfile.encoding=UTF-8

![image-20210513225023837](..\typora-user-images\image-20210513225023837.png)



## 2、Servlet使用

### 2.1、实现Servlet的三种方式

实现Servlet的三种方式
方式一：编写一个类去实现Servlet接口(必须重写Servlet接口里面所有的抽象方法)

方式二：编写一个类去继承GenericServlet抽象类(重写生命周期的service方法（抽象方法）)GenericServle抽象类它实现了Servlet接口，还实现了ServletConfig接口(这个接口中提供了一个getServletContext方法)可以在编写一个类中直接调用getServletContext方法就可以获得ServletContext对象。（开发中不常用）

方式三：编写一个类去继承HttpServlet抽象类(没有抽象方法！根据页面的提交方式决定重写doGet或者doPost方法)

### 2.2、Servlet的两种创建方式

~~~markdown
url-pattern匹配规则

精确匹配    格式：  /具体的名称    只有ur路径是具体的名称时才会触发Servlet

后缀匹配    格式：  *.xxx        只要是以xxx结尾的就匹配触发Servlet

通配符匹配  格式：  /*           匹配所有请求，包含服务器的所有资源

通配符匹配  格式：  /            匹配所有请求，包含服务器的所有资源，不包括.jsp


load-on-startup
1 元素标记容器是否应该在web应用程序启动的时候就加载这个servlet，(实例化并调用其init()方法)。
2 它的值必须是一个整数，表示servlet被加载的先后顺序。
3 如果该元素的值为负数或者没有设置，则容器会当Servlet被请求时再加载。
4 如果值为正整数或者0时，表示容器在应用启动时就加载并初始化这个servlet，值越小，servlet的优先级越高，就越先被加载。值相同时，容   器就会自己选择顺序来加载。
~~~

### 2.3、Servlet的两种配置方式

WebServlet注解

| 属性名         | **类型**       | **描述**                                                     |
| :------------- | :------------- | :----------------------------------------------------------- |
| name           | String         | 指定Servlet 的 name 属性，等价于 <servlet-name>。如果没有显式指定，则该 Servlet 的取值即为类的全限定名。 |
| value          | String[]       | 该属性等价于 urlPatterns 属性。两个属性不能同时使用。        |
| urlPatterns    | String[]       | 指定一组 Servlet 的 URL 匹配模式。等价于<url-pattern>标签。  |
| loadOnStartup  | int            | 指定 Servlet 的加载顺序，等价于 <load-on-startup>标签。      |
| initParams     | WebInitParam[] | 指定一组 Servlet 初始化参数，等价于<init-param>标签。        |
| asyncSupported | boolean        | 声明 Servlet 是否支持异步操作模式，等价于<async-supported> 标签。 |
| description    | String         | 该 Servlet 的描述信息，等价于 <description>标签。            |
| displayName    | String         | 该 Servlet 的显示名，通常配合工具使用，等价于 <display-name>标签。 |

### 2.4、Servlet中文乱码

#### 2.4.1、GET中文乱码

在Toamcat7及以下

客户端以UTF-8的编码传输数据到服务端，而服务器的request对象使用的是ISO-8859-1这个字符编码来接收数据，服务器和客户端沟通的编码不一致导致中文乱码。解决方式是在接收到数据后，先获取request对象以ISO-8859-1字符编码收到的元素数据的字节数组，然后通过字节数组已指定的编码构建字符串，解决乱码问题。

Tomcat8的版本GET基本就不会乱码了

~~~java
String name =new String(request.getParameter("name").getBytes("ISO-8859-1"),"UTF-8");
~~~



#### 2.4.2、POST中文乱码

~~~java
request.setCharacterEncoding("UTF-8");
~~~



#### 2.4.3、Servlet输出中文

浏览器识别不到返回的中文是什么编码格式，就会默认使用GBK2312，如果返回的是UTF-8格式的那么在浏览器上就会显示乱码

~~~java
servletResponse.setContentType("text/html;charset=utf-8");
~~~


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



## 2、实现Servlet的三种方式

实现Servlet的三种方式
方式一：编写一个类去实现Servlet接口(必须重写Servlet接口里面所有的抽象方法)

方式二：编写一个类去继承GenericServlet抽象类(重写生命周期的service方法（抽象方法）)GenericServle抽象类它实现了Servlet接口，还实现了ServletConfig接口(这个接口中提供了一个getServletContext方法)可以在编写一个类中直接调用getServletContext方法就可以获得ServletContext对象。（开发中不常用）

方式三：编写一个类去继承HttpServlet抽象类(没有抽象方法！根据页面的提交方式决定重写doGet或者doPost方法)



## 3、url-pattern匹配规则

### 3.1、精确匹配

格式： /具体的名称 

只有ur路径是具体的名称时才会触发Servlet

### 3.2、后缀匹配

格式：*.xxx

只要是以xxx结尾的就匹配触发Servlet

### 3.3、通配符匹配

格式：/*

匹配所有请求，包含服务器的所有资源



格式：/

匹配所有请求，包含服务器的所有资源，不包括.jsp
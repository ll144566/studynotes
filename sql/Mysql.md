[toc]



# 1、MySql服务的启动和停止

MySQL 8

方式1

计算机右键 -> 管理 -> 计算机管理(本地) -> 服务和应用程序 -> 服务 -> MySQL80

方式2

net start mysql80

# 2、登录MySql

mysql -h localhost -P 3306 -u root -p

mysql  -u root -p

-h -P -u后面可加可不加空格 -p后面如果写密码的话，不能有空格

# 3、mysql常见命令介绍

~~~mysql
SHOW DATABASES; #查看数据库

USE test; #（数据库名）使用数据库

SHOW TABLES；#查看当前表

SHOW TABLES FROM mysql;#查看表

SELECT DATABASE();#查看当前数据库

SELECT VERSION();#查看数据库版本 （mysql --verion 未登录情况下）

DESC departments;# 显示表的结构
~~~



# 4、mysql的语法规范

1. 不区分大小写，但建议关键字大写，表名、列名小写；

2. 每条命令最好用分号结尾

3. 每条命令根据需要，可以进行缩进或换行

4. 注释

   - 单行注释：# 或 --
   - 多行注释：/**/

   

# 5、DQL语言的学习

**数据查询语言**

## 5.1、基础查询

语法：

select 查询列表 from 表名

特点：

1. 查询列表可以是：表中的数据、常量值、表达式、函数
2. 查询的结果是一个虚拟的表格

### 5.1.1、查询表中的单个字段

~~~mysql
SELECT last_name FROM employees;
~~~

### 5.1.2、查询多个字段

~~~mysql
SELECT last_name,salary,email FROM employees;
~~~

<font color = red>注意：select `name` from employees; 其中name为关键字，需要加上``</font>

### 5.1.3、查询常量值

~~~mysql
SELECT 100；

SELECT ‘Tom’；
~~~

### 5.1.4、查询表达式

~~~mysql
SELECT 100%98；
~~~

### 5.1.5、查询函数

~~~mysql
SELECT VERSION();
~~~

### 5.1.6、起别名

**方式1**

~~~mysql
SELECT 100%98 AS 结果;
SELECT last_name AS 姓名 ,salary AS 工资,email AS 邮箱 FROM employees;

SELECT IFNULL(commission_pct,0) AS 奖金率,commission_pct FROM employees;
~~~

**方式2**

~~~mysql
SELECT last_name 姓名 ,salary 工资,email 邮箱 FROM employees;
SELECT salary "out put" FROM employees; # 特殊符号需要加引号 单双引号皆可
~~~

### 5.1.7、去重

~~~mysql
SELECT DISTINCT department_id FROM employees; # 只能查询一个字段
~~~

### 5.1.7、+的作用

<font color = red>mysql中的加号仅仅只有一个功能：**运算符**</font>

~~~mysql
SELECT 100 + 90; #两个操作数都为数值型，则作加法运算
SELECT '123' + 90;#其中一方为字符型，试图将字符型数值转换为数值型，如果转换成功，则继续作加法运算，如果转换失败，则将字符值转换为0
SELECT 'Tom' + 90;

SELECT NULL + 90;#只要一方为null，则结果肯定为null
~~~

~~~mysql
SELECT CONCAT('a', 'b', 'c') AS 结果;# 使用concat连接

SELECT CONCAT(first_name,'-',last_name) AS `name` FROM employees;
~~~

## 5.2、条件查询

**语法**

SELECT 查询列表 FROM 表名 WHERE 筛选条件

执行顺序 from  where  select

**分类**

1. 按条件表达式查询

   条件运算符： >    <    =    !=     <>    >=    <=

2. 按逻辑表达式分类
   逻辑运算符：&&    ||    ！    and    or    not    

3. 模糊查询
   
   - like
      一般和通配符搭配使用 % 任意多字符（包含0个） _单个字符 无法匹配null
   - between and
   - in 列表的值必须一致或兼容，不能使用通配符
   - is null

### 5.2.1、按条件表达式查询

~~~mysql
# 案例 查询工资大于12000的缘故信息
SELECT 
    *
FROM
    employees
WHERE
    salary > 1200;
    
# 案例 查询部门编号不等于90的员工名和部分编号

SELECT 
    department_id, last_name
FROM
    employees
WHERE
    department_id <> 90; 
~~~

### 5.2.2、按逻辑表达式查询

~~~mysql
# 案例 查询工资在10000到20000之间的员工名、工资以及奖金
SELECT 
    last_name, salary, commission_pct
FROM
    employees
WHERE
    salary >= 10000 AND salary <= 20000;
    
    
# 案例 查询部门编号不是90到110之间，或者工资高于15000的员工信息
SELECT 
    *
FROM
    employees
WHERE
    department_id < 90
        OR department_id > 100
        OR salary > 15000;
~~~

### 5.2.3、模糊查询

#### 5.2.3.1、like

~~~mysql
# 案例 查询员工名字中包含字符a的员工信息
SELECT 
    *
FROM
    employees
WHERE
    last_name LIKE '%a%';
    
# 案例 查询员工名中第二个字符为_的员工

SELECT 
    *
FROM
    employees
WHERE
    last_name LIKE '_\_%';
    
    
SELECT 
    *
FROM
    employees
WHERE
    last_name LIKE '_$_%' ESCAPE '$';  # ESCAPE指定转义字符
~~~

#### 5.2.3.2、between and

~~~mysql
# 案例 查询员工编号在100到120之间的员工信息
SELECT 
    *
FROM
    employees
WHERE
    employee_id >= 100
        AND employee_id <= 200
        
SELECT 
    *
FROM
    employees
WHERE
    employee_id BETWEEN 100 AND 200; # between and包含区间，
    
    
# 案例 查询员工编号不在100到120之间的员工信息    
SELECT 
    *
FROM
    employees
WHERE
    employee_id NOT BETWEEN 100 AND 200; 
~~~

#### 5.2.3.3、in

~~~mysql
# 案例 查询员工的工种编号是IT_PROG、AD_VP、AD_PRES中的一个员工名和工种编号
SELECT 
    last_name, job_id
FROM
    employees
WHERE
    job_id = 'IT_PROG' OR job_id = 'AD_VP'
        OR job_id = 'AD_PRES';
        
        
SELECT 
    last_name, job_id
FROM
    employees
WHERE
    job_id IN ('IT_PROG' , 'AD_VP', 'AD_PRES');
~~~

#### 5.2.3.4、is null

~~~~mysql
# 案例 查询没有奖金的员工和奖金率

#错误
SELECT 
    last_name, commission_pct
FROM
    employees
WHERE
    commission_pct = NULL;  # 等号不能判断null
    
# 正确    
SELECT 
    last_name, commission_pct
FROM
    employees
WHERE
    commission_pct IS NULL;
~~~~

#### 5.2.3.4、安全等于



~~~mysql
# 案例
SELECT 
    last_name, commission_pct
FROM
    employees
WHERE
    commission_pct <=> NULL; 
~~~

<font color = "red"><=>不仅能判断null 还能判断普通数值</font>

<font color = "red">is null 仅可以判断null值</font>

<font color = "red">= 仅可以判断非null值</font>



## 5.3、排序查询

**语法**

select 查询列表 from 表 【where 筛选条件】 order by 排序列表 【asc|desc】

**特点**

1. asc代表的是升序，desc代表的是降序，不写默认是升序；
2. order by子句中可以支持单个字段、多个字段、函数、别名；
3. order by子句中一般是放在查询语句的最后面，limit子句除外。

~~~mysql
# 案例 查询员工信息，要求工资从高到低排序
SELECT 
    *
FROM
    employees
ORDER BY salary DESC;


# 案例 查询部门编号>=90的员工信息，按入职时间的先后进行
SELECT 
    *
FROM
    employees
WHERE
    employee_id >= 90
ORDER BY hiredate ASC;


# 案例 按年薪的高低显示员工的信息和年薪（按表达式）
SELECT 
    *, salary * 12 * (1 + IFNULL(commission_pct, 0)) AS 年薪
FROM
    employees
ORDER BY salary * 12 * (1 + IFNULL(commission_pct, 0)) DESC;


# 案例 按年薪的高低显示员工的信息和年薪（按别名）
SELECT 
    *, salary * 12 * (1 + IFNULL(commission_pct, 0)) AS 年薪
FROM
    employees
ORDER BY 年薪 DESC;


# 案例 按姓名的长度显示员工的姓名和工资（按函数排序）
SELECT 
    LENGTH(last_name), last_name, salary
FROM
    employees
ORDER BY LENGTH(last_name) DESC;


# 案例 查询员工信息，要求先按工资排序，再按员工编号排序
SELECT 
    *
FROM
    employees
ORDER BY salary ASC , employee_id DESC;
~~~

## 5.4、常见函数

**概念**

类似于java的方法，将一组逻辑函数封装在方法体中，对外暴露方法名

调用 select 函数名（实参列表）【from 表】

**分类**

1. 单行函数
   如concat、length、ifnull
2. 分组函数（又称统计函数、聚合函数）

### 5.4.1、单行函数

#### 5.4.1.1、字符函数

1、length 获取参数值的字节个数

~~~mysql
SELECT LENGTH("jhon");
SELECT LENGTH("哈哈");
~~~

2、concat 拼接字符串

~~~mysql
SELECT CONCAT('q', 'll');


SELECT 
    CONCAT(last_name, '_', first_name)
FROM
    employees;
    
SELECT 
    CONCAT(last_name,
            ' earns ',
            salary,
            ' monthly but wants ',
            salary * 3) 'dream salary'
FROM
    employees;
~~~

3、upper lower

~~~mysql
# 案例 将姓大写，名小写，然后拼接
SELECT CONCAT('q', 'll');
SELECT 
    CONCAT(UPPER(last_name), '_', LOWER(first_name))
FROM
    employees;
~~~

4、substr（或substring）

~~~mysql
# 索引从1开始
# 截取指定索引及后面所有字符串
SELECT SUBSTR('李莫愁爱上了陆展元', 7) out_put;


#截取指定索引处后面指定长度的字符
SELECT SUBSTR('李莫愁爱上了陆展元', 1,3) out_put;


# 姓名中首字符大写，其它字母小写，然后使用_拼接显示
SELECT 
    CONCAT(UPPER(SUBSTR(last_name, 1, 1)),
            LOWER(SUBSTR(last_name, 2)),
            '_',
            LOWER(first_name))
FROM
    employees;
~~~

5、instr

~~~mysql
# 返回子串第一次出现的索引，如果不存在返回0
SELECT 
    INSTR('杨不悔爱上了殷六侠',
            '殷六侠') out_put;
~~~

6、trim

~~~mysql
# 默认去除两边的空格
SELECT LENGTH(TRIM('    张翠山    ')) out_put;

#去除指定字符
SELECT TRIM('a' FROM 'aaaaaa张aa翠山aaaaaaaa') out_put;
~~~

7、lpad rpad

~~~mysql
#用指定的字符左（右）填充指定长度
SELECT LPAD('殷素素', 10, '*') out_put;

#若指定长度小于指定字符串则截取
SELECT RPAD('殷素素', 2, '*') out_put;
~~~

8、replace

~~~mysql
SELECT REPLACE('aabbccdd', 'bc', 'kk') out_put;
~~~

#### 5.4.1.2、数学函数

1、round 四舍五入

~~~mysql
#保留整数
SELECT ROUND(-1.55);

#保留指定位数
SELECT ROUND(1.569, 2);
~~~

2、ceil 向上取整

~~~mysql
# 返回>=该参数的最小整数
SELECT CEIL(1.1);

SELECT CEIL(-1.1);
~~~

3、floor向下取整

~~~mysql
SELECT FLOOR(-9.9);
SELECT FLOOR(9.9);
~~~

4、truncate截断

~~~mysql
#不会四舍五入
select truncate(1.69,1);
~~~

5、 mod取余

~~~mysql
SELECT MOD(- 10, 3);
SELECT MOD(10, 3);
~~~

6、rand

~~~mysql
SELECT RAND()
~~~



#### 5.4.1.3、日期函数

1、now 返回当前系统日期加时间

~~~mysql
SELECT NOW();
~~~

2、curdate返回当前系统日期

~~~mysql
SELECT CURDATE();
~~~

3、curtime返回当前时间

~~~mysql
SELECT CURTIME();
~~~

4、获取指定年、月、日、时、分、秒

~~~mysql
SELECT YEAR(NOW());
SELECT YEAR('1949-10-1');
SELECT YEAR(CURDATE());
SELECT 
    YEAR(hiredate) 年
FROM
    employees;
    
select month(now());
select monthname(now()); //返回英文月份
~~~

5、str_to_date:将日期个数的字符转换成指定格式的日期

~~~mysql
SELECT STR_TO_DATE('9-12-1999', '%m-%d-%Y');
SELECT STR_TO_DATE('9*12 1999', '%d*%m %Y');
~~~

| 序号 | 格式符 | 功能                |
| ---- | ------ | ------------------- |
| 1    | %Y     | 四位的年份          |
| 2    | %y     | 2位的年份           |
| 3    | %m     | 月份（01，02...12） |
| 4    | %c     | 月份（1，2...12）   |
| 5    | %d     | 日（01，02，..）    |
| 6    | %H     | 小时（24小时制）    |
| 7    | %h     | 小时（12小时制）    |
| 8    | %i     | 分钟（00，01..59）  |
| 9    | %s     | 秒（00，01...59）   |

6、date_format

~~~mysql
select date_format(now(),'%Y年%m月%d日') 时间;

# 查询有奖金的员工名和入职日期(**月/**年 **日)
SELECT 
    last_name, DATE_FORMAT(hiredate, '%m月/%d日 %y年')
FROM
    employees;
~~~

#### 5.4.1.4、其它函数

~~~mysql
SELECT VERSION();
SELECT DATABASE();
SELECT USER();
~~~

#### 5.4.1.5、流程控制函数

1、if函数：if else的效果

~~~mysql
SELECT IF(10 < 5, '小', '大');

SELECT 
    last_name,
    IF(commission_pct IS NULL,
        '没奖金，呵呵',
        salary * 12 * commission_pct) 奖金
FROM
    employees;
~~~

2、case函数

(1) switch case的效果

**语法**

case 要判断的字段或表达式

when 常量1 then 要显示的值1或语句1

when 常量1 then 要显示的值1或语句1

else  要显示的值n或语句n

end

~~~mysql
# 查询员工的工资，部门编号和新工资。要求
# 部门编号=30 显示的工资为当前工资*1.1
# 部门编号=40 显示的工资为当前工资*1.2
# 部门编号=50 显示的工资为当前工资*1.3
# 其它部门 显示的工资为当前工资
SELECT 
    salary,
    department_id,
    CASE department_id
        WHEN 30 THEN salary * 1.1
        WHEN 40 THEN salary * 1.2
        WHEN 50 THEN salary * 1.3
        ELSE salary
    END 新工资
FROM
    employees;
~~~

(2) 多重if效果

**语法**

case

when 条件1 then 要显示的值1或语句1

when 条件2then 要显示的值1或语句2

else  要显示的值n或语句n

end

~~~mysql
# 查询员工的工资工资情况
# 工资>20000，显示A级别
# 工资>15000，显示B级别
# 工资>10000，显示C级别
# 其它，显示D级别
SELECT 
    last_name,
    salary,
    CASE
        WHEN salary > 20000 THEN 'A级别'
        WHEN salary > 15000 THEN 'B级别'
        WHEN salary > 10000 THEN 'C级别'
        ELSE 'D级别'
    END 工资级别
FROM
    employees;
~~~

### 5.4.2、分组函数

**功能**

用作统计使用，又称聚合函数或统计函数或组函数

**分类**

sun 求和、avg 平均值、max最大值、min 最小值、count 计算个数

#### 5.4.2.1、简单的使用

~~~mysql
SELECT 
    SUM(salary) 总工资,
    AVG(salary) 平均工资,
    MAX(salary) 最高工资,
    MIN(salary) 最低工资,
    COUNT(salary) 参与统计人数
FROM
    employees;
~~~

#### 5.4.2.1、参数支持类型

1. sum、avg一般用于处理数值型，max、min、count可以处理任何类型

2. 以上分组函数忽略null值

3. 可以和distinct搭配实现去重的运算

   ~~~mysql
   SELECT 
       SUM(DISTINCT (salary)), SUM(salary)
   FROM
       employees;
       
   SELECT 
       COUNT(DISTINCT (salary)), COUNT(salary)
   FROM
       employees;
   ~~~

4. count函数的详细介绍

   ~~~mysql
   SELECT 
       COUNT(commission_pct)
   FROM
       employees;
       
       
   SELECT 
       COUNT(*)
   FROM
       employees;
       
       
   SELECT 
       COUNT(1)
   FROM
       employees;
       
   ~~~

   <font color = "red">一般使用count(*), count(字段)会忽略null值</font>

   

   **效率：**

   - MYISAM存储下，count(*)的效率最高

   - INNODB存储引擎下，count(*)和count(1)差不多，比count(字段)要高一些
   
5. 和分组函数一同查询的字段要求是group by后的字段

6. 练习

   ~~~mysql
   # 案例 查询员工表中的最大入职时间和最小入职时间的相差天数
   SELECT 
       DATEDIFF(MAX(hiredate), MIN(hiredate)) DIFFERENCE
   FROM
       employees;
   
   
   # 案例 查询部门编号为90的员工个数
   SELECT 
       COUNT(*)
   FROM
       employees
   WHERE
       department_id = 90; 
   ~~~


## 5.5、分组查询

**语法**

select 分组函数，列（要求出现在group by的后面）

from 表

【where 筛选条件】

group by 分组的列表

【order by 子句】

**注意**

查询列表必须特殊，要求是分组函数和group by后出现的字段

**特点**

1. 和分组函数一同查询的字段必须是group by后出现的字段
2. 筛选分为两类：分组前筛选和分组后筛选
3. 分组可以按单个字段也可以按多个字段
4. 可以搭配着排序使用

**问题**

问题1：分组函数做筛选能不能放在where后面
答：不能

问题2：where——group by——having

一般来讲，能用分组前筛选的，尽量使用分组前筛选，提高效率

### 5.5.1、简单分组查询

~~~mysql
# 查询每个部门的平均工资


# 案例 查询每个工种的最高工资
SELECT 
    job_id, MAX(salary)
FROM
    employees
GROUP BY job_id;


# 案例 查询每个位置上的部门个数
SELECT 
    location_id, COUNT(*)
FROM
    departments
GROUP BY location_id;

~~~

### 5.5.2、添加筛选条件

~~~mysql
# 案例 查询邮箱中包含a字符的每个部门的平均工资
SELECT 
    department_id, AVG(salary)
FROM
    employees
WHERE
    email LIKE '%a%' # 也可写INSTR(email, 'a') != 0
GROUP BY department_id;


# 案例 查询有奖金的每个领导手下员工的最高工资
SELECT 
    manager_id, MAX(salary)
FROM
    employees
WHERE
    NOT commission_pct IS NULL # 也可写 commission_pct IS NOT NULL
GROUP BY manager_id;
~~~

### 5.5.3、添加复杂筛选条件

<font color = "red">having中的字段要在select中出现过，不然不然无法识别</font>

~~~mysql
# 案例 查询哪个部门的员工数大于2
SELECT 
    department_id, COUNT(*) 总数
FROM
    employees
GROUP BY department_id
HAVING 总数 > 2;


# 案例 查询每个工种有奖金的员工工资大于10000的工种编号和最高工资
SELECT 
    job_id, MAX(salary) 最高工资
FROM
    employees
WHERE
    commission_pct IS NOT NULL
GROUP BY job_id
HAVING 最高工资 > 10000;


# 案例 查询领导编号大于102的每个领导手下的最低工资大于5000的领导编号
SELECT 
    manager_id, MIN(salary) 最低工资
FROM
    employees
WHERE
    manager_id > 102
GROUP BY manager_id
HAVING 最低工资 > 5000;
~~~

### 5.5.4、按表达式或函数分组

~~~mysql
# 案例 以员工名长度分类，并不同长度的数量
SELECT 
    LENGTH(last_name) 姓名长度, COUNT(*) 数量
FROM
    employees
GROUP BY 姓名长度;


# 案例 以员工名长度分类，并筛选数量大于5的，
SELECT 
    LENGTH(last_name) 姓名长度, COUNT(*) 数量
FROM
    employees
GROUP BY 姓名长度
HAVING 数量 > 5;
~~~

### 5.5.5、按多个字段分组

~~~mysql
# 案例 查询每个部门每个工种的员工的平均工资
SELECT 
    department_id, job_id, AVG(salary)
FROM
    employees
GROUP BY department_id , job_id;  # department_id , job_id顺序可更换
~~~

### 5.5.6、添加排序

~~~mysql
# 案例 查询每个部门每个员工的平均工资大于10000元的，并且按平均工资的高低显示（排除部门id为null）
# department_id 可在分组前查询，也可在分组后查询，推荐前者

# 第一种方法
SELECT 
    department_id, job_id, AVG(salary) 平均工资
FROM
    employees
WHERE
    department_id IS NOT NULL
GROUP BY department_id , job_id
HAVING 平均工资 > 10000
ORDER BY AVG(salary) DESC;

# 第二种方法
SELECT 
    department_id, job_id, AVG(salary) 平均工资
FROM
    employees
GROUP BY department_id , job_id
HAVING department_id IS NOT NULL
    AND 平均工资 > 10000
ORDER BY AVG(salary) DESC;
~~~

## 5.6、连接查询

**含义**

又称多表查询，当查询的字段来自于多个表时就会用到连接查询

**笛卡尔乘积现象**

表1 有m行，表2 有n行，结果 = m*n行

**发生原因**

没有有效的连接条件

**避免方法**

添加有效的连接条件

**分类**

1. 按年代分类
   sql92标准：仅仅支持内连接
   sql99标准【推荐】：支持内连接、外连接（左外连接和右外连接）+交叉连接
2. 按功能分类
   - 内连接
     等值连接、非等值连接、自连接
   - 外连接
     左连接、右连接、全外连接
   - 交叉连接

### 5.6.1、sql92标准

#### 5.6.1.1、等值连接

**特点**

1. 多表等值连接的结果为多表的交集部分
2. n表连接，至少需要n-1个连接条件
3. 多表的顺序没有要求
4. 可以为表取别名
5. 可以搭配前面接收的所有子句使用，比如排序、分组、筛选

##### 5.6.1.1.1 基础查询

~~~mysql
# 案例 查询女神名和对应的男神名
SELECT 
    `name`, boyname
FROM
    boys,
    beauty
WHERE
    boyfriend_id = boys.id;
    
    
# 案例 查询员工名和对应的部门名
SELECT 
    last_name, department_name
FROM
    departments,
    employees
WHERE
    departments.department_id = employees.department_id;
~~~

##### 5.6.1.1.2、为表取别名

<font color = "red">如果为表取了别名，则查询的字段就不能使用原来的表名去限定</font>

~~~mysql
# 案例 查询员工名、工种号、工种名
SELECT 
    first_name, e.job_id, job_title #为表起了别名 这里只能使用别名e 或 j
FROM
    employees e,
    jobs j
WHERE
    e.job_id = j.job_id;
~~~

##### 5.6.1.1.2、两个表的顺序可以互换

##### 5.6.1.1.4、加筛选

~~~mysql
# 案例 查询有奖金的员工名、部门名
SELECT 
    last_name, department_name, commission_pct
FROM
    employees e,
    departments d
WHERE
    e.department_id = d.department_id
        AND commission_pct IS NOT NULL;
        
        
# 案例 查询城市名中第二个字符为o的部门名和城市名
SELECT 
    department_name, city
FROM
    departments d,
    locations l
WHERE
    d.location_id = l.location_id
        AND city LIKE '_o%';
~~~

##### 5.6.1.1.5、加分组

~~~mysql
# 案例 查询每个城市的部门个数
SELECT 
    city, COUNT(*)
FROM
    locations l,
    departments d
WHERE
    l.location_id = d.location_id
GROUP BY city;

# 案例 查询有奖金的每个部门的部门名和部门的领导编号和该部门的最低工资
SELECT 
    department_name, d.manager_id, MIN(salary)
FROM
    departments d,
    employees e
WHERE
    d.department_id = e.department_id
        AND commission_pct IS NOT NULL
GROUP BY d.department_id; 
~~~

##### 5.6.1.1.6、排序

~~~mysql
# 案例 查询每个工种的工种名和员工的个数，并且按员工个数排序
SELECT 
    job_title, COUNT(*) total
FROM
    jobs j,
    employees e
WHERE
    j.job_id = e.job_id
GROUP BY job_title
ORDER BY total DESC;  
~~~

##### 5.6.1.1.6、三表连接

~~~mysql
# 案例 查询员工名、部门名和所在的城市
SELECT 
    last_name, department_name, city
FROM
    employees e,
    departments d,
    locations l
WHERE
    e.department_id = d.department_id
        AND d.location_id = l.location_id;
~~~

#### 5.6.1.2、非等值连接

~~~mysql
# 案例 查询员工的工资和工资级别
SELECT 
    salary, grade_level
FROM
    employees e,
    job_grades jg
WHERE
    salary BETWEEN lowest_sal AND highest_sal;
~~~

#### 5.6.1.3、自连接

~~~mysql
# 案例 查询员工名和上级的名字

#员工和员工的领导都在员工表里
SELECT 
    e.last_name 员工名, l.last_name 领导名
FROM
    employees e,
    employees l
WHERE
    e.manager_id = l.employee_id
~~~

### 5.6.2 、sql99标准

**语法**

select 查询列表

from 表1 别名 【连接类型】

join 表2 别名

on 连接条件

【where 筛选条件】

【group by 分组】

【having 筛选条件】

【order by 排序列表】

**分类**

1. 内连接 inner

2. 外连接 （outer可省略）
   - 左外：left outer
   - 右外： right outer
   - 全外：full outer
3. 交叉连接：cross

#### 5.6.2.1、内连接

**语法**

select 查询列表

from 表1 别名

inner join 表2 别名

on 连接条件

**分类**

1. 等值连接
2. 非等值连接
3. 自连接

**特点**

1. 可添加排序、分组、筛选
2. inner可省略
3. 筛选条件放在where、后面，连接条件放在on后面，提高分离性，便于阅读
4. inner join连接和sql92语法中的等值连接效果是一样的，都是查询多表的交集

##### 5.6.2.1.1、等值连接

~~~mysql
# 案例 查询员工名、部门名
SELECT 
    last_name, department_name
FROM
    employees e
        INNER JOIN
    departments d ON e.department_id = d.department_id;
    
    
# 案例 查询名字中包含e的员工名和工种名 （添加筛选）
SELECT 
    last_name, job_title
FROM
    employees e
        INNER JOIN
    jobs j ON e.job_id = j.job_id
WHERE
    last_name LIKE '%e%';
    
 
# 案例 查询部门个数>3的城市名和部门个数 （分组+筛选）
SELECT 
    city, COUNT(*) 部门个数
FROM
    locations l
        INNER JOIN
    departments d ON l.location_id = d.location_id
GROUP BY city
HAVING 部门个数 > 3;


# 案例 查询部门员工数大于2的部门名和员工个数，并按个数排序 （添加排序）
SELECT 
    department_name, COUNT(*) 部门员工数
FROM
    employees e
        INNER JOIN
    departments d ON e.department_id = d.department_id
GROUP BY department_name
HAVING 部门员工数 > 3
ORDER BY 部门员工数 DESC;


# 案例 查询员工名、部门名、工种名，并按部门名排序
SELECT 
    last_name, department_name, job_title
FROM
    employees e
        INNER JOIN
    departments d ON e.department_id = d.department_id
        INNER JOIN
    jobs j ON e.job_id = j.job_id
ORDER BY department_name DESC;
~~~

##### 5.6.2.1.2、非值连接

~~~mysql
# 案例 查询员工的工资和工资级别
SELECT 
    last_name, grade_level
FROM
    employees e
        JOIN
    job_grades jg ON salary BETWEEN lowest_sal AND highest_sal; 
    
# 案例 查询工资级别个数大于2的，并且按工资级别降序
SELECT 
    grade_level, COUNT(*) 总数
FROM
    employees e
        JOIN
    job_grades jg ON salary BETWEEN lowest_sal AND highest_sal
GROUP BY grade_level
HAVING 总数 > 2
ORDER BY grade_level DESC;
~~~

##### 5.6.2.1.3、自连接

~~~mysql
# 案例 查询员工名和上级的名字
SELECT 
    e.last_name, l.last_name
FROM
    employees e
        JOIN
    employees l ON e.manager_id = l.employee_id;
    
# 案例 查询姓名中包含j的员工的名字、上级的名字
SELECT 
    e.last_name, l.last_name
FROM
    employees e
        JOIN
    employees l ON e.manager_id = l.employee_id
WHERE
    e.last_name LIKE '%k%';
~~~

#### 5.6.2.2、外连接

**应用场景**

用于查询一个表中有，另一个表中没有的记录

**特点**

1. 外连接的查询结果为主表中的所有记录，如果从表中有何它匹配的，则显示匹配的值，如果从表中没有和他匹配的，则显示null。筛选过后，外连接的查询结果 = 内连接查询结果+主表中有而从表中没有的记录。
2. 左外连接中，left join左边的是主表，右外连接中right join右边的是主表。
3. 左外和右外交换两个表的顺序，可以实现同样的效果。
4. 全外连接=内连接+表1中有但表2中没有+表2中有但表1中没有

~~~mysql
# 案例 查询男朋友不在男神表的女神名
SELECT 
    `name`, boyfriend_id, boys.*
FROM
    beauty
        LEFT JOIN
    boys ON boyfriend_id = boys.id
WHERE
    boys.id IS NULL; # 此处最好选择主键，主键不为空。因为其它键值可能为空导致查询结果项多余实际结果
    

# 案例 查询哪个部门没有员工
SELECT 
    d.*, employee_id
FROM
    employees e
        RIGHT JOIN
    departments d ON e.department_id = d.department_id
WHERE
    e.employee_id IS NULL;
~~~

<font color = 'red'>思考：下面两个查询结果的异同</font>

~~~mysql
SELECT 
    `name`, boyfriend_id, boys.*
FROM
    beauty
        JOIN
    boys ON boyfriend_id = boys.id;
    
    
SELECT 
    `name`, boyfriend_id, boys.*
FROM
    beauty
        LEFT JOIN
    boys ON boyfriend_id = boys.id; 
~~~

**mysql不支持全外连接**

#### 5.6.2.3、交叉连接

~~~mysql
# 就是笛卡尔乘积
SELECT 
    beauty.*, boys.*
FROM
    beauty
        CROSS JOIN
    boys;
~~~

#### 5.6.2.4、多表连接练习

~~~mysql
# 案例 查询编号>3的女神的男朋友信息，如果有则列出详细，如果没有，用null填充
SELECT 
    beauty.id, beauty.`name`, boys.*
FROM
    beauty
        LEFT JOIN
    boys ON beauty.boyfriend_id = boys.id
WHERE
    beauty.id > 3;
    
# 案例 查询哪个城市没有部门
SELECT 
    city, d.*
FROM
    locations l
        LEFT JOIN
    departments d ON l.location_id = d.location_id
WHERE
    d.department_id IS NULL;
    
    
# 案例 查询部门名为SAL或IT的员工信息
SELECT 
    d.department_name, e.*
FROM
    departments d
        JOIN
    employees e ON d.department_name IN ('SAL' , 'IT');
~~~

## 5.7 子查询

**分类**

1. 按子查询出现的位置
   - select 后面（仅仅支持标量子查询）
   - from后面（支持表子查询）
   - where或having后面★
     - 标量子查询（单行）
     - 列子查询（多行）
     - 行子查询
   - exists后面（相关子查询）
2. 按结果集的行列式不同
   - 标量子查询（结果集只有一行一列）
   - 列子查询（结果集只有一列多行）
   - 行子查询（结果集只有一行多列）
   - 表子查询（结果集一般为多行多列）

### 5.7.1、where或having后面

**特点**

1. 子查询放在小括号内
2. 子查询一般放在条件的右侧
3. 标量子查询，一般搭配单行操作符使用 >  <  >=  <=  <>,列子查询一般搭配多行操作符使用 in、any/some、all
4. 子查询的执行优先于主查询

#### 5.7.1.1、标量子查询

~~~mysql
# 案例 谁的工资比Abel高
SELECT 
    *
FROM
    employees
WHERE
    salary > (SELECT 
            salary
        FROM
            employees
        WHERE
            last_name = 'Abel');
            
            
# 案例 返回job_id与141号员工相同，salary比143号员工多的员工 姓名，job_id和工资
SELECT 
    last_name, job_id, salary
FROM
    employees
WHERE
    job_id <=> (SELECT 
            job_id
        FROM
            employees
        WHERE
            employee_id = '141')
        AND salary > (SELECT 
            salary
        FROM
            employees
        WHERE
            employee_id = '143');
            
            
# 案例 返回工资最少的员工的last_name,job_id,salary;
SELECT 
    last_name, job_id, MIN(salary)
FROM
    employees; # 多个相同最低工资的员工只能显示一个
    
    
SELECT 
    last_name, job_id, salary
FROM
    employees
WHERE
    salary = (SELECT 
            MIN(salary)
        FROM
            employees);
            
# 案例 查询最低工资大于50号部门最低工资的部门id和其最低工资
SELECT 
    department_id, MIN(salary)
FROM
    employees
GROUP BY department_id
HAVING MIN(salary) > (SELECT 
        MIN(salary)
    FROM
        employees
    WHERE
        department_id = '50');
        
SELECT 
    department_id, MIN(salary)
FROM
    employees
GROUP BY department_id
HAVING MIN(salary) > ANY (SELECT 
        salary
    FROM
        employees
    WHERE
        department_id = '50'); # 使用any实现，不是一行一列       
~~~

#### 5.7.1.2、列子查询

使用多行操作符

| 操作符    | 含义                       |
| --------- | -------------------------- |
| IN/NOT IN | 等于表中的任意一个         |
| ANY\|SOME | 和子查询返回的某一个值比较 |
| ALL       | 和子查询返回的所有值比较   |

~~~mysql
# 案例 返回location_id是1400或1700的部门中的所有员工姓名

# 连接查询
SELECT 
    last_name
FROM
    employees e
        JOIN
    departments d ON e.department_id = d.department_id
WHERE
    location_id IN (1400 , 1700);
    
# 列子查询   
SELECT 
    last_name
FROM
    employees
WHERE
    department_id IN (SELECT DISTINCT
            department_id
        FROM
            departments
        WHERE
            location_id IN (1400 , 1700));
            
            
# 案例 返回其它部门中比工种为 `IT_PROG`部门任一工资低的员工的工号、姓名、job_id以及salary
SELECT 
    employee_id, last_name, salary
FROM
    employees
WHERE
    job_id != 'IT_PROG'
        AND salary < ANY (SELECT 
            salary
        FROM
            employees
        WHERE
            job_id = 'IT_PROG')
~~~

#### 5.7.1.3、行子查询

~~~mysql
# 案例 查询员工编号最小并且工资最高的员工信息

# 普通方法
SELECT 
    *
FROM
    employees
WHERE
    employee_id = (SELECT 
            MIN(employee_id)
        FROM
            employees)
        AND salary = (SELECT 
            MAX(salary)
        FROM
            employees);
# 行子查询      
SELECT 
    *
FROM
    employees
WHERE
    (employee_id , salary) = (SELECT 
            MIN(employee_id), MAX(salary)
        FROM
            employees)
~~~

### 5.7.2、select后面的子查询

只能是一行一列，仅支持标量子查询

<font color = "red">下面案例 1中的两个查询方法 连接查询不会显示员工数为0的情况（不符合题意），子查询会显示</font>

~~~mysql
# 案例 1 查询每个部门的员工个数
# 连接查询
SELECT 
    d.department_name, COUNT(*) 个数
FROM
    employees e
        JOIN
    departments d ON e.department_id = d.department_id
GROUP BY department_name;

# 子查询

# 疑问 里面的子查询是根据连接条件分组？
SELECT 
    d.department_name,
    (SELECT 
            COUNT(*)
        FROM
            employees e
        WHERE
            e.department_id = d.department_id) 个数
FROM
    departments d;
    
# 外连接
SELECT 
    department_name, COUNT(employee_id) # 这里不能是count(*) 不然部门人数为0的也会显示1
FROM
    departments d
        LEFT JOIN
    employees e ON d.department_id = e.department_id
GROUP BY d.department_id
    
# 案例 查询员工号=102的部门名
# 方法 1
SELECT 
    (SELECT 
            department_name
        FROM
            departments d
        WHERE
            d.department_id = e.department_id) 部门名
FROM
    employees e
WHERE
    e.employee_id = 102;
    
# 方法 2
SELECT 
    (SELECT 
            department_name
        FROM
            departments d
                JOIN
            employees e ON d.department_id = e.department_id
        WHERE
            employee_id = 102) 部门名;
~~~

### 5.7.3、from后面的子查询

~~~mysql
# 案例 查询每个部门的平均工资的等级
SELECT 
    department_id, grade_level
FROM
    (SELECT 
        AVG(salary) 部门平均, department_id
    FROM
        employees
    GROUP BY department_id) avg_dep
        JOIN
    job_grades ON 部门平均 BETWEEN lowest_sal AND highest_sal;
~~~

### 5.7.4、exists后面的子查询（相关子查询）

会先执行外查询

最终结果只有1和0

~~~mysql
# 案例 查询有员工名的部门名

# 使用内连接查询
SELECT DISTINCT
    department_name
FROM
    departments d
        JOIN
    employees e ON d.department_id = e.department_id;
    
# 使用 in
SELECT 
    department_name
FROM
    departments d
WHERE
    department_id IN (SELECT 
            department_id
        FROM
            employees);
    
# 使用子查询
SELECT 
    department_name
FROM
    departments d
WHERE
    EXISTS( SELECT 
            *
        FROM
            employees e
        WHERE
            d.department_id = e.department_id)
            
# 案例 查询没有女朋友的男神信息
# 连接查询
SELECT 
    boys.*, beauty.`name`
FROM
    boys
        LEFT JOIN
    beauty ON beauty.boyfriend_id = boys.id
HAVING `name` IS NULL;

# in
SELECT 
    boys.*
FROM
    boys
WHERE
    boys.id NOT IN (SELECT 
            boyfriend_id
        FROM
            beauty);
# 连接查询 
SELECT 
    boys.*
FROM
    boys
WHERE
    NOT EXISTS( SELECT 
            boyfriend_id
        FROM
            beauty
        WHERE
            boys.id = boyfriend_id);

~~~

<font color = "red">SELECT 10 NOT IN (1 , 2, NULL);返回值为null</font>



### 5.7.5、练习

~~~mysql
# 1. 查询和Zlotkey相同部门的员工的姓名和工资
SELECT 
    last_name, salary
FROM
    employees
WHERE
    department_id = (SELECT 
            department_id
        FROM
            employees
        WHERE
            last_name = 'Zlotkey');
 
 
# 2. 查询工资比公司平均工资高的员工的员工号，姓名和工资
SELECT 
    employee_id, last_name, salary
FROM
    employees
WHERE
    salary > (SELECT 
            AVG(salary)
        FROM
            employees);
            
            
# 3. 查询各部门中工资比本部门平均工资高的员工的员工号，姓名和工资
SELECT 
    employee_id, last_name, salary
FROM
    employees e
        JOIN
    (SELECT 
        AVG(salary) avg_salary, department_id
    FROM
        employees
    GROUP BY department_id) avg_dep ON e.department_id = avg_dep.department_id
WHERE
    salary > avg_dep.avg_salary; 
    
# 4. 查询和姓名中包含字母u的员工在相同部门的员工的员工号和姓名
SELECT 
    employee_id, last_name
FROM
    employees
WHERE
    department_id IN (SELECT DISTINCT
            department_id
        FROM
            employees
        WHERE
            last_name LIKE '%u%');
            
            
# 5. 查询在部门的location_id为1700的部门工作的员工的员工号
# 使用in
SELECT 
    employee_id
FROM
    employees
WHERE
    department_id IN (SELECT DISTINCT
            department_id
        FROM
            departments
        WHERE
            location_id = 1700);
# 使用any            
SELECT 
    employee_id
FROM
    employees
WHERE
    department_id = ANY (SELECT DISTINCT
            department_id
        FROM
            departments
        WHERE
            location_id = 1700);            

# 6. 查询管理者是King的的员工的姓名和工资

# 子查询
SELECT 
    last_name, salary
FROM
    employees
WHERE
    manager_id IN (SELECT 
            employee_id
        FROM
            employees
        WHERE
            last_name = 'K_ing');
# 连接查询            
SELECT 
    e.last_name, e.salary
FROM
    employees e
        JOIN
    employees l ON e.manager_id = l.employee_id
WHERE
    l.last_name = 'K_ing';     
    
    
# 查询工资最高的员工的姓名，要求first_name和last_name显示为一列列名为姓.名，    
SELECT 
    CONCAT(first_name, '.', last_name) `姓.名`
FROM
    employees
WHERE
    salary = (SELECT 
            MAX(salary)
        FROM
            employees);
~~~



### 5.7.6、子查询经典案例

~~~mysql
# 1. 查询工资最低的员工信息: last_name, salary
SELECT 
    last_name, salary
FROM
    employees
WHERE
    salary = (SELECT 
            MIN(salary)
        FROM
            employees);
  
  
# 2. 查询平均工资最低的部门信息
# 方法1
select last_name,salary from employees where salary = (select min(salary) from employees);

SELECT 
    *
FROM
    departments
WHERE
    department_id = (SELECT 
            department_id
        FROM
            (SELECT 
                AVG(salary) avg_salary, department_id
            FROM
                employees
            GROUP BY department_id
            ORDER BY avg_salary ASC
            LIMIT 1) avg_dep);
# 方法2 
SELECT 
    *
FROM
    departments
WHERE
    department_id = (SELECT 
            department_id
        FROM
            (SELECT 
                AVG(salary) avg_salary, department_id
            FROM
                employees
            GROUP BY department_id
            HAVING avg_salary = (SELECT 
                    MIN(ag)
                FROM
                    (SELECT 
                    AVG(salary) ag
                FROM
                    employees
                GROUP BY department_id) avg_dep)) min_dep);
   
   
# 3. 查询平均工资最低的部门信息和该部门的平均工资
SELECT 
    d.*, min_dep.avg_salary
FROM
    departments d
        JOIN
    (SELECT 
        department_id, avg_salary
    FROM
        (SELECT 
        AVG(salary) avg_salary, department_id
    FROM
        employees
    GROUP BY department_id
    ORDER BY avg_salary ASC
    LIMIT 1) avg_dep) min_dep ON d.department_id = min_dep.department_id;


# 4. 查询平均工资最高的 job 信息
SELECT 
    *
FROM
    jobs
WHERE
    job_id = (SELECT 
            job_id
        FROM
            (SELECT 
                AVG(salary) avg_salary, job_id
            FROM
                employees
            GROUP BY job_id
            ORDER BY avg_salary DESC
            LIMIT 1) max_avg);
            
            
# 5. 查询平均工资高于公司平均工资的部门有哪些? 
# 方法1 （查询时包含了部门信息 ）
SELECT 
    d.*, avg_salary_company, avg_salary_department
FROM
    departments d
        JOIN
    (SELECT 
        AVG(salary) avg_salary_department, department_id
    FROM
        employees
    GROUP BY department_id) avg_department
        JOIN
    (SELECT 
        AVG(salary) avg_salary_company
    FROM
        employees) avg_company ON d.department_id = avg_department.department_id
HAVING avg_salary_department > avg_salary_company; 

# 方法 2 （查询时包含了部门信息 ）
SELECT 
    *
FROM
    departments
WHERE
    department_id IN (SELECT 
            department_id
        FROM
            (SELECT 
                AVG(salary) avg_department, department_id
            FROM
                employees
            GROUP BY department_id
            HAVING avg_department > (SELECT 
                    AVG(salary)
                FROM
                    employees)) avg_filter)

# 方法 3
SELECT 
    AVG(salary), department_id
FROM
    employees
GROUP BY department_id
HAVING AVG(salary) > (SELECT 
        AVG(salary)
    FROM
        employees)


# 6. 查询出公司中所有 manager 的详细信息.
SELECT 
    *
FROM
    employees
WHERE
    employee_id IN (SELECT DISTINCT
            manager_id
        FROM
            employees);
            
            
# 7. 各个部门中 最高工资中最低的那个部门的 最低工资是多少
# 使用limit和排序
SELECT 
    MAX(salary) max_salary
FROM
    employees
GROUP BY department_id
ORDER BY max_salary ASC
LIMIT 1;

#使用子查询
SELECT 
    MIN(max_salary)
FROM
    (SELECT 
        MAX(salary) max_salary
    FROM
        employees
    GROUP BY department_id) max_salary_dep;
    
    
# 8. 查询平均工资最高的部门的 manager 的详细信息: last_name, department_id, email, salary
# 自连接查询
SELECT 
    m.last_name,
    m.department_id,
    m.email,
    m.salary,
    AVG(e.salary) avg_salary_dep
FROM
    employees e
        JOIN
    employees m ON e.manager_id = m.employee_id
GROUP BY m.department_id
ORDER BY avg_salary_dep DESC
LIMIT 1;

# 子查询
SELECT 
    last_name, d.department_id, email, salary 
FROM
    employees e 
    INNER JOIN departments d 
        ON d.manager_id = e.employee_id 
WHERE d.department_id = 
    (SELECT 
        department_id 
    FROM
        employees 
    GROUP BY department_id 
    ORDER BY AVG(salary) DESC 
    LIMIT 1) ;
~~~



## 5.8、分页查询

**应用场景**

当要显示的数据一页显示不全，需要分页提交sql请求

**语法**

select 查询列表

from 表 【join 表2】

where 筛选条件

group by 分组字段

having 分组后的筛选

order by 排序的字段

limit offset, size;

offset要显示的起始索引

size要显示的条目个数

**特点**

1. limit语句放在查询语句最后

~~~mysql
# 案例 查询前五条员工信息
SELECT 
    *
FROM
    employees
LIMIT 0 , 5;


SELECT 
    *
FROM
    employees
LIMIT 5; # 0 可省略


# 案例 查询第11至第25的员工信息
SELECT 
    *
FROM
    employees
LIMIT 10 , 25;


# 案例 查询有奖金的且工资较高的前10名员工信息
SELECT 
    *
FROM
    employees
WHERE
    commission_pct IS NOT NULL
ORDER BY salary DESC
LIMIT 10;
~~~

## 5.9、联合查询

将多条查询语句得结果合并成一个结果

~~~mysql
# 引入得案例：查询部门编号>90或邮箱包含a得员工信息
# 普通查询
SELECT 
    *
FROM
    employees
WHERE
    email LIKE '%a%' OR department_id > 90;
    
# 联合查询    
SELECT 
    *
FROM
    employees
WHERE
    email LIKE '%a%' 
UNION SELECT 
    *
FROM
    employees
WHERE
    department_id > 90
~~~

**应用场景**

 要查询的结果来自于多个表，且多个表没有直接的连接关系，但查询的信息一致时（查询的列一致，比如都查性别） 1、将一条比较复杂的查询语句拆分成多条语句 2、适用于查询多个表的时候，查询的列基本是一致 

**特点**★ 

1. 要求多条查询语句的查询列数是一致的！
2. 要求多条查询语句的查询的每一列的类型和顺序最好一致 
3. union关键字默认去重，如果使用union all 可以包含重复项

# 6、DML语言的学习

**数据操纵语言DML**

## 6.1、插入

### 6.1.1、方式1

**语法**

insert into 表名 (列名) values (值 )

~~~mysql
*/
SELECT * FROM beauty;
#1.插入的值的类型要与列的类型一致或兼容
INSERT INTO beauty(id,NAME,sex,borndate,phone,photo,boyfriend_id)
VALUES(13,'唐艺昕','女','1990-4-23','1898888888',NULL,2);

#2.不可以为null的列必须插入值。可以为null的列如何插入值？
#方式一：
INSERT INTO beauty(id,NAME,sex,borndate,phone,photo,boyfriend_id)
VALUES(13,'唐艺昕','女','1990-4-23','1898888888',NULL,2);

#方式二：

INSERT INTO beauty(id,NAME,sex,phone)
VALUES(15,'娜扎','女','1388888888');


#3.列的顺序是否可以调换
INSERT INTO beauty(NAME,sex,id,phone)
VALUES('蒋欣','女',16,'110');


#4.列数和值的个数必须一致

INSERT INTO beauty(NAME,sex,id,phone)
VALUES('关晓彤','女',17,'110');

#5.可以省略列名，默认所有列，而且列的顺序和表中列的顺序一致

INSERT INTO beauty
VALUES(18,'张飞','男',NULL,'119',NULL,NULL);
~~~

### 6.1.2、方式2

**语法**
insert into 表名
set 列名=值,列名=值,...

~~~mysql
#方式二：
/*

语法：
insert into 表名
set 列名=值,列名=值,...
*/


INSERT INTO beauty
SET id=19,NAME='刘涛',phone='999';
~~~

### 6.1.3、两种方式大pk ★

~~~mysql
#1、方式一支持插入多行,方式二不支持

INSERT INTO beauty
VALUES(23,'唐艺昕1','女','1990-4-23','1898888888',NULL,2)
,(24,'唐艺昕2','女','1990-4-23','1898888888',NULL,2)
,(25,'唐艺昕3','女','1990-4-23','1898888888',NULL,2);

#2、方式一支持子查询，方式二不支持

# 将查询结果插入beauty表
INSERT INTO beauty(id,NAME,phone)
SELECT 26,'宋茜','11809866';

INSERT INTO beauty(id,NAME,phone)
SELECT id,boyname,'1234567'
FROM boys WHERE id<3;
~~~

## 6.2 修改语句

### 6.2.1、修改单表记录

**语法**

update 表名 set 列 = 新值，列 = 新值， ... where 筛选条件

~~~mysql
# 案例 修改beauty表中行唐的女神的电话为13899888899
UPDATE beauty 
SET 
    phone = '13899888899'
WHERE
    NAME LIKE '唐%';
    
# 案例 修改boys表中id为2的名称为张飞，魅力值为10
UPDATE boys 
SET 
    boyName = '张飞',
    userCP = 10
WHERE
    id = 2;
~~~

<font color = "red">mysql update时报错You are using safe update mode</font>

**这是因为MySql运行在safe-updates模式下，该模式会导致非主键条件下无法执行update或者delete命令，**

**执行命令SET SQL_SAFE_UPDATES = 0;修改下数据库模式**

**如果想要提高数据库安全等级，可以在恢复回原有的设置，执行命令：**

```mysql
SET SQL_SAFE_UPDATES = 1;
```

### 6.2.2、修改多表记录

**语法**

**sql92语法**

update 表1 别名，表2 别名

set 列名=值,列名=值,...

where 连接条件

and 筛选条件

**sql99语法**

update 表1 别名，inner|left|right join 表2 别名

on连接条件

set 列名=值,列名=值,...

where 筛选条件



~~~mysql
# 案例 修改张无忌女朋友的手机号为114
# sql99
UPDATE boys
        JOIN
    beauty ON boys.id = boyfriend_id 
SET 
    phone = '114'
WHERE
    boyname = '张无忌';
# sql92    
UPDATE boys,
    beauty 
SET 
    phone = '118'
WHERE
    boys.id = boyfriend_id
        AND boyname = '张无忌';    
        
# 案例 修改没有男朋友的女神的男朋友编号为2号
UPDATE beauty
        LEFT JOIN
    boys ON boys.id = boyfriend_id 
SET 
    boyfriend_id = 2
WHERE
    boys.id IS NULL;
~~~

## 6.3、删除语句

### 6.3.1、delete

#### 6.3.1.1、单表的删除★

**语法**

delete from 表名 limit 1

delete from 表名 where 筛选条件

~~~mysql
# 案例 删除手机号以9结尾的女神信息
DELETE FROM beauty 
WHERE
    phone LIKE '%9';   
~~~

#### 6.3.1.2、多表的删除

**sql92**

delete  别名1 from 表1 别名1 ，表2 别名2 where 连接条件 and 筛选条件

**sql99**

delete  别名1 from 表1 别名1  inner|left|right join 表2 别名2 on 连接条件 where 筛选条件

delete  别名1，别名2  from 表1 别名1  inner|left|right join 表2 别名2 on 连接条件 where 筛选条件 （两个表都删）

~~~mysql
# 案例 删除张无忌的女朋友的信息
# sql99
DELETE beauty FROM beauty
        JOIN
    boys ON boys.id = boyfriend_id 
WHERE
    boyname = '张无忌';

# sql92
DELETE beauty FROM beauty,
    boys 
WHERE
    boys.id = boyfriend_id
    AND boyname = '张无忌';
    
# 案例 删除黄晓明以及他女朋友的信息 
DELETE be , bo FROM beauty be
        JOIN
    boys bo ON bo.id = boyfriend_id
        AND boyname = '黄晓明'
~~~



### 6.3.2、truncate

**语法**

truncate table 表名

~~~mysql
# 将魅力值>100的男神删除

# 错误 truncate 不能添加条件
truncate boys where userCP > 100;
~~~

### 6.3.3、delete 和 truncate的区别

1. delete可以加where条件，truncate不行。
2. truncate删除效率高一丢丢。
3. 如果要删除的表中有自增长列，使用delete删除再插入数据，自增长列的值从断点开始，
   使用truncate删除再插入数据，自增长列的值从1开始。
4. truncate没有返回值，delete会返回删除的行数。
5. truncate删除不能回滚，delete删除可以回滚

## 6.4、测试

### 6.4.1、题目

**1、运行以下脚本创建表 my_employees**

~~~mysql
CREATE TABLE my_employees (
    Id INT(10),
    First_name VARCHAR(10),
    Last_name VARCHAR(10),
    Userid VARCHAR(10),
    Salary DOUBLE(10 , 2 )
);

CREATE TABLE users (
    id INT,
    userid VARCHAR(10),
    department_id INT
)
~~~

**2、显示表 my_employees 的结构**

**3、向 my_employees 表中插入下列数据**

| ID   | FIRST_NAME | LAST_NAME | USERID   | SALARY |
| ---- | ---------- | --------- | -------- | ------ |
| 1    | patel      | Ralph     | Rpatel   | 895    |
| 2    | Dancs      | Betty     | Bdancs   | 860    |
| 3    | Biri       | Ben       | Bbiri    | 1100   |
| 4    | Newman     | Chad      | Cnewman  | 750    |
| 5    | Ropeburn   | Audrey    | Aropebur | 1550   |

**4、向 users 表中插入数据**

| id   | userid   | department_id |
| ---- | -------- | ------------- |
| 1    | Rpatel   | 10            |
| 2    | Bdancs   | 10            |
| 3    | Bbiri    | 20            |
| 4    | Cnewman  | 30            |
| 5    | Aropebur | 40            |

**5、将 3 号员工的 last_name 修改为“drelxer”**
**6、将所有工资少于 900 的员工的工资修改为 1000**
**7、将 userid 为 Bbiri 的 user 表和 my_employees 表的记录全部删除**
**8、删除所有数据**
**9、检查所作的修正 **
**10、清空表 my_employees**

### 6.4.2、答案

~~~mysql
#1.	运行以下脚本创建表my_employees

USE myemployees;
CREATE TABLE my_employees(
	Id INT(10),
	First_name VARCHAR(10),
	Last_name VARCHAR(10),
	Userid VARCHAR(10),
	Salary DOUBLE(10,2)
);
CREATE TABLE users(
	id INT,
	userid VARCHAR(10),
	department_id INT

);
#2.	显示表my_employees的结构
DESC my_employees;

#3.	向my_employees表中插入下列数据
ID	FIRST_NAME	LAST_NAME	USERID	SALARY
1	patel		Ralph		Rpatel	895
2	Dancs		Betty		Bdancs	860
3	Biri		Ben		Bbiri	1100
4	Newman		Chad		Cnewman	750
5	Ropeburn	Audrey		Aropebur	1550

#方式一：
INSERT INTO my_employees
VALUES(1,'patel','Ralph','Rpatel',895),
(2,'Dancs','Betty','Bdancs',860),
(3,'Biri','Ben','Bbiri',1100),
(4,'Newman','Chad','Cnewman',750),
(5,'Ropeburn','Audrey','Aropebur',1550);
DELETE FROM my_employees;
#方式二：

INSERT INTO my_employees
SELECT 1,'patel','Ralph','Rpatel',895 UNION
SELECT 2,'Dancs','Betty','Bdancs',860 UNION
SELECT 3,'Biri','Ben','Bbiri',1100 UNION
SELECT 4,'Newman','Chad','Cnewman',750 UNION
SELECT 5,'Ropeburn','Audrey','Aropebur',1550;

				
#4.	 向users表中插入数据
1	Rpatel	10
2	Bdancs	10
3	Bbiri	20
4	Cnewman	30
5	Aropebur	40

INSERT INTO users
VALUES(1,'Rpatel',10),
(2,'Bdancs',10),
(3,'Bbiri',20);




#5.将3号员工的last_name修改为“drelxer”
UPDATE my_employees SET last_name='drelxer' WHERE id = 3;



#6.将所有工资少于900的员工的工资修改为1000
UPDATE my_employees SET salary=1000 WHERE salary<900;

#7.将userid 为Bbiri的user表和my_employees表的记录全部删除

DELETE u,e
FROM users u
JOIN my_employees e ON u.`userid`=e.`Userid`
WHERE u.`userid`='Bbiri';

#8.删除所有数据

DELETE FROM my_employees;
DELETE FROM users;
#9.检查所作的修正

SELECT * FROM my_employees;
SELECT * FROM users;

#10.清空表my_employees
TRUNCATE TABLE my_employees;
~~~



# 7、DDL语言的学习

**数据定义语言**

## 7.1、库的管理

**创建、修改、删除**

### 7.1.1、库的创建

**语法**

create database 库名

~~~mysql
# 创建库 books
CREATE DATABASE IF NOT EXISTS books;
~~~

### 7.1.2、库的修改

~~~mysql
# 更改库的字符集
ALTER DATABASE books CHARACTER SET gbk;

~~~

### 7.1.3、库的删除

~~~mysql
# 库的删除
DROP DATABASE IF EXISTS books;
~~~



## 7.2、表的管理

**创建、修改、删除**

### 7.2.1、表的创建

**语法**

create table 表名 (

​					列名	列的类型	【（长度）约束】,

​					列名	列的类型	【（长度）约束】,

​					列名	列的类型	【（长度）约束】,

​                    ...

​					列名	列的类型	【（长度）约束】,

)

~~~mysql
# 创建表book
CREATE TABLE IF NOT EXISTS book (
    id INT, #编号
    bName VARCHAR(20), #图书名
    price DOUBLE, #价格
    author INT, #作者编号
    publishDate DATETIME #出版日期
);
# 创建表author
CREATE TABLE IF NOT EXISTS author (
    id INT,
    au_name VARCHAR(20),
    nation VARCHAR(10)
)
~~~

### 7.2.2、表的修改

~~~mysql
# 以下可省略COLUMN

# 修改列名
ALTER TABLE book CHANGE COLUMN publishDate pubDate DATETIME; 

# 修改列的类型货约束
ALTER TABLE book CHANGE COLUMN pubDate pubDate TEXT;
ALTER TABLE book MODIFY COLUMN pubDate TEXT;

# 添加新列
ALTER TABLE author ADD COLUMN annul DOUBLE;

# 添加新列并指定位置
ALTER TABLE author ADD COLUMN annul DOUBLE 【first】;
ALTER TABLE author ADD COLUMN annul DOUBLE 【after 字段名】;


# 删除列
ALTER TABLE author drop COLUMN annul;

# 修改表名
ALTER TABLE author RENAME TO book_author;
~~~

### 7.2.3、表的删除

~~~mysql
DROP TABLE IF EXISTS book_author；
~~~

### 7.2.4、表的复制

~~~mysql
# 仅仅复制表的结构
CREATE TABLE copy_author LIKE book_author;

# 复制表的结构加数据
CREATE TABLE author_complete select * from author;

# 复制表的结构加数据（筛选）
CREATE TABLE author_complete_filter select id,au_name from author where nation = '中国';

# 仅仅复制表的部分结构
CREATE TABLE author_complete_filter select id,au_name from author where 1=2; # select 返回0列即可
~~~

### 7.2.5、练习

~~~mysql
#1.	创建表dept1
NAME	NULL?	TYPE
id		INT(7)
NAME		VARCHAR(25)


CREATE TABLE dept1(
	id INT(7),
	NAME VARCHAR(25)
	

);
#2.	将表departments中的数据插入新表dept2中

CREATE TABLE dept2
SELECT department_id,department_name
FROM myemployees.departments;


#3.	创建表emp5
NAME	NULL?	TYPE
id		INT(7)
First_name	VARCHAR (25)
Last_name	VARCHAR(25)
Dept_id		INT(7)

CREATE TABLE emp5(
id INT(7),
first_name VARCHAR(25),
last_name VARCHAR(25),
dept_id INT(7)

);


#4.	将列Last_name的长度增加到50

ALTER TABLE emp5 MODIFY COLUMN last_name VARCHAR(50);
#5.	根据表employees创建employees2

CREATE TABLE employees2 LIKE myemployees.employees;

#6.	删除表emp5
DROP TABLE IF EXISTS emp5;

#7.	将表employees2重命名为emp5

ALTER TABLE employees2 RENAME TO emp5;

#8.在表dept和emp5中添加新列test_column，并检查所作的操作

ALTER TABLE emp5 ADD COLUMN test_column INT;
#9.直接删除表emp5中的列 dept_id
DESC emp5;
ALTER TABLE emp5 DROP COLUMN test_column;
~~~

## 7.3、常见的数据类型

**数值型**
	整型
	小数：
		定点数
		浮点数
**字符型**
	较短的文本：char、varchar
	较长的文本：text、blob（较长的二进制数据）

**日期型**

### 7.3.1、整型

**分类**
tinyint、smallint、mediumint、int/integer、bigint
1	 2		3	4		8

特点：
① 如果不设置无符号还是有符号，默认是有符号，如果想设置无符号，需要添加unsigned关键字
② 如果插入的数值超出了整型的范围,会报out of range异常
③ 如果不设置长度，会有默认的长度
长度代表了显示的最大宽度，如果不够会用0在左边填充，但必须搭配zerofill使用！zerofill默认为无符号

~~~mysql
# 1.如何设置无符号和有符号
CREATE TABLE IF NOT EXISTS tab_int (
    t1 INT,
    t2 INT UNSIGNED
);

#最大宽度 （zerofill默认为无符号）
CREATE TABLE IF NOT EXISTS tab_int (
    t1 INT ZEROFILL,
    t2 INT ZEROFILL
);
~~~

### 7.3.2 、小数

**分类**

1. 浮点型
   float(M,D)
   double(M,D)
2. 定点型
   dec(M，D)
   decimal(M,D)

**特点**

1. M：整数部位+小数部位
   D：小数部位
2. M和D都可以省略
   如果是decimal，则M默认为10，D默认为0
   如果是float和double，则会根据插入的数值的精度来决定精度
3. 定点型的精确度较高，如果要求插入数值的精度较高如货币运算等则考虑使用

~~~mysql
CREATE TABLE IF NOT EXISTS tab_float (
    f1 FLOAT(5 , 2 ),
    f2 DOUBLE(5 , 2 ),
    f3 DECIMAL(5 , 2 )
);

INSERT INTO tab_float VALUES(123.4523,123.4523,123.4523); # 小数超出位数四舍五入
INSERT INTO tab_float VALUES(1234.4523,123.4523,123.4523);# 整数超出位数报错
~~~

### 7.3.3 字符

#### 7.3.3.1、较短的文本

char
varchar

**特点**

| 写法               | M的意思                         | 特点           | 空间的耗费 | 效率 |
| ------------------ | ------------------------------- | -------------- | ---------- | ---- |
| char	char(M)    | 最大的字符数，可以省略，默认为1 | 固定长度的字符 | 比较耗费   | 高   |
| varchar varchar(M) | 最大的字符数，不可以省略        | 可变长度的字符 | 比较节省   | 低   |

#### 7.3.3.2、其他

binary和varbinary用于保存较短的二进制
enum用于保存枚举
set用于保存集合

#### 7.3.3.3、较长的文本

text
blob(较大的二进制)

#### 7.3.3.4、枚举

~~~mysql
CREATE TABLE IF NOT EXISTS tab_char (
    c_enum ENUM('a', 'b', 'c')
);
drop table if exists tab_char;
SELECT 
    *
FROM
    tab_char;
insert into tab_char values ('a');
insert into tab_char values ('A');
insert into tab_char values ('m'); # 无法插入或为空
insert into tab_char values ('b');
~~~

#### 7.3.3.5、Set类型

说明：和Enum类型类似，里面可以保存0~64个成员。和Enum类型最大的区 别是：SET类型一次可以选取多个成员，而Enum只能选一个 根据成员个数不同，存储所占的字节也不同

~~~mysql
CREATE TABLE IF NOT EXISTS tab_set (
    s SET('a', 'b', 'c', 'd')
);
insert into tab_set values ('a');
insert into tab_set values ('a,b');
insert into tab_set values ('a,b,e'); #失败
~~~

### 7.3.4、日期

**分类**
date只保存日期
time 只保存时间
year只保存年

datetime保存日期+时间
timestamp保存日期+时间

**datetime和timestamp的区别** 

1、Timestamp支持的时间范围较小，取值范围： 19700101080001——2038年的某个时间 Datetime的取值范围：1000-1-1 ——9999—12-31

2、timestamp和实际时区有关，更能反映实际的日 期，而datetime则只能反映出插入时的当地时区 

3、timestamp的属性受Mysql版本和SQLMode的影响 很大

~~~mysql
CREATE TABLE IF NOT EXISTS tab_date (
    t1 DATETIME,
    t2 TIMESTAMP
);
insert into tab_date value(now(),now());
SELECT * FROM tab_date;
show variables like 'time_zone'; # 查看时区
set time_zone = '+9:00'; # 修改时区 默认为system
~~~

## 7.4、常见约束

**含义**

一种限制，用于限制表中的数据，为了保证表中的数据的准确和可靠性

**分类**

六大约束

1. NOT NULL：非空，用于保证该字段的值不能为空
   比如姓名、学号等
2. DEFAULT:默认，用于保证该字段有默认值
   比如性别
3. PRIMARY KEY:主键，用于保证该字段的值具有唯一性，并且非空
   比如学号、员工编号等
4. UNIQUE:唯一，用于保证该字段的值具有唯一性，可以为空
   比如座位号
5. CHECK:检查约束【mysql中不支持】
   比如年龄、性别
6. FOREIGN KEY:外键，用于限制两个表的关系，用于保证该字段的值必须来自于主表的关联列的值
   	在从表添加外键约束，用于引用主表中某列的值
   比如学生表的专业编号，员工表的部门编号，员工表的工种编号
   	

**添加约束的时机**
	1.创建表时
	2.修改表时
	

**约束的添加分类**
	1. 列级约束：
	六大约束语法上都支持，但外键约束没有效果
 	2. 表级约束：
     除了非空、默认，其他的都支持

**主键和唯一的大对比**

组合 primary key(id,name)

|      | 保证唯一性 | 是否允许为空 | 一个表中可以有多少个 | 是否允许组合 |
| ---- | ---------- | ------------ | -------------------- | ------------ |
| 主键 | √          | ×            | 至多有1个            | √，但不推荐  |
| 唯一 | √          | √            | 可以有多个           | √，但不推荐  |

**外键**

1. 要求在从表设置外键关系
2. 从表的外键列的类型和主表的关联列的类型要求一致或兼容，名称无要求
3. 主表的关联列必须是一个key（一般是主键或唯一）
4. 插入数据时，先插入主表，再插入从表删除数据时，先删除从表，再删除主表

### 7.4.1、创建表时添加约束

#### 7.4.1.1、添加列级约束

~~~mysql
CREATE TABLE stuinfo (
    id INT PRIMARY KEY,
    stuName VARCHAR(20) NOT NULL,
    gender CHAR(1) CHECK (gender = '男' OR gender = '女'),
    seat INT UNIQUE,
    age INT DEFAULT 18,
    majorid INT REFERENCES major (id)
);

#查看stuinfo中的所有索引，包括主键、外键、唯一
SHOW INDEX FROM stuinfo;
~~~

#### 7.4.1.2、添加表级约束

**语法**

在各个字段最下面

<font color = "red">constraint 约束名 可选，不写会有默认名</font>

【constraint 约束名】 约束类型(字段名)

~~~mysql
CREATE TABLE IF NOT EXISTS stuinfo (
    id INT,
    stuname VARCHAR(20),
    gender CHAR(1),
    seat INT,
    age INT,
    majorid INT,
    CONSTRAINT pk PRIMARY KEY (id),
    CONSTRAINT uq UNIQUE (seat),
    CONSTRAINT ck CHECK (gender IN ('男' , '女')),
    CONSTRAINT fk_stuinfo_major FOREIGN KEY (majorid)
        REFERENCES major (id)
)
~~~

#### 7.4.1.3、一般写法

~~~mysql
CREATE TABLE stuinfo (
    id INT PRIMARY KEY,
    stuName VARCHAR(20) NOT NULL,
    gender CHAR(1) CHECK (gender = '男' OR gender = '女'),
    seat INT UNIQUE,
    age INT DEFAULT 18,
    majorid INT,
    CONSTRAINT fk_stuinfo_major FOREIGN KEY (majorid)
        REFERENCES major (id)
);
~~~

#### 7.4.1.3、列级约束和表级约束的区别

| 位置     | 支持的约束类型                             | 是否可以起约束名     |
| :------- | ------------------------------------------ | -------------------- |
| 列级约束 | 列的后面	语法都支持，但外键没有效果     | 不可以               |
| 表级约束 | 所有列的下面	默认和非空不支持，其他支持 | 可以（主键没有效果） |



### 7.4.2、修改表时添加约束

~~~mysql
#1.添加非空约束
ALTER TABLE stuinfo MODIFY COLUMN stuname VARCHAR(20)  NOT NULL;
#2.添加默认约束
ALTER TABLE stuinfo MODIFY COLUMN age INT DEFAULT 18;
#3.添加主键
#①列级约束
ALTER TABLE stuinfo MODIFY COLUMN id INT PRIMARY KEY;
#②表级约束
ALTER TABLE stuinfo ADD PRIMARY KEY(id);

#4.添加唯一

#①列级约束
ALTER TABLE stuinfo MODIFY COLUMN seat INT UNIQUE;
#②表级约束
ALTER TABLE stuinfo ADD UNIQUE(seat);


#5.添加外键
ALTER TABLE stuinfo ADD CONSTRAINT fk_stuinfo_major FOREIGN KEY(majorid) REFERENCES major(id); 

#三、修改表时删除约束

#1.删除非空约束
ALTER TABLE stuinfo MODIFY COLUMN stuname VARCHAR(20) NULL;

#2.删除默认约束
ALTER TABLE stuinfo MODIFY COLUMN age INT ;

#3.删除主键
ALTER TABLE stuinfo DROP PRIMARY KEY;

#4.删除唯一
ALTER TABLE stuinfo DROP INDEX seat;

#5.删除外键
ALTER TABLE stuinfo DROP FOREIGN KEY fk_stuinfo_major;

SHOW INDEX FROM stuinfo;

~~~

### 7.4.3、删除主表记录

**级联删除**

添加外键时末尾加上 on delete cascade

例 alter table stuinfo add constraint fk_stu_major foreign key(majorid) references major(id) on delete cascade;

**级联置空**

添加外键时末尾加上 on delete set null

例 alter table stuinfo add constraint fk_stu_major foreign key(majorid) references major(id) on delete set null;

## 7.5、标识列

又称为自增长列
含义：可以不用手动的插入值，系统提供默认的序列值

特点：
1、标识列必须和主键搭配吗？不一定，但要求是一个key
2、一个表可以有几个标识列？至多一个！
3、标识列的类型只能是数值型
4、标识列可以通过 SET auto_increment_increment=3;设置步长
可以通过 手动插入值，设置起始值

### 7.5.1、创建表时设置标识列

~~~mysql
CREATE TABLE tab_identity (
    id INT PRIMARY KEY AUTO_INCREMENT,
    `name` VARCHAR(20)
);
drop table tab_identity;
insert into tab_identity values (null,'john');
insert into tab_identity values (null,'tom');
~~~

### 7.5.2、修改表时设置标识列

~~~mysql
alter table tab_identity modify column id int primary key AUTO_INCREMENT;
~~~

### 7.5.3、修改表时删除标识列

~~~mysql
alter table tab_identity modify column id int;
~~~



#  8、TCL语言的学习

**事务控制语言**

Transaction Control Language 事务控制语言

## 8.1、事务
一个或一组sql语句组成一个执行单元，这个执行单元要么全部执行，要么全部不执行。

事务：事务由单独单元的一个或多个SQL语句组成，在这 个单元中，每个MySQL语句是相互依赖的。而整个单独单 元作为一个不可分割的整体，如果单元中某条SQL语句一 旦执行失败或产生错误，整个单元将会回滚。所有受到影 响的数据将返回到事物开始以前的状态；如果单元中的所 有SQL语句均执行成功，则事物被顺利执行



1、概念：在mysql中的数据用各种不同的技术存储 在文件（或内存）中。

2、通过show engines；来查看mysql支持的存储引 擎。

3、 在mysql中用的最多的存储引擎有：innodb， myisam ,memory 等。其中innodb支持事务，而 myisam、memory等不支持事务

**案例**

转账

张三丰  1000
郭襄	1000

update 表 set 张三丰的余额=500 where name='张三丰'
意外
update 表 set 郭襄的余额=1500 where name='郭襄'

## 8.2、事务的特性
ACID
原子性：一个事务不可再分割，要么都执行要么都不执行
一致性：一个事务执行会使数据从一个一致状态切换到另外一个一致状态
隔离性：一个事务的执行不受其他事务的干扰
持久性：一个事务一旦提交，则会永久的改变数据库的数据.

事务的ACID(acid)属性 

1. 原子性（Atomicity） 原子性是指事务是一个不可分割的工作单位，事务中的操作要么 都发生，要么都不发生。
2. 一致性（Consistency） 事务必须使数据库从一个一致性状态变换到另外一个一致性状态 。 
3.  隔离性（Isolation） 事务的隔离性是指一个事务的执行不能被其他事务干扰，即一个 事务内部的操作及使用的数据对并发的其他事务是隔离的，并发 执行的各个事务之间不能互相干扰。 
4. 持久性（Durability） 持久性是指一个事务一旦被提交，它对数据库中数据的改变就是 永久性的，接下来的其他操作和数据库故障不应该对其有任何影 响

## 8.3、事务的创建

### 8.3.1、分类

1. 隐式事务：事务没有明显的开启和结束的标记
   比如insert、update、delete语句delete from 表 where id =1;
2. 显式事务：事务具有明显的开启和结束的标记
   前提：必须先设置自动提交功能为禁用
   set autocommit=0;

### 8.3.2、set autocommit和START TRANSACTION

set autocommit=0指事务非自动提交，自此句执行以后，每个SQL语句或者语句块所在的事务都需要显示"commit"才能提交事务。

1. 不管autocommit 是1还是0 

   START TRANSACTION 后，只有当commit数据才会生效，ROLLBACK后就会回滚。

2. 当autocommit 为 0 时

   不管有没有START TRANSACTION。
   只有当commit数据才会生效，ROLLBACK后就会回滚。

3. 如果autocommit 为1 ，并且没有START TRANSACTION 。
   调用ROLLBACK是没有用的。即便设置了SAVEPOINT。

### 8.3.3、使用

1. 步骤1：开启事务
   set autocommit=0;
   start transaction;可选的
2. 步骤2：编写事务中的sql语句(select insert update delete)
   语句1;
   语句2;
   ...
3. 步骤3：结束事务
   commit;提交事务
   rollback;回滚事务

savepoint 节点名;设置保存点



~~~mysql
create table if not exists account(id int primary key auto_increment,username varchar(20),balance double);

insert into account values (null,'张无忌',1000),(null,'赵敏',1000);

set autocommit = 0;

start transaction;

update account set balance = 500 where username = '张无忌';

update account set balance = 1500 where username = '赵敏';

commit;

rollback;

select * from account;
~~~

### 8.3.4、回滚点

~~~mysql
set autocommit = 0;
update account set balance = 1000;
savepoint a;
update account set balance = 1500 where id = 1;
rollback to a; # savepoint a;之前的修改还在
~~~





## 8.4、事务的隔离级别

### 8.4.1、并发问题

对于同时运行的多个事务, 当这些事务访问数据库中相同的数据时, 如果没 有采取必要的隔离机制, 就会导致各种并发问题: 

1. 脏读: 对于两个事务 T1, T2, T1 读取了已经被 T2 更新但还没有被提交的字段.  之后, 若 T2 回滚, T1读取的内容就是临时且无效的. 
2.  不可重复读: 对于两个事务T1, T2, T1 读取了一个字段, 然后 T2 更新了该字段.  之后, T1再次读取同一个字段, 值就不同了. 
3.  幻读: 对于两个事务T1, T2, T1 从一个表中读取了一个字段, 然后 T2 在该表中插 入了一些新的行. 之后, 如果 T1 再次读取同一个表, 就会多出几行. 



### 8.4.2、数据库事务的隔离性:

数据库系统必须具有隔离并发运行各个事务的能力,  使它们不会相互影响, 避免各种并发问题.   一个事务与其他事务隔离的程度称为隔离级别. 数据库规定了多种事务隔 离级别, 不同隔离级别对应不同的干扰程度, 隔离级别越高, 数据一致性就 越好, 但并发性越弱

| 隔离级别         | 脏读 | 不可重复读 | 幻读 |
| ---------------- | ---- | ---------- | ---- |
| read uncommitted | √    | √          | √    |
| read committed   | ×    | √          | √    |
| repeatable read  | ×    | ×          | √    |
| serializable     | ×    | ×          | ×    |



mysql中默认 第三个隔离级别 repeatable read
oracle中默认第二个隔离级别 read committed
#### 8.4.2.1、查看隔离级别
低版本 select @@tx_isolation;

高版本 select @@transaction_isolation;  

<font color = "red">由于mysql数据库的更新，在旧版本中tx_isolation是作为transaction_isolation的别名被应用的，新版本已经弃用了，所以输入会显示未知变量
把**tx_isolation**换成**transaction_isolation**,就可以显示结果。</font>



你可以用下列语句查询全局和会话事务隔离级别：

```mysql
# 低版本
SELECT @@global.tx_isolation; 
SELECT @@session.tx_isolation; 
SELECT @@tx_isolation;

#高版本
select @@global.transaction_isolation;
SELECT @@session.transaction_isolation; 
SELECT @@transaction_isolation; 
```

#### 8.4.2.2、设置隔离级别

**方式1**

设置当前 mySQL 连接的隔离级别:   set session  transaction isolation level read committed; 

设置数据库系统的全局的隔离级别:  set global transaction isolation level read committed; 

<font color = "red">set global不会更改当前会话的事物隔离级别,只对新打开的连接有效</font>

**方式2**

set @@transaction_isolation='read-uncommitted';

set sesstion transaction_isolation='read-committed' <=> SET transaction_isolation='read-uncommitted';

set global transaction_isolation='read-committed'

## 8.5、delete和truncate在事务使用时的区别

~~~mysql
USE students;
SET autocommit = 0;
DELETE FROM account;
ROLLBACK; 
SELECT * FROM account;

SET autocommit = 0;
TRUNCATE account;
ROLLBACK;
SELECT * FROM account;
~~~

# 9、视图的讲解

**含义**

视图：MySQL从5.0.1版本开始提供视图功能。一种虚拟 存在的表，行和列的数据来自定义视图的查询中使用的表 ，并且是在使用视图时动态生成的，只保存了sql逻辑，不 保存查询结果 

**应用场景**

- 多个地方用到同样的查询结果 
- 该查询结果使用的sql语句较复杂

**好处**

- 重用sql语句
- 简化复杂的sql操作，不必知道它的查询细节 
- 保护数据，提高安全性

**与表的区别**

|      | 创建语法的关键字 | 是否实际占用物理空间 | 使用                         |
| ---- | ---------------- | -------------------- | ---------------------------- |
| 视图 | create view      | 只是保存了sql逻辑    | 增删改查，只是一般不能增删改 |
| 表   | create table     | 保存了数据           | 增删改查                     |

**案例**

~~~mysql
# 案例 查询姓张的学生名和专业名
SELECT stuname,majorname
FROM stuinfo s
INNER JOIN major m ON s.majorid = m.id
WHERE s.stuname LIKE '张%'

CREATE VIEW v1
SELECT stuname,majorname
FROM stuinfo s
INNER JOIN major m ON s.majorid = m.id
WHERE s.stuname LIKE '张%'

SELECT * FROM v1

~~~



## 9.1、创建视图 

**语法**

create view 视图名

as 查询语句

~~~mysql
# 案例1.查询邮箱中包含a字符的员工名、部门名和工种信息
# 创建
CREATE VIEW myv1 AS
    SELECT 
        last_name, department_name, job_title
    FROM
        employees e
            JOIN
        departments d ON e.department_id = d.department_id
            JOIN
        jobs j ON e.job_id = e.job_id;
# 查询        
SELECT 
    *
FROM
    myv1
WHERE
    last_name LIKE '%a%'
    
    
# 案例2.查询各部门的平均工资级别
CREATE VIEW myv2 AS
    SELECT 
        department_id, AVG(salary) ag
    FROM
        employees
    GROUP BY department_id;
    
    
# 案例3.查询平均工资最低的部门信息
SELECT * FROM myv2 ORDER BY ag LIMIT 1;


# 案例4.查询平均工资最低的部门名和工资
CREATE VIEW myv3 AS SELECT * FROM myv2 ORDER BY ag LIMIT 1;

SELECT d.*,m.ag FROM myv3 m JOIN departments d WHERE m.department_id = d.department_id;
~~~

## 9.2、视图的修改

**方式一**

create or replace view 视图名

as

查询语句

**方式二**

alter view myv3

as

select * from

## 9.3、视图的删除

~~~mysql
DROP VIEW myv1,myv2,myv3;
~~~

## 9.4、视图的查看

~~~mysql
DESC myv1;

SHOW CREATE VIEW myv1;
~~~

## 9.5、视图的更新

**可能只有增删改的其中一种不能执行**

视图的可更新性和视图中查询的定义有关系，以下类型的 视图是不能更新的。 

- 包含以下关键字的sql语句：

  分组函数、distinct、group by 、having、union或者union all 

-  常量视图

  例：

  ~~~mysql
  CREATE OR REPLACE VIEW myv2
  AS
  SELECT 'john' NAME;
  ~~~

  

- Select中包含子查询

- join

- from一个不能更新的视图 

- where子句的子查询引用了from子句中的
  例：

  ~~~mysql
  CREATE OR REPLACE VIEW myv6
  AS
  
  SELECT last_name,email,salary
  FROM employees
  WHERE employee_id IN(
  	SELECT  manager_id
  	FROM employees
  	WHERE manager_id IS NOT NULL
  );
  ~~~

  

## 9.6、练习

~~~mysql
#一、创建视图emp_v1,要求查询电话号码以‘011’开头的员工姓名和工资、邮箱

CREATE OR REPLACE VIEW emp_v1 AS
    SELECT 
        last_name, salary, email
    FROM
        employees
    WHERE
        phone_number LIKE '011%';

#二、创建视图emp_v2，要求查询部门的最高工资高于12000的部门信息
CREATE OR REPLACE VIEW emp_v2 AS
    SELECT 
        d.*, max_dep
    FROM
        departments d
            JOIN
        (SELECT 
            MAX(salary) max_dep, department_id
        FROM
            employees
        GROUP BY department_id
        HAVING max_dep > 12000) f1 ON d.department_id = f1.department_id;
~~~

# 10、变量

## 10.1、系统变量

变量由系统提供，不是用户定义，属于服务器层面

使用的语法

### 10.1.1、查看所有的系统变量

show global variables;

show session variables;

### 10.1.2、查看满足条件的系统变量

show global variables like '',

show session variables  like '';

### 10.1.3、查看指定的系统变量值

select @@global.系统变量名;

select @@session.系统变量名

### 10.1.4、为系统变量赋值

**方式一**

set @@global.系统变量名=值;

set @@session.系统变量名=值;

**方式二**

set global 系统变量名=值;

set session 系统变量名=值;

如果是全局级别，则需要加global；如果是会话级别，则需要加session；不写默认session



## 10.2、自定义变量

### 10.2.1、用户变量

#### 10.2.1.1、作用域

针对于当前会话(连接)有效，同于会话变量的作用域

#### 10.2.1.2、声明并初始化

~~~mysql
SET @用户变量名 = 值;

SET @用户变量名 := 值;

SELECT @用户变量名 := 值;
~~~

#### 10.2.1.3、赋值

**方式一**

~~~mysql
SET @用户变量名 = 值;

SET @用户变量名 := 值;

SELECT @用户变量名 := 值;
~~~

**方式二**

~~~mysql
SELECT 字段 INTO 变量名 FROM 表; #将查询的字段赋值给 变量
~~~

3、使用

~~~mysql
SELECT @用户变量名;
~~~



### 10.2.2、局部变量

使用见11章

#### 10.2.1.1、作用域

仅仅在定它的begin end中有效，应用在begin end中的第一句话。

#### 10.2.2.2、声明

~~~mysql
DECLARE 变量名 类型;

DECLARE 变量名 类型 DEFAULT 值;
~~~



#### 10.2.2.3、赋值

~~~mysql
SET 局部变量名 = 值;

SET 局部变量名 := 值;

SELECT @局部变量名 := 值;
~~~



#### 10.2.2.4、使用

~~~mysql
SELECT 局部变量名
~~~



### 10.2.3、用户变量和局部变量的对比

|          | 作用域              | 定义位置            | 语法                     |
| -------- | :------------------ | ------------------- | ------------------------ |
| 用户变量 | 当前会话            | 会话的任何地方      | 加@符号，不用指定类型    |
| 局部变量 | 定义它的BEGIN END中 | BEGIN END的第一句话 | 一般不用加@,需要指定类型 |

### 10.2.4、练习

~~~mysql
# 案例：声明两个变量并赋初值值，求和

SET @m=1;
SET @n=2;
SET @sum=@m+@n;
SELECT @sum;

~~~



# 11、存储过程和函数

## 11.1、存储过程

类似与java中的方法
**存储过程**
含义：一组预先编译好的SQL语句的集合，理解成批处理语句
1、提高代码的重用性
2、简化操作
3、减少了编译次数并且减少了和数据库服务器的连接次数，提高了效率

### 11.1.1、创建语法

CREATE PROCEDURE 存储过程名(参数列表)
BEGIN

存储过程体（一组合法的SQL语句）

END

#### 11.1.1.1、参数

参数列表包含三部分
参数模式  参数名  参数类型
举例：
in stuname varchar(20)

参数模式：

- in：该参数可以作为输入，也就是该参数需要调用方传入值
- out：该参数可以作为输出，也就是该参数可以作为返回值
- inout：该参数既可以作为输入又可以作为输出，也就是该参数既需要传入值，又可以返回值

#### 11.1.1.2、存储过程体

1. 如果存储过程体仅仅只有一句话，begin end可以省略。
2. 存储过程体中的每条sql语句的结尾要求必须加分号。
3. 存储过程的结尾可以使用 delimiter 重新设置
   delimiter 结束标记
   例：
   delimiter $

### 11.1.2、调用语法

CALL 存储过程名(实参列表);

### 11.1.3、删除和查看

~~~mysql
DROP PROCEDURE test_pro3;
# 不支持desc
SHOW CREATE PROCEDURE test_pro2;
~~~

### 11.1.4、练习

~~~mysql
#1.空参列表
#案例：插入到admin表中五条记录
DELIMITER $
CREATE PROCEDURE myp1()
BEGIN
	INSERT INTO account VALUES(1,"张三",100),(2,"李四",1000),(3,"王五",1500);
END $

# 调用
call myp1()$


#2.创建带in模式参数的存储过程
#案例1：创建存储过程实现 根据女神名，查询对应的男神信息
DELIMITER $
CREATE PROCEDURE myp2(in beautyName varchar(20))
BEGIN
	SELECT bo.* FROM boys bo JOIN beauty be ON bo.id = be.boyfriend_id WHERE be.`name` = beautyName;
END $
call myp2("小昭")$


#案例2 ：创建存储过程实现，用户是否登录成功
DELIMITER $
CREATE PROCEDURE myp3(in username varchar(20),in `password` varchar(20))
BEGIN
	DECLARE result int default 0;
    select count(*) into result from admins where admins.username=username and admins.password=password;
    select if(result>0,'成功','失败');
END $
call myp3("张三","111")$


#3.创建out 模式参数的存储过程
#案例1：根据输入的女神名，返回对应的男神名
DELIMITER $
CREATE PROCEDURE myp5(IN beautyName VARCHAR(20), OUT boyName VARCHAR(20))
BEGIN
	SELECT bo.boyName INTO boyName FROM boys bo JOIN beauty be ON bo.id = be.boyfriend_id WHERE be.name=beautyName;
end $

CALL myp5('小昭',@bName)$
SELECT @bName;

#案例2：根据输入的女神名，返回对应的男神名和魅力值
DELIMITER $
CREATE PROCEDURE myp6(IN beautyName VARCHAR(20), OUT boyName VARCHAR(20),out userCP INT)
BEGIN
	SELECT bo.boyName, bo.userCP INTO boyName, userCP FROM boys bo JOIN beauty be ON bo.id = be.boyfriend_id WHERE be.name=beautyName;
end $

CALL myp6('小昭',@bName,@userCP)$
SELECT @bName,@userCP$


#4.创建带inout模式参数的存储过程
#案例1：传入a和b两个值，最终a和b都翻倍并返回
DELIMITER $
CREATE PROCEDURE myp8(INOUT a INT,INOUT b INT)
BEGIN
	SET a=a*2;
    SET b=b*3;
END$

SET @n=10$
SET @m=20$
CALL myp8(@n,@m)$
SELECT @n,@m$
~~~

### 11.1.5、小测试

~~~mysql
#一、创建存储过程实现传入用户名和密码，插入到admin表中
DELIMITER $
CREATE PROCEDURE test_pro1(IN username varchar(20),IN loginpwd VARCHAR(20))
BEGIN
	INSERT INTO admin(admin.username,password) values(username,loginpwd);
END$
CALL test_pro1("张辽","777")$

#二、创建存储过程实现传入女神编号，返回女神名称和女神电话
DELIMITER $
CREATE PROCEDURE test_pro2(IN id INT,OUT NAME VARCHAR(20),OUT phone VARCHAR(20))
BEGIN
	SELECT be.name,be.phone into NAME,phone from beauty be where be.id = id;
END$
CALL test_pro2(1,@name,@phone)$
SELECT @name,@phone;

#三、创建存储存储过程或函数实现传入两个女神生日，返回大小
DELIMITER $
CREATE PROCEDURE test_pro3(IN birth1 DATETIME,IN birth2 DATETIME,OUT result INT)
BEGIN
	SELECT DATEDIFF(birth1,birth2) INTO result;
END $

CALL test_pro3(now(),now(),@result)$
select @result$


#四、创建存储过程或函数实现传入一个日期，格式化成xx年xx月xx日并返回
DELIMITER $
CREATE PROCEDURE test_pro4(IN mydate DATETIME, OUT formateDate VARCHAR(20))
BEGIN
	SELECT DATE_FORMAT(mydate,"%y年%m月%d日") INTO formateDate;
END $

CALL test_pro4(NOW(),@formateDate)$
SELECT @formateDate$

#五、创建存储过程或函数实现传入女神名称，返回：女神 and 男神  格式的字符串
如 传入 ：小昭
返回： 小昭 AND 张无忌
DELIMITER $
CREATE PROCEDURE test_pro5(IN beautyName varchar(20), OUT couples VARCHAR(40))
BEGIN
	SELECT concat(be.name," AND ",IFNULL(boyName,'null')) INTO couples FROM boys bo RIGHT JOIN beauty be on bo.id = be.boyfriend_id WHERE be.name = beautyName;
END $
CALL test_pro5('柳岩',@couples)$
SELECT @couples$


#六、创建存储过程或函数，根据传入的条目数和起始索引，查询beauty表的记录
DELIMITER $
CREATE PROCEDURE test_pro6(IN startIndex INT,IN size INT)
BEGIN
	SELECT * FROM beauty LIMIT startIndex,size;
END $

CALL test_pro6(3,5)$
~~~

## 11.2、函数

**含义**

一组预先编译好的SQL语句的集合，理解成批处理语句
1、提高代码的重用性
2、简化操作
3、减少了编译次数并且减少了和数据库服务器的连接次数，提高了效率

**区别**

存储过程：可以有0个返回，也可以有多个返回，适合做批量插入、批量更新
函数：有且仅有1 个返回，适合做处理数据后返回一个结果

### 11.2.1、创建语法
~~~mysql
CREATE FUNCTION 函数名(参数列表) RETURNS 返回类型
BEGIN
	函数体
END
~~~

**注意**

1. 参数列表 包含两部分：
   参数名 参数类型

2. 函数体：肯定会有return语句，如果没有会报错。如果return语句没有放在函数体的最后也不报错，但不建议;

3. 函数体中仅有一句话，则可以省略begin end

4. 使用 delimiter语句设置结束标记

### 11.2.2、调用语法

SELECT 函数名(参数列表)

### 11.2.3、案例演示

END	Error Code: 1418. This function has none of DETERMINISTIC, NO SQL, or READS SQL DATA in its declaration and binary logging is enabled (you *might* want to use the less safe log_bin_trust_function_creators variable)	0.000 sec$

解决方法 set global log_bin_trust_function_creators=1；

~~~mysql
#1.无参有返回
# 案例 返回公司员工的个数
DELIMITER $
CREATE FUNCTION myf1() RETURNS INT
BEGIN
	DECLARE c INT DEFAULT 0;
	SELECT COUNT(*) INTO C FROM employees;
    RETURN c;
END $

set global log_bin_trust_function_creators=1$

SELECT myf1()$


#2.有参有返回
#案例1：根据员工名，返回它的工资
DELIMITER $
CREATE FUNCTION myf2(enpName VARCHAR(20)) RETURNS INT
BEGIN
	DECLARE mSalary INT DEFAULT 0;
	SELECT salary INTO mSalary FROM employees where last_name = enpName LIMIT 1;
    RETURN mSalary;
END$
select myf2('k_ing')$

#案例2：根据部门名，返回该部门的平均工资
DELIMITER $
CREATE FUNCTION myf3(depName VARCHAR(20)) RETURNS INT
BEGIN
	DECLARE mSalary INT DEFAULT 0;
	
	SELECT AVG(salary) INTO mSalary 
	FROM employees e JOIN departments d 
	ON e.department_id = d.department_id 
	WHERE department_name = depName;
	
    RETURN mSalary;
END$
select myf3('IT')$
~~~

### 11.2.4、查看函数

~~~mysql
SHOW CREATE FUNCTION myf3;
~~~

### 11.2.5、删除函数
~~~mysql
DROP FUNCTION myf3;
~~~

### 11.2.6、案例

~~~mysql
#、创建函数，实现传入两个float，返回二者之和
DELIMITER $
CREATE FUNCTION myf4(num1 FLOAT, num2 FLOAT) RETURNS FLOAT
BEGIN
	DECLARE sum FLOAT DEFAULT 0;
    SELECT num1+num2 INTO sum;  # 或者 SET sum = num1+num2;
    RETURN sum;
END $
SELECT myf4(1,3);
~~~



# 12、流程控制结构


顺序、分支、循环

## 12.1、分支结构
### 12.1.1.if函数
**语法**：if(条件,值1，值2)
**功能**：实现双分支

### 12.1.2.case结构
#### 12.1.2.1、情况1：类似于switch

一般用于实现的等值判断

~~~mysql
CASE 变量|表达式|字段
WHEN 要判断的值1 THEN 返回的值1|或语句1;
WHEN 要判断的值2 THEN 返回的值2|或语句2;
...
ELSE 语句n;
END CASE;
~~~



#### 12.1.2.1、情况2：

一般用于实现区间判断

~~~mysql
CASE 
WHEN 条件1 THEN 语句1|或语句1;
WHEN 条件2 THEN 语句2|或语句2;
...
ELSE 语句n;
END CASE;
~~~

**END CASE 和END**
END 作为表示式使用，可取别名。（**一般**放在begin end外面）

END CASE作为单独的语句使用。（**一般**放在begin end里面面）

### 12.1.3、if结构

**语法**

~~~mysql
if 条件1 then 语句1;
elseif 条件2 then 语句2;
....
else 语句n;
end if;
~~~

**功能**

类似于多重if

只能应用在begin end 中

### 12.1.3、案例

~~~mysql
#案例1：创建函数，实现传入成绩，如果成绩>90,返回A，如果成绩>80,返回B，如果成绩>60,返回C，否则返回D
# when
DELIMITER $
CREATE FUNCTION test_case(score FLOAT) RETURNS CHAR
BEGIN
	DECLARE grade CHAR(1) DEFAULT 'A';
	CASE
    WHEN score BETWEEN 90 AND 100 THEN SET grade = 'A';
    WHEN score BETWEEN 80 AND 90 THEN SET grade = 'B';
    WHEN score BETWEEN 60 AND 80 THEN SET grade = 'C';
    ELSE SET grade = 'D';
    END CASE;
    RETURN grade;
END$
SELECT test_case(79)$

#if分支
DELIMITER $
CREATE FUNCTION test_if(score FLOAT) RETURNS CHAR
BEGIN
    IF score BETWEEN 90 AND 100 THEN RETURN 'A';
    ELSEIF score BETWEEN 80 AND 90 THEN RETURN 'B';
    ELSEIF score BETWEEN 60 AND 80 THEN RETURN 'C';
    ELSE RETURN 'D';
    END IF;
END$
SELECT test_if(70)$

# 存储过程
DELIMITER $
CREATE PROCEDURE test_case(IN score FLOAT) 
BEGIN
	DECLARE grade CHAR(1) DEFAULT 'A';
	CASE
    WHEN score BETWEEN 90 AND 100 THEN SET grade = 'A';
    WHEN score BETWEEN 80 AND 90 THEN SET grade = 'B';
    WHEN score BETWEEN 60 AND 80 THEN SET grade = 'C';
    ELSE SET grade = 'D';
    END CASE;
    SELECT grade;
END$
CALL test_case(60)$

#or
DELIMITER $
CREATE PROCEDURE test_case(score FLOAT)
BEGIN 	
	CASE 
	WHEN score>90 THEN SELECT 'A';
	WHEN score>80 THEN SELECT 'B';
	WHEN score>60 THEN SELECT 'C';
	ELSE SELECT 'D';
	END CASE;
END $


~~~

## 12.2、循环

while、loop、repeat

**循环控制**

iterate类似于continue，

leave类似于break。

### 12.2.1、while

~~~mysql
【标签】while 循环条件 do
循环体
end while 【标签】
~~~

### 12.2.2、loop

~~~mysql
【标签】loop
循环体
end loop 【标签】
~~~

### 12.2.3、repeat

~~~mysql
【标签】repeat
循环体
until 结束循环的条件
end repeat 【标签】
~~~

### 12.2.4、练习

~~~mysql
#1.没有添加循环控制语句
#案例：批量插入，根据次数插入到admin表中多条记录
DELIMITER $
CREATE PROCEDURE pro_while(IN insertCount INT)
BEGIN
	DECLARE i INT DEFAULT 1;
    WHILE i <= insertCount DO
		INSERT INTO admin (username,password) VALUES(CONCAT('Rose',i),'666');
        SET i = i+1;
    END WHILE ;
END$
CALL pro_while(100)$

#2.添加leave语句
DELIMITER $
CREATE PROCEDURE pro_while(IN insertCount INT)
BEGIN
	DECLARE i INT DEFAULT 1;
    a:WHILE i <= insertCount DO
		INSERT INTO admin (username,password) VALUES(CONCAT('Rose',i),'666');
        IF i>=20 THEN LEAVE a;
        END IF;
        SET i = i+1;
    END WHILE ;
END$

CALL pro_while(100)$


#2.已知表stringcontent
#其中字段：
#id 自增长
#content varchar(20)

#向该表插入指定个数的，随机的字符串
CREATE table stringcontent(id int AUTO_INCREMENT PRIMARY key,content VARCHAR(20))$
CREATE PROCEDURE test_randstr_insert(IN insertCount INT)
BEGIN
	DECLARE i INT DEFAULT 1;
    DECLARE str VARCHAR(26) DEFAULT 'abcdefghijklmnopqrstuvwxyz';
    DECLARE startIndex INT DEFAULT 0; #字符串开始位置
    DECLARE length INT DEFAULT 0; #实际选取字符串长度
    DECLARE leftLength INT DEFAULT 0; #字符串长度范围
    WHILE i <= insertCount DO
		SET startIndex = FLOOR(RAND()*(26));
        SELECT IF(26-startIndex>20,20,26-startIndex) INTO leftLength;
        SET length = FLOOR(RAND()*leftLength+1);
        INSERT INTO stringcontent (content)VALUES(SUBSTR(str,startIndex,length));
        SET i = i+1;
	END WHILE;
END$
CALL test_randstr_insert(10)$
~~~



# 13、注意

<font color = "red">SELECT 10 NOT IN (1 , 2, NULL);返回值为null</font>



<font color = "red">如果为表取了别名，则查询的字段就不能使用原来的表名去限定</font>



<font color = red>注意：select `name` from employees; 其中name为关键字，需要加上``</font>



<font color = "red"><=>不仅能判断null 还能判断普通数值</font>



<font color = "red">is null 仅可以判断null值</font>



<font color = "red">= 仅可以判断非null值</font>



<font color = "red">一般使用count(*), count(字段)会忽略null值</font>



<font color = "red">having中的字段要在select中出现过，不然不然无法识别</font>



<font color = "red">select  not null;select  null;返回值都为null</font>



<font color = "red">执行顺序</font>

~~~mysql
# select语句完整语法
SELECT 
DISTINCT <select_list>
FROM <left_table>
<join_type> JOIN <right_table>
ON <join_condition>
WHERE <where_condition>
GROUP BY <group_by_list>
HAVING <having_condition>
ORDER BY <order_by_condition>
LIMIT <limit_number>

# 执行顺序
from →join →on →where →group by→having→select→order by→limit
~~~





<font color = "red">mysql update时报错You are using safe update mode</font>

**这是因为MySql运行在safe-updates模式下，该模式会导致非主键条件下无法执行update或者delete命令，**

**执行命令SET SQL_SAFE_UPDATES = 0;修改下数据库模式**

**如果想要提高数据库安全等级，可以在恢复回原有的设置，执行命令：**

```mysql
SET SQL_SAFE_UPDATES = 1;
```



<font color = "red">数据查询语言DQL，数据操纵语言DML，数据定义语言DDL，数据控制语言DCL</font>





<font color = "red">END	Error Code: 1418. This function has none of DETERMINISTIC, NO SQL, or READS SQL DATA in its declaration and binary logging is enabled (you *might* want to use the less safe log_bin_trust_function_creators variable)	0.000 sec$</font>

<font color = "red">解决方法 set global log_bin_trust_function_creators=1；</font>

<font color = "red">查询：show variables like 'log_bin_trust_function_creators'; </font>

<font color = "red">或 select @@global.log_bin_trust_function_creators</font>


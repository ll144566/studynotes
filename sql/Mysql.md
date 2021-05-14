![image-20200903233126291](..\typora-user-images\image-20200903233126291.png)

# 1. MySql常见命令

~~~mysql
show databases; 查看所有数据库
select database(); 查看当前数据库
show tables; 查看当前数据库所有表
show tables from sql; 查看sql数据库所有表
select version(); 查看数据库版本
~~~

# 2.查询

## 1.基础查询

### 2.1.1取别名

~~~mysql
select 100+20 as result;
select 100+50 result;
~~~

### 2.1.2去重

~~~mysql
select distinct age from ...
~~~

### 2.1.3+

~~~markdown
1. 两个操作数都为数值，则做加法运算
2. 一个为数值，另一个为字符，试图将字符转为数值，若失败则将字符转换成0
3. null+数值结果为null
4. 连接多个查询结果 select concat( name, age,sex )
~~~

### 2.1.4ifnull

~~~mysql
select ifnull(age,0) 如果age为null 则返回0
~~~

### 2.1.5模糊查询


~~~markdown
1. LIKE
_ 单个字符
% 任意多字符
\_ 匹配下划线 or LIKE '_$_%' escape '$' 指定$为转义字符，也可换为其它字符
模糊匹配不匹配空值
2. between and
3. in 
4. is null, is not null
~~~

### 2.1.6 is null

= 或<>不能用于判断null值，可以使用is null判断， is not null非空

is not null不能用于具体值

### 2.1.7安全等于<=>

可以判断null值，也能判断普通值

... where salary <=> null;

 where salary is null

### 2.1.8isnull

判断字段是否为null，是返回1，否则返回0

### 2.1.9排序

desc asc 降序 升序

### 2.1.10字符串操作

~~~mysql
select substr()/substring()
substr("asd",3)
substr("sadasd",1,3)

instr("112233","23");返回23出现的位置
lpad("112233",10,"*")左填充
rpad("112233",10,"*")右填充
select replace('1122334455','3','?') 字符串替换
group_concat()
concat
concat_ws()
upper()
lower()
length()
trim()
ltrim()
rtrim()
~~~

### 2.1.11数学函数

~~~mysql
select round(-1.55)四舍五入 -2
select ceil(-1.55)向上取整 -1
select floor(-1.55)向下取整 -2
select truncate(-1.6999,3) 截断 -1.69
select mod(10,-3)取余 1
select rand()随机数
~~~

### 2.1.12日期

~~~mysql
select now();
select curdate();
select curtime();
select year(now());
select year('1996-06-14');
select month(curtime())
select datediff('1998-03-02','1993-3-2');
select str_to_date('1998-3-2','%Y-%c-%d');将字符串转成指定格式日期 '1993-03-02'
select str_to_date('3-2-1993','%c-%d-%Y'); '1993-03-02'

select date_format(now(),'%y年%c月%d日') 日期;将日期转换成字符串 '20年8月18日'
select monthname()以英文形式返回月
~~~

![image-20200818212949323](..\typora-user-images\image-20200818212949323.png)

### 2.1.13其他函数

version，datbase，user

### 2.1.14流程控制

~~~mysql
select if(10<5,'大','小'); 相当于三目运算符

1. java中的switch case
case 字段或表达式 
when 常量1 then 执行语句1
when 常量2 then 执行语句2
else 执行语句else
end


select ...,
case ...
when 值1 then 语句1
when 值1 then 语句1
...
when 值n then 语句n
else 语句n+1 
end[别名]
from ...

2. 多重if
case ...
when 条件1 then 语句1
when 条件1 then 语句1
...
when 条件n then 语句n
else 语句n+1 
end[别名]

注意case后面有字段或表达式时when后面只能跟常量
例

CASE SCORE WHEN 'A' THEN '优' ELSE '不及格' END
CASE SCORE WHEN 'B' THEN '良' ELSE '不及格' END
CASE SCORE WHEN 'C' THEN '中' ELSE '不及格' END

等同于，使用CASE WHEN条件表达式函数实现：
CASE 
WHEN SCORE = 'A' THEN '优'
WHEN SCORE = 'B' THEN '良'
WHEN SCORE = 'C' THEN '中' ELSE '不及格' END
~~~

### 2.1.15聚合函数（分组函数）

~~~mysql
sum(),只统计不为空的个数，一般用于处理数值型
avg(),忽略null及个数，一般用于处理数值型
count(),max(),min(),可以处理任意类型
count(*)只要有一列不为null即参与统计

可配合distinct使用
select count(distinct salary),count(salary)from salaries;

select count(1)|count("da")|count(*) 查询总数

效率
myisam存储引擎下，count(*)的效率高
innodb存储引擎下，count(*)和count(1)的效率差不多，比count(字段)要高一些
~~~

### 2.1.16分组查询


~~~mysql
语法
	select 分组函数，列（要求出现在 group by后面）
	from 表
	[where 筛选条件]
	group by 分组列表
	[order by]
	[having ]
	注意：查询列表要求是分组函数，和 group by后面出现的字段
	
例
-- 查询每个工种有奖金的员工的最高工资大于1200的
SELECT 
    employee_id, salary, MAX(job_id)
FROM
    employees
WHERE
    commission_pct IS NOT NULL
    -- commission_pct IS NOT NULL and salary > 1200 ×
GROUP BY job_id
HAVING MAX(salary) > 12000;
select * from employees where commission_pct IS NOT NULL and salary > 12000;

-- 查询领导编号>102的每个领导手下的最低工资>5000的领导编号是哪个，以及最低工资
SELECT 
    manager_id, MIN(salary) min_salary
FROM
    employees
WHERE
    manager_id > 102
GROUP BY manager_id
HAVING min_salary > 5000
order by min_salary asc;


	                          数据源              关键字
	分组前筛选：原始表           group by子句前      where
	分组后筛选：分组后的结果集    group by 子句hou    having
	注意 ：
	1 分组函数做条件肯定是放在having子句中
	2 能用分组前筛选的，优先考虑使用分组前筛选，效率更高，行被分组需要时间，降低分组的行数可以提高效率，所以使用where 字句代替havingz字句
-- 按表达式或函数分组	
-- 按员工姓名的长度进行分组，查询每一组员工的个数，筛选员工个数>5的有哪些
SELECT 
    COUNT(*) c, LENGTH(last_name) len_name
FROM
    employees
GROUP BY len_name
HAVING c > 5;

-- 按多个字段分组
-- 查询每个部门每个工种的员工的平均工资
SELECT 
    AVG(salary), department_id, job_id
FROM
    employees
GROUP BY job_id , department_id;  
-- job_id , department_id都一样的进行分组


-- 查询每个部门每个工种的员工的平均工资大于10000，并且按平均工资的高低显示
SELECT 
    AVG(salary) avg_salary, job_id, department_id
FROM
    employees
GROUP BY job_id , department_id
HAVING avg_salary > 10000
ORDER BY avg_salary DESC;
~~~

### 2.1.17连接查询

~~~mysql
分类
	按年代分类
	sql192标准：mysql仅仅支持内连接
	sql199标准【推荐】mysql支持 内连接、外连接（左外连接、右外连接）、交叉连接
	
	按功能分类
		内连接：
			等值连接
			非等值连接
			子连接
		外连接：
			左外连接
			右外连接
			全外连接
		交叉连接
		
注意： 如果为表取了表明，则查询的字段就不能使用原来的表名去限定
		MySQL where 查询只能使用表的别名，不能使用字段的别名

sql192标准
#案例1 查询boys、beauty对应关系
select `name`,boyname from boys,beauty where boys.id=beauty.boyfriend_id;
#案例2 查询员工名和对应的部门名
select last_name,department_name
from employees,departments where employees.department_id=departments.department_id;

#2 查询员工们名，工种名，工种号
select last_name,e. job_id,job_title from employees e,jobs j where e.job_id=j.job_id;

#查询有奖金的员工名，部门名
SELECT 
    first_name, department_name, commission_pct
FROM
    departments d,
    employees e
WHERE
    d.department_id = e.department_id
        AND commission_pct IS NOT NULL;
        
# 查询每个城市的部门个数
SELECT 
    COUNT(*) sum, city
FROM
    departments d,
    locations l
WHERE
    d.location_id = l.location_id
GROUP BY city;

---------------------------------------------------------------------------------------------------------
#查询有奖金的每个部门的部门名和部门的领导编号和该部门的最低工资
SELECT 
    department_name, d.manager_id,min(salary)
FROM
    departments d,
    employees e
WHERE
    e.department_id = d.department_id
        AND commission_pct IS NOT NULL
GROUP BY department_name , d.manager_id;
---------------------------------------------------------------------------------------------------------


#查询每个工种的工种名和员工的个数，并按员工个数降序
SELECT 
    job_title, COUNT(*) num
FROM
    employees e,
    jobs d
WHERE
    e.job_id = d.job_id
GROUP BY job_title
ORDER BY COUNT(*) DESC


#自连接
SELECT 
    e.employee_id, e.last_name, m.employee_id, m.manager_id
FROM
    employees e,
    employees m
WHERE
    e.employee_id = m.employee_id;
    

~~~


~~~mysql
 sql199标准   
#查询名字中包含e的员工名和工种名
SELECT 
    last_name a, job_title
FROM
    employees e
        INNER JOIN
    jobs j ON e.job_id = j.job_id
WHERE # or and
    last_name LIKE '%e%';
    
#查询部门个数>3的城市名和部门个数
SELECT 
    city, COUNT(*) sum
FROM
    locations l
        INNER JOIN
    departments d ON l.location_id = d.location_id
GROUP BY city
HAVING sum > 3


# 查询部门的员工个数大于3的部门名和员工个数，并按个数降序

SELECT 
    department_name, COUNT(*) sum
FROM
    departments d
        INNER JOIN
    employees e ON d.department_id = e.department_id
GROUP BY department_name
HAVING sum > 3
ORDER BY sum DESC;

#查询员工名，部门名，工种名，并按部门号排序
SELECT 
    last_name, department_name, job_title
FROM
    departments d
        INNER JOIN
    employees e ON d.department_id = e.department_id
        INNER JOIN
    jobs j ON e.job_id = j.job_id
    
    
#自连接
#查询员工的名字，上级的名字
SELECT 
    e.last_name, m.last_name
FROM
    employees e
        INNER JOIN
    employees m ON e.manager_id = m.employee_id;
    
    
#外连接
#查询哪个城市没有部门
SELECT 
    city,l.location_id ,d.location_id depart,department_name
FROM
    locations l
        left JOIN
    departments d ON l.location_id = d.location_id
    where d.department_id is null
    
#查询部门名为SAL或IT的员工信息
SELECT 
    e.*, d.department_name, d.department_id
FROM
    departments d
        LEFT JOIN
    employees e ON d.department_id = e.department_id
WHERE
    d.department_name IN ('SAL' , 'IT')
~~~

### 2.1.18子查询

~~~markdown
1. 含义
出现在其他语句中的select语句，称为子查询或内查询
外部的查询语句，称为主查询或外查询
2. 分类
 按出现的位置
  1. select后面：仅支持标量子查询
  2. from后面：支持表子查询
  3. where或having后面 *
  4. exists 相关子查询，表子查询
 
 按结果集的行列数不同
  1. 标量子查询（结果集只有一行一列）
  2. 列子查询（结果集有一列多行）
  3. 列子查询（结果集有一行多列）
  4. 表子查询（结果集一般为多行多列）
~~~

~~~mysql

~~~


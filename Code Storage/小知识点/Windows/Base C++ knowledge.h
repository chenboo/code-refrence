C++自动提供的成员函数及成员有：
(1)默认构造函数
(2)复制构造函数
(3)默认析构函数
(4)赋值操作符
(5)地址操作符即this指针
这五种函数如果用户没有定义,则系统会自动创建一个。

关于C++的面向对象的特征：类的封装

类的成员函数可以访问类的私有成员变量，
对这一句的理解当初没仔细想，其实他的意思是，这个类的成员变量，
可以是任何该类对象的成员变量。
举例如下:

#include <iostream>
using namespace std;

class A
{
public:
	A()
	{
		m_nx = 0;
	}
	A(int nx)
	{
		m_nx = nx;
	}

private:
	int		m_nx;

public:
	int plus(A& a)
	{
		return m_nx + a.m_nx;
	}

	int Minus(A& a)
	{
		return m_nx - a.m_nx;
	}
};

//函数可以操纵对象a的私有变量m_nx而不会出现错误。
//一个函数的声明可以有多个，但是定义只有一个。
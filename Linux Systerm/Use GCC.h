//GCC / G++ 使用方法：


//1.编译C++程序要用g++
//如有一个测试程序： Test.cpp
//Test.cpp
#include <iostream>
using namespace std;

int main()
{
	cout<<"Hello world!"<<endl;
	getchar();
}

//生成可执行程序的方法：
//方法1：
//	在cygwin中直接输入: g++ Test.cpp -o Test
//方法2：
//	1、先输入: g++ -c Test.cpp //将Test.cpp编译成Test.o
//	2、再输入：g++ -o Test Test.o //将Test.o连接生成Test.exe


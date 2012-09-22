//定时关机，主要是解释如何定时,效率比较低


#include <windows.h>
#include <ctime>
#include <iostream>
using namespace std;
int Time_Tick()
{
	SYSTEMTIME sys_time;
	GetLocalTime(&sys_time);
	cout<<"now the time is:"<<sys_time.wHour<<sys_time.wMinute<<sys_time.wSecond<<"\n";

	time_t time_now;
	time_t time_all;
	time_t time_user_H;
	time_t time_user_M;
	time_t time_user_S;

	cout<<"your time...\n";
	cout<<"Hour:";
	cin>>time_user_H;

	cout<<"Minute:";
	cin>>time_user_M;

	cout<<"Second:";
	cin>>time_user_S;

	time_now = time(NULL);
	time_all = time_user_H * 3600 + time_user_M * 60 + time_user_S + time_now;
LOOP:
	time_now = time(NULL);
	if(time_now == time_all)
	{
		return 1;
	}
	else
	{
		goto LOOP;
	}
}

/* 我们来分析，第一段代码中是使用了api来获取系统的当前时间，第二段代码中，是使用time()，也同样获取当前的系统时间，
返回的结果是一个time_t类型，是一个大整数，不过其值表示从CUT（Coordinated Universal Time）时间1970年1月1日00:00:00（称为UNIX系统的Epoch时间）到当前时刻的秒数。
所以，大家可以单独写一个小程序来做个试验，time()函数输出的是一个很大的数字。
需要注意的是，要使用time()函数和time_t这个变量声明符需要在前面添加#include <time.h>（用于C语言编程中）或者#include <ctime>（用于C++编程中）。 */


/* 这部分就是定时功能的实现，我们来分析一下吧：用time_user_H、time_user_M、time_user_S三个来获取用户需要设置的时间，
然后，time_now用来获取系统当前时间，time_all用于算出触发事件时的系统时间然后相应的将用户预设时间中的小时分秒都换算为秒。
在LOOP中,不断获取系统时间，不断与time_all比较，当两者相等时，则触发事件。OK，程序逻辑就是这样。
效率自然不用说，相当的低，如果哪位朋友有更好的想法，更高效的写法，也希望分享一下，我也是个喜欢狂热的榨取系统资源的人，在此先谢过了~。 */



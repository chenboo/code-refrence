//��ʱ�ػ�����Ҫ�ǽ�����ζ�ʱ,Ч�ʱȽϵ�


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

/* ��������������һ�δ�������ʹ����api����ȡϵͳ�ĵ�ǰʱ�䣬�ڶ��δ����У���ʹ��time()��Ҳͬ����ȡ��ǰ��ϵͳʱ�䣬
���صĽ����һ��time_t���ͣ���һ����������������ֵ��ʾ��CUT��Coordinated Universal Time��ʱ��1970��1��1��00:00:00����ΪUNIXϵͳ��Epochʱ�䣩����ǰʱ�̵�������
���ԣ���ҿ��Ե���дһ��С�������������飬time()�����������һ���ܴ�����֡�
��Ҫע����ǣ�Ҫʹ��time()������time_t���������������Ҫ��ǰ�����#include <time.h>������C���Ա���У�����#include <ctime>������C++����У��� */


/* �ⲿ�־��Ƕ�ʱ���ܵ�ʵ�֣�����������һ�°ɣ���time_user_H��time_user_M��time_user_S��������ȡ�û���Ҫ���õ�ʱ�䣬
Ȼ��time_now������ȡϵͳ��ǰʱ�䣬time_all������������¼�ʱ��ϵͳʱ��Ȼ����Ӧ�Ľ��û�Ԥ��ʱ���е�Сʱ���붼����Ϊ�롣
��LOOP��,���ϻ�ȡϵͳʱ�䣬������time_all�Ƚϣ����������ʱ���򴥷��¼���OK�������߼�����������
Ч����Ȼ����˵���൱�ĵͣ������λ�����и��õ��뷨������Ч��д����Ҳϣ������һ�£���Ҳ�Ǹ�ϲ�����ȵ�եȡϵͳ��Դ���ˣ��ڴ���л����~�� */



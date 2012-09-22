//枚举(Enum)的使用
#include <iostream>
using namespace std;

enum Weekday
{
	Monday = 1,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday
};


int main()
{
	cout<<"nihao"<<endl;

	Weekday nWeek;

	nWeek = (Weekday)6;//必须强制转换，否则编译错误

	cout<<nWeek<<endl;

	getchar();
	return 0;
}
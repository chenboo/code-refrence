//ö��(Enum)��ʹ��
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

	nWeek = (Weekday)6;//����ǿ��ת��������������

	cout<<nWeek<<endl;

	getchar();
	return 0;
}
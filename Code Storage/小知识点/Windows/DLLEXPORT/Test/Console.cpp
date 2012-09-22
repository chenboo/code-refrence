#include <iostream>
using namespace std;

__declspec(dllimport) int PengJia(int x,int y);
__declspec(dllimport) int PengJian(int x, int y);

int main()
{
	int x = 10;
	int y = 5;

	int sum = PengJia(x,y);
	int Min = PengJian(x,y);

	cout<<"sum = "<<sum<<endl;
	cout<<"Min = "<<Min<<endl;

	getchar();

	return 0;
}
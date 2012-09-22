#include <iostream>
using namespace std;
#include "okspace.h"
//using namespace ok;

int main()
{
	ok::Base	base;
	base.Set(100);
	cout<<base.Get()<<endl;
	getchar();

	return 0;
}

//介绍几个平常用得比较多的获取系统信息的函数

//获取桌面的大小(不包含任务栏区域)

CRect rcWorkArea;
SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);


//获得屏幕宽度和高度
int nFullWidth = GetSystemMetrics(SM_CXSCREEN);
int nFullHeight = GetSystemMetrics(SM_CYSCREEN);
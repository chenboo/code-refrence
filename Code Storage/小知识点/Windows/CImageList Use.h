//use of CImageList

//要添加头文件
#include "afxcmn.h"

//第一种:
CImageList	m_ImageList;
m_ImageList.Create(32, 32, ILC_COLOR32, 10, 30);
m_ImageList.Add(hIcon);


//第二种:
CImageList img;
img.Create(IDB_BITMAP1, 16, ILC_COLOR16, RGB(255, 0, 255));

//第三种:
CBitmap bmp;
bmp.LoadBitmap(IDB_BITMAP1);

CImageList img;
img.Create(16, 15, ILC_COLOR16 | ILC_MASK, 0, 1);
img.Add(&bmp, RGB(255, 0, 255));

//这种情况下要加上ILC_MASK风格。

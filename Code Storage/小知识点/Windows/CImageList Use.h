//use of CImageList

//Ҫ���ͷ�ļ�
#include "afxcmn.h"

//��һ��:
CImageList	m_ImageList;
m_ImageList.Create(32, 32, ILC_COLOR32, 10, 30);
m_ImageList.Add(hIcon);


//�ڶ���:
CImageList img;
img.Create(IDB_BITMAP1, 16, ILC_COLOR16, RGB(255, 0, 255));

//������:
CBitmap bmp;
bmp.LoadBitmap(IDB_BITMAP1);

CImageList img;
img.Create(16, 15, ILC_COLOR16 | ILC_MASK, 0, 1);
img.Add(&bmp, RGB(255, 0, 255));

//���������Ҫ����ILC_MASK���

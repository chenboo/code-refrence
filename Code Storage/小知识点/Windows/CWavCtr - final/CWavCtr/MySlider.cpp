// MySlider.cpp : implementation file
//

#include "stdafx.h"
#include "CWavCtr.h"
#include "MySlider.h"


// CMySlider

IMPLEMENT_DYNAMIC(CMySlider, CSliderCtrl)

CMySlider::CMySlider()
{

}

CMySlider::~CMySlider()
{
}


BEGIN_MESSAGE_MAP(CMySlider, CSliderCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CMySlider message handlers



void CMySlider::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	 CRect rcSlider;
    GetChannelRect(&rcSlider);
    int nMax = GetRangeMax();
    float fInterval = float(rcSlider.Width())/nMax;
    int nPos = (point.x-rcSlider.left+fInterval-1)/fInterval; //��Ŀ��λ��

    CSliderCtrl::SetPos(nPos); //��Ŀ��λ��
    CSliderCtrl::OnLButtonDown(nFlags, point);
    CSliderCtrl::SetPos(nPos); //�ظ���Ŀ��λ��
}

void CMySlider::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	 CRect rcSlider;
    GetChannelRect(&rcSlider);
    int nMax = GetRangeMax();
    float fInterval = float(rcSlider.Width())/nMax;
    int nPos = (point.x-rcSlider.left+fInterval-1)/fInterval; //��Ŀ��λ��

    CSliderCtrl::SetPos(nPos); //��Ŀ��λ��
    CSliderCtrl::OnLButtonUp(nFlags, point);
    CSliderCtrl::SetPos(nPos); //�ظ���Ŀ��λ��
}

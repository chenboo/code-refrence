// WavWnd.cpp : implementation file
//

#include "stdafx.h"
#include "CWavCtr.h"
#include "WavWnd.h"


// CWavWnd

IMPLEMENT_DYNAMIC(CWavWnd, CWnd)

CWavWnd::CWavWnd()
: sample_channel(0)
, sample_BperSec(0)
, sample_Hz(0)
, FileName(_T(""))
, FilePath(_T(""))
, sample_time(0)
, current_time(0)
, wavSize(0)
, IsPlaying(false)
, sample_bit(0)
, back(false)
, reOpen(false)
, Current_pos(0)
, current_x(0)
, current_yL(0)
, current_yR(0)
, P_Direction(true)
{
	m_nDrawed = 0;
	data_ptr = NULL;
	m_pData = NULL;
}

CWavWnd::~CWavWnd()
{
	m_dcMem.SelectObject(m_pOldBmp);
	m_dcMem.DeleteDC();
	m_bitmap.DeleteObject();
}


BEGIN_MESSAGE_MAP(CWavWnd, CWnd)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CWavWnd message handlers



void CWavWnd::OnTimer(UINT_PTR nIDEvent)
{
	CRect Rc;
	GetClientRect(&Rc);
	/*CDC* pDC=GetDC();
	CDC dc;
	dc.CreateCompatibleDC(pDC);*/

	if(reOpen)
	{
		reOpen=!reOpen;
		m_dcMem.FillSolidRect(Rc,RGB(255,255,255));
		Current_pos=0;
		current_time=0;
	}
	if(back)
	{
		if(Current_pos<Rc.right-Rc.left)
		{
			m_dcMem.FillSolidRect(Current_pos,Rc.top,Rc.Width(),Rc.Height(),RGB(255,255,255));
		}
		back=false;
	}
	if(sample_channel==2&&sample_bit==16)   SixDouble();
	else if(sample_channel==1&&sample_bit==16) SixSingle();
	else if(sample_channel==2&&sample_bit==8) EigDouble();
	else if(sample_channel==1&&sample_bit==8) EigDouble();


	CTimeSpan tc(current_time);
	CTimeSpan ts(sample_time);
	if(tc<ts)
	{
		if(IsPlaying)
		{
			if(P_Direction) Current_pos++;
			else Current_pos--;
		}
	}
	else
	{
		if(IsPlaying)
		{
			if(P_Direction==false)
			{
				if(current_time!=0) Current_pos--;
			}
		}
	}






	/*dc.DeleteDC();*/

	CWnd::OnTimer(nIDEvent);
}

void CWavWnd::OnPaint()
{
	// device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	CPaintDC dc(this);

	CRect r;
	GetClientRect(&r);


	dc.BitBlt(0,0,r.Width(),r.Height(),&m_dcMem,0,0,SRCCOPY);
}

int CWavWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	SetTimer(2,50,NULL);

	return 0;
}

void CWavWnd::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(2);
	if(!data_ptr) delete[] data_ptr;
	CWnd::OnClose();
}
void CWavWnd::SixDouble()                                               //双画笔 16位
{
	CRect Rc;
	GetClientRect(&Rc);
	m_dcMem.FillSolidRect(0,0,Rc.Width(),Rc.Height(),RGB(255,255,255));

	CPen penB(PS_SOLID,1,RGB(0,0,0));
	CPen penW(PS_SOLID,1,RGB(255,255,255));
	CPen penL(PS_SOLID,1,RGB(0,255,0));
	CPen penR(PS_SOLID,1,RGB(0,0,255));


	int i=0,j=0,min;
	if(Current_pos<(Rc.right-Rc.left))
	{
		min=0;
	}
	else
	{
		min=Current_pos -(Rc.right-Rc.left);
	}
	double cof=Rc.bottom-Rc.top;
	cof=cof/2;
	cof=cof/65535.0;

	int* tempI=reinterpret_cast<int *>(data_ptr);

	CPen*pOld=m_dcMem.SelectObject(&penW);
	for(j=0;j<Rc.right-Rc.left;j++)
	{
		m_dcMem.MoveTo(j,(Rc.top+Rc.bottom)/2);
		m_dcMem.LineTo(j,Rc.top);
		m_dcMem.MoveTo(j,(Rc.top+Rc.bottom)/2);
		m_dcMem.LineTo(j,Rc.bottom);
	}

	float k=0;
	for(i=min;k+min<Current_pos;i++)
	{
		int a=*(tempI+i*sample_BperSec/(4*20));
		BYTE p[4];
		p[0] = a & 0xff;
		p[1] = (a >> 8) & 0xff;
		p[2] = (a >> 16) & 0xff;
		p[3] = (a >> 24) & 0xff;
		short y=(p[1]<<8)|p[0];


		y=y*cof;
		m_dcMem.SelectObject(&penL);
		m_dcMem.MoveTo(k,(Rc.top+Rc.bottom)/4);
		m_dcMem.LineTo(k,(Rc.top+Rc.bottom)/4-y);

		y=(p[3]<<8)|p[2];


		y=y*cof;
		m_dcMem.SelectObject(&penR);
		m_dcMem.MoveTo(k,(Rc.top+Rc.bottom)*3/4);
		m_dcMem.LineTo(k,(Rc.top+Rc.bottom)*3/4+y);
		k+=1;
	}
	if(min==0)
	{
		if(current_x>Current_pos) current_yL=current_yR=0;
		else
		{
			int a=*(tempI+current_x*sample_BperSec/(4*20));
			BYTE p[4];
			p[0] = a & 0xff;
			p[1] = (a >> 8) & 0xff;
			p[2] = (a >> 16) & 0xff;
			p[3] = (a >> 24) & 0xff;
			current_yL=(p[1]<<8)|p[0];
			current_yR=(p[3]<<8)|p[2];
		}
	}
	else
	{
		int k=current_x;
		k+=min;
		int a=*(tempI+k*sample_BperSec/(4*20));
		BYTE p[4];
		p[0] = a & 0xff;
		p[1] = (a >> 8) & 0xff;
		p[2] = (a >> 16) & 0xff;
		p[3] = (a >> 24) & 0xff;
		current_yL=(p[1]<<8)|p[0];
		current_yR=(p[3]<<8)|p[2];
	}

	//画红色标记线
	CPen Red(PS_SOLID,2,RGB(255,0,0));
	m_dcMem.SelectObject(&Red);
	m_dcMem.MoveTo(current_x,Rc.bottom);
	m_dcMem.LineTo(current_x,Rc.top);


	m_dcMem.SelectObject(&penB);
	m_dcMem.MoveTo(Rc.left,(Rc.top+Rc.bottom)/2);
	m_dcMem.LineTo(Rc.right,(Rc.top+Rc.bottom)/2);
	m_dcMem.MoveTo(Rc.left,Rc.top);
	m_dcMem.LineTo(Rc.left,Rc.bottom);

	m_dcMem.SelectObject(pOld);
	Invalidate();
}

void CWavWnd::EigDouble()                                                //双画笔，8位
{
	CRect Rc;
	GetClientRect(&Rc);

	m_dcMem.FillSolidRect(0,0,Rc.Width(),Rc.Height(),RGB(255,255,255));


	CPen penB(PS_SOLID,1,RGB(0,0,0));
	CPen penW(PS_SOLID,1,RGB(255,255,255));
	CPen penL(PS_SOLID,1,RGB(0,255,0));
	CPen penR(PS_SOLID,1,RGB(0,0,255));



	int i=0,j=0,min;
	if(Current_pos<Rc.right-Rc.left)
	{
		min=0;
	}
	else
	{
		min=Current_pos-(Rc.right-Rc.left);
	}



	double cof=Rc.bottom-Rc.top;
	cof=cof/2;
	cof=cof/256.0;


	unsigned int* tempI=reinterpret_cast<unsigned int *>(data_ptr);

	CPen*pOld=m_dcMem.SelectObject(&penW);
	for(j=0;j<Rc.right-Rc.left;j++)
	{
		m_dcMem.MoveTo(j,Rc.bottom);
		m_dcMem.LineTo(j,Rc.top);
	}
	j=0;

	if(min+current_x>Current_pos)
	{
		current_yL=0;
		current_yR=0;
	}
	float k=0;
	for(i=min;k+min<Current_pos;i++)
	{
		unsigned int a=(*(tempI+i*sample_BperSec/(4*20)));
		BYTE p[4];
		p[0] = a & 0xff;
		p[1] = (a >> 8) & 0xff;
		p[2] = (a >> 16) & 0xff;
		p[3] = (a >> 24) & 0xff;

		short y=p[0]-128;
		y=y*cof;
		m_dcMem.SelectObject(&penL);
		m_dcMem.MoveTo(k,(Rc.top+Rc.bottom)/4);
		m_dcMem.LineTo(k,(Rc.top+Rc.bottom)/4-y);
		
		y=p[1]-128;
		y=y*cof;
		m_dcMem.SelectObject(&penR);
		m_dcMem.MoveTo(k,(Rc.top+Rc.bottom)*3/4);
		m_dcMem.LineTo(k,(Rc.top+Rc.bottom)*3/4-y);
		
		k+=0.5;
		y=p[2]-128;
		y=y*cof;
		m_dcMem.SelectObject(&penL);
		m_dcMem.MoveTo(k,(Rc.top+Rc.bottom)/4);
		m_dcMem.LineTo(k,(Rc.top+Rc.bottom)/4-y);

		y=p[3]-128;
		y=y*cof;
		m_dcMem.SelectObject(&penR);
		m_dcMem.MoveTo(k,(Rc.top+Rc.bottom)*3/4);
		m_dcMem.LineTo(k,(Rc.top+Rc.bottom)*3/4-y);
		k+=0.5;
	}


	if(min==0)
	{
		if(current_x>Current_pos) current_yL=current_yR=0;
		else
		{
			unsigned int a=*(tempI+current_x*sample_BperSec/(4*20));
			BYTE p[4];
			p[0] = a & 0xff;
			p[1] = (a >> 8) & 0xff;
			p[2] = (a >> 16) & 0xff;
			p[3] = (a >> 24) & 0xff;
			current_yL=p[2];
			current_yR=p[3];
		}
	}
	else
	{
		int k=current_x;
		k+=min;
		unsigned int a=*(tempI+k*sample_BperSec/(4*20));
		BYTE p[4];
		p[0] = a & 0xff;
		p[1] = (a >> 8) & 0xff;
		p[2] = (a >> 16) & 0xff;
		p[3] = (a >> 24) & 0xff;
		current_yL=p[2];
		current_yR=p[3];
	}
	//画红色标记线
	CPen Red(PS_SOLID,2,RGB(255,0,0));
	m_dcMem.SelectObject(&Red);
	m_dcMem.MoveTo(current_x,Rc.bottom);
	m_dcMem.LineTo(current_x,Rc.top);

	m_dcMem.SelectObject(&penB);
	m_dcMem.MoveTo(Rc.left,(Rc.top+Rc.bottom)/2);
	m_dcMem.LineTo(Rc.right,(Rc.top+Rc.bottom)/2);
	m_dcMem.MoveTo(Rc.left,Rc.top);
	m_dcMem.LineTo(Rc.left,Rc.bottom);

	m_dcMem.SelectObject(pOld);
	Invalidate();


}

void CWavWnd::SixSingle()                                            //单画笔，16位
{
	current_yR=0;
	CRect Rc;
	GetClientRect(&Rc);

	m_dcMem.FillSolidRect(0,0,Rc.Width(),Rc.Height(),RGB(255,255,255));

	CPen penB(PS_SOLID,1,RGB(0,0,0));
	CPen penW(PS_SOLID,1,RGB(255,255,255));
	CPen penL(PS_SOLID,1,RGB(0,255,0));


	int i=0,j=0,min;
	if(Current_pos<Rc.right-Rc.left)
	{
		min=0;
	}
	else
	{
		min=Current_pos-(Rc.right-Rc.left);
	}





	double cof=Rc.bottom-Rc.top;
	cof=cof/65535.0;

	int* tempI=reinterpret_cast<int *>(data_ptr);

	CPen*pOld=m_dcMem.SelectObject(&penW);
	for(j=0;j<Rc.right-Rc.left;j++)
	{
		m_dcMem.MoveTo(j,Rc.bottom);
		m_dcMem.LineTo(j,Rc.top);
	}
	j=0;

	float k=0;
	for(i=min;k+min<Current_pos;i++)
	{
		int a=*(tempI+i*sample_BperSec/(4*20));
		BYTE p[4];
		p[0] = a & 0xff;
		p[1] = (a >> 8) & 0xff;
		p[2] = (a >> 16) & 0xff;
		p[3] = (a >> 24) & 0xff;
		short y=(p[1]<<8)|p[0];
		y=y*cof;
		m_dcMem.SelectObject(&penL);
		m_dcMem.MoveTo(k,(Rc.bottom+Rc.top)/2);
		m_dcMem.LineTo(k,(Rc.bottom+Rc.top)/2-y);
		y=(p[3]<<8)|p[2];


		y=y*cof;
		m_dcMem.MoveTo(k,(Rc.bottom+Rc.top)/2);
		m_dcMem.LineTo(k,(Rc.bottom+Rc.top)/2-y);
		k+=0.5;
	}

	
	if(min==0)
	{
		if(current_x>Current_pos) current_yL=current_yR=0;
		else
		{
			unsigned int a=*(tempI+current_x*sample_BperSec/(4*20));
			BYTE p[4];
			p[0] = a & 0xff;
			p[1] = (a >> 8) & 0xff;
			p[2] = (a >> 16) & 0xff;
			p[3] = (a >> 24) & 0xff;
			current_yL=(p[3]<<8)|p[2];
			current_yR=0;
		}
	}
	else
	{
		int k=current_x;
		k+=min;
		int a=*(tempI+k*sample_BperSec/(4*20));
		BYTE p[4];
		p[0] = a & 0xff;
		p[1] = (a >> 8) & 0xff;
		p[2] = (a >> 16) & 0xff;
		p[3] = (a >> 24) & 0xff;
		current_yL=(p[3]<<8)|p[2];
		current_yR=0;
	}

	//画红色标记线
	CPen Red(PS_SOLID,2,RGB(255,0,0));
	m_dcMem.SelectObject(&Red);
	m_dcMem.MoveTo(current_x,Rc.bottom);
	m_dcMem.LineTo(current_x,Rc.top);


	m_dcMem.SelectObject(&penB);
	m_dcMem.MoveTo(Rc.left,Rc.bottom);
	m_dcMem.LineTo(Rc.right,Rc.bottom);
	m_dcMem.MoveTo(Rc.left,Rc.top);
	m_dcMem.LineTo(Rc.left,Rc.bottom);

	m_dcMem.SelectObject(pOld);
	Invalidate();



}
void CWavWnd::EigSingle()                                          //单画笔，8位
{
	current_yR=0;
	CRect Rc;
	GetClientRect(&Rc);

	m_dcMem.FillSolidRect(0,0,Rc.Width(),Rc.Height(),RGB(255,255,255));

	CPen penB(PS_SOLID,1,RGB(0,0,0));
	CPen penW(PS_SOLID,1,RGB(255,255,255));
	CPen penL(PS_SOLID,1,RGB(0,255,0));


	int i=0,j=0,min;
	if(Current_pos<Rc.right-Rc.left)
	{
		min=0;
	}
	else
	{
		min=Current_pos-(Rc.right-Rc.left);
	}





	double cof=Rc.bottom-Rc.top;
	cof=cof/256.0;

	unsigned int* tempI=reinterpret_cast<unsigned int *>(data_ptr);

	CPen*pOld=m_dcMem.SelectObject(&penW);
	for(j=0;j<Rc.right-Rc.left;j++)
	{
		m_dcMem.MoveTo(j,Rc.bottom);
		m_dcMem.LineTo(j,Rc.top);
	}

	float k=0;
	for(i=min;k+min<Current_pos;i++)
	{
		unsigned int a=*(tempI+i*sample_BperSec/(4*20));
		BYTE p[4];
		p[0] = a & 0xff;
		p[1] = (a >> 8) & 0xff;
		p[2] = (a >> 16) & 0xff;
		p[3] = (a >> 24) & 0xff;

		short y; 
		y=p[0]-128;
		y=y*cof;
		m_dcMem.SelectObject(&penL);
		m_dcMem.MoveTo(k,(Rc.bottom+Rc.top)/2);
		m_dcMem.LineTo(k,(Rc.bottom+Rc.top)/2-y);


		y=p[1]-128;
		y=y*cof;
		m_dcMem.SelectObject(&penL);
		m_dcMem.MoveTo(k,(Rc.bottom+Rc.top)/2);
		m_dcMem.LineTo(k,(Rc.bottom+Rc.top)/2-y);


		y=p[2]-128;
		y=y*cof;
		m_dcMem.SelectObject(&penL);
		m_dcMem.MoveTo(k,(Rc.bottom+Rc.top)/2);
		m_dcMem.LineTo(k,(Rc.bottom+Rc.top)/2-y);



		y=p[3]-128;
		y=y*cof;
		m_dcMem.SelectObject(&penL);
		m_dcMem.MoveTo(k,(Rc.bottom+Rc.top)/2);
		m_dcMem.LineTo(k,(Rc.bottom+Rc.top)/2-y);


		k+=0.5;
	}

		if(min==0)
	{
		if(current_x>Current_pos) current_yL=current_yR=0;
		else
		{
			unsigned int a=*(tempI+current_x*sample_BperSec/(4*20));
			BYTE p[4];
			p[0] = a & 0xff;
			p[1] = (a >> 8) & 0xff;
			p[2] = (a >> 16) & 0xff;
			p[3] = (a >> 24) & 0xff;
			current_yL=p[3];
			current_yR=0;
		}
	}
	else
	{
		int k=current_x;
		k+=min;
		int a=*(tempI+k*sample_BperSec/(4*20));
		BYTE p[4];
		p[0] = a & 0xff;
		p[1] = (a >> 8) & 0xff;
		p[2] = (a >> 16) & 0xff;
		p[3] = (a >> 24) & 0xff;
		current_yL=p[3];
		current_yR=0;
	}

	//画红色标记线
	CPen Red(PS_SOLID,2,RGB(255,0,0));
	m_dcMem.SelectObject(&Red);
	m_dcMem.MoveTo(current_x,Rc.bottom);
	m_dcMem.LineTo(current_x,Rc.top);


	m_dcMem.SelectObject(&penB);
	m_dcMem.MoveTo(Rc.left,Rc.bottom);
	m_dcMem.LineTo(Rc.right,Rc.bottom);
	m_dcMem.MoveTo(Rc.left,Rc.top);
	m_dcMem.LineTo(Rc.left,Rc.bottom);

	m_dcMem.SelectObject(pOld);
	Invalidate();
}


void CWavWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	current_x=point.x;
	CWnd::OnLButtonDown(nFlags, point);
}

void CWavWnd::OnSize(UINT nType, int cx, int cy)
{
	if (cx <= 0 || cy <= 0) {
		return;
	}

	//初始化内存dc
	if (!m_dcMem.GetSafeHdc()) {
		CRect r;
		GetClientRect(&r);
		CDC*pDC=GetDC();
		m_dcMem.CreateCompatibleDC(pDC);
		m_bitmap.CreateCompatibleBitmap(pDC,r.Width(),r.Height());
		m_pOldBmp=m_dcMem.SelectObject(&m_bitmap);

		m_dcMem.FillSolidRect(r,RGB(255,255,255));
		CPen penB(PS_SOLID,1,RGB(0,0,0));

		CPen* pOldPen = m_dcMem.SelectObject(&penB);
		m_dcMem.MoveTo(r.left,(r.top+r.bottom)/2);
		m_dcMem.LineTo(r.right,(r.top+r.bottom)/2);
		m_dcMem.MoveTo(r.left,r.top);
		m_dcMem.LineTo(r.left,r.bottom);
		m_dcMem.SelectObject(pOldPen);
	}

	CWnd::OnSize(nType, cx, cy);
}


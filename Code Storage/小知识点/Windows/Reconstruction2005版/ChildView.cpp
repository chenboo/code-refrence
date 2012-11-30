// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "Reconstruction.h"
#include "ChildView.h"
#include "MainFrm.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PI			3.1415926
#define CENTERX		541
#define CENTERY		298
#define RADIUS		172

#define WM_CONTROL	(WM_USER + 10)
#define WM_SETCLR	(WM_USER + 12)

CChildView::CChildView()
{
	m_pDC = NULL;
	m_Rotation.xDelta = m_Rotation.yDelta = 0;
	m_Scale.xScale = m_Scale.yScale = 1.0;
	m_clrBk.clrR = 0.0f;
	m_clrBk.clrG = 0.0f;
	m_clrBk.clrB = 0.2f;
	m_pDibBits = NULL;
	m_nList = 0;

	m_bHint = FALSE;
	m_bLight = FALSE;
	m_bRotation = FALSE;
	m_bScaleAll = FALSE;
	LoadBmp(_T("res\\colorball.bmp"));
}
CChildView::~CChildView()
{
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_CONTROL,DoControl)
	ON_MESSAGE(WM_SETCLR,SetTheClr)
END_MESSAGE_MAP()

// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

//用于初始化渲染描述表
void CChildView::InitHGLRC()
{
	m_pDC = new CClientDC(this);
	HGLRC hrc;
	if(!SetPixelFormatInfo())
		return;
	hrc = wglCreateContext(m_pDC->GetSafeHdc());
	wglMakeCurrent(m_pDC->GetSafeHdc(),hrc);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//设置像素模式
BOOL CChildView::SetPixelFormatInfo()
{
	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | 
		PFD_DOUBLEBUFFER ;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.cDepthBits = 32;

	int pixelformat;

	if((pixelformat = ChoosePixelFormat(m_pDC->GetSafeHdc(),&pfd)) == 0 )
	{
		MessageBox(_T("ChoosePixelFormat failed"));
		return FALSE;
	}

	if(!SetPixelFormat(m_pDC->GetSafeHdc(),pixelformat,&pfd))
	{
		MessageBox(_T("SetPixelFormat failed"));
		return FALSE;
	}
	return TRUE;

}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	//	return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}


//加载位图
void CChildView::LoadBmp(CString strfilename)
{
	CFile file;
	if(file.Open(strfilename,CFile::modeRead))
	{
		int ret = 0;
		int headpos = 0;
		int nCount = 0;

		ret = file.Read(&m_bmfileheader,sizeof(BITMAPFILEHEADER));
		if(ret != sizeof(BITMAPFILEHEADER)||(m_bmfileheader.bfType != 0x4d42))	
		{
			MessageBox(_T("图片格式不符"));
			return ;
		}
		ret = file.Read(&m_bmheader,sizeof(BITMAPINFOHEADER));
		if(ret != sizeof(BITMAPINFOHEADER))
		{
			MessageBox(_T("图片格式不符"));
			return ;
		}

		DWORD	bytes_per_line;
		bytes_per_line = (m_bmheader.biWidth * m_bmheader.biBitCount + 7)/8;
		bytes_per_line = (bytes_per_line + 3)/4;
		bytes_per_line *= 4;

		if(m_pDibBits)
		{
			delete [] m_pDibBits;
			m_pDibBits = NULL;
		}

		nCount = bytes_per_line * m_bmheader.biHeight;
		m_pDibBits = new char[nCount];
		file.Seek(m_bmfileheader.bfOffBits,CFile::begin);
		nCount = file.Read(m_pDibBits,nCount);

		file.Close();
	}
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1)
	{
		if(m_Rotation.xDelta == 360.0f){m_Rotation.xDelta = 0.0f;}else{m_Rotation.xDelta += 1.0f;}
		if(m_Rotation.yDelta == 360.0f){m_Rotation.yDelta = 0.0f;}else{m_Rotation.yDelta += 1.0f;}			
		Invalidate(FALSE);
	}
	CWnd::OnTimer(nIDEvent);
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if( cx < 0 || cy < 0)
		return;

	CRect rect;
	m_wndControl.GetWindowRect(rect);
	int nWidth = rect.Width();
	m_wndControl.ScWndToOrg();
	m_wndControl.MoveWindow(0,0,nWidth,cy);

	glViewport(nWidth,0,cx - nWidth,cy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)(cx - nWidth)/cy, 1.0, 15.0);
	//RedrawWindow();

}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_wndControl.Create(CControlBoard::IDD, this))
		return -1;
	m_wndControl.ShowWindow(SW_SHOW);
	m_wndControl.GetMessageID(WM_CONTROL,WM_SETCLR);

	InitHGLRC();
	MakeModelList();

	float	ambientProperties[]  = {0.2f, 0.2f, 0.2f, 0.1f};
	float	diffuseProperties[]  = {0.2f, 0.2f, 0.2f, 1.0f};
	float	specularProperties[] = {0.5f, 0.5f, 0.5f, 1.0f};
	float	position[] = {-1.0,0.1f,1.0f,0.0f};

	glLightfv( GL_LIGHT0, GL_AMBIENT, ambientProperties);
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseProperties);
	glLightfv( GL_LIGHT0, GL_SPECULAR, specularProperties);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);

	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);

	return 0;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	static BOOL bBusy = FALSE;
	if(bBusy) return;

	glClearColor(m_clrBk.clrR,m_clrBk.clrG,m_clrBk.clrB,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	DoDraw();
	glFinish();
	SwapBuffers(wglGetCurrentDC());
	bBusy = FALSE;

	CString strx,stry;
	strx.Format(_T("x方向旋转角度：%.1f"),m_Rotation.xDelta);
	stry.Format(_T("y方向旋转角度：%.1f"),m_Rotation.yDelta);
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CStatusBar* pStatus = &pFrame->m_wndStatusBar;
	if(pStatus)
	{
		pStatus->SetPaneStyle(1,SBPS_POPOUT);
		pStatus->SetPaneStyle(2,SBPS_POPOUT);
		pStatus->SetPaneText(1,strx);
		pStatus->SetPaneText(2,stry);
	}
}

//绘制
void CChildView::DoDraw()
{
	glTranslatef(0.0f,0.0f,-2.0f);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

	glPushMatrix();
	glScalef(m_Scale.xScale,m_Scale.yScale,1.0f);
	glRotated(m_Rotation.xDelta,1.0,0.0,0.0);
	glRotated(m_Rotation.yDelta,0.0,1.0,0.0);
	DrawTextTure();
	glPopMatrix();

	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_POINT_SMOOTH);
}

//绘制图片纹理
void CChildView::DrawTextTure()
{
	SetTheBmpMap();
	glTexImage2D(GL_TEXTURE_2D,0,3,m_bmheader.biWidth,m_bmheader.biHeight,
		0,GL_BGR_EXT,GL_UNSIGNED_BYTE,m_pDibBits);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glCallList(m_nList);
}

//设置纹理映射方式
void CChildView::SetTheBmpMap()	
{
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
	glShadeModel(GL_SMOOTH);
}

//用显示列表的方式三维建模
void CChildView::MakeModelList()
{
	glPushMatrix();
	m_nList = glGenLists(1);
	glListBase(m_nList);
	glNewList(m_nList,GL_COMPILE);

	int i, j;
	float theta1,theta2,theta3;
	float x, y, z, px, py, pz, tex_x, tex_y;
	float Precision = 200.0f;
	float Radius = 0.5;

	for( j = 0; j <= Precision/2 - 1; j++ )
	{ 
		theta1 = j*2*PI/Precision - PI/2;
		theta2 = (j+1)*2*PI/Precision - PI/2;

		glBegin(GL_QUAD_STRIP);
		for(i = 0; i <= Precision; i++ )
		{
			theta3 = i*2*PI/Precision;

			x = cos(theta2) * cos(theta3);
			y = sin(theta2);
			z = cos(theta2) * sin(theta3);
			px = Radius*x;
			py = Radius*y;
			pz = Radius*z;

			tex_x = 1.0f - i/Precision;
			tex_y = 2.0*(j+1.0)/Precision;

			glTexCoord2f(tex_x, tex_y);
			glVertex3f(px,py,pz);

			x = cos(theta1) * cos(theta3);
			y = sin(theta1);
			z = cos(theta1) * sin(theta3);
			px = Radius*x;
			py = Radius*y;
			pz = Radius*z;

			tex_x = 1.0f - i/Precision;
			tex_y = 2.0*j/Precision;

			glTexCoord2f(tex_x, tex_y);
			glVertex3f(px,py,pz);
		}
		glEnd();
	}
	glEndList();
	glPopMatrix();
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_ptDelPos = point;
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR1));

	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(MK_LBUTTON == nFlags)
	{
		if(m_Rotation.xDelta >= 360.0f || m_Rotation.xDelta <= -360.0f)
			m_Rotation.xDelta = 0.0f;
		if(m_Rotation.yDelta >= 360.0f || m_Rotation.yDelta <= -360.0f)
			m_Rotation.yDelta = 0.0f;

		m_Rotation.xDelta-= (m_ptDelPos.y - point.y)/2.0f;
		m_Rotation.yDelta -= (m_ptDelPos.x - point.x)/2.0f;
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR1));
		m_ptDelPos = point;
		Invalidate(FALSE);
	}
	else
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR2));

	CWnd::OnMouseMove(nFlags, point);
}


LRESULT CChildView::DoControl(WPARAM wParam, LPARAM lParam)
{
	if(IDC_BTN_BMP == wParam)
	{
		CString filter;
		filter = "位图文件(*.bmp)|*.bmp||";
		CFileDialog filedlg(TRUE,_T(".bmp"),NULL,OFN_HIDEREADONLY,filter);
		if(IDOK == filedlg.DoModal())
		{
			CString str;
			str = filedlg.GetPathName();
			LoadBmp(str);
		}
	}

	if(ID_HINT == wParam)
	{
		m_bHint = !m_bHint;
		if(m_bHint)	
		{	
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		}
		else
		{
			glDisable(GL_LINE_SMOOTH);glDisable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
			glLineWidth(1.0);
		}
	}
	if(ID_LIGHTING == wParam)
	{
		m_bLight = !m_bLight;
		if(m_bLight) 
		{
			glEnable(GL_LIGHT0);
			glEnable(GL_LIGHTING);
		}
		else 
		{
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHTING);
		}
	}
	if(ID_ROTATION == wParam)
	{
		m_bRotation = !m_bRotation;
		if(m_bRotation)
			SetTimer(1,100,NULL);
		else KillTimer(1);
	}
	if(ID_WIREFRAME == wParam)
	{
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	}
	if(ID_VERTEX == wParam) 
	{
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
	}
	if(ID_TEXTURE == wParam)
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glEnable(GL_TEXTURE_2D);
	}

	if(ID_SCALEALL == wParam)
	{
		m_bScaleAll = !m_bScaleAll;
		if(m_bScaleAll)
			m_Scale.xScale = m_Scale.yScale = 1.0;
	}
	if(ID_SCALE_X == wParam || ID_SCALE_Y == wParam)
	{
		if(ID_SCALE_X == wParam)
			m_Scale.xScale = (float)lParam/50.0;
		else if(ID_SCALE_Y == wParam)
			m_Scale.yScale = (float)lParam/50.0;
		if(m_bScaleAll)
			m_Scale.xScale = m_Scale.yScale = (float)lParam/50.0;
	}

	if(IDC_BTN_ANGLERESET == wParam)
		m_Rotation.xDelta = m_Rotation.yDelta = 0;

	Invalidate(FALSE);
	return 0;
}

LRESULT CChildView::SetTheClr(WPARAM wParam, LPARAM lParam)
{
	COLORREF clr = (COLORREF)wParam;
	m_clrBk.clrR = (float)GetRValue(clr)/255.0;
	m_clrBk.clrG = (float)GetGValue(clr)/255.0;
	m_clrBk.clrB = (float)GetBValue(clr)/255.0;

	clr = (COLORREF)lParam;
	m_clrLight.clrR = (float)GetRValue(clr)/255.0;
	m_clrLight.clrG = (float)GetGValue(clr)/255.0;
	m_clrLight.clrB = (float)GetBValue(clr)/255.0;

	float	ambientProperties[]  = {m_clrLight.clrR,m_clrLight.clrG,m_clrLight.clrB,1.0f};
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambientProperties);

	Invalidate(FALSE);
	return 0;
}

void CChildView::OnDestroy()
{
	CWnd::OnDestroy();
	KillTimer(1);
	HGLRC	hrc;
	hrc = ::wglGetCurrentContext();
	::wglMakeCurrent(NULL,  NULL);
	if (hrc)
		::wglDeleteContext(hrc);
	if(m_pDC)
		delete m_pDC;
	if(m_pDibBits)
	{
		delete [] m_pDibBits;
		m_pDibBits = NULL;
	}
}
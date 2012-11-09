// ControlBoard.cpp : 实现文件
//

#include "stdafx.h"
#include "Reconstruction.h"
#include "ControlBoard.h"

#define LINESIZE	8

// CControlBoard 对话框

IMPLEMENT_DYNAMIC(CControlBoard, CDialog)

CControlBoard::CControlBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CControlBoard::IDD, pParent)
{
	m_ClrBk = RGB(0,0,51);
	m_ClrLight = RGB(0,0,51);
	
}

CControlBoard::~CControlBoard()
{
}

void CControlBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, ID_BK_COLOR, m_ControlBkclr);
	DDX_Control(pDX, ID_LIGHT_COLOR, m_ControlLightclr);
	DDX_Control(pDX, ID_SCALE_X, m_xSlider);
	DDX_Control(pDX, ID_SCALE_Y, m_ySlider);

}

BEGIN_MESSAGE_MAP(CControlBoard, CDialog)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_COMMAND_RANGE(ID_HINT,ID_SCALEALL,OnControl)
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

//取得向父窗口发送消息的消息ID
void CControlBoard::GetMessageID(UINT nMsg,UINT nMsgClr)	
{
	m_nMsg[0] = nMsg;
	m_nMsg[1] = nMsgClr;
}
//让窗口滑回到原始位置
void CControlBoard::ScWndToOrg()
{
	ScrollWindow(0,m_nVScrollPos);
	m_nVScrollPos = 0;
}
void CControlBoard::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	int nVmax = 0;
	if(cy < m_nViewHeight)
	{
		nVmax = m_nViewHeight - 1;
		m_nVPageSize = cy;
	}

	SCROLLINFO scrinfo;
	scrinfo.fMask = SIF_PAGE | SIF_RANGE;
	scrinfo.nMax = nVmax;
	scrinfo.nMin = 0;
	scrinfo.nPage = m_nVPageSize;

	SetScrollInfo(SB_VERT,&scrinfo,TRUE);
}

BOOL CControlBoard::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect;
	GetWindowRect(rect);
	m_nViewHeight = rect.Height();
	m_nVScrollPos = 0;

	m_xSlider.SetRange(1,100,TRUE);
	m_ySlider.SetRange(1,100,TRUE);


	m_xSlider.SetPos(50);
	m_ySlider.SetPos(50);


	CheckRadioButton(ID_VERTEX,ID_TEXTURE,ID_TEXTURE);

	m_bScaleAll = FALSE;
	return TRUE; 
}

HBRUSH CControlBoard::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	CRect rect;
	CBrush* pOldBrush;
	if(m_brush.GetSafeHandle())
		m_brush.DeleteObject();
	switch(pWnd->GetDlgCtrlID())
	{
	case ID_BK_COLOR:
		m_brush.CreateSolidBrush(m_ClrBk);
		pOldBrush = pDC->SelectObject(&m_brush);
		pWnd->GetClientRect(rect);
		pDC->Rectangle(rect);
		pDC->SelectObject(pOldBrush);
		break;
	case ID_LIGHT_COLOR:
		m_brush.CreateSolidBrush(m_ClrLight);
		pOldBrush = pDC->SelectObject(&m_brush);
		pWnd->GetClientRect(rect);
		pDC->Rectangle(rect);
		pDC->SelectObject(pOldBrush);	
	default:
		break;
	}
	return hbr;
}

void CControlBoard::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int nDelta = 0;
	switch(nSBCode)
	{
	case SB_LINEUP:
		nDelta = - LINESIZE;
		break;
	case SB_LINEDOWN:
		nDelta = LINESIZE;
		break;
	case SB_PAGEUP:
		nDelta = - m_nVPageSize;
		break;
	case SB_PAGEDOWN:
		nDelta = m_nVPageSize;
		break;
	case SB_THUMBTRACK:
		nDelta = nPos - m_nVScrollPos;
		break;
	default :
		return;
	}
	int nScrollPos = m_nVScrollPos + nDelta;
	int nMaxPos = m_nViewHeight - m_nVPageSize;

	if (nScrollPos < 0)
		nDelta = - m_nVScrollPos;
	else if (nScrollPos > nMaxPos)
		nDelta = nMaxPos - m_nVScrollPos;

	if(nDelta != 0)
	{
		m_nVScrollPos += nDelta;
		SetScrollPos(SB_VERT,m_nVScrollPos);
		ScrollWindow(0,-nDelta);
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CControlBoard::OnControl(UINT nID)
{
	GetParent()->PostMessage(m_nMsg[0],nID);
	if(ID_SCALEALL == nID)
	{
		m_bScaleAll = !m_bScaleAll;
		if(m_bScaleAll)
		{
			m_xSlider.SetPos(50);
			m_ySlider.SetPos(50);
		}
	}
}

void CControlBoard::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect[2];
	m_ControlBkclr.GetWindowRect(rect[0]);
	m_ControlLightclr.GetWindowRect(rect[1]);
	ScreenToClient(rect[0]);
	ScreenToClient(rect[1]);
	if(rect[0].PtInRect(point))
	{
		CColorDialog clrDlg(m_ClrBk);
		if(IDOK == clrDlg.DoModal())
			m_ClrBk = clrDlg.GetColor();
		InvalidateRect(rect[0]);
	}
	else
	if(rect[1].PtInRect(point))
	{
		CColorDialog clrDlg(m_ClrLight);
		if(IDOK == clrDlg.DoModal())
			m_ClrLight = clrDlg.GetColor();
		InvalidateRect(rect[1]);

	}
	GetParent()->PostMessage(m_nMsg[1],m_ClrBk,m_ClrLight);

	CDialog::OnLButtonDown(nFlags, point);
}

void CControlBoard::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_brush.GetSafeHandle())
		m_brush.DeleteObject();
}

void CControlBoard::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int pos = 0;
	int nID = pScrollBar->GetDlgCtrlID();
	switch(nID)
	{
	case ID_SCALE_X:
		pos = m_xSlider.GetPos();
		break;
	case ID_SCALE_Y:
		pos = m_ySlider.GetPos();
		break;
	default:
		break;
	}
	if(m_bScaleAll)
	{
		m_xSlider.SetPos(pos);
		m_ySlider.SetPos(pos);
	}
	GetParent()->PostMessage(m_nMsg[0],nID,pos);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

// SpeakersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "libElcbaseUI.h"
#include "SpeakMemberDlg.h"
#include "SpeakWnd.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CSpeakMemberDlg dialog

IMPLEMENT_DYNAMIC(CSpeakMemberDlg, CDialog)

CSpeakMemberDlg::CSpeakMemberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpeakMemberDlg::IDD, pParent)
{
	m_hNofityWnd = NULL;
	m_nNotifyMsg = 0;
}

CSpeakMemberDlg::~CSpeakMemberDlg()
{
}

void CSpeakMemberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPEAKERS, m_wndSpeakersList);
}


BEGIN_MESSAGE_MAP(CSpeakMemberDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SPEAKERS, &CSpeakMemberDlg::OnMemberListItemChanged)
	ON_NOTIFY(NM_DBLCLK, IDC_SPEAKERS, &CSpeakMemberDlg::OnNMDblclkSpeakers)
END_MESSAGE_MAP()


// CSpeakMemberDlg message handlers

void CSpeakMemberDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndSpeakersList.GetSafeHwnd())) {
		m_wndSpeakersList.SetWindowPos(NULL, 0, 32, cx, cy - 32, SWP_NOZORDER);
	}
}

BOOL CSpeakMemberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyle(0, WS_CLIPCHILDREN);

	m_brush.CreateSolidBrush(RGB(221,231,238));

	CBitmap bmpBase;
	m_imgList.Create(16, 16, ILC_MASK | ILC_COLOR24, 0, 1);
	bmpBase.Attach(::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_GROUP_LIST_STATE), 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION));
	m_imgList.Add(&bmpBase, RGB(255,0,255));

	m_wndSpeakersList.SetImageList(&m_imgList, LVSIL_SMALL);
	m_wndSpeakersList.SetBkColor(RGB(250,250,250));

	return TRUE;
}

HBRUSH CSpeakMemberDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_STATIC) {
		pDC->SetBkMode(TRANSPARENT);
	}

	return m_brush;
}

void CSpeakMemberDlg::OnMemberListItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	::PostMessage(m_hNofityWnd, m_nNotifyMsg, 0, 0);
	
	*pResult = 0;
}

void CSpeakMemberDlg::SetNotifyWindow(HWND hWnd, UINT nMsg)
{
	m_hNofityWnd = hWnd;
	m_nNotifyMsg = nMsg;
}

void CSpeakMemberDlg::OnNMDblclkSpeakers(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	::ScreenToClient(m_wndSpeakersList.GetSafeHwnd(), &pt);

	UINT uFlag = 0;
	m_wndSpeakersList.HitTest(pt, &uFlag);

	if (uFlag & LVHT_ONITEM) {
		CSpeakWnd* pSpeakWnd = (CSpeakWnd *)GetParent()->GetParent();
		pSpeakWnd->GetAvailableMemberDlg()->OnBnClickedClear();
	}

	*pResult = 0;
}

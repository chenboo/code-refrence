// TranslateMemberDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "TranslateMemberDlg.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CTranslateMemberDlg dialog

IMPLEMENT_DYNAMIC(CTranslateMemberDlg, CDialog)

CTranslateMemberDlg::CTranslateMemberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTranslateMemberDlg::IDD, pParent)
{
	m_brBkgnd.CreateSolidBrush(RGB(221,231,238));

	m_hNofityWnd = NULL;
	m_nNotifyMsg = 0;
}

CTranslateMemberDlg::~CTranslateMemberDlg()
{
}

void CTranslateMemberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRANSLATE_MEMBER_LIST, m_wndTranslateList);
}


BEGIN_MESSAGE_MAP(CTranslateMemberDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_TRANSLATE_MEMBER_LIST, &CTranslateMemberDlg::OnTranslateMemberListBeginLabelEdit)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_TRANSLATE_MEMBER_LIST, &CTranslateMemberDlg::OnTranslateMemberListEndLabelEdit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TRANSLATE_MEMBER_LIST, &CTranslateMemberDlg::OnMemberListItemChanged)
END_MESSAGE_MAP()


// CTranslateMemberDlg message handlers

BOOL CTranslateMemberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyle(0, WS_CLIPCHILDREN);

	CString strText;
	strText.LoadString(IDS_LANGUAGE);
	m_wndTranslateList.InsertColumn(0, strText, 0, 100);
	strText.LoadString(IDS_TRANSLATOR);
	m_wndTranslateList.InsertColumn(1, strText, 0, 100);

	m_wndTranslateList.ModifyStyle(0, LVS_SINGLESEL);
	m_wndTranslateList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	return TRUE;
}

HBRUSH CTranslateMemberDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_STATIC) 
	{
		pDC->SetBkMode(TRANSPARENT);
	}

	return (HBRUSH)m_brBkgnd;
}

void CTranslateMemberDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndTranslateList.GetSafeHwnd()))
	{
		CRect rcList;
		m_wndTranslateList.GetWindowRect(&rcList);
		ScreenToClient(&rcList);
		rcList.right = cx;
		rcList.bottom = cy;

		m_wndTranslateList.MoveWindow(rcList);
	}
}

void CTranslateMemberDlg::OnTranslateMemberListBeginLabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	//NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	m_wndTranslateList.GetEditControl()->LimitText(31);
	
	*pResult = 0;
}

void CTranslateMemberDlg::OnTranslateMemberListEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	if (pDispInfo->item.pszText != NULL)
	{
		m_wndTranslateList.SetItemText(pDispInfo->item.iItem, 0, pDispInfo->item.pszText);
	}

	*pResult = 0;
}

void CTranslateMemberDlg::OnMemberListItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	::PostMessage(m_hNofityWnd, m_nNotifyMsg, 0, 0);
	
	*pResult = 0;
}

void CTranslateMemberDlg::SetNotifyWindow(HWND hWnd, UINT nMsg)
{
	m_hNofityWnd = hWnd;
	m_nNotifyMsg = nMsg;
}

// SceneMemberDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "SceneMemberDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CSceneMemberDlg dialog

IMPLEMENT_DYNAMIC(CSceneMemberDlg, CDialog)

CSceneMemberDlg::CSceneMemberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSceneMemberDlg::IDD, pParent)
{
	m_brBkgnd.CreateSolidBrush(RGB(221,231,238));

	m_hNofityWnd = NULL;
	m_nNotifyMsg = 0;
}

CSceneMemberDlg::~CSceneMemberDlg()
{
}

void CSceneMemberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCENE_MEMBER_LIST, m_wndSceneMemberList);
}


BEGIN_MESSAGE_MAP(CSceneMemberDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SCENE_MEMBER_LIST, &CSceneMemberDlg::OnSceneMemberListChanged)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_SCENE_MEMBER_LIST, &CSceneMemberDlg::OnSceneMemberListBeginLabelEdit)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_SCENE_MEMBER_LIST, &CSceneMemberDlg::OnSceneMemberListEndLabelEdit)
END_MESSAGE_MAP()


// CSceneMemberDlg message handlers

BOOL CSceneMemberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyle(0, WS_CLIPCHILDREN);

	CString strText;

	strText.LoadString(IDS_SCENE_IMPERSONATOR);
	m_wndSceneMemberList.InsertColumn(0, strText, 0, 100);

	strText.LoadString(IDS_SCENE_ROLE);
	m_wndSceneMemberList.InsertColumn(0, strText, 0, 70);

	m_wndSceneMemberList.ModifyStyle(0, LVS_SINGLESEL);
	m_wndSceneMemberList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	return TRUE;
}

HBRUSH CSceneMemberDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_STATIC) 
	{
		pDC->SetBkMode(TRANSPARENT);
	}

	return (HBRUSH)m_brBkgnd;
}

void CSceneMemberDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndSceneMemberList.GetSafeHwnd()))
	{
		CRect rcList;
		m_wndSceneMemberList.GetWindowRect(&rcList);
		ScreenToClient(&rcList);
		rcList.right = cx;
		rcList.bottom = cy;

		m_wndSceneMemberList.MoveWindow(rcList);
	}
}

void CSceneMemberDlg::OnSceneMemberListChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	::PostMessage(m_hNofityWnd, m_nNotifyMsg, 0, 0);

	*pResult = 0;
}

void CSceneMemberDlg::OnSceneMemberListBeginLabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	//NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	m_wndSceneMemberList.GetEditControl()->LimitText(31);
		
	*pResult = 0;
}

void CSceneMemberDlg::OnSceneMemberListEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;
	
	if (pDispInfo->item.pszText != NULL) {
		if (_tcslen(pDispInfo->item.pszText) == 0) {
			ElcMessageBox(IDS_SCENE_NAME_EMPTY, MB_OK, GetSafeHwnd());
			m_wndSceneMemberList.SetFocus();
			m_wndSceneMemberList.EditLabel(pDispInfo->item.iItem);
		}
		else {
			BOOL bSuccessful = TRUE;
			int nItem = m_wndSceneMemberList.GetNextItem(-1, LVNI_ALL);
			while (nItem != -1) {
				if (nItem != pDispInfo->item.iItem) {
					if (m_wndSceneMemberList.GetItemText(nItem, 0).CompareNoCase(pDispInfo->item.pszText) == 0) {
						bSuccessful = FALSE;
						break;
					}
				}

				nItem = m_wndSceneMemberList.GetNextItem(nItem, LVNI_ALL);
			}

			if (bSuccessful) {
				m_wndSceneMemberList.SetItemText(pDispInfo->item.iItem, 0, pDispInfo->item.pszText);
			}
			else {
				ElcMessageBox(IDS_SCENE_NAME_DUPLICATE, MB_OK, GetSafeHwnd());
				m_wndSceneMemberList.SetFocus();
				m_wndSceneMemberList.EditLabel(pDispInfo->item.iItem);
			}
		}
	}
}

void CSceneMemberDlg::SetNotifyWindow(HWND hWnd, UINT nMsg)
{
	m_hNofityWnd = hWnd;
	m_nNotifyMsg = nMsg;
}

CString CSceneMemberDlg::GetAvailableName()
{
	CString strDef, strName;
	strDef.LoadString(IDS_SCENE_ROLE);
	strName = strDef;

	int nIndex = 1;
	int nItem = m_wndSceneMemberList.GetNextItem(-1, LVNI_ALL);
	while (nItem != -1) {
		if (strName.CompareNoCase(m_wndSceneMemberList.GetItemText(nItem, 0)) == 0) {
			strName.Format(_T("%s (%d)"), strDef, nIndex);
			nIndex ++;
			nItem = -1;
		}

		nItem = m_wndSceneMemberList.GetNextItem(nItem, LVNI_ALL);
	}

	return strName;
}
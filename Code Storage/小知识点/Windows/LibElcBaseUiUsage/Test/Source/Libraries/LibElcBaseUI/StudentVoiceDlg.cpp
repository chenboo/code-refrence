// StudentVoiceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "StudentVoiceDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SV_TABCTRL_HEIGHT		30
#define SV_HMARGIN				5
#define SV_VMARGIN				5

#define WM_TABCTRL_NOTIYFY		(WM_APP + 100)

// CStudentVoiceDlg dialog

IMPLEMENT_DYNAMIC(CStudentVoiceDlg, CElcSkinDialog)

CStudentVoiceDlg::CStudentVoiceDlg(CWnd* pParent /*=NULL*/)
	: CElcSkinDialog(CStudentVoiceDlg::IDD, pParent)
{
}

CStudentVoiceDlg::~CStudentVoiceDlg()
{
}

void CStudentVoiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CElcSkinDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStudentVoiceDlg, CElcSkinDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_TABCTRL_NOTIYFY, OnTabCtrlNotify)
END_MESSAGE_MAP()


// CStudentVoiceDlg message handlers

BOOL CStudentVoiceDlg::OnInitDialog()
{
	CElcSkinDialog::OnInitDialog();

	if (!m_wndTab.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, 0)) 
	{
		_TRACE(_T("Create tabctrl failed!\n"));
		return FALSE;
	}
	m_wndTab.SetNotifyWindow(GetSafeHwnd(), WM_TABCTRL_NOTIYFY);
	m_wndTab.ShowBorder(FALSE);

	CString strTitle;
	if (m_studParams.dwVoiceMask & STUD_VT_SPEAK) 
	{
		if (!m_dlgSpeak.Create(CSpeakDlg::IDD, this)) 
		{
			_TRACE(_T("Create Speak Dialog Failed. \n"));
			return FALSE;
		}

		m_dlgSpeak.Initialize(&m_studParams);
		m_dlgSpeak.ShowWindow(SW_HIDE);

		strTitle.LoadString(IDS_TITLE_SPEAK);
		int nItem = m_wndTab.AddItem(strTitle);
		m_wndTab.SetItemData(nItem, (DWORD_PTR)&m_dlgSpeak);
	}

	if (m_studParams.dwVoiceMask & STUD_VT_SCENE) 
	{
		if (!m_dlgScene.Create(CSceneDlg::IDD, this)) 
		{
			_TRACE(_T("Create Speak Dialog Failed. \n"));
			return FALSE;
		}

		m_dlgScene.Initialize(&m_studParams);
		m_dlgScene.ShowWindow(SW_HIDE);

		strTitle.LoadString(IDS_TITLE_SCENE);
		int nItem = m_wndTab.AddItem(strTitle);
		m_wndTab.SetItemData(nItem, (DWORD_PTR)&m_dlgScene);
	}

	if (m_studParams.dwVoiceMask & STUD_VT_TRANSLATE) 
	{
		if (!m_dlgTranslate.Create(CTranslateDlg::IDD, this)) 
		{
			_TRACE(_T("Create Speak Dialog Failed. \n"));
			return FALSE;
		}

		m_dlgTranslate.Initialize(&m_studParams);
		m_dlgTranslate.ShowWindow(SW_HIDE);

		strTitle.LoadString(IDS_TITLE_TRANSLATE);
		int nItem = m_wndTab.AddItem(strTitle);
		m_wndTab.SetItemData(nItem, (DWORD_PTR)&m_dlgTranslate);
	}

	((CWnd*)m_wndTab.GetItemData(0))->ShowWindow(SW_SHOW);

	Relayout();

	return TRUE;
}

void CStudentVoiceDlg::OnSize(UINT nType, int cx, int cy)
{
	CElcSkinDialog::OnSize(nType, cx, cy);

	if (!m_wndTab.GetSafeHwnd())
		return;

	Relayout();
}

void CStudentVoiceDlg::Relayout()
{
	CRect rcClient;
	GetClientRect(rcClient);
	CRect rcTab, rcDlg;
	rcTab.left = SV_HMARGIN;
	rcTab.right = rcClient.Width() - SV_HMARGIN;
	rcTab.top = SV_VMARGIN;
	rcTab.bottom = rcTab.top + SV_TABCTRL_HEIGHT;

	rcDlg.left = SV_HMARGIN;
	rcDlg.right = rcClient.Width() - SV_HMARGIN;
	rcDlg.top = rcTab.bottom;
	rcDlg.bottom = rcClient.Height() - SV_VMARGIN;

	m_wndTab.MoveWindow(rcTab);

	for (int i=0; i < m_wndTab.GetItemCount(); i ++) {
		((CWnd*)m_wndTab.GetItemData(i))->MoveWindow(rcDlg);
	}
}

void CStudentVoiceDlg::SetParams(PSTUDV_PARAMS pParams)
{
	memcpy(&m_studParams, pParams, sizeof(STUDV_PARAMS));
}

LRESULT CStudentVoiceDlg::OnTabCtrlNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ELCTAB_ITEMCHANGED) {
		((CWnd*)m_wndTab.GetItemData(HIWORD(lParam)))->ShowWindow(SW_SHOW);
		((CWnd*)m_wndTab.GetItemData(LOWORD(lParam)))->ShowWindow(SW_HIDE);
	}
	return 0;
}

void CStudentVoiceDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 550;
	lpMMI->ptMinTrackSize.y = 400;

	CElcSkinDialog::OnGetMinMaxInfo(lpMMI);
}

void CStudentVoiceDlg::OnDestroy()
{
	if (m_nFlags & (WF_MODALLOOP|WF_CONTINUEMODAL))
		EndModalLoop(-1);

	CElcSkinDialog::OnDestroy();
}

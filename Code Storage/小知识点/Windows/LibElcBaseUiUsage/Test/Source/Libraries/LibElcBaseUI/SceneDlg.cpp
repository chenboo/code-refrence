// SceneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "SceneDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CSceneDlg dialog

IMPLEMENT_DYNAMIC(CSceneDlg, CDialog)

CSceneDlg::CSceneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSceneDlg::IDD, pParent)
{
	m_brBkgnd.CreateSolidBrush(RGB(250,250,250));
}

CSceneDlg::~CSceneDlg()
{
}

void CSceneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSceneDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(ID_BTN_OK, &CSceneDlg::OnBnClickedBtnOk)

	ON_MESSAGE(WM_LIST_CHANGED, OnListChanged)
END_MESSAGE_MAP()


// CSceneDlg message handlers

BOOL CSceneDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndIcon.SubclassDlgItem(IDC_SCENE_ICON, this);
	m_wndIcon.SetImage(IDR_RT_SCENE_ICON);

	ModifyStyle(0, WS_CLIPCHILDREN);

	CRect rcClient, rcBtn;
	GetDlgItem(ID_BTN_OK)->GetWindowRect(&rcBtn);
	ScreenToClient(&rcBtn);
	GetClientRect(&rcClient);

	m_nBtnHMargin = rcClient.right - rcBtn.right;

	CRect rcSceneWnd;
	GetDlgItem(IDC_SCENE_WND_POS)->GetWindowRect(&rcSceneWnd);
	ScreenToClient(&rcSceneWnd);

	m_wndSence.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rcSceneWnd, this, 0);

	GetDlgItem(ID_BTN_OK)->EnableWindow(FALSE);

	return TRUE;
}

void CSceneDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndSence.GetSafeHwnd())) 
	{
		CRect rcSceneWnd;
		m_wndSence.GetWindowRect(&rcSceneWnd);
		ScreenToClient(&rcSceneWnd);

		rcSceneWnd.right = cx;
		rcSceneWnd.bottom = cy;
		m_wndSence.MoveWindow(rcSceneWnd);
	}

	CWnd* pBtn = GetDlgItem(ID_BTN_OK);
	if (pBtn && ::IsWindow(pBtn->GetSafeHwnd()))
	{
		CRect rcBtn;
		pBtn->GetWindowRect(&rcBtn);
		ScreenToClient(&rcBtn);
		int nWidth = rcBtn.Width();

		rcBtn.right = cx - m_nBtnHMargin;
		rcBtn.left = rcBtn.right - nWidth;
		
		pBtn->MoveWindow(rcBtn);
	}
}

HBRUSH CSceneDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return (HBRUSH)m_brBkgnd;
}

void CSceneDlg::OnBnClickedBtnOk()
{
	if (!m_pCallback)
		return;

	const CListCtrl* pList = &m_wndSence.GetSceneMemberDlg()->m_wndSceneMemberList;
	if (!pList)
		return;

	int nCount = pList->GetItemCount();
	if (nCount <= 0)
		return;

	PSTUDV_ITEM psvItem = new STUDV_ITEM[pList->GetItemCount()];

	for (int i=0; i<nCount; i++)
	{
		_tcscpy(psvItem[i].szFriendName, pList->GetItemText(i, 0));
		psvItem[i].pMemberInfo = (PMEMBERINFO)pList->GetItemData(i);
	}

	m_pCallback->OnStudentVoiceStart(STUD_VT_SCENE, nCount, psvItem);

	delete []psvItem;

	GetParent()->PostMessage(WM_COMMAND, IDOK);
}

BOOL CSceneDlg::Initialize(PSTUDV_PARAMS pParams)
{
	m_pCallback = pParams->pCallback;
	return m_wndSence.Initialize(pParams);
}

LRESULT CSceneDlg::OnListChanged(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(ID_BTN_OK)->EnableWindow(wParam > 0);

	return 0;
}
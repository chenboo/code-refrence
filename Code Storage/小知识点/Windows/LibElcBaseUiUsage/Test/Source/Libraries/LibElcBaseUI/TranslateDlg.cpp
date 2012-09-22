// TranslateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "TranslateDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CTranslateDlg dialog

IMPLEMENT_DYNAMIC(CTranslateDlg, CDialog)

CTranslateDlg::CTranslateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTranslateDlg::IDD, pParent)
{
	m_brBkgnd.CreateSolidBrush(RGB(250,250,250));
}

CTranslateDlg::~CTranslateDlg()
{
}

void CTranslateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTranslateDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_BTN_OK, &CTranslateDlg::OnBnClickedBtnOk)

	ON_MESSAGE(WM_LIST_CHANGED, OnListChanged)
END_MESSAGE_MAP()


// CTranslateDlg message handlers

BOOL CTranslateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndIcon.SubclassDlgItem(IDC_TRANSLATE_ICON, this);
	m_wndIcon.SetImage(IDR_RT_TRANSLATE_ICON);

	ModifyStyle(0, WS_CLIPCHILDREN);

	CRect rcClient, rcBtn;
	GetDlgItem(ID_BTN_OK)->GetWindowRect(&rcBtn);
	ScreenToClient(&rcBtn);
	GetClientRect(&rcClient);
	m_nBtnHMargin = rcClient.right - rcBtn.right;

	CRect rcTranslateWnd;
	GetDlgItem(IDC_TRANSLATE_WND_POS)->GetWindowRect(&rcTranslateWnd);
	ScreenToClient(&rcTranslateWnd);

	m_wndTranslate.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rcTranslateWnd, this, 0);

	GetDlgItem(ID_BTN_OK)->EnableWindow(FALSE);

	return TRUE;
}

HBRUSH CTranslateDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return (HBRUSH)m_brBkgnd;
}

void CTranslateDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndTranslate.GetSafeHwnd())) 
	{
		CRect rcTranslateWnd;
		m_wndTranslate.GetWindowRect(&rcTranslateWnd);
		ScreenToClient(&rcTranslateWnd);

		rcTranslateWnd.right = cx;
		rcTranslateWnd.bottom = cy;
		m_wndTranslate.MoveWindow(rcTranslateWnd);
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

void CTranslateDlg::OnBnClickedBtnOk()
{
	if (!m_pCallback)
		return;

	const CListCtrl* pList = &m_wndTranslate.GetTranslateMemberDlg()->m_wndTranslateList;
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

	m_pCallback->OnStudentVoiceStart(STUD_VT_TRANSLATE, nCount, psvItem);

	delete []psvItem;

	GetParent()->PostMessage(WM_COMMAND, IDOK);
}

BOOL CTranslateDlg::Initialize(PSTUDV_PARAMS pParams)
{
	m_pCallback = pParams->pCallback;
	return m_wndTranslate.Initialize(pParams);
}

LRESULT CTranslateDlg::OnListChanged(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(ID_BTN_OK)->EnableWindow(wParam > 0);

	return 0;
}
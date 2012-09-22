// SpeakDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "SpeakDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CSpeakDlg dialog

IMPLEMENT_DYNAMIC(CSpeakDlg, CDialog)

CSpeakDlg::CSpeakDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpeakDlg::IDD, pParent)
{
	m_brBkgnd.CreateSolidBrush(RGB(250,250,250));
	m_pCallback = NULL;
}

CSpeakDlg::~CSpeakDlg()
{
}

void CSpeakDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSpeakDlg, CDialog)
	ON_BN_CLICKED(ID_BTN_OK, &CSpeakDlg::OnBnClickedBtnOk)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()

	ON_MESSAGE(WM_LIST_CHANGED, OnListChanged)
END_MESSAGE_MAP()


// CSpeakDlg message handlers

BOOL CSpeakDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndIcon.SubclassDlgItem(IDC_SPEAK_ICON, this);
	m_wndIcon.SetImage(IDR_RT_SPEAK_ICON);

	ModifyStyle(0, WS_CLIPCHILDREN);

	CRect rcSpeakWnd;
	GetDlgItem(IDC_SPEAK_WND_POS)->GetWindowRect(&rcSpeakWnd);
	ScreenToClient(&rcSpeakWnd);

	CRect rcClient, rcBtn;
	GetDlgItem(ID_BTN_OK)->GetWindowRect(&rcBtn);
	ScreenToClient(&rcBtn);
	GetClientRect(&rcClient);

	m_nBtnHMargin = rcClient.right - rcBtn.right;

	m_wndSpeak.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rcSpeakWnd, this, 0);

	GetDlgItem(ID_BTN_OK)->EnableWindow(FALSE);

	return TRUE;
}

void CSpeakDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndSpeak.GetSafeHwnd())) {
		CRect rcSpeakWnd;
		m_wndSpeak.GetWindowRect(&rcSpeakWnd);
		ScreenToClient(&rcSpeakWnd);

		rcSpeakWnd.right = cx;
		rcSpeakWnd.bottom = cy;
		m_wndSpeak.MoveWindow(rcSpeakWnd);
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

HBRUSH CSpeakDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return (HBRUSH)m_brBkgnd;
}

void CSpeakDlg::OnBnClickedBtnOk()
{
	if (!m_pCallback)
		return;

	const CListCtrl* pList = &m_wndSpeak.GetSpeakMemberDlg()->m_wndSpeakersList;
	if (!pList)
		return;

	int nCount = pList->GetItemCount();
	if (nCount <= 0)
		return;

	PSTUDV_ITEM psvItem = new STUDV_ITEM[pList->GetItemCount()];

	for (int i=0; i<nCount; i++)
	{
		psvItem[i].pMemberInfo = (PMEMBERINFO)pList->GetItemData(i);
	}

	m_pCallback->OnStudentVoiceStart(STUD_VT_SPEAK, nCount, psvItem);

	delete []psvItem;

	GetParent()->PostMessage(WM_COMMAND, IDOK);
}

BOOL CSpeakDlg::Initialize(PSTUDV_PARAMS pParams)
{
	m_pCallback = pParams->pCallback;
	return m_wndSpeak.Initialize(pParams);
}

LRESULT CSpeakDlg::OnListChanged(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(ID_BTN_OK)->EnableWindow(wParam > 0);

	return 0;
}
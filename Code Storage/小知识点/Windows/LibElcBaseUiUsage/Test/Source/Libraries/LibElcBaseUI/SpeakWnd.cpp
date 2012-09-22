// SpeakWnd.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "SpeakWnd.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CSpeakWnd

IMPLEMENT_DYNAMIC(CSpeakWnd, CWnd)

CSpeakWnd::CSpeakWnd() : m_wndSpliter(2, 1, 150, 3)
{

}

CSpeakWnd::~CSpeakWnd()
{
}


BEGIN_MESSAGE_MAP(CSpeakWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_UPDATE_BUTTON_STATE, OnUpdateButtonState)
END_MESSAGE_MAP()



// CSpeakWnd message handlers



int CSpeakWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndSpliter.Create(this)) 
	{
		_TRACE(_T("Create Spliter window faield. \n"));
		return -1;
	}

	if (!m_dlgSpeakMember.Create(CSpeakMemberDlg::IDD, &m_wndSpliter)) 
	{
		_TRACE(_T("Create Speak Member Dialog faield. \n"));
		return -1;
	}

	m_dlgSpeakMember.SetNotifyWindow(GetSafeHwnd(), WM_UPDATE_BUTTON_STATE);

	m_dlgAvailableMember.EnableCheckItem(FALSE);
	m_dlgAvailableMember.ShowSwitchButton(TRUE);
	m_dlgAvailableMember.SetCallback(this);
	m_dlgAvailableMember.SetNotifyWindow(GetSafeHwnd(), WM_UPDATE_BUTTON_STATE);

	if (!m_dlgAvailableMember.Create(CAvailableMemberDlg::IDD, &m_wndSpliter)) 
	{
		_TRACE(_T("Create Available Member Dialog faield. \n"));
		return -1;
	}

	m_wndSpliter.SetPane(0, &m_dlgSpeakMember);
	m_wndSpliter.SetPane(1, &m_dlgAvailableMember);

	int nSize[] = {160, 400};
	m_wndSpliter.SetPaneSizes(nSize);

	return 0;
}

void CSpeakWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndSpliter.GetSafeHwnd()))
		m_wndSpliter.MoveWindow(0, 0, cx, cy);
}

BOOL CSpeakWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL CSpeakWnd::Initialize(PSTUDV_PARAMS pParams)
{
	return m_dlgAvailableMember.Initialize(pParams);
}

void CSpeakWnd::OnAddItem()
{
	CListCtrl* pMemberList = &m_dlgAvailableMember.m_wndMemberList;

	int nItem = pMemberList->GetNextItem(-1, LVNI_SELECTED);
	while (nItem != -1)
	{
		PMEMBERINFO pMemberInfo = (PMEMBERINFO)pMemberList->GetItemData(nItem);
		if (!HasItem(pMemberInfo)) 
		{
			if (AddItem(pMemberInfo))
				m_dlgAvailableMember.DelItem(pMemberInfo);
		}

		nItem = pMemberList->GetNextItem(-1, LVNI_SELECTED);
	}

	GetParent()->PostMessage(WM_LIST_CHANGED, (WPARAM)m_dlgSpeakMember.m_wndSpeakersList.GetItemCount());
}

void CSpeakWnd::OnDelItem()
{
	CListCtrl* pListCtrl = &m_dlgSpeakMember.m_wndSpeakersList;

	int nItem = pListCtrl->GetNextItem(-1, LVNI_SELECTED);
	while (nItem != -1)
	{
		m_dlgAvailableMember.AddItem((PMEMBERINFO)pListCtrl->GetItemData(nItem));
		pListCtrl->DeleteItem(nItem);
		nItem = pListCtrl->GetNextItem(-1, LVNI_SELECTED);
	}

	GetParent()->PostMessage(WM_LIST_CHANGED, (WPARAM)m_dlgSpeakMember.m_wndSpeakersList.GetItemCount());
}

BOOL CSpeakWnd::HasItem(PMEMBERINFO pMemberInfo)
{
	CListCtrl* pSpeakList = &m_dlgSpeakMember.m_wndSpeakersList;

	int nItem = pSpeakList->GetNextItem(-1, LVNI_ALL);
	while (nItem != -1)
	{
		if (pSpeakList->GetItemData(nItem) == (DWORD)pMemberInfo)
			return TRUE;

		nItem = pSpeakList->GetNextItem(nItem, LVNI_ALL);
	}

	return FALSE;
}

BOOL CSpeakWnd::AddItem(PMEMBERINFO pMemberInfo)
{
	CListCtrl* pSpeakList = &m_dlgSpeakMember.m_wndSpeakersList;

	DWORD dwItemData = (DWORD)pMemberInfo;

	int nImage = GetStateIconIndex(pMemberInfo);
	int nItem = pSpeakList->InsertItem(pSpeakList->GetItemCount(), 
		pMemberInfo->szDefaultName, nImage);

	pSpeakList->SetItemData(nItem, dwItemData);

	return TRUE;
}

const CSpeakMemberDlg* CSpeakWnd::GetSpeakMemberDlg()
{
	return &m_dlgSpeakMember;
}

CAvailableMemberDlg* CSpeakWnd::GetAvailableMemberDlg()
{
	return &m_dlgAvailableMember;
}

LRESULT CSpeakWnd::OnUpdateButtonState(WPARAM wParam, LPARAM lParam)
{
	BOOL bSet = m_dlgAvailableMember.m_wndMemberList.GetSelectedCount() > 0;
	BOOL bClear = m_dlgSpeakMember.m_wndSpeakersList.GetSelectedCount() > 0;

	m_dlgAvailableMember.EnableButtons(bSet, bClear);

	return 0;
}
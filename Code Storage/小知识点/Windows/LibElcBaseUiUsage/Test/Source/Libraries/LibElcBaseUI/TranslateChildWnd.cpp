// SpeakWnd.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "TranslateChildWnd.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



// CTranslateChildWnd

IMPLEMENT_DYNAMIC(CTranslateChildWnd, CWnd)

CTranslateChildWnd::CTranslateChildWnd() : m_wndSpliter(2, 1, 150, 3)
{

}

CTranslateChildWnd::~CTranslateChildWnd()
{
}


BEGIN_MESSAGE_MAP(CTranslateChildWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()

	ON_MESSAGE(WM_UPDATE_BUTTON_STATE, OnUpdateButtonState)
END_MESSAGE_MAP()



// CTranslateChildWnd message handlers



int CTranslateChildWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndSpliter.Create(this)) 
	{
		_TRACE(_T("Create Spliter window faield. \n"));
		return -1;
	}

	if (!m_dlgTranslateMember.Create(CTranslateMemberDlg::IDD, &m_wndSpliter)) 
	{
		_TRACE(_T("Create Translate Member Dialog faield. \n"));
		return -1;
	}

	m_dlgTranslateMember.SetNotifyWindow(GetSafeHwnd(), WM_UPDATE_BUTTON_STATE);

	m_dlgAvailableMember.EnableCheckItem(FALSE);
	m_dlgAvailableMember.ShowSwitchButton(TRUE);
	m_dlgAvailableMember.EnableTeacher(TRUE);

	if (!m_dlgAvailableMember.Create(CAvailableMemberDlg::IDD, &m_wndSpliter)) 
	{
		_TRACE(_T("Create Available Member Dialog faield. \n"));
		return -1;
	}

	m_dlgAvailableMember.EnableMultiSel(FALSE);
	m_dlgAvailableMember.SetCallback(this);
	m_dlgAvailableMember.SetNotifyWindow(GetSafeHwnd(), WM_UPDATE_BUTTON_STATE);

	m_wndSpliter.SetPane(0, &m_dlgTranslateMember);
	m_wndSpliter.SetPane(1, &m_dlgAvailableMember);

	int nSize[] = {160, 400};
	m_wndSpliter.SetPaneSizes(nSize);

	return 0;
}

void CTranslateChildWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndSpliter.GetSafeHwnd()))
		m_wndSpliter.MoveWindow(0, 0, cx, cy);
}

BOOL CTranslateChildWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL CTranslateChildWnd::Initialize(PSTUDV_PARAMS pParams)
{
	return m_dlgAvailableMember.Initialize(pParams);
}

void CTranslateChildWnd::OnAddItem()
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
			else
				break;
		}

		nItem = pMemberList->GetNextItem(-1, LVNI_SELECTED);
	}

	GetParent()->PostMessage(WM_LIST_CHANGED, (WPARAM)m_dlgTranslateMember.m_wndTranslateList.GetItemCount());
}

void CTranslateChildWnd::OnDelItem()
{
	CListCtrl* pListCtrl = &m_dlgTranslateMember.m_wndTranslateList;

	int nItem = pListCtrl->GetNextItem(-1, LVNI_SELECTED);
	while (nItem != -1)
	{
		m_dlgAvailableMember.AddItem((PMEMBERINFO)pListCtrl->GetItemData(nItem));
		pListCtrl->DeleteItem(nItem);
		nItem = pListCtrl->GetNextItem(-1, LVNI_SELECTED);
	}

	GetParent()->PostMessage(WM_LIST_CHANGED, (WPARAM)m_dlgTranslateMember.m_wndTranslateList.GetItemCount());
}

BOOL CTranslateChildWnd::HasItem(PMEMBERINFO pMemberInfo)
{
	CListCtrl* pSpeakList = &m_dlgTranslateMember.m_wndTranslateList;

	int nItem = pSpeakList->GetNextItem(-1, LVNI_ALL);
	while (nItem != -1)
	{
		if (pSpeakList->GetItemData(nItem) == (DWORD)pMemberInfo)
			return TRUE;

		nItem = pSpeakList->GetNextItem(nItem, LVNI_ALL);
	}

	return FALSE;
}

BOOL CTranslateChildWnd::AddItem(PMEMBERINFO pMemberInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CListCtrl* pList = &m_dlgTranslateMember.m_wndTranslateList;

	//DWORD dwItemData = (DWORD)pMemberInfo;

	if (pList->GetItemCount() >= SV_MAX_TRANSLATE_ROLE)
	{
		CString strText;
		strText.Format(IDS_PROMPT_MAX_TRANSLATE_COUNT, SV_MAX_TRANSLATE_ROLE);
		ElcMessageBox(strText, MB_OK|MB_ICONWARNING, GetSafeHwnd());
		return FALSE;
	}

	CString strText;
	strText.LoadString(IDS_LANGUAGE);
	int nItem = pList->InsertItem(pList->GetItemCount(), strText);
	pList->SetItemText(nItem, 1, pMemberInfo->szDefaultName);
	pList->SetItemData(nItem, (DWORD)pMemberInfo);

	pList->SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
	pList->SetFocus();
	pList->EditLabel(nItem);

	return TRUE;
}

const CTranslateMemberDlg* CTranslateChildWnd::GetTranslateMemberDlg()
{
	return &m_dlgTranslateMember;
}

LRESULT CTranslateChildWnd::OnUpdateButtonState(WPARAM wParam, LPARAM lParam)
{
	BOOL bSet = m_dlgAvailableMember.m_wndMemberList.GetSelectedCount() > 0;
	BOOL bClear = m_dlgTranslateMember.m_wndTranslateList.GetSelectedCount() > 0;

	m_dlgAvailableMember.EnableButtons(bSet, bClear);

	return 0;
}
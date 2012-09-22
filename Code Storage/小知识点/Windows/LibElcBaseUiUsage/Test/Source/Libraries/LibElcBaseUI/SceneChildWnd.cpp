// SpeakWnd.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "SceneChildWnd.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CSceneChildWnd

IMPLEMENT_DYNAMIC(CSceneChildWnd, CWnd)

CSceneChildWnd::CSceneChildWnd() : m_wndSpliter(2, 1, 150, 3)
{

}

CSceneChildWnd::~CSceneChildWnd()
{
}


BEGIN_MESSAGE_MAP(CSceneChildWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_UPDATE_BUTTON_STATE, OnUpdateButtonState)
END_MESSAGE_MAP()



// CSceneChildWnd message handlers



int CSceneChildWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndSpliter.Create(this)) 
	{
		_TRACE(_T("Create Spliter window faield. \n"));
		return -1;
	}

	if (!m_dlgSceneMember.Create(CSceneMemberDlg::IDD, &m_wndSpliter)) 
	{
		_TRACE(_T("Create Scene Member Dialog faield. \n"));
		return -1;
	}

	m_dlgSceneMember.SetNotifyWindow(GetSafeHwnd(), WM_UPDATE_BUTTON_STATE);

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

	m_wndSpliter.SetPane(0, &m_dlgSceneMember);
	m_wndSpliter.SetPane(1, &m_dlgAvailableMember);

	int nSize[] = {160, 400};
	m_wndSpliter.SetPaneSizes(nSize);

	return 0;
}

void CSceneChildWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndSpliter.GetSafeHwnd()))
		m_wndSpliter.MoveWindow(0, 0, cx, cy);
}

BOOL CSceneChildWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL CSceneChildWnd::Initialize(PSTUDV_PARAMS pParams)
{
	return m_dlgAvailableMember.Initialize(pParams);
}

void CSceneChildWnd::OnAddItem()
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

	GetParent()->PostMessage(WM_LIST_CHANGED, (WPARAM)m_dlgSceneMember.m_wndSceneMemberList.GetItemCount());
}

void CSceneChildWnd::OnDelItem()
{
	CListCtrl* pListCtrl = &m_dlgSceneMember.m_wndSceneMemberList;

	int nItem = pListCtrl->GetNextItem(-1, LVNI_SELECTED);
	while (nItem != -1)
	{
		m_dlgAvailableMember.AddItem((PMEMBERINFO)pListCtrl->GetItemData(nItem));
		pListCtrl->DeleteItem(nItem);
		nItem = pListCtrl->GetNextItem(-1, LVNI_SELECTED);
	}

	GetParent()->PostMessage(WM_LIST_CHANGED, (WPARAM)m_dlgSceneMember.m_wndSceneMemberList.GetItemCount());
}

BOOL CSceneChildWnd::HasItem(PMEMBERINFO pMemberInfo)
{
	CListCtrl* pSpeakList = &m_dlgSceneMember.m_wndSceneMemberList;

	int nItem = pSpeakList->GetNextItem(-1, LVNI_ALL);
	while (nItem != -1)
	{
		if (pSpeakList->GetItemData(nItem) == (DWORD)pMemberInfo)
			return TRUE;

		nItem = pSpeakList->GetNextItem(nItem, LVNI_ALL);
	}

	return FALSE;
}

BOOL CSceneChildWnd::AddItem(PMEMBERINFO pMemberInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CListCtrl* pList = &m_dlgSceneMember.m_wndSceneMemberList;

	//DWORD dwItemData = (DWORD)pMemberInfo;

	if (pList->GetItemCount() >= SV_MAX_SCENE_ROLE)
	{
		CString strText;
		strText.Format(IDS_PROMPT_MAX_SCENE_COUNT, SV_MAX_SCENE_ROLE);
		ElcMessageBox(strText, MB_OK|MB_ICONWARNING, GetSafeHwnd());
		return FALSE;
	}

	int nItem = pList->InsertItem(pList->GetItemCount(), m_dlgSceneMember.GetAvailableName());
	pList->SetItemText(nItem, 1, pMemberInfo->szDefaultName);
	pList->SetItemData(nItem, (DWORD)pMemberInfo);

	pList->SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
	pList->SetFocus();
	pList->EditLabel(nItem);

	return TRUE;
}

const CSceneMemberDlg* CSceneChildWnd::GetSceneMemberDlg()
{
	return &m_dlgSceneMember;
}

LRESULT CSceneChildWnd::OnUpdateButtonState(WPARAM wParam, LPARAM lParam)
{
	BOOL bSet = m_dlgAvailableMember.m_wndMemberList.GetSelectedCount() > 0;
	BOOL bClear = m_dlgSceneMember.m_wndSceneMemberList.GetSelectedCount() > 0;

	m_dlgAvailableMember.EnableButtons(bSet, bClear);

	return 0;
}


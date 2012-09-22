// StudentDemoListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "libElcbaseUI.h"
#include "StudentDemoListDlg.h"
#include "i18n.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CStudentDemoListDlg dialog

IMPLEMENT_DYNAMIC(CStudentDemoListDlg, CElcSkinDialog)

CStudentDemoListDlg::CStudentDemoListDlg(CWnd* pParent /*=NULL*/)
	: CElcSkinDialog(CStudentDemoListDlg::IDD, pParent)
{
	m_pTeacherFrame = NULL;
	m_pCallback = NULL;
	m_pDemoMemberInfo = NULL;

	m_pParams = NULL;

	m_pWndParent = pParent;
}

CStudentDemoListDlg::~CStudentDemoListDlg()
{
}

void CStudentDemoListDlg::DoDataExchange(CDataExchange* pDX)
{
	CElcSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AVAILABLE_LIST, m_wndAvailableList);
	DDX_Control(pDX, IDC_LIST_TYPE, m_wndListType);
}


BEGIN_MESSAGE_MAP(CStudentDemoListDlg, CElcSkinDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(ID_BTN_OK, &CStudentDemoListDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(ID_BTN_CANCEL, &CStudentDemoListDlg::OnBnClickedBtnCancel)
	ON_BN_CLICKED(IDC_BTN_HELP, &CStudentDemoListDlg::OnBnClickedBtnHelp)
	ON_CBN_SELCHANGE(IDC_LIST_TYPE, &CStudentDemoListDlg::OnCbnSelchangeListType)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AVAILABLE_LIST, &CStudentDemoListDlg::OnLvnItemchangedAvailableList)
	ON_BN_CLICKED(IDC_CHK_SELECT_ALL, &CStudentDemoListDlg::OnBnClickedChkSelectAll)
END_MESSAGE_MAP()


// CStudentDemoListDlg message handlers

void CStudentDemoListDlg::SetParams(PSTUDD_PARAMS pParams)
{
	m_pTeacherFrame = pParams->pTeacherFrame;
	m_pDemoMemberInfo = pParams->pDemoMemberInfo;

	m_pParams = pParams;
}

BOOL CStudentDemoListDlg::OnInitDialog()
{
	CElcSkinDialog::OnInitDialog();

	AdjustImagePanePos(IDC_IMAGE_PANE);
	BindButton(IDC_BTN_HELP);
	BindButton(ID_BTN_OK);
	BindButton(ID_BTN_CANCEL);
	BindPictureCtrl(IDC_STUDENT_DEMO_ICON, IDR_RT_STUDENT_DEMO);

	if (!GetParent()) {
		SetParent(m_pWndParent);
		SetOwner(m_pWndParent);
	}

	CString strText;
	strText.LoadString(IDS_DEMO_TO_ALL);
	m_wndListType.InsertString(0, strText);
	strText.LoadString(IDS_DEMO_TO_STUDENT);
	m_wndListType.InsertString(1, strText);
	strText.LoadString(IDS_DEMO_TO_GROUP);
	m_wndListType.InsertString(2, strText);
	m_wndListType.SetCurSel(0);

	m_wndAvailableList.EnableCheck(IDB_STATE_IMAGE_LIST16);
	m_wndAvailableList.EnableSelect(TRUE);

	m_nSelectType = DEMO_TO_ALL;
	GetDlgItem(IDC_CHK_SELECT_ALL)->EnableWindow(m_nSelectType != DEMO_TO_ALL);
	InitAvailableList(m_nSelectType);
	UpdateButtonState();

	if (m_pTeacherFrame) {
		DWORD dwCallbackMask;
		dwCallbackMask =  ITeacherFrame::ELC_MEMBER_CONNECTED
						| ITeacherFrame::ELC_MEMBER_DISCONNECTED
						| ITeacherFrame::ELC_MEMBER_NAMECHANGED
						| ITeacherFrame::ELC_EXIT;
		m_pTeacherFrame->RegisterCallback(dwCallbackMask, this, NULL);
	}

	if (m_pDemoMemberInfo) {
		GetWindowText(strText);
		strText += (CString)_T(" [") + m_pDemoMemberInfo->szDefaultName + (CString)_T("] ");
		SetWindowText(strText);
	}

	return TRUE;
}

void CStudentDemoListDlg::OnBnClickedBtnOk()
{
	int nIndex = m_wndListType.GetCurSel();
	SetStudentSelected(nIndex);

	CElcSkinDialog::OnOK();
}

void CStudentDemoListDlg::OnBnClickedBtnCancel()
{
	CElcSkinDialog::OnCancel();
}

void CStudentDemoListDlg::OnBnClickedBtnHelp()
{
	//CTeacherDlg* pParent = (CTeacherDlg *)GetParent();
	//pParent->PostMessage((WM_USER + 219), 48, 0);
}

void CStudentDemoListDlg::OnCbnSelchangeListType()
{
	m_nSelectType = m_wndListType.GetCurSel();	
	InitAvailableList(m_nSelectType);
	UpdateButtonState();

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHK_SELECT_ALL);
	pBtn->EnableWindow(m_nSelectType != DEMO_TO_ALL);
	pBtn->SetCheck(BST_UNCHECKED);
}

void CStudentDemoListDlg::UpdateButtonState()
{
	BOOL bRet = (m_nSelectType == DEMO_TO_ALL);	

	if (!bRet) 
	{
		int nCount = m_wndAvailableList.GetItemCount();
		for (int i=0; i < nCount; i ++)
		{
			if (m_wndAvailableList.GetCheck(i)) {
				bRet = TRUE;
				break;
			}
		}
	}
	
	GetDlgItem(ID_BTN_OK)->EnableWindow(bRet);
}

void CStudentDemoListDlg::SetStudentSelected(int nType)
{
	if (!m_pTeacherFrame)
		return;

	if (!m_pDemoMemberInfo)
		return;

	PMEMBERINFO ppAllMemberInfo[ELC_MAX_MEMBER_COUNT] = {0};
	int nAllCount = m_pTeacherFrame->GetAllMemberInfoPtrs(ppAllMemberInfo);

	int nSelectCount = 0;
	PMEMBERINFO ppSelectMemberInfo[ELC_MAX_MEMBER_COUNT] = {0};

	if (DEMO_TO_ALL == nType) 
	{
		for (int i=0; i<nAllCount; i++) {
			if (ppAllMemberInfo[i] != m_pDemoMemberInfo) {
				ppSelectMemberInfo[nSelectCount] = ppAllMemberInfo[i];
				nSelectCount ++;
			}
		}
	}
	else if (DEMO_TO_GROUP == nType) 
	{
		typedef vector<CString> CGroupName;
		CGroupName arrGroup;

		int nCount = m_wndAvailableList.GetItemCount();
		for (int i=0; i < nCount; i++) 
		{
			if (m_wndAvailableList.GetCheck(i))
				arrGroup.push_back(m_wndAvailableList.GetItemText(i, 0));
		}

		if (arrGroup.empty())
			return;
		
		CGroupName::iterator it = arrGroup.begin();
		for (; it != arrGroup.end(); it ++) 
		{
			for (int i=0; i<nAllCount; i++) {
				PMEMBERINFO pTemp = ppAllMemberInfo[i];
				if (pTemp == m_pDemoMemberInfo)
					continue;

				if (_tcscmp(pTemp->szGroupName, (*it)) == 0) {
					ppSelectMemberInfo[nSelectCount] = pTemp;
					nSelectCount ++;
				}
			}
		}
	}
	else if (DEMO_TO_PART == nType) 
	{
		int nCount = m_wndAvailableList.GetItemCount();
		for (int i=0; i < nCount; i++) 
		{
			if (!m_wndAvailableList.GetCheck(i))
				continue;

			PMEMBERINFO pTemp = (PMEMBERINFO)m_wndAvailableList.GetItemData(i);
			ppSelectMemberInfo[nSelectCount] = pTemp;
			nSelectCount ++;
		}
	}

	m_pParams->nRecvCount = nSelectCount;
	memcpy(m_pParams->ppRecvMemberInfo, ppSelectMemberInfo, sizeof(PMEMBERINFO) * nSelectCount);
}

void CStudentDemoListDlg::InitAvailableList(int nType)
{
	if (!m_pTeacherFrame)
		return;

	if (!m_pDemoMemberInfo)
		return;

	m_wndAvailableList.DeleteAllItems();
	m_wndAvailableList.EnableWindow(0 != nType);

	PMEMBERINFO ppMemberInfo[ELC_MAX_MEMBER_COUNT] = {0};
	int nCount = m_pTeacherFrame->GetAllMemberInfoPtrs(ppMemberInfo);

	if (DEMO_TO_ALL == nType) 
	{		
	}
	else if (DEMO_TO_GROUP == nType) 
	{
		typedef map<CString, CString> CGroupList;
		CGroupList listGroup;

		PMEMBERINFO pTemp = NULL;
		for (int i = 0; i < nCount; i++) 
		{
			pTemp = ppMemberInfo[i];
			if (m_pDemoMemberInfo == pTemp)
				continue;

			if (pTemp && (_tcslen(pTemp->szGroupName) > 0))
			{
				CGroupList::iterator it = listGroup.find(pTemp->szGroupName);
				if (it == listGroup.end()) 
				{
					listGroup.insert(CGroupList::value_type(pTemp->szGroupName, pTemp->szGroupName));
					int nItem = m_wndAvailableList.InsertItem(m_wndAvailableList.GetItemCount(), pTemp->szGroupName);
					m_wndAvailableList.SetCheck(nItem, FALSE);
				}
			}
		}
	}
	else if (DEMO_TO_PART == nType) 
	{
		PMEMBERINFO pTemp = NULL;
		for (int i = 0; i < nCount; i++) 
		{
			pTemp = ppMemberInfo[i];
			if (m_pDemoMemberInfo == pTemp)
				continue;

			if (pTemp && pTemp->bLoggedIn) 
			{
				int nItem = m_wndAvailableList.InsertItem(m_wndAvailableList.GetItemCount(), pTemp->szDefaultName);
				m_wndAvailableList.SetItemData(nItem, (DWORD_PTR)pTemp);
				m_wndAvailableList.SetCheck(nItem, FALSE);
			}
		}
	}
	else 
		ASSERT(0);
}

void CStudentDemoListDlg::OnLvnItemchangedAvailableList(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	UpdateButtonState();

	*pResult = 0;
}


void CStudentDemoListDlg::OnMemberConnected(PMEMBERINFO pMemberInfo, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (DEMO_TO_PART != m_nSelectType)
		return;

	if (!pMemberInfo)
		return;

	int nItem = m_wndAvailableList.InsertItem(m_wndAvailableList.GetItemCount(), pMemberInfo->szDefaultName);
	m_wndAvailableList.SetItemData(nItem, (DWORD_PTR)pMemberInfo);
	m_wndAvailableList.SetCheck(nItem, FALSE);
}

void CStudentDemoListDlg::OnMemberDisconnected(PMEMBERINFO pMemberInfo, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (DEMO_TO_PART != m_nSelectType)
		return;

	if (!pMemberInfo)
		return;

	int nItem = FindItem(pMemberInfo);
	if (nItem < 0)
		return;

	m_wndAvailableList.DeleteItem(nItem);
}

void CStudentDemoListDlg::OnMemberNameChanged(PMEMBERINFO pMemberInfo, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (DEMO_TO_PART != m_nSelectType)
		return;

	if (!pMemberInfo)
		return;

	int nItem = FindItem(pMemberInfo);
	if (nItem < 0)
		return;

	m_wndAvailableList.SetItemText(nItem, 0, pMemberInfo->szDefaultName);
}

void CStudentDemoListDlg::OnFrameExit(DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CElcSkinDialog::OnCancel();
}

void CStudentDemoListDlg::OnDestroy()
{
	if (m_pTeacherFrame) {
		m_pTeacherFrame->UnRegisterCallback(this);
	}

	if (m_nFlags & (WF_MODALLOOP|WF_CONTINUEMODAL))
		EndModalLoop(-1);

	__super::OnDestroy();
}

void CStudentDemoListDlg::OnBnClickedChkSelectAll()
{
	BOOL bRet = ((CButton*)GetDlgItem(IDC_CHK_SELECT_ALL))->GetCheck();
	int nCount = m_wndAvailableList.GetItemCount();
	for (int i=0; i < nCount; i++)
	{
		m_wndAvailableList.SetCheck(i, bRet);
	}
	
	UpdateButtonState();
}

int CStudentDemoListDlg::FindItem(PMEMBERINFO pMemberInfo)
{
	PMEMBERINFO pTemp;

	int nCount = m_wndAvailableList.GetItemCount();
	for (int i=0; i < nCount; i ++)
	{
		pTemp = (PMEMBERINFO)m_wndAvailableList.GetItemData(i);
		if (pTemp && (pTemp == pMemberInfo))
			return i;
	}

	return -1;
}
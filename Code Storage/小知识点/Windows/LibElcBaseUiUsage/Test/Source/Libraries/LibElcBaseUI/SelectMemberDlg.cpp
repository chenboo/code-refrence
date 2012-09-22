// SelectMemberDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "SelectMemberDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CSelectMemberDlg dialog

IMPLEMENT_DYNAMIC(CSelectMemberDlg, CElcSkinDialog)

CSelectMemberDlg::CSelectMemberDlg(CWnd* pParent /*=NULL*/)
	: CElcSkinDialog(CSelectMemberDlg::IDD, pParent)
{
	m_pTeacherFrame = NULL;
	m_dwFuncMask = 0;
	m_pCallback = NULL;
	m_pExcludeMember = NULL;

	m_nCount = 0;
}

CSelectMemberDlg::~CSelectMemberDlg()
{
}

void CSelectMemberDlg::DoDataExchange(CDataExchange* pDX)
{
	CElcSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBER_LIST, m_wndMemberList);
	DDX_Control(pDX, IDC_CHECK_SELECT_ALL, m_btnSelectAll);
}


BEGIN_MESSAGE_MAP(CSelectMemberDlg, CElcSkinDialog)
	ON_BN_CLICKED(IDC_BTN_HELP, &CSelectMemberDlg::OnBnClickedBtnHelp)
	ON_BN_CLICKED(ID_BTN_OK, &CSelectMemberDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(ID_BTN_CANCEL, &CSelectMemberDlg::OnBnClickedBtnCancel)
	ON_BN_CLICKED(IDC_CHECK_SELECT_ALL, &CSelectMemberDlg::OnBnClickedCheckSelectAll)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CSelectMemberDlg message handlers

void CSelectMemberDlg::SetParams(PSTUDC_PARAMS pParams, int nCount, PMEMBERINFO* ppMemberInfo)
{
	m_pTeacherFrame = pParams->pTeacherFrame;
	m_dwFuncMask = pParams->dwFuncMask;
	m_pCallback = pParams->pCallback;
	m_pExcludeMember = pParams->pExcludeMember;

	m_nCount = nCount;
	for (int i=0; i<nCount; i++) {
		m_ppMemberInfo[i] = ppMemberInfo[i];
	}
}

BOOL CSelectMemberDlg::OnInitDialog()
{
	CElcSkinDialog::OnInitDialog();

	AdjustImagePanePos(IDC_IMAGE_PANE);
	BindButton(ID_BTN_OK);
	BindButton(ID_BTN_CANCEL);
	BindButton(IDC_BTN_HELP);

	m_wndIcon.SubclassDlgItem(IDC_SELECT_STUDENT_ICON, this);
	m_wndIcon.SetImage(IDR_RT_SELECT_STUDENT);

	CBitmap bmpBase;
	m_imgList.Create(16, 16, ILC_MASK | ILC_COLOR24, 0, 1);
	bmpBase.Attach(::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_GROUP_LIST_STATE), 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION));
	m_imgList.Add(&bmpBase, RGB(255,0,255));

	m_wndMemberList.SetImageList(&m_imgList, LVSIL_SMALL);
	m_wndMemberList.SetBkColor(RGB(250,250,250));

	m_wndMemberList.SetExtendedStyle(m_wndMemberList.GetExtendedStyle() | LVS_EX_CHECKBOXES);

	if (m_pTeacherFrame)
	{
		DWORD dwMask;
		dwMask = ITeacherFrame::ELC_MEMBER_CONNECTED | 
				 ITeacherFrame::ELC_MEMBER_DISCONNECTED | 
				 ITeacherFrame::ELC_MEMBER_ADDED | 
				 ITeacherFrame::ELC_MEMBER_REMOVED |
				 ITeacherFrame::ELC_MEMBER_NAMECHANGED |
				 ITeacherFrame::ELC_MEMBER_HANDUP |
				 ITeacherFrame::ELC_MEMBER_HANDDOWN |
				 ITeacherFrame::ELC_FUNCTION_CHANGED;
		m_pTeacherFrame->RegisterCallback(dwMask, this, NULL);
	}

	InitList();

	return TRUE;
}

void CSelectMemberDlg::OnBnClickedBtnHelp()
{
}

void CSelectMemberDlg::OnBnClickedBtnOk()
{
	int nAddCount = 0;
	int nDelCount = 0;
	PMEMBERINFO ppMemberAdd[ELC_MAX_MEMBER_COUNT] = {0};
	PMEMBERINFO ppMemberDel[ELC_MAX_MEMBER_COUNT] = {0};

	for (int i=0; i<m_wndMemberList.GetItemCount(); i++) {
		PMEMBERINFO pMemberInfo = (PMEMBERINFO)m_wndMemberList.GetItemData(i);

		if (m_wndMemberList.GetCheck(i)) {
			if (!HasMember(pMemberInfo))
				ppMemberAdd[nAddCount ++] = pMemberInfo;
		} else
		{
			if (HasMember(pMemberInfo))
				ppMemberDel[nDelCount ++] = pMemberInfo;
		}
	}

	if (m_pCallback) {
		m_pCallback->OnStudentCheckChanged(m_dwFuncMask, TRUE, nAddCount, ppMemberAdd);
		m_pCallback->OnStudentCheckChanged(m_dwFuncMask, FALSE, nDelCount, ppMemberDel);
	}

	CElcSkinDialog::OnOK();
}

void CSelectMemberDlg::OnBnClickedBtnCancel()
{
	CElcSkinDialog::OnCancel();
}

void CSelectMemberDlg::OnBnClickedCheckSelectAll()
{
	BOOL bCheck = m_btnSelectAll.GetCheck();
	for (int i=0; i<m_wndMemberList.GetItemCount(); i++) {
		m_wndMemberList.SetCheck(i, bCheck);
	}
}

void CSelectMemberDlg::OnDestroy()
{
	if (m_pTeacherFrame) {
		m_pTeacherFrame->UnRegisterCallback(this);
	}

	if (m_nFlags & (WF_MODALLOOP|WF_CONTINUEMODAL))
		EndModalLoop(-1);

	__super::OnDestroy();
}

void CSelectMemberDlg::OnMemberConnected(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nItem = FindItemByMac(abMac);
	if (nItem < 0)
		return;

	m_wndMemberList.SetItem(nItem, 0, LVIF_IMAGE | LVIF_TEXT, 
		lpszName, STATEICON_LOGIN, 0, 0, 0);
}

void CSelectMemberDlg::OnMemberDisconnected(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nItem = FindItemByMac(abMac);
	if (nItem < 0)
		return;

	m_wndMemberList.SetItem(nItem, 0, LVIF_IMAGE | LVIF_TEXT, 
		lpszName, STATEICON_LOGOUT, 0, 0, 0);
}

void CSelectMemberDlg::OnMemberAdded(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

void CSelectMemberDlg::OnMemberRemoved(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nItem = FindItemByMac(abMac);
	if (nItem < 0)
		return;

	m_wndMemberList.DeleteItem(nItem);
}

void CSelectMemberDlg::OnMemberNameChanged(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nItem = FindItemByMac(abMac);
	if (nItem < 0)
		return;

	m_wndMemberList.SetItem(nItem, 0, LVIF_TEXT, 
		lpszName, 0, 0, 0, 0);
}

void CSelectMemberDlg::OnMemberHandUp(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nItem = FindItemByMac(abMac);
	if (nItem < 0)
		return;

	m_wndMemberList.SetItem(nItem, 0, LVIF_IMAGE | LVIF_TEXT, 
		lpszName, STATEICON_HANDUP, 0, 0, 0);
}

void CSelectMemberDlg::OnMemberHandDown(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nItem = FindItemByMac(abMac);
	if (nItem < 0)
		return;

	PMEMBERINFO pMemberInfo = (PMEMBERINFO)m_wndMemberList.GetItemData(nItem);
	if (!pMemberInfo)
		return;

	int nImage = GetStateIconIndex(pMemberInfo);

	m_wndMemberList.SetItem(nItem, 0, LVIF_IMAGE | LVIF_TEXT, 
		lpszName, nImage, 0, 0, 0);
}

void CSelectMemberDlg::OnFunctionChanged(int nFunction, UINT64 dwCommand, BOOL bStart)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (bStart || m_dwFuncMask != dwCommand)
		return;

	PostMessage(WM_COMMAND, ID_BTN_CANCEL);
}

void CSelectMemberDlg::InitList()
{
	PMEMBERINFO ppMemberInfo[ELC_MAX_MEMBER_COUNT] = {0};
	int nCount = m_pTeacherFrame->GetAllMemberInfoPtrs(ppMemberInfo);

	for (int i=0; i<nCount; i++) 
	{
		if (!ppMemberInfo[i]) 
			continue;

		if (m_pExcludeMember && m_pExcludeMember == ppMemberInfo[i])
			continue;
		
		int nImage = GetStateIconIndex(ppMemberInfo[i]);
		int nItem = m_wndMemberList.InsertItem(m_wndMemberList.GetItemCount(), 
			ppMemberInfo[i]->szDefaultName, nImage);
		m_wndMemberList.SetItemData(nItem, (DWORD)ppMemberInfo[i]);

		if (HasMember(ppMemberInfo[i])) {
			m_wndMemberList.SetCheck(nItem, TRUE);
		}
	}
}

int CSelectMemberDlg::FindItemByMac(BYTE* abMac)
{
	PMEMBERINFO pMemberInfo = NULL;
	int nCount = m_wndMemberList.GetItemCount();
	for (int i=0; i<nCount; i++) 
	{
		pMemberInfo = (PMEMBERINFO)m_wndMemberList.GetItemData(i);
		if (pMemberInfo && (memcmp(pMemberInfo->id.abMac, abMac, 6) == 0))
			return i;
	}

	return -1;
}

BOOL CSelectMemberDlg::HasMember(PMEMBERINFO pMemberInfo)
{
	if (m_nCount == 0) {
		return (pMemberInfo->dwCommand & m_dwFuncMask) ? TRUE : FALSE;
	}

	for (int i=0; i<m_nCount; i++) {
		if (pMemberInfo == m_ppMemberInfo[i])
			return TRUE;
	}

	return FALSE;
}
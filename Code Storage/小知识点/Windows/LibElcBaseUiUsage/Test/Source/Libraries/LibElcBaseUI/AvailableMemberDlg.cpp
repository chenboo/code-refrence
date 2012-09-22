// SelectMemberDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LibElcBaseUI.h"
#include "AvailableMemberDlg.h"
#include "StudentVoiceDlg.h"

#pragma warning ( disable : 4311 4312 )

#define TOP_MARGIN		32
#define LEFT_MARGIN		32

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// CAvailableMemberDlg dialog

IMPLEMENT_DYNAMIC(CAvailableMemberDlg, CDialog)

CAvailableMemberDlg::CAvailableMemberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAvailableMemberDlg::IDD, pParent)
{
	m_bCheck = TRUE;
	m_bShowBtn = FALSE;
	m_bHasTeacher = FALSE;

	m_pCallback = NULL;

	m_pTeacherFrame = NULL;

	m_dwFuncMask = 0;

	m_hNofityWnd = NULL;
	m_nNotifyMsg = NULL;

	memset(&m_teacherInfo, 0, sizeof(m_teacherInfo));
	m_teacherInfo.bLoggedIn = TRUE;
	CString strTeacher;
	strTeacher.LoadString(IDS_TEACHER);
	_tcscpy(m_teacherInfo.szDefaultName, strTeacher);
}

CAvailableMemberDlg::~CAvailableMemberDlg()
{
}

void CAvailableMemberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMBER_LIST, m_wndMemberList);
	DDX_Control(pDX, IDC_SET, m_btnSet);
	DDX_Control(pDX, IDC_CLEAR, m_btnClear);
	DDX_Control(pDX, ID_FILTER_CLASS, m_wndClassList);
	DDX_Control(pDX, ID_FILTER_KEYWORD, m_wndKeyword);
}


BEGIN_MESSAGE_MAP(CAvailableMemberDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_SET, &CAvailableMemberDlg::OnBnClickedSet)
	ON_BN_CLICKED(IDC_CLEAR, &CAvailableMemberDlg::OnBnClickedClear)
	ON_WM_KILLFOCUS()
	ON_CBN_SELCHANGE(ID_FILTER_CLASS, &CAvailableMemberDlg::OnCbnSelchangeFilterClass)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MEMBER_LIST, &CAvailableMemberDlg::OnMemberListItemChanged)
	ON_EN_CHANGE(ID_FILTER_KEYWORD, &CAvailableMemberDlg::OnEnChangeFilterKeyword)
	ON_NOTIFY(NM_DBLCLK, IDC_MEMBER_LIST, &CAvailableMemberDlg::OnNMDblclkMemberList)
END_MESSAGE_MAP()


// CAvailableMemberDlg message handlers

void CAvailableMemberDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndMemberList.GetSafeHwnd())) {

		if (m_bShowBtn) {
			m_wndMemberList.SetWindowPos(NULL, LEFT_MARGIN, TOP_MARGIN, cx-LEFT_MARGIN, cy - TOP_MARGIN, SWP_NOZORDER);
		} else 
			m_wndMemberList.SetWindowPos(NULL, 0, TOP_MARGIN, cx, cy - TOP_MARGIN, SWP_NOZORDER);
	}
}

int CAvailableMemberDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_brush.CreateSolidBrush(RGB(221,231,238));

	CBitmap bmpBase;
	m_imgList.Create(16, 16, ILC_MASK | ILC_COLOR24, 0, 1);
	bmpBase.Attach(::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_GROUP_LIST_STATE), 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION));
	m_imgList.Add(&bmpBase, RGB(255,0,255));

	return 0;
}

BOOL CAvailableMemberDlg::Initialize(PSTUDV_PARAMS pParams)
{
	if (!pParams)
		return FALSE;

	// add teacher 
	if (m_bHasTeacher)
	{
		AddItem(&m_teacherInfo);
	}

	m_pTeacherFrame = pParams->pTeacherFrame;
	if (!m_pTeacherFrame)
		return FALSE;

	m_dwFuncMask = pParams->dwFuncMask;

	PMEMBERINFO ppMemberInfo[ELC_MAX_MEMBER_COUNT] = {0};
	int nCount = m_pTeacherFrame->GetAllMemberInfoPtrs(ppMemberInfo);

	for (int i=0; i<nCount; i++) 
	{
		//if (ppMemberInfo[i]->bLoggedIn && (ppMemberInfo[i]->dwState & pParams->dwFuncMask))
		if (ppMemberInfo[i]->dwState & pParams->dwFuncMask)
			AddItem(ppMemberInfo[i]);
	}

	DWORD dwMask = ITeacherFrame::ELC_MEMBER_CONNECTED |
				ITeacherFrame::ELC_MEMBER_DISCONNECTED |
				ITeacherFrame::ELC_MEMBER_HANDDOWN |
				ITeacherFrame::ELC_MEMBER_HANDUP |
				ITeacherFrame::ELC_MEMBER_NAMECHANGED |
				ITeacherFrame::ELC_FUNCTION_CHANGED;

	m_pTeacherFrame->RegisterCallback(dwMask, this, NULL);

	return TRUE;
}

void CAvailableMemberDlg::EnableButtons(BOOL bSet, BOOL bClear)
{
	m_btnSet.EnableWindow(bSet);
	m_btnClear.EnableWindow(bClear);
}

void CAvailableMemberDlg::SetCallback(IAvailableMemberDlg* pCallback)
{
	m_pCallback = pCallback;
}

void CAvailableMemberDlg::SetNotifyWindow(HWND hWnd, UINT nMsg)
{
	m_hNofityWnd = hWnd;
	m_nNotifyMsg = nMsg;
}

HBRUSH CAvailableMemberDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		return m_brush;
	}

	return hbr;
}

BOOL CAvailableMemberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyle(0, WS_CLIPCHILDREN);

	m_wndMemberList.SetImageList(&m_imgList, LVSIL_SMALL);
	m_wndMemberList.SetBkColor(RGB(250,250,250));

	if (m_bCheck)
		m_wndMemberList.SetExtendedStyle(m_wndMemberList.GetExtendedStyle() | LVS_EX_CHECKBOXES);
	
	m_btnSet.ShowWindow(m_bShowBtn ? SW_SHOW : SW_HIDE);
	m_btnClear.ShowWindow(m_bShowBtn ? SW_SHOW : SW_HIDE);

	m_btnSet.EnableWindow(FALSE);
	m_btnClear.EnableWindow(FALSE);

	// Initialize Class Fileter
	CString strFilter;
	strFilter.LoadString(IDS_FILTER_ALL);
	m_wndClassList.AddString(strFilter);

	strFilter.LoadString(IDS_FILTER_HANDUP);
	m_wndClassList.AddString(strFilter);

	m_wndClassList.SetCurSel(0);

	return TRUE;
}

BOOL CAvailableMemberDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rcCleint;
	GetClientRect(&rcCleint);

	CRect rcHeader(0,0,rcCleint.Width(), TOP_MARGIN);
	pDC->FillSolidRect(rcHeader, RGB(221,231,238));

	if (m_bShowBtn) {
		CRect rcBtn(0, TOP_MARGIN, LEFT_MARGIN, rcCleint.Height());
		pDC->FillSolidRect(rcBtn, RGB(250,250,250));
	}

	return TRUE;
}

void CAvailableMemberDlg::EnableCheckItem(BOOL bCheck)
{
	m_bCheck = bCheck;
}

void CAvailableMemberDlg::ShowSwitchButton(BOOL bShow)
{
	m_bShowBtn = bShow;
}

void CAvailableMemberDlg::EnableMultiSel(BOOL bOn)
{
	if (!::IsWindow(m_wndMemberList.GetSafeHwnd()))
	{
		ASSERT(0);
		return ;
	}

	if (bOn)
		m_wndMemberList.ModifyStyle(LVS_SINGLESEL, 0);
	else
		m_wndMemberList.ModifyStyle(0, LVS_SINGLESEL);
}

void CAvailableMemberDlg::EnableTeacher(BOOL bEnable)
{
	m_bHasTeacher = bEnable;
}

void CAvailableMemberDlg::OnBnClickedSet()
{
	if (!m_bShowBtn)
		return;

	if (m_pCallback)
		m_pCallback->OnAddItem();
}

void CAvailableMemberDlg::OnBnClickedClear()
{
	if (!m_bShowBtn)
		return;

	if (m_pCallback)
		m_pCallback->OnDelItem();
}

void CAvailableMemberDlg::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);
}

void CAvailableMemberDlg::AddItem(PMEMBERINFO pMemberInfo)
{
	if (!pMemberInfo)
		return;

	DWORD dwItemData = (DWORD)pMemberInfo;

	if (m_MemberList.Find(dwItemData) != NULL)
		return;

	m_MemberList.AddTail(dwItemData);

	int nImage = GetStateIconIndex(pMemberInfo);
	int nItem = m_wndMemberList.InsertItem(m_wndMemberList.GetItemCount(), 
		pMemberInfo->szDefaultName, nImage);

	m_wndMemberList.SetItemData(nItem, dwItemData);

	/*if (pMemberInfo->dwCommand & CMD_BROADCAST) {
		m_wndMemberList.SetCheck(nItem, TRUE);
	}*/

	if (_tcslen(pMemberInfo->szGroupName) > 0) 
	{
		POSITION pos = m_listGroup.Find(pMemberInfo->szGroupName);
		if (NULL == pos) 
		{
			m_listGroup.AddTail(pMemberInfo->szGroupName);
			m_wndClassList.AddString(pMemberInfo->szGroupName);
		}
	}
}

void CAvailableMemberDlg::DelItem(PMEMBERINFO pMemberInfo)
{
	if (!pMemberInfo)
		return;

	DWORD dwItemData = (DWORD)pMemberInfo;

	POSITION pos = m_MemberList.Find(dwItemData);
	if (pos == NULL)
		return;

	m_MemberList.RemoveAt(pos);

	int nItem = FindItemByMac(pMemberInfo->id.abMac);
	m_wndMemberList.DeleteItem(nItem);
}

void CAvailableMemberDlg::OnMemberConnected(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nItem = FindItemByMac(abMac);
	if (nItem < 0)
		return;

	PMEMBERINFO pMemberInfo = (PMEMBERINFO)m_wndMemberList.GetItemData(nItem);
	int nImage = GetStateIconIndex(pMemberInfo);

	m_wndMemberList.SetItem(nItem, 0, LVIF_IMAGE | LVIF_TEXT, 
		lpszName, nImage, 0, 0, 0);

	m_wndMemberList.RedrawItems(nItem, nItem);
}

void CAvailableMemberDlg::OnMemberDisconnected(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nItem = FindItemByMac(abMac);
	if (nItem < 0)
		return;

	PMEMBERINFO pMemberInfo = (PMEMBERINFO)m_wndMemberList.GetItemData(nItem);
	int nImage = GetStateIconIndex(pMemberInfo);

	m_wndMemberList.SetItem(nItem, 0, LVIF_IMAGE | LVIF_TEXT, 
		lpszName, nImage, 0, 0, 0);

	m_wndMemberList.RedrawItems(nItem, nItem);
}

void CAvailableMemberDlg::OnMemberNameChanged(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nItem = FindItemByMac(abMac);
	if (nItem < 0)
		return;

	PMEMBERINFO pMemberInfo = (PMEMBERINFO)m_wndMemberList.GetItemData(nItem);
	int nImage = GetStateIconIndex(pMemberInfo);

	m_wndMemberList.SetItem(nItem, 0, LVIF_IMAGE | LVIF_TEXT, 
		lpszName, nImage, 0, 0, 0);

	m_wndMemberList.RedrawItems(nItem, nItem);
}

void CAvailableMemberDlg::OnMemberHandUp(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nItem = FindItemByMac(abMac);
	if (nItem < 0)
		return;

	PMEMBERINFO pMemberInfo = (PMEMBERINFO)m_wndMemberList.GetItemData(nItem);
	int nImage = GetStateIconIndex(pMemberInfo);

	m_wndMemberList.SetItem(nItem, 0, LVIF_IMAGE | LVIF_TEXT, 
		lpszName, nImage, 0, 0, 0);

	m_wndMemberList.RedrawItems(nItem, nItem);
}

void CAvailableMemberDlg::OnMemberHandDown(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int nItem = FindItemByMac(abMac);
	if (nItem < 0)
		return;

	PMEMBERINFO pMemberInfo = (PMEMBERINFO)m_wndMemberList.GetItemData(nItem);
	int nImage = GetStateIconIndex(pMemberInfo);

	m_wndMemberList.SetItem(nItem, 0, LVIF_IMAGE | LVIF_TEXT, 
		lpszName, nImage, 0, 0, 0);

	m_wndMemberList.RedrawItems(nItem, nItem);
}

void CAvailableMemberDlg::OnFunctionChanged(int nFunction, UINT64 dwCommand, BOOL bStart)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (bStart || m_dwFuncMask != dwCommand)
		return;

	CStudentVoiceDlg* pDlgVoice = (CStudentVoiceDlg *)GetParent()->GetParent()->GetParent()->GetParent();
	if (!pDlgVoice)
		return;

	if (!IsWindow(pDlgVoice->GetSafeHwnd()))
		return;

	pDlgVoice->PostMessage(WM_COMMAND, IDCANCEL);
}

int CAvailableMemberDlg::FindItemByMac(BYTE* abMac)
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
void CAvailableMemberDlg::OnCbnSelchangeFilterClass()
{
	m_wndMemberList.DeleteAllItems();
	UpdateListMembers();
}

void CAvailableMemberDlg::UpdateListMembers()
{
	CString strKeyword;
	CString strName;
	m_wndKeyword.GetWindowText(strKeyword);

	PMEMBERINFO pMemberInfo;

	int nItem = m_wndClassList.GetCurSel();
	if (nItem == 0) { // Show all members

		POSITION pos = m_MemberList.GetHeadPosition();
		while (pos) {
			pMemberInfo = (PMEMBERINFO)m_MemberList.GetNext(pos);

			strName = pMemberInfo->szDefaultName;
			if ((!strKeyword.IsEmpty()) &&
				(strName.Find(strKeyword, 0) == -1))
			{
				continue;
			}

			int nImage = GetStateIconIndex(pMemberInfo);
			int nItem = m_wndMemberList.InsertItem(m_wndMemberList.GetItemCount(), 
				pMemberInfo->szDefaultName, nImage);

			DWORD dwItemData = (DWORD)pMemberInfo;
			m_wndMemberList.SetItemData(nItem, dwItemData);

			/*
			if (pMemberInfo->dwCommand & CMD_BROADCAST) {
				m_wndMemberList.SetCheck(nItem, TRUE);
			}
			*/
		}
	} else if (nItem == 1) // Show handup members
	{

		POSITION pos = m_MemberList.GetHeadPosition();
		while (pos) {
			pMemberInfo = (PMEMBERINFO)m_MemberList.GetNext(pos);

			strName = pMemberInfo->szDefaultName;
			if ((!strKeyword.IsEmpty()) &&
				(strName.Find(strKeyword, 0) == -1))
			{
				continue;
			}

			if (pMemberInfo->dwState & CMD_HANDUP) {
				int nImage = GetStateIconIndex(pMemberInfo);
				int nItem = m_wndMemberList.InsertItem(m_wndMemberList.GetItemCount(), 
					pMemberInfo->szDefaultName, nImage);

				DWORD dwItemData = (DWORD)pMemberInfo;
				m_wndMemberList.SetItemData(nItem, dwItemData);

				/*if (pMemberInfo->dwCommand & CMD_BROADCAST) {
					m_wndMemberList.SetCheck(nItem, TRUE);
				}*/
			}
		}

	} else // Group name
	{
		CString strGroupName;
		m_wndClassList.GetLBText(nItem, strGroupName);

		POSITION pos = m_MemberList.GetHeadPosition();
		while (pos) {
			pMemberInfo = (PMEMBERINFO)m_MemberList.GetNext(pos);

			strName = pMemberInfo->szDefaultName;
			if ((!strKeyword.IsEmpty()) &&
				(strName.Find(strKeyword, 0) == -1))
			{
				continue;
			}

			if (strGroupName.Compare(pMemberInfo->szGroupName) == 0) {
				int nImage = GetStateIconIndex(pMemberInfo);
				int nItem = m_wndMemberList.InsertItem(m_wndMemberList.GetItemCount(), 
					pMemberInfo->szDefaultName, nImage);

				DWORD dwItemData = (DWORD)pMemberInfo;
				m_wndMemberList.SetItemData(nItem, dwItemData);
			}
		}
	}
}

void CAvailableMemberDlg::OnDestroy()
{
	if (m_pTeacherFrame)
		m_pTeacherFrame->UnRegisterCallback(this);

	__super::OnDestroy();
}

void CAvailableMemberDlg::OnMemberListItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	::PostMessage(m_hNofityWnd, m_nNotifyMsg, 0, 0);
	
	*pResult = 0;
}

void CAvailableMemberDlg::OnEnChangeFilterKeyword()
{
	m_wndMemberList.DeleteAllItems();
	UpdateListMembers();
}

void CAvailableMemberDlg::OnOK()
{
}

void CAvailableMemberDlg::OnCancel()
{
}

void CAvailableMemberDlg::OnNMDblclkMemberList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	::ScreenToClient(m_wndMemberList.GetSafeHwnd(), &pt);

	UINT uFlag = 0;
	m_wndMemberList.HitTest(pt, &uFlag);

	if (uFlag & LVHT_ONITEM) {
		OnBnClickedSet();
	}

	*pResult = 0;
}

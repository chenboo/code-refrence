// NumberLimitEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NumberLimitEdit.h"


// CNumberLimitEdit

IMPLEMENT_DYNAMIC(CNumberLimitEdit, CEdit)

CNumberLimitEdit::CNumberLimitEdit()
{
	m_nCurrent = 0;

	m_nMax = -1;
	m_nMin = -1;

	m_bEmptyText = FALSE;

	m_hNotify = NULL;
	m_nNotifyMsg = 0;
}

CNumberLimitEdit::~CNumberLimitEdit()
{
}


BEGIN_MESSAGE_MAP(CNumberLimitEdit, CEdit)
	ON_CONTROL_REFLECT(EN_CHANGE, &CNumberLimitEdit::OnEnChange)
END_MESSAGE_MAP()



// CNumberLimitEdit message handlers

BOOL CNumberLimitEdit::InitEdit(int nDefault, int nMin, int nMax, UINT nErrorMsgID, UINT nTitleID)
{
	CString strTitle;
	strTitle.LoadString(nTitleID);
	return InitEdit(nDefault, nMin, nMax, nErrorMsgID, strTitle);
}

BOOL CNumberLimitEdit::InitEdit(int nDefault, int nMin, int nMax, UINT nErrorMsgID, LPCTSTR lpszTitle)
{
	if (nMax < nMin || nDefault < nMin || nDefault > nMax) {
		ASSERT(0);
		return FALSE;
	}

	m_nCurrent = nDefault;
	m_strEditText.Format(_T("%d"), m_nCurrent);
	m_nMin = nMin;
	m_nMax = nMax;
	m_nErrMsgID = nErrorMsgID;
	m_strTitle = lpszTitle;

	if (::IsWindow(m_hWnd))
		SetWindowText(m_strEditText);

	return TRUE;
}

void CNumberLimitEdit::SetRange(int nMin, int nMax)
{
	if (nMax < nMin) {
		ASSERT(0);
		return;
	}
	if (m_nCurrent < nMin)
		m_nCurrent = nMin;
	if (m_nCurrent > nMax)
		m_nCurrent = nMax;

	if (::IsWindow(m_hWnd)) {
		SetDisplayNum(m_nCurrent);
	}

	m_nMin = nMin;
	m_nMax = nMax;
}

void CNumberLimitEdit::SetEditChangeNotify(HWND hWnd, UINT uMsg)
{
	m_hNotify = hWnd;
	m_nNotifyMsg = uMsg;
}

BOOL CNumberLimitEdit::ValidityCheck(BOOL bShowMsg, BOOL bRecoverDefault)
{
	CString strValue;
	GetWindowText(strValue);

	
	if (strValue.GetLength() == 0) {
		if (bShowMsg) {
			ReportErrorMsg();
			//SetDisplayNum(m_nCurrent);
		}

		if (bRecoverDefault)
			SetDisplayNum(m_nCurrent);

		return FALSE;
	}

	int nNew = _tstoi(strValue);

	if (   m_nMin != m_nMax 
		&& (nNew < m_nMin || nNew > m_nMax) ) 
	{
		if (bShowMsg)
			ReportErrorMsg();
		if (bRecoverDefault)
			SetDisplayNum(m_nCurrent);

		return FALSE;
	}

	m_nCurrent = nNew;
	return TRUE;
}

void CNumberLimitEdit::OnEnChange()
{
	CString strValue;
	GetWindowText(strValue);

	if (strValue.GetLength() == 0) {
		m_bEmptyText = TRUE;
		if (m_hNotify != NULL && ::IsWindow(m_hNotify))
			::PostMessage(m_hNotify, m_nNotifyMsg, GetDlgCtrlID(), 0);
		return;
	}

	for (int i=0; i < strValue.GetLength(); i ++) {
		if (   strValue.GetAt(i) < 0x30
			|| strValue.GetAt(i) > 0x39 ) 
		{
			SetWindowText(m_strEditText);
			//SetDisplayNum(m_nCurrent);
			if (m_hNotify != NULL && ::IsWindow(m_hNotify))
				::PostMessage(m_hNotify, m_nNotifyMsg, GetDlgCtrlID(), 0);
			return ;
		}
	}

	m_bEmptyText = FALSE;
	m_strEditText = strValue;

	if (m_hNotify != NULL && ::IsWindow(m_hNotify))
		::PostMessage(m_hNotify, m_nNotifyMsg, GetDlgCtrlID(), 0);
}

void CNumberLimitEdit::SetDisplayNum(int nNum)
{
	CString strValue;
	strValue.Format(_T("%d"), nNum);
	SetWindowText(strValue);
}

void CNumberLimitEdit::ReportErrorMsg()
{
	CString strMsg, strTitle;
	strMsg.Format(m_nErrMsgID, m_nMin, m_nMax);	

	UINT nType = MB_OK;

	DWORD dwLayout = 0;
	GetProcessDefaultLayout(&dwLayout);
	if (dwLayout == LAYOUT_RTL) {
		nType |= MB_RTLREADING | MB_RIGHT;
	}
	
	MessageBox(strMsg, m_strTitle, nType);
}

LRESULT CNumberLimitEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_SETTEXT == message) {
		m_nCurrent = _tstoi((LPCTSTR)lParam);
		m_strEditText = (LPCTSTR)lParam;
	}

	return CEdit::WindowProc(message, wParam, lParam);
}

void CNumberLimitEdit::RecoverLast()
{
	SetWindowText(m_strEditText);
}

BOOL CNumberLimitEdit::IsEditEmpty()
{
	return m_bEmptyText;
}
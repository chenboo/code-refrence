#pragma once


// CNumberLimitEdit

class CNumberLimitEdit : public CEdit
{
	DECLARE_DYNAMIC(CNumberLimitEdit)

public:
	CNumberLimitEdit();
	virtual ~CNumberLimitEdit();

	// ALL in ONE settings
	BOOL InitEdit(int nDefault, int nMin, int nMax, UINT nErrorMsgID, UINT nTitleID);
	BOOL InitEdit(int nDefault, int nMin, int nMax, UINT nErrorMsgID, LPCTSTR lpszTitle);
	BOOL ValidityCheck(BOOL bShowMsg = TRUE, BOOL bRecoverDefault = FALSE);

	void SetRange(int nMin, int nMax);
	void SetEditChangeNotify(HWND hWnd, UINT uMsg);

	void RecoverLast();
	BOOL IsEditEmpty();

public:
	int m_nCurrent;

protected:
	void SetDisplayNum(int nNum);
	void ReportErrorMsg();

protected:
	int m_nMax;
	int m_nMin;

	HWND m_hNotify;
	UINT m_nNotifyMsg;

	UINT m_nErrMsgID;
	
	CString m_strTitle;
	CString m_strEditText;
	BOOL m_bEmptyText;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnEnChange();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

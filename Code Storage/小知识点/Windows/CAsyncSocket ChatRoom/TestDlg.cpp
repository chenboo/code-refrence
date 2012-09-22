// TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DLG.h"
#include "TestDlg.h"


// CTestDlg dialog

IMPLEMENT_DYNAMIC(CTestDlg, CDialog)

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{

}

CTestDlg::~CTestDlg()
{
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_SET, &CTestDlg::OnBnClickedBtnSet)
	ON_BN_CLICKED(IDC_BTN_RECOVER, &CTestDlg::OnBnClickedBtnRecover)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CTestDlg message handlers

void CTestDlg::OnBnClickedBtnSet()
{
}

void CTestDlg::OnBnClickedBtnRecover()
{
}

void CTestDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);
}

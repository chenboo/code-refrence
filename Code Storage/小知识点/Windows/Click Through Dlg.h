// Create Click through DLG

//Main set the Two Extend Styles of the dialog
//WS_EX_LAYERED and WS_EX_TRANSPARENT

//Example:

//set the dialog to click through state
void CTestDlg::OnBnClickedBtnSet()
{
	ModifyStyleEx(NULL, WS_EX_LAYERED | WS_EX_TRANSPARENT);
	SetLayeredWindowAttributes(RGB(236, 233, 216), (255 * 70) / 100, LWA_ALPHA);
}

//Recover the Orgin State
void CTestDlg::OnBnClickedBtnRecover()
{
	ModifyStyleEx(WS_EX_LAYERED | WS_EX_TRANSPARENT,NULL);
	Invalidate();
}

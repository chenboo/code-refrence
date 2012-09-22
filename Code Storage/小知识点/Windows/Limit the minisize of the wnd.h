//to limit the minisize of the window
//add code in the MainFrm.cpp

ON_WM_GETMINMAXINFO()		
		
	
void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 1000;
	lpMMI->ptMinTrackSize.y = 800;

	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

//add declaration in the MainFrm.h
afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
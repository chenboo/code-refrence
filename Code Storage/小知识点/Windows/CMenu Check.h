//为菜单(CMenu)添加选中(check)标记

//问题：一般情况下我们用UPDATE_COMMAND_UI来修改菜单的状态(enable/disable, check/uncheck, change text),
//但这个方法在一个基于对话框上的菜单却没有效果。

void CTestDlg::OnUpdateFileExit(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(FALSE); 
    pCmdUI->SetCheck(TRUE); 
    pCmdUI->SetRadio(TRUE); 
    pCmdUI->SetText("Close"); 
//以上方法在MDI、SDI程序中都能起作用，在对话框中却没有效果,根本没有调用这个函数。
}
    
//原因分析：当显示一个下拉的菜单的时候，在显示菜单前会发送WM_INITMENUPOPUP消息。
//而CFrameWnd::OnInitMenuPopup函数会刷新这个菜单项，同时如果有UPDATE_COMMAND_UI响应函数，则调用它。
//通过它来更新反应每个菜单的外观效果(enabled/disabled, checked/unchecked).
//在一个基于对话框的程序中，因为没有OnInitMenuPopup函数，所以不会调用UPDATE_COMMAND_UI响应函数，
//而是使用了CWnd类的默认处理, 这种处理没有调用UPDATE_COMMAND_UI响应函数。

//解决方法如下：
//第一步：
//在对话框类的.cpp文件，添加一个ON_WM_INITMENUPOPUP入口到消息映射里面
BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
//}}AFX_MSG_MAP
ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()

//第二步：
//在对话框类的.h文件添加消息函数声明。
afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);


//第三步：
//在对话框类的.cpp文件添加如下函数代码(大部分代码取自WinFrm.cpp文件的函数CFrameWnd::OnInitMenuPopup):

void C******Dlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
{
    ASSERT(pPopupMenu != NULL);
    // Check the enabled state of various menu items.

    CCmdUI state;
    state.m_pMenu = pPopupMenu;
    ASSERT(state.m_pOther == NULL);
    ASSERT(state.m_pParentMenu == NULL);

    // Determine if menu is popup in top-level menu and set m_pOther to
    // it if so (m_pParentMenu == NULL indicates that it is secondary popup).
    HMENU hParentMenu;
    if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
        state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
    else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
    {
        CWnd* pParent = this;
           // Child windows don't have menus--need to go to the top!
        if (pParent != NULL &&
           (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
        {
           int nIndexMax = ::GetMenuItemCount(hParentMenu);
           for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
           {
            if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
            {
                // When popup is found, m_pParentMenu is containing menu.
                state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
                break;
            }
           }
        }
    }

    state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
    for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
      state.m_nIndex++)
    {
        state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
        if (state.m_nID == 0)
           continue; // Menu separator or invalid cmd - ignore it.

        ASSERT(state.m_pOther == NULL);
        ASSERT(state.m_pMenu != NULL);
        if (state.m_nID == (UINT)-1)
        {
           // Possibly a popup menu, route to first item of that popup.
           state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
           if (state.m_pSubMenu == NULL ||
            (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
            state.m_nID == (UINT)-1)
           {
            continue;       // First item of popup can't be routed to.
           }
           state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
        }
        else
        {
           // Normal menu item.
           // Auto enable/disable if frame window has m_bAutoMenuEnable
           // set and command is _not_ a system command.
           state.m_pSubMenu = NULL;
           state.DoUpdate(this, FALSE);
        }

        // Adjust for menu deletions and additions.
        UINT nCount = pPopupMenu->GetMenuItemCount();
        if (nCount < state.m_nIndexMax)
        {
           state.m_nIndex -= (state.m_nIndexMax - nCount);
           while (state.m_nIndex < nCount &&
            pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
           {
            state.m_nIndex++;
           }
        }
        state.m_nIndexMax = nCount;
    }
}


//通过这三步后UPDATE_COMMAND_UI的消息就能响应了, 可以在响应函数中进行
//对菜单项的设置。

//参考: http://support.microsoft.com/kb/242577/en-us
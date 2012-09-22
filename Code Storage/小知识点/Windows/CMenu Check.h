//Ϊ�˵�(CMenu)���ѡ��(check)���

//���⣺һ�������������UPDATE_COMMAND_UI���޸Ĳ˵���״̬(enable/disable, check/uncheck, change text),
//�����������һ�����ڶԻ����ϵĲ˵�ȴû��Ч����

void CTestDlg::OnUpdateFileExit(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(FALSE); 
    pCmdUI->SetCheck(TRUE); 
    pCmdUI->SetRadio(TRUE); 
    pCmdUI->SetText("Close"); 
//���Ϸ�����MDI��SDI�����ж��������ã��ڶԻ�����ȴû��Ч��,����û�е������������
}
    
//ԭ�����������ʾһ�������Ĳ˵���ʱ������ʾ�˵�ǰ�ᷢ��WM_INITMENUPOPUP��Ϣ��
//��CFrameWnd::OnInitMenuPopup������ˢ������˵��ͬʱ�����UPDATE_COMMAND_UI��Ӧ���������������
//ͨ���������·�Ӧÿ���˵������Ч��(enabled/disabled, checked/unchecked).
//��һ�����ڶԻ���ĳ����У���Ϊû��OnInitMenuPopup���������Բ������UPDATE_COMMAND_UI��Ӧ������
//����ʹ����CWnd���Ĭ�ϴ���, ���ִ���û�е���UPDATE_COMMAND_UI��Ӧ������

//����������£�
//��һ����
//�ڶԻ������.cpp�ļ������һ��ON_WM_INITMENUPOPUP��ڵ���Ϣӳ������
BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
//}}AFX_MSG_MAP
ON_WM_INITMENUPOPUP()
END_MESSAGE_MAP()

//�ڶ�����
//�ڶԻ������.h�ļ������Ϣ����������
afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);


//��������
//�ڶԻ������.cpp�ļ�������º�������(�󲿷ִ���ȡ��WinFrm.cpp�ļ��ĺ���CFrameWnd::OnInitMenuPopup):

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


//ͨ����������UPDATE_COMMAND_UI����Ϣ������Ӧ��, ��������Ӧ�����н���
//�Բ˵�������á�

//�ο�: http://support.microsoft.com/kb/242577/en-us
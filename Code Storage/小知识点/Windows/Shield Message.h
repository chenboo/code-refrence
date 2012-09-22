//the follow example show how
//to shield some message
//not let wnd to respond


BOOL CRrdjDlg::PreTranslateMessage(MSG* pMsg)     
{    
    // TODO: Add your specialized code here and/or call the base class    
    if(pMsg->message == WM_KEYDOWN)      
    {      
        if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)      
        {      
          return   TRUE;      
        }      
    }       
    return CDialog::PreTranslateMessage(pMsg);    
}
void CBoardWnd::TrackGraphMenu(CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_MENU_DOGRAPH);
	CMenu* pSubMenu = menu.GetSubMenu(0);

	ASSERT(m_pGraph);
	DrawType nType = m_DrawPagesArray[m_nSelPage].GetTheGraphType(m_pGraph);

	if(nType == DT_ARTISTPEN)
	{
		pSubMenu->EnableMenuItem(ID_PROPERTIES_SET, MF_BYCOMMAND | MF_GRAYED);
	}

	if(DT_TEXT != nType || !(m_pGraph->m_bFinish))
	{
		pSubMenu->EnableMenuItem(ID_EDIT, MF_BYCOMMAND | MF_GRAYED);
	}
	
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}
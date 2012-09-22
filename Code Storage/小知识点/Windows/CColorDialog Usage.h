//颜色对话框的使用		
COLORREF clr = m_WndBoard.GetBoardColor();
CColorDialog crdlg(clr);

if(IDOK == crdlg.DoModal())
{
	clr = crdlg.GetColor();
	m_WndBoard.SetBkGround(clr);
}
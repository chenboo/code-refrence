//当创建一个弹出菜单时，如何在菜单消失时进行处理
//因为有时菜单消失是因为点击了菜单以外的区域，并不一定点击了菜单项

//处理的方法是响应WM_EXITMENULOOP消息
//处理步骤:

//1>
ON_WM_EXITMENULOOP()
//2>
afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
//3>
void CXXXX::OnExitMenuLoop(BOOL bIsTrackPopupMenu)
{


}
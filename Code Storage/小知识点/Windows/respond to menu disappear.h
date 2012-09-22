//对于弹出菜单来说，当用户点击菜单上的某一项或在菜单外点击时，弹出菜单都会消失。

//有时我们需要处理在菜单消失时处理一些事情，遮盖怎么办呢？

//这里有一个知识点，但菜单消失时，弹出菜单的窗口将会收到一个通知消息
//WM_EXITMENULOOP

//对这个通知的响应就可以让我们处理这些事情
ON_WM_EXITMENULOOP()

//函数声明：afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);

//函数实现：
void CVoiceMonitorView::OnExitMenuLoop(BOOL bIsTrackPopupMenu)
{
	m_SettingMonitBar.SetButtonChecked(IDC_BTN_SET_VOICE_MONITOR, FALSE);
}


//这样就完成了

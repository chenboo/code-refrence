//���ڵ����˵���˵�����û�����˵��ϵ�ĳһ����ڲ˵�����ʱ�������˵�������ʧ��

//��ʱ������Ҫ�����ڲ˵���ʧʱ����һЩ���飬�ڸ���ô���أ�

//������һ��֪ʶ�㣬���˵���ʧʱ�������˵��Ĵ��ڽ����յ�һ��֪ͨ��Ϣ
//WM_EXITMENULOOP

//�����֪ͨ����Ӧ�Ϳ��������Ǵ�����Щ����
ON_WM_EXITMENULOOP()

//����������afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);

//����ʵ�֣�
void CVoiceMonitorView::OnExitMenuLoop(BOOL bIsTrackPopupMenu)
{
	m_SettingMonitBar.SetButtonChecked(IDC_BTN_SET_VOICE_MONITOR, FALSE);
}


//�����������

�������ʹ��ϵͳ���̹��ܣ�
//1������һ�����̳�Ա������
CTrayIcon	m_TrayIcon;

//2:�����������CreateIcon��������һ������
m_TrayIcon.CreateIcon(this,ID_TRAYICON,hIcon,_T("drawer"));

//3:������̻ص���Ϣ����Ӧ
����Ϣ��Ӧ�����е��ã�
m_TrayIcon.OnNotify(WParam,LParam);

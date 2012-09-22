介绍如何使用系统托盘功能：
//1：定义一个托盘成员变量：
CTrayIcon	m_TrayIcon;

//2:调用托盘类的CreateIcon函数构造一个托盘
m_TrayIcon.CreateIcon(this,ID_TRAYICON,hIcon,_T("drawer"));

//3:添加托盘回调消息的响应
在消息响应函数中调用：
m_TrayIcon.OnNotify(WParam,LParam);

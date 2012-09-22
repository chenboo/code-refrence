//Respond to Command Messages
//The Command Messages are continuous(对相连的命令消息的响应)

//1、在cpp文件中添加如下所示的宏:

ON_COMMAND_RANGE(ID_VIEW_LINE1,ID_VIEW_LINE24,OnSelLine)

//2、在.h文件中添加消息响应函数声明:

afx_msg void OnSelLine(UINT nID);

//3、消息响应函数实现:

void CXXXXX::OnSelLine(UINT nID)
{

}

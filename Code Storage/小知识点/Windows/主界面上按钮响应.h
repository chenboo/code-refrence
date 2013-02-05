//在语音教室中对主界面上的按钮的响应的流程：
//下面以AnswerSheet功能为例来说明一下：
(一)在语音教室中，界面上的按钮有些是通过皮肤所提供的四张图片来设置的
但由于皮肤所提供的四张图片是固定的，不利于进行拉伸，所以最后界面上的
按钮的图片很多是通过自定义来实现的。

class CTeacherDlg : public	CSkinDialog

CSkinDialog 是一个用于解析图片和XML信息的对话框，因而可以实现快速的换肤。

(1)在当创建按钮时会给按钮设定一个ID值，以及它所要进行发送消息的窗口的句柄:
这里就是给AnswerSheet按钮设定了ID_ANSWER_SHEET这个ID，以及将CTeacherDlg窗口句柄传递给了它，

(2)当我们点击测试按钮时，该按钮就会向CTeacherDlg发送WM_COMMAND消息，CTeacherDlg进行响应。
CTeacherDlg.cpp中:

ON_COMMAND(ID_ANSWER_SHEET, OnAnswerSheet)

void CTeacherDlg::OnAnswerSheet()
{
	if(IsStartFunction(FUNCTION_ANSWER_SHEET)){
		GetAnswerSheetView()->StopAnswerSheet(FALSE);
	}
	else{

		GetAnswerSheetView()->StartAnswerSheet();
	}
}

//就这样了。
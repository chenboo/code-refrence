//�����������ж��������ϵİ�ť����Ӧ�����̣�
//������AnswerSheet����Ϊ����˵��һ�£�
(һ)�����������У������ϵİ�ť��Щ��ͨ��Ƥ�����ṩ������ͼƬ�����õ�
������Ƥ�����ṩ������ͼƬ�ǹ̶��ģ������ڽ������죬�����������ϵ�
��ť��ͼƬ�ܶ���ͨ���Զ�����ʵ�ֵġ�

class CTeacherDlg : public	CSkinDialog

CSkinDialog ��һ�����ڽ���ͼƬ��XML��Ϣ�ĶԻ����������ʵ�ֿ��ٵĻ�����

(1)�ڵ�������ťʱ�����ť�趨һ��IDֵ���Լ�����Ҫ���з�����Ϣ�Ĵ��ڵľ��:
������Ǹ�AnswerSheet��ť�趨��ID_ANSWER_SHEET���ID���Լ���CTeacherDlg���ھ�����ݸ�������

(2)�����ǵ�����԰�ťʱ���ð�ť�ͻ���CTeacherDlg����WM_COMMAND��Ϣ��CTeacherDlg������Ӧ��
CTeacherDlg.cpp��:

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

//�������ˡ�
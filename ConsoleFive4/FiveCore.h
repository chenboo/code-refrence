#pragma once
#include "FiveArr.h"

class CFiveArr;

//�ж�ʤ���ĺ����㷨��
class CFiveCore
{
public:
	//���� ���ӷ���'O' ��ʾAʤ������ ���ӷ���'X' ��ʾBʤ������ �ո�' ' ��ʾû�зֳ�ʤ����
	char JudgeWin(CFiveArr& fiveArr, char cChessFlag);
	char JudgeWin(CFiveArr& fiveArr);
};
#pragma once
#include "FiveArr.h"

class CFiveArr;

//�ж�ʤ���ĺ����㷨��
class CFiveCore
{
public:
	static BOOL bWinFlag;
public:
	void JudgeWin(CFiveArr& fiveArr);
	BOOL JudgeWin(CFiveArr& fiveArr, const char cQiZi);
};
#pragma once
#include "FiveArr.h"

class CFiveArr;

//判断胜负的核心算法类
class CFiveCore
{
public:
	static BOOL bWinFlag;
public:
	void JudgeWin(CFiveArr& fiveArr);
	BOOL JudgeWin(CFiveArr& fiveArr, const char cQiZi);
};
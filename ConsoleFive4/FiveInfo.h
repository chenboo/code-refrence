#pragma once
#include "FiveArr.h"

//提示信息类
class CFiveInfo
{
public:
	void ShowMsg(BOOL bPlayer);
	BOOL IsPosAllow(int nPosRow, int nPosCol);
	BOOL IsPosAllow(int nPos);
	BOOL IsQiZiExist(CFiveArr& fiveArr, CFivePos& pos);
};
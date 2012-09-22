#pragma once
#include "FiveArr.h"

class CFiveArr;

//判断胜负的核心算法类
class CFiveCore
{
public:
	//返回 棋子符号'O' 表示A胜，返回 棋子符号'X' 表示B胜，返回 空格' ' 表示没有分出胜负。
	char JudgeWin(CFiveArr& fiveArr, char cChessFlag);
	char JudgeWin(CFiveArr& fiveArr);
};
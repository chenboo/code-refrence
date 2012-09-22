#include "StdAfx.h"
#include "FiveInfo.h"

//提示信息
void CFiveInfo::ShowMsg(BOOL bPlayer)
{
	if(bPlayer)
	{
		cout << "现在轮到玩家A(执子为O)！";
	}
	else
	{
		cout << "现在轮到玩家B(执子为X)！";
	}

	cout << "请输入棋子坐标, "
		 << "棋子坐标在 1 和 " << SIZE << "之间,"
		 << endl
		 << "请按(行 列)方式输入棋子坐标, "
		 << "例如:3 3"
		 << endl;
}

//判断棋子坐标是否合法（重载函数）
BOOL CFiveInfo::IsPosAllow(int nPosRow, int nPosCol)
{
	if(IsPosAllow(nPosRow) && IsPosAllow(nPosCol))
	{
		return TRUE;
	}
	return FALSE;
}

//判断棋子坐标是否合法（重载函数）
BOOL CFiveInfo::IsPosAllow(int nPos)
{
	if(nPos >= 1 && nPos <= SIZE)
	{
		return TRUE;
	}
	return FALSE;
}

//判断当前坐标是否已有棋子
BOOL CFiveInfo::IsQiZiExist(CFiveArr& fiveArr, CFivePos& pos)
{
	if(fiveArr.GetArr(pos)==CHESS_A || fiveArr.GetArr(pos)==CHESS_B)
	{
		return TRUE;
	}
	return FALSE;
}
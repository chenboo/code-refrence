#include "StdAfx.h"
#include "FiveArr.h"

//初始化棋子信息的二维数组
void CFiveArr::InitArr()
{
	for(int i=0; i<SIZE; ++i)
	{
		for(int j=0; j<SIZE; ++j)
		{
			m_cArr[i][j] = SPACE;
		}
	}
}

//显示数组信息
void CFiveArr::PrintArr()
{
	for(int i=0; i<SIZE; ++i)
	{
		for(int j=0; j<SIZE; ++j)
		{
			cout << m_cArr[i][j];
		}
		cout << endl;
	}
}

void CFiveArr::SetArr(CFivePos& pos, char cPlayer)
{
	m_cArr[pos.GetRow()][pos.GetCol()] = cPlayer;
}

char CFiveArr::GetArr(CFivePos& pos)
{
	return m_cArr[pos.GetRow()][pos.GetCol()];
}
#include "StdAfx.h"
#include "FiveArr.h"

//��ʼ��������Ϣ�Ķ�ά����
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

//��ʾ������Ϣ
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
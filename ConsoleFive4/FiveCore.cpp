#include "StdAfx.h"
#include "FiveCore.h"

//���� ���ӷ���'O' ��ʾAʤ������ ���ӷ���'X' ��ʾBʤ������ �ո�' ' ��ʾû�зֳ�ʤ����
char CFiveCore::JudgeWin(CFiveArr &fiveArr, char cChessFlag)
{
	int nRow, nCol, nCount, nTmp;

	//�жϺ���
	for(nRow=0; nRow<SIZE; ++nRow)
	{
		nCount = 0;
		for(nCol=0; nCol<SIZE; ++nCol)
		{
			if(cChessFlag == fiveArr.m_cArr[nRow][nCol])
				++nCount;
			else
				nCount = 0;
			if(5 == nCount)
				return cChessFlag;
		}
	}
	
	//�ж�����
	for(nCol=0; nCol<SIZE; ++nCol)
	{
		nCount = 0;
		for(nRow=0; nRow<SIZE; ++nRow)
		{
			if(cChessFlag == fiveArr.m_cArr[nRow][nCol])
				++nCount;
			else
				nCount = 0;
			if(5 == nCount)
				return cChessFlag;
		}
	}

	//�ж� '\' �ϲ� �� �������Խ����ϲ�(�����Խ���)
	for(nCol=0; nCol<SIZE; ++nCol)
	{
		nTmp = nCol;
		nCount = 0;
		for(nRow=0; nRow<SIZE-nTmp; ++nRow)
		{
			if(cChessFlag == fiveArr.m_cArr[nRow][nCol])
				++nCount;
			else
				nCount = 0;
			if(5 == nCount)
				return cChessFlag;

			++nCol;
		}
		nCol = nTmp;
	}

	//�ж� '\' �²� �� �������Խ����²�(�������Խ���)
	for(nRow=1; nRow<SIZE; ++nRow)
	{
		nTmp = nRow;
		nCount = 0;
		for(nCol=0; nCol<SIZE-nTmp; ++nCol)
		{
			if(cChessFlag == fiveArr.m_cArr[nRow][nCol])
				++nCount;
			else
				nCount = 0;
			if(5 == nCount)
				return cChessFlag;

			++nRow;
		}
		nRow = nTmp;
	}

	//�ж� '/' �ϲ� �� ���̸��Խ����ϲ�(�����Խ���)
	for(nRow=4; nRow<SIZE; ++nRow)
	{
		nTmp = nRow;
		nCount = 0;
		for(nCol=0; nCol<=nTmp; ++nCol)
		{
			if(cChessFlag == fiveArr.m_cArr[nRow][nCol])
				++nCount;
			else
				nCount = 0;
			if(5 == nCount)
				return cChessFlag;

			--nRow;
		}
		nRow = nTmp;
	}

	//�ж� '/' �²� �� ���̸��Խ����²�(�������Խ���)
	for(nCol=1; nCol<SIZE; ++nCol)
	{
		nTmp = nCol;
		nCount = 0;
		for(nRow=SIZE-1; nRow>=nTmp; --nRow)
		{
			if(cChessFlag == fiveArr.m_cArr[nRow][nCol])
				++nCount;
			else
				nCount = 0;
			if(5 == nCount)
				return cChessFlag;

			++nCol;
		}
		nCol = nTmp;
	}

	return ' ';
}

char CFiveCore::JudgeWin(CFiveArr &fiveArr)
{
	char cChessFlag = JudgeWin(fiveArr, 'O');
	if(cChessFlag == ' ')
	{
		cChessFlag = JudgeWin(fiveArr, 'X');
	}
	return cChessFlag;
}
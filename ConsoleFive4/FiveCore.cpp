#include "StdAfx.h"
#include "FiveCore.h"

//返回 棋子符号'O' 表示A胜，返回 棋子符号'X' 表示B胜，返回 空格' ' 表示没有分出胜负。
char CFiveCore::JudgeWin(CFiveArr &fiveArr, char cChessFlag)
{
	int nRow, nCol, nCount, nTmp;

	//判断横向
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
	
	//判断纵向
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

	//判断 '\' 上部 即 棋盘主对角线上部(包括对角线)
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

	//判断 '\' 下部 即 棋盘主对角线下部(不包括对角线)
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

	//判断 '/' 上部 即 棋盘副对角线上部(包括对角线)
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

	//判断 '/' 下部 即 棋盘副对角线下部(不包括对角线)
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
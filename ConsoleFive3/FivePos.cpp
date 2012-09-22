#include "StdAfx.h"
#include "FivePos.h"


void CFivePos::SetPos(int nRow, int nCol)
{
	m_nRow = nRow;
	m_nCol = nCol;
}

int CFivePos::GetRow()
{
	return m_nRow;
}

int CFivePos::GetCol()
{
	return m_nCol;
}
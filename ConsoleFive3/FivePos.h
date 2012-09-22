#pragma once

//Æå×Ó×ø±êÀà
class CFivePos
{
protected:
	int m_nRow;
	int m_nCol;

public:
	void SetPos(int nRow, int nCol);
	int GetRow();
	int GetCol();
};

#pragma once
#include "FivePos.h"
#include "FiveCore.h"

//������Ϣ��
class CFiveArr
{
protected:
	char m_cArr[SIZE][SIZE];
	friend class CFiveCore;

public:
	void InitArr();
	void PrintArr();
	void SetArr(CFivePos& pos, char cPlayer);
	char GetArr(CFivePos& pos);
};

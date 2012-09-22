#include "StdAfx.h"
#include "FiveCore.h"

BOOL CFiveCore::bWinFlag = FALSE;

BOOL CFiveCore::JudgeWin(CFiveArr& fiveArr, const char cQiZi)
{
	for(int i=0; i<SIZE; ++i)
	{
		for(int j=0; j<SIZE; ++j)
		{
			if(cQiZi == fiveArr.m_cArr[i][j])
			{
				for(int nRowInc=-1; nRowInc<=1; ++nRowInc)
				{
					for(int nColInc=-1; nColInc<=1; ++nColInc)
					{
						int nI = i+nRowInc;
						int nJ = j+nColInc;
						if(nI>=0 && nJ>=0 && nI<SIZE && nJ<SIZE)
						{
							if( (nRowInc!=0 || nColInc!=0 ) && cQiZi == fiveArr.m_cArr[nI][nJ])
							{
								for(int nCount=1; nCount<=3; ++nCount)
								{
									nI+=nRowInc;
									nJ+=nColInc;
									if(nI>=0 && nJ>=0 && nI<SIZE && nJ<SIZE)
									{
										if(cQiZi == fiveArr.m_cArr[nI][nJ])
										{
											bWinFlag = TRUE;
										}
										else
										{
											bWinFlag = FALSE;
											break;
										}
									}
									else
									{
										bWinFlag = FALSE;
										break;
									}
								}

								if(bWinFlag)
								{
									return bWinFlag;
								}
							}
						}
					}
				}
			}
		}
	}

	return FALSE;
}

void CFiveCore::JudgeWin(CFiveArr& fiveArr)
{
	if (JudgeWin(fiveArr, 'O'))
	{
		 cout << "o \n";
	}

	if (JudgeWin(fiveArr, 'X'))
	{
		cout << "x \n";
	}
	
}
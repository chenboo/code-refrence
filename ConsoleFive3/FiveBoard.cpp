#include "StdAfx.h"
#include "FiveBoard.h"

const char* szHeader = "    1   2   3   4   5   6   7   8   9   10 ";
const char* szRow = "   --- --- --- --- --- --- --- --- --- ---";

//绘制棋盘
void CFiveBoard::DrawBoard()
{
	cout << szHeader <<endl;
	for(int nRow=0; nRow<SIZE; nRow++)
	{
		cout << szRow <<endl;

		char szBuffer[10];
		sprintf(szBuffer, "%2d", nRow+1); 
		cout << szBuffer;

		for(int nCol=0; nCol<=SIZE; nCol++)
		{
			cout << "|   ";
		}
		cout << endl;
	}
	cout << szRow << endl;
}

//玩家落子后重新绘制棋盘
void CFiveBoard::ChangeBoard(CFiveArr& fiveArr)
{
	cout << szHeader <<endl;
	for(int nRow=0; nRow<SIZE; nRow++)
	{
		cout << szRow <<endl;

		char szBuffer[10];
		sprintf(szBuffer, "%2d", nRow+1); 
		cout << szBuffer;

		for(int nCol=0; nCol<=SIZE; nCol++)
		{
			if(nCol == SIZE)
			{
				cout << "|   ";
				continue;
			}

			CFivePos pos;
			pos.SetPos(nRow, nCol);
			char cPlayer = fiveArr.GetArr(pos);

			if( cPlayer != SPACE )
			{
				cout << "| " << cPlayer << ' ';
			}
			else
			{
				cout << "|   ";
			}
		}
		cout << endl;
	}
	cout << szRow << endl;
}
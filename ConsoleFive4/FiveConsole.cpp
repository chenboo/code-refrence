// ConsoleFive.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "FiveInfo.h"
#include "FiveBoard.h"
#include "FiveArr.h"

//游戏主函数
void MainGame()
{
	CFiveInfo fiveInfo;
	CFiveBoard fiveBoard;
	CFiveArr fiveArr;
	CFiveCore fiveCore;
	CFivePos pos;

	fiveBoard.DrawBoard();
	fiveArr.InitArr();

	BOOL bPlayer = TRUE; //默认玩家A先走（执子O）
	char cPlayer;

	char cChessFlag = ' ';

	while(' ' == cChessFlag)
	{
		fiveInfo.ShowMsg(bPlayer);

		int nPosRow, nPosCol;
		fflush(stdin);
		scanf("%d", &nPosRow);
		scanf("%d", &nPosCol);

		if( ! fiveInfo.IsPosAllow(nPosRow, nPosCol))
		{
			cout << "请输入合法的棋子坐标!" << endl;
			continue;
		}
		
		pos.SetPos(nPosRow-1, nPosCol-1);

		if( fiveInfo.IsQiZiExist(fiveArr, pos) )
		{
			cout << "该位置已经有棋子了!" << endl;
			continue;
		}

		if (bPlayer)
		{
			cPlayer = CHESS_A;
			bPlayer = FALSE;
		}
		else
		{
			cPlayer = CHESS_B;
			bPlayer = TRUE;
		}

		fiveArr.SetArr(pos, cPlayer);

		fiveBoard.ChangeBoard(fiveArr);

		//fiveArr.PrintArr();

		cChessFlag = fiveCore.JudgeWin(fiveArr);

		if('O' == cChessFlag)
		{
			cout << "玩家A获胜！" << endl;
		}
		if('X' == cChessFlag)
		{
			cout << "玩家B获胜！" << endl;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	BOOL bIsContinue = TRUE;
	while(bIsContinue)
	{
		MainGame();

		cout << "是否想再来一局？(Y/N)" << endl;

		char cChoice;
		cin >> cChoice;

		if(cChoice != 'Y' && cChoice != 'y')
		{
			bIsContinue = FALSE;
		}
	}
	return 0;
}


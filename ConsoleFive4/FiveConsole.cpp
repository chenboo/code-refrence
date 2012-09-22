// ConsoleFive.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "FiveInfo.h"
#include "FiveBoard.h"
#include "FiveArr.h"

//��Ϸ������
void MainGame()
{
	CFiveInfo fiveInfo;
	CFiveBoard fiveBoard;
	CFiveArr fiveArr;
	CFiveCore fiveCore;
	CFivePos pos;

	fiveBoard.DrawBoard();
	fiveArr.InitArr();

	BOOL bPlayer = TRUE; //Ĭ�����A���ߣ�ִ��O��
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
			cout << "������Ϸ�����������!" << endl;
			continue;
		}
		
		pos.SetPos(nPosRow-1, nPosCol-1);

		if( fiveInfo.IsQiZiExist(fiveArr, pos) )
		{
			cout << "��λ���Ѿ���������!" << endl;
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
			cout << "���A��ʤ��" << endl;
		}
		if('X' == cChessFlag)
		{
			cout << "���B��ʤ��" << endl;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	BOOL bIsContinue = TRUE;
	while(bIsContinue)
	{
		MainGame();

		cout << "�Ƿ�������һ�֣�(Y/N)" << endl;

		char cChoice;
		cin >> cChoice;

		if(cChoice != 'Y' && cChoice != 'y')
		{
			bIsContinue = FALSE;
		}
	}
	return 0;
}


#include "StdAfx.h"
#include "FiveInfo.h"

//��ʾ��Ϣ
void CFiveInfo::ShowMsg(BOOL bPlayer)
{
	if(bPlayer)
	{
		cout << "�����ֵ����A(ִ��ΪO)��";
	}
	else
	{
		cout << "�����ֵ����B(ִ��ΪX)��";
	}

	cout << "��������������, "
		 << "���������� 1 �� " << SIZE << "֮��,"
		 << endl
		 << "�밴(�� ��)��ʽ������������, "
		 << "����:3 3"
		 << endl;
}

//�ж����������Ƿ�Ϸ������غ�����
BOOL CFiveInfo::IsPosAllow(int nPosRow, int nPosCol)
{
	if(IsPosAllow(nPosRow) && IsPosAllow(nPosCol))
	{
		return TRUE;
	}
	return FALSE;
}

//�ж����������Ƿ�Ϸ������غ�����
BOOL CFiveInfo::IsPosAllow(int nPos)
{
	if(nPos >= 1 && nPos <= SIZE)
	{
		return TRUE;
	}
	return FALSE;
}

//�жϵ�ǰ�����Ƿ���������
BOOL CFiveInfo::IsQiZiExist(CFiveArr& fiveArr, CFivePos& pos)
{
	if(fiveArr.GetArr(pos)==CHESS_A || fiveArr.GetArr(pos)==CHESS_B)
	{
		return TRUE;
	}
	return FALSE;
}
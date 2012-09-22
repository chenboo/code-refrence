#include "stdafx.h"
#include "ListSortUtils.h"
#include "CodeSnippets/protocol.h"

int CALLBACK SortByTextProc(LPARAM lpszText1, LPARAM lpszText2, LPARAM bAscending)
{
	if (!lpszText1 || !lpszText2) {
		ASSERT(0);
		return 0;
	}

	if (bAscending) 
		return _tcscmp((LPCTSTR)lpszText1, (LPCTSTR)lpszText2);
	else
		return _tcscmp((LPCTSTR)lpszText2, (LPCTSTR)lpszText1);
}

int CALLBACK SortByIntegerProc(LPARAM number1, LPARAM number2, LPARAM bAscending)
{
	if (bAscending)
		return (number1 >= number2);
	else
		return (number2 >= number1);
}

int CALLBACK SortByULONGLONG(ULONGLONG number1, ULONGLONG number2, LPARAM bAscending)
{
	if (bAscending)
		return (number1 >= number2);
	else
		return (number2 >= number1);
}

int CALLBACK SortByCharProc(LPARAM char1, LPARAM char2, LPARAM bAscending)
{
	if (!char1 || !char2) {
		ASSERT(0);
		return 0;
	}

	if (bAscending) 
		return stricmp((const char *)char1, (const char *)char2);
	else
		return stricmp((const char *)char2, (const char *)char1);
}

int CALLBACK SortByStatusProc(LPARAM lParam1, LPARAM lParam2, LPARAM bAscending)
{
	if (!lParam1 || !lParam2) {
		ASSERT(0);
		return 0;
	}

	PMEMBERINFO pMemberInfo1 = (PMEMBERINFO)lParam1;
	PMEMBERINFO pMemberInfo2 = (PMEMBERINFO)lParam2;
	if (bAscending == 0) {
		pMemberInfo1 = (PMEMBERINFO)lParam2;
		pMemberInfo2 = (PMEMBERINFO)lParam1;
	}

	if (pMemberInfo1->bLoggedIn == pMemberInfo2->bLoggedIn) {
		if (!pMemberInfo1->bLoggedIn)
			return 0;

		if (pMemberInfo1->dwState > pMemberInfo2->dwState)
			return -1;
		else if (pMemberInfo1->dwState == pMemberInfo2->dwState)
			return 0;
		else
			return 1;
	}
	else if (pMemberInfo1->bLoggedIn)
		return -1;
	else
		return 1;
}
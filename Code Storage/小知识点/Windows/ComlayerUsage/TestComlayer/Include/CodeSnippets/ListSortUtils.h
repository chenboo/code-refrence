#pragma once

#ifndef __LISTSORT_UTILS_H__
#define __LISTSORT_UTILS_H__

typedef struct ELC_LIST_SORT {
	int			nSortType; // this type is defined in the view which used
	BOOL		bAscending;
	DWORD_PTR	dwData;

	ELC_LIST_SORT() {
		nSortType = -1;
		bAscending = TRUE;
		dwData = 0;
	}
} *PELC_LIST_SORT;

// base sort functions
int CALLBACK SortByTextProc(LPARAM lpszText1, LPARAM lpszText2, LPARAM bAscending);
int CALLBACK SortByIntegerProc(LPARAM number1, LPARAM number2, LPARAM bAscending);
int CALLBACK SortByULONGLONG(ULONGLONG number1, ULONGLONG number2, LPARAM bAscending);
int CALLBACK SortByCharProc(LPARAM char1, LPARAM char2, LPARAM bAscending);

int CALLBACK SortByStatusProc(LPARAM lParam1, LPARAM lParam2, LPARAM bAscending);

#endif // __LISTSORT_UTILS_H__
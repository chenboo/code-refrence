// TimeoutHeap.h: interface for the CTimeoutHeap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEOUTHEAP_H__FB364A98_73E9_4050_A73E_DE5AF415CD1C__INCLUDED_)
#define AFX_TIMEOUTHEAP_H__FB364A98_73E9_4050_A73E_DE5AF415CD1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <limits.h>

typedef struct _HEAP_ENTRY {
	DWORD dwTimeout;
	DWORD dwParam1;
	DWORD dwParam2;
	DWORD dwParam3;
} HEAP_ENTRY, *PHEAP_ENTRY;

enum {
	TIMEOUT_HEAP_PARAM1 = 1,
	TIMEOUT_HEAP_PARAM2 = 2,
	TIMEOUT_HEAP_PARAM3 = 4
};

class LIBNET_API CTimeoutHeap  
{
public:
	enum { DEFAULT_HEAP_SIZE = 1024 };

public:
	CTimeoutHeap(
		BOOL *pbInitSuccess = NULL,
		int nStartSize = DEFAULT_HEAP_SIZE, 
		int nMaxSize = INT_MAX,
		int nIncrement = DEFAULT_HEAP_SIZE);
	virtual ~CTimeoutHeap();

	BOOL Create(
		int nStartSize = DEFAULT_HEAP_SIZE, 
		int nMaxSize = INT_MAX,
		int nIncrement = DEFAULT_HEAP_SIZE);
	void Destroy();

public:
	int	 GetCount() {
		return m_nFreeIndex - 1;
	}

	int	 GetSize() {
		return m_nHeapSize;
	}

public:
	BOOL Insert(DWORD dwTimeout, DWORD dwParam1, 
				DWORD dwParam2, DWORD dwParam3);
	BOOL Delete(DWORD dwParam1, DWORD dwParam2, 
				DWORD dwParam3, DWORD dwMask);

	BOOL Delete(DWORD dwParam1) {
		return Delete(dwParam1, 0, 0, TIMEOUT_HEAP_PARAM1);
	}

	BOOL Delete(DWORD dwParam1, DWORD dwParam2) {
		return Delete(dwParam1, dwParam2, 0, 
			TIMEOUT_HEAP_PARAM1 | TIMEOUT_HEAP_PARAM2);
	}

	BOOL Delete(DWORD dwParam1, DWORD dwParam2, DWORD dwParam3) {
		return Delete(dwParam1, dwParam2, dwParam3, 
			TIMEOUT_HEAP_PARAM1 | TIMEOUT_HEAP_PARAM2 | TIMEOUT_HEAP_PARAM3);
	}

	void DeleteAll() {
		m_nFreeIndex = 1;
	}

	BOOL PeekMin(DWORD *pdwTimeout, DWORD *pdwParam1, 
		DWORD *pdwParam2, DWORD *pdwParam3) {
		if (m_nFreeIndex <= 1)
			return FALSE;

		if (NULL != pdwTimeout)
			*pdwTimeout = m_ppHeapEntries[1]->dwTimeout;
		if (NULL != pdwParam1)
			*pdwParam1 = m_ppHeapEntries[1]->dwParam1;
		if (NULL != pdwParam2)
			*pdwParam2 = m_ppHeapEntries[1]->dwParam2;
		if (NULL != pdwParam3)
			*pdwParam3 = m_ppHeapEntries[1]->dwParam3;
		
		return TRUE;
	}

	BOOL ExtractMin(DWORD *pdwTimeout, DWORD *pdwParam1, 
		DWORD *pdwParam2, DWORD *pdwParam3) {
		PHEAP_ENTRY pEntry = Extract(1);
		if (NULL == pEntry)
			return FALSE;

		if (NULL != pdwTimeout)
			*pdwTimeout = pEntry->dwTimeout;
		if (NULL != pdwParam1)
			*pdwParam1 = pEntry->dwParam1;
		if (NULL != pdwParam2)
			*pdwParam2 = pEntry->dwParam2;
		if (NULL != pdwParam3)
			*pdwParam3 = pEntry->dwParam3;

		return TRUE;
	}

	int IncrementSize(int nIncrement);

protected:
	void SwapEntry(int nIndex1, int nIndex2) {
		PHEAP_ENTRY pTemp = m_ppHeapEntries[nIndex1];
		m_ppHeapEntries[nIndex1] = m_ppHeapEntries[nIndex2];
		m_ppHeapEntries[nIndex2] = pTemp;
	}

	void ForwardAdjustment(int nIndex) {
		while (nIndex > 1) {
			int nParent = nIndex >> 1;
			
			if (m_ppHeapEntries[nIndex]->dwTimeout < m_ppHeapEntries[nParent]->dwTimeout) {
				SwapEntry(nIndex, nParent);
				nIndex = nParent;
			}
			else
				break;
		}
	}

	void BackwardAdjustment(int nIndex) {
		while (nIndex < m_nFreeIndex) {
			int nSmallest = nIndex;
			int nLeftChild = nIndex * 2;
			int nRightChild = (nIndex * 2) + 1;

			if ((nLeftChild < m_nFreeIndex) && 
				(m_ppHeapEntries[nLeftChild]->dwTimeout < m_ppHeapEntries[nSmallest]->dwTimeout))
				nSmallest = nLeftChild;
			if ((nRightChild < m_nFreeIndex) && 
				(m_ppHeapEntries[nRightChild]->dwTimeout < m_ppHeapEntries[nSmallest]->dwTimeout))
				nSmallest = nRightChild;
			 
			if (nIndex == nSmallest)
				break;
			
			SwapEntry(nIndex, nSmallest);
			nIndex = nSmallest;
		}
	}

	PHEAP_ENTRY Extract(int nIndex);

protected:
	typedef std::list<PHEAP_ENTRY> CHeapAllocList;

	int				m_nIncrementSize;
	int				m_nMaxSize;
	int				m_nFreeIndex;
	int				m_nHeapSize;

	PHEAP_ENTRY *	m_ppHeapEntries;
	CHeapAllocList *m_pHeapBuffer;
};

#endif // !defined(AFX_TIMEOUTHEAP_H__FB364A98_73E9_4050_A73E_DE5AF415CD1C__INCLUDED_)

#ifndef __HEAP_H__
#define __HEAP_H__

#include <list>
#include <limits.h>

using namespace std;

template <class KEY, class T>
class CHeap
{
// Constants
public:
	enum {
		UNLIMITED_SIZE = INT_MAX,

		DEF_INITIAL_SIZE = 100,
		DEF_INCREMENT_SIZE = 50,
		DEF_MAX_SIZE = UNLIMITED_SIZE
	};

// Types
public:
	typedef struct _HEAP_ENTRY {
		int nIndex;
		KEY key;
		T	item;
	} HEAP_ENTRY, *PHEAP_ENTRY;

// Construction & destruction
public:
	CHeap(
		int nInitialSize = DEF_INITIAL_SIZE, 
		int nIncrementSize = DEF_INCREMENT_SIZE, 
		int nMaxSize = DEF_MAX_SIZE) {

		if (nInitialSize < 2)
			nInitialSize = 2;

		m_nIncrementSize = nIncrementSize;
		m_nMaxSize = nMaxSize;
		m_nHeapSize = 0;
		m_nFreeIndex = 1;
		m_nInactiveIndex = m_nHeapSize;

		m_ppHeapEntries = NULL;
		IncrementSize(nInitialSize);
	}

	virtual ~CHeap() {
		FreeEntries();
	}

// Attributes
public:
	int GetFirstIndex() {
		return 1;
	}

	int GetLastIndex() {
		return m_nFreeIndex - 1;
	}

	int GetCount() {
		return m_nFreeIndex - 1;
	}

	int GetSize() {
		return m_nHeapSize;
	}

// Operations
public:
	PHEAP_ENTRY Add(KEY key) {
		if (m_nFreeIndex >= m_nHeapSize) {
			if (IncrementSize(m_nIncrementSize) <= 0)
				return NULL;
		}

		PHEAP_ENTRY pEntry = m_ppHeapEntries[m_nFreeIndex];
		pEntry->nIndex = m_nFreeIndex;
		pEntry->key = key;

		ForwardAdjustment(m_nFreeIndex);
		m_nFreeIndex++;
		return pEntry;
	}

	PHEAP_ENTRY GetMin() {
		return GetItem(1);
	}

	PHEAP_ENTRY PeekMin() {
		return PeekItem(1);
	}

	PHEAP_ENTRY GetItem(int nIndex) {
		if ((nIndex >= m_nFreeIndex) || (nIndex < 1))
			return NULL;

		PHEAP_ENTRY pEntry = m_ppHeapEntries[nIndex];
		m_ppHeapEntries[nIndex] = m_ppHeapEntries[m_nFreeIndex - 1];
		m_ppHeapEntries[nIndex]->nIndex = nIndex;
		m_nFreeIndex--;

		m_ppHeapEntries[m_nFreeIndex] = pEntry;

		BackwardAdjustment(nIndex);
		return pEntry;
	}

	PHEAP_ENTRY PeekItem(int nIndex) {
		if ((nIndex >= m_nFreeIndex) || (nIndex < 1))
			return NULL;

		return m_ppHeapEntries[nIndex];
	}

	bool SetKey(int nIndex, KEY keyNew) {
		if ((nIndex >= m_nFreeIndex) || (nIndex < 1))
			return false;

		PHEAP_ENTRY pEntry = m_ppHeapEntries[nIndex];
		if (keyNew > pEntry->key) {
			pEntry->key = keyNew;
			BackwardAdjustment(nIndex);
		}
		else if (keyNew < pEntry->key) {
			pEntry->key = keyNew;
			ForwardAdjustment(nIndex);
		}

		return true;
	}

	bool DeleteItem(int nIndex) {
		return (GetItem(nIndex) != NULL);
	}

	void Clear() {
		m_nFreeIndex = 1;
	}

// Implementation
protected:
	typedef list<PHEAP_ENTRY> CHeapAllocList;

	int IncrementSize(int nIncrement) {
		nIncrement = min(nIncrement, m_nMaxSize - m_nHeapSize);
		if (nIncrement <= 0)
			return 0;

		PHEAP_ENTRY pHeapEntriesAlloc = NULL;
		PHEAP_ENTRY *ppHeapEntries = NULL;

		do {
			PHEAP_ENTRY pHeapEntriesAlloc = new HEAP_ENTRY[nIncrement];
			if (NULL == pHeapEntriesAlloc)
				break;

			PHEAP_ENTRY *ppHeapEntries = new PHEAP_ENTRY[nIncrement + m_nHeapSize];
			if (NULL == ppHeapEntries)
				break;

			if (NULL != m_ppHeapEntries) {
				memcpy(ppHeapEntries, m_ppHeapEntries, sizeof(PHEAP_ENTRY) * m_nInactiveIndex);
				memcpy(ppHeapEntries, m_ppHeapEntries + m_nInactiveIndex, sizeof(PHEAP_ENTRY) * (m_nHeapSize - m_nInactiveIndex));
			}
			for (int i = 0; i < nIncrement; i++) {
				ppHeapEntries[i + m_nHeapSize] = pHeapEntriesAlloc + i;
			}
			delete []m_ppHeapEntries;
			m_ppHeapEntries = ppHeapEntries;
			
			m_listAlloc.push_back(pHeapEntriesAlloc);
			m_nHeapSize += nIncrement;

			return nIncrement;
		} while (0);
		
		if (NULL != pHeapEntriesAlloc) {
			delete []pHeapEntriesAlloc;
		}
		if (NULL != ppHeapEntries) {
			delete []ppHeapEntries;
		}
		return 0;
	}

	void SwapEntry(int nIndex1, int nIndex2) {
		PHEAP_ENTRY pTemp = m_ppHeapEntries[nIndex1];
		m_ppHeapEntries[nIndex1] = m_ppHeapEntries[nIndex2];
		m_ppHeapEntries[nIndex2] = pTemp;

		m_ppHeapEntries[nIndex1]->nIndex = nIndex1;
		m_ppHeapEntries[nIndex2]->nIndex = nIndex2;
	}

	void ForwardAdjustment(int nIndex) {
		while (nIndex > 1) {
			int nParent = nIndex >> 1;
			
			if (m_ppHeapEntries[nIndex]->key < m_ppHeapEntries[nParent]->key) {
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
				(m_ppHeapEntries[nLeftChild]->key < m_ppHeapEntries[nSmallest]->key))
				nSmallest = nLeftChild;
			if ((nRightChild < m_nFreeIndex) && 
				(m_ppHeapEntries[nRightChild]->key < m_ppHeapEntries[nSmallest]->key))
				nSmallest = nRightChild;
			 
			if (nIndex == nSmallest)
				break;
			
			SwapEntry(nIndex, nSmallest);
			nIndex = nSmallest;
		}
	}

	void FreeEntries() {
		Clear();

		CHeapAllocList::iterator i;
		for (i = m_listAlloc.begin(); i != m_listAlloc.end(); i++) {
			PHEAP_ENTRY pEntries = *i;
			delete []pEntries;
		}
		m_listAlloc.clear();

		if (NULL != m_ppHeapEntries) {
			delete []m_ppHeapEntries;
			m_ppHeapEntries = NULL;
		}

		m_nHeapSize = 0;
		m_nFreeIndex = 1;
		m_nInactiveIndex = m_nHeapSize;
	}

	int		m_nIncrementSize;
	int		m_nMaxSize;
	int		m_nFreeIndex;
	int		m_nHeapSize;
	int		m_nInactiveIndex;

	PHEAP_ENTRY *		m_ppHeapEntries;
	CHeapAllocList		m_listAlloc;
};

#endif //__HEAP_H__
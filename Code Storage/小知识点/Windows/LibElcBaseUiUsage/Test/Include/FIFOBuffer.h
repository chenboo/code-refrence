#pragma once

#include <list>

class CFIFOBuffer
{
protected:
	CFIFOBuffer() : m_pbMem(NULL) {}

public:
	virtual ~CFIFOBuffer() {
		Deinitialize();
	}

public:
	BOOL Initialize(int cbMaxBlock, int cMaxBlocks) {
		m_pbMem = (LPBYTE)malloc(cbMaxBlock * cMaxBlocks);
		if (m_pbMem == NULL)
			return FALSE;

		memset(m_pbMem, 0, cbMaxBlock * cMaxBlocks);

		LPBYTE pbBlock = m_pbMem;
		for (int iBlock = 0; iBlock < cMaxBlocks; iBlock++) {
			BUFFER_ITEM item;
			item.pvBlock = (LPVOID)pbBlock;
			item.cbBlock = cbMaxBlock;
			m_listFreeBlocks.push_back(item);
			pbBlock += cbMaxBlock;
		}

		InitializeCriticalSection(&m_cs);
		return TRUE;
	}

	void Deinitialize() {
		if (m_pbMem) {
			free(m_pbMem);
			m_pbMem = NULL;
		}

		DeleteCriticalSection(&m_cs);
	}

public:
	static CFIFOBuffer& GetInstance() {
		static CFIFOBuffer instance;
		return instance;
	}

	bool IsEmpty() {
		return m_listInUseBlocks.empty();
	}

	void PutFreeBlock(LPVOID pvBlock) {
		BUFFER_ITEM item;
		item.pvBlock = pvBlock;

		m_listFreeBlocks.push_back(item);
	}

	LPVOID GetFreeBlock() {
		if (m_listFreeBlocks.empty())
			return NULL;

		BUFFER_ITEM& item = m_listFreeBlocks.front();
		LPVOID pvBlock = item.pvBlock;
		m_listFreeBlocks.pop_front();

		return pvBlock;
	}

	void PutBlock(LPVOID pvBlock, int cbBlock) {
		BUFFER_ITEM item;
		item.pvBlock = pvBlock;
		item.cbBlock = cbBlock;

		m_listInUseBlocks.push_back(item);
	}

	LPVOID GetBlock() {
		if (m_listInUseBlocks.empty())
			return NULL;

		BUFFER_ITEM& item = m_listInUseBlocks.front();
		LPVOID pvBlock = item.pvBlock;
		m_listInUseBlocks.pop_front();

		return pvBlock;
	}

	CRITICAL_SECTION& GetLock() {
		return m_cs;
	}

protected:
	CRITICAL_SECTION	m_cs;

	LPBYTE m_pbMem;
	
	struct BUFFER_ITEM {
		BUFFER_ITEM() {
		}

		LPVOID pvBlock;
		int	   cbBlock;
	};

	std::list<BUFFER_ITEM> m_listInUseBlocks;
	std::list<BUFFER_ITEM> m_listFreeBlocks;
};
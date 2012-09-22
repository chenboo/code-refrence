#pragma once 

#include <list>


class CElcDynamicBuffer
{
public:
	CElcDynamicBuffer() {
		m_cMaxBlocks = 0;
		m_cBlocks = 0;
		InitializeCriticalSection(&m_cs);
	}
	virtual ~CElcDynamicBuffer() {
		Deinitialize();
		DeleteCriticalSection(&m_cs);
	}

	BOOL Initialize(int cbBlock, int cMaxBlocks) {
		BUFFER_ITEM item;
		item.pvBlock = (LPBYTE)malloc(cbBlock);
		item.cbBlock = cbBlock;

		if (item.pvBlock == NULL)
			return FALSE;

		m_listFreeBlocks.push_back(item);

		m_cMaxBlocks = cMaxBlocks;
		m_cBlocks = 1;

		return TRUE;
	}
	BOOL Initialize(int cMaxBlocks) {
		m_cMaxBlocks = cMaxBlocks;
		m_cBlocks = 0;
		return TRUE;
	}
	void Deinitialize() {
		while (!m_listFreeBlocks.empty()) {
			BUFFER_ITEM& item = m_listFreeBlocks.front();
			free(item.pvBlock);
			m_listFreeBlocks.pop_front();
		}

		while (!m_listInUseBlocks.empty()) {
			BUFFER_ITEM& item = m_listInUseBlocks.front();
			free(item.pvBlock);
			m_listInUseBlocks.pop_front();
		}

		m_cMaxBlocks = 0;
		m_cBlocks = 0;
	}

	void ResetBuffer() {
		while (!m_listFreeBlocks.empty()) {
			BUFFER_ITEM& item = m_listFreeBlocks.front();
			free(item.pvBlock);
			m_listFreeBlocks.pop_front();
		}

		while (!m_listInUseBlocks.empty()) {
			BUFFER_ITEM& item = m_listInUseBlocks.front();
			free(item.pvBlock);
			m_listInUseBlocks.pop_front();
		}
		m_cBlocks = 0;
	}

	CRITICAL_SECTION& GetLock() {
		return m_cs;
	}

public:
	LPVOID GetFreeBlock(int cbNeed, int& cbActual) {
		if (!m_listFreeBlocks.empty()) {
			BUFFER_ITEM item = m_listFreeBlocks.front();
			m_listFreeBlocks.pop_front();

			if (m_cBlocks > m_cMaxBlocks * 3 / 4) {
				_TRACE(_T("buffering high, m_cBlocks: %d, m_cMaxBlocks:%d\n"), m_cBlocks, m_cMaxBlocks);
			}

			if (item.cbBlock >= cbNeed) {
				cbActual = item.cbBlock;
				return item.pvBlock;
			}
			else {
				free(item.pvBlock);
				item.pvBlock = (LPBYTE)malloc(cbNeed);
				if (item.pvBlock == NULL) {
					item.cbBlock = 0;
					return NULL;
				}

				cbActual = cbNeed;
				item.cbBlock = cbNeed;
				return item.pvBlock;
			}
		}
		else {
			if (m_cBlocks >= m_cMaxBlocks) {
				_TRACE(_T("No free buffers, max buffer counts=%d\n"), m_cMaxBlocks);
				return NULL;
			}

			BUFFER_ITEM item;
			item.pvBlock = (LPBYTE)malloc(cbNeed);
			item.cbBlock = cbNeed;

			if (item.pvBlock == NULL)
				return NULL;

			cbActual = cbNeed;
			m_cBlocks ++;

			_TRACE(_T("malloc a new buffer, m_cBlocks:%d\n"), m_cBlocks);
			return item.pvBlock;
		}
	}

	void PutFreeBlock(LPVOID pvBlock, int cbBlock) {
		BUFFER_ITEM item;
		item.pvBlock = pvBlock;
		item.cbBlock = cbBlock;

		m_listFreeBlocks.push_back(item);
	}

	LPVOID GetBlock(int& cbBlock) {
		if (m_listInUseBlocks.empty())
			return NULL;

		BUFFER_ITEM& item = m_listInUseBlocks.front();
		LPVOID pvBlock = item.pvBlock;
		cbBlock = item.cbBlock;
		m_listInUseBlocks.pop_front();

		return pvBlock;
	}
	void PutBlock(LPVOID pvBlock, int cbBlock) {
		BUFFER_ITEM item;
		item.pvBlock = pvBlock;
		item.cbBlock = cbBlock;

		m_listInUseBlocks.push_back(item);
	}

	BOOL IsEmpty() {
		return m_listInUseBlocks.empty();
	}

protected:
	typedef struct BUFFER_ITEM {
		LPVOID pvBlock;
		int	   cbBlock;
	}*PBUFFER_ITEM;

	std::list<BUFFER_ITEM>	m_listInUseBlocks;
	std::list<BUFFER_ITEM>	m_listFreeBlocks;

	int						m_cMaxBlocks;
	int						m_cBlocks;

	CRITICAL_SECTION		m_cs;
};
#include "stdafx.h"
#include "PacketHandler.h"

CPacketHandler::CPacketHandler(ICommunicationLayer* pComLayer) 
{
	m_pComLayer = pComLayer;

	m_nRef = 1;
}

CPacketHandler::~CPacketHandler()
{
}

LONG CPacketHandler::AddRef() 
{
	return InterlockedIncrement(&m_nRef);
}

LONG CPacketHandler::Release() 
{
	LONG nRet = InterlockedDecrement(&m_nRef);
	if (nRet <= 0)
		delete this;

	return nRet;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CPacketHandlerManager::CPacketHandlerManager(void)
{
	InitializeCriticalSection(&m_csPacket);

	m_nRef = 1;
}

CPacketHandlerManager::~CPacketHandlerManager(void)
{
	DeleteAllPacketHandler();

	DeleteCriticalSection(&m_csPacket);
}

LONG CPacketHandlerManager::AddRef()
{
	return InterlockedIncrement(&m_nRef);
}

LONG CPacketHandlerManager::Release()
{
	LONG nRet = InterlockedDecrement(&m_nRef);
	if (nRet <= 0)
		delete this;

	return nRet;
}

BOOL CPacketHandlerManager::AddPacketHandler(CPacketHandler* pPacketHandler)
{
	CAutoSectionLock lock(&m_csPacket);

	DWORD dwMagic = pPacketHandler->GetPacketMagic();
	CPacketHandler* pTempHandler = FindPacketHandler(dwMagic);
	if (pTempHandler) {
		pTempHandler->Release();
		return FALSE;
	}

	pPacketHandler->AddRef();
	m_mapPacketHandler[dwMagic] = pPacketHandler;
	return TRUE;
}

void CPacketHandlerManager::DeletePacketHandler(DWORD dwMagic)
{
	CAutoSectionLock lock(&m_csPacket);

	CPacketHandlerMap::iterator i = m_mapPacketHandler.find(dwMagic);
	if (m_mapPacketHandler.end() == i)
		return;

	(*i).second->Release();
	m_mapPacketHandler.erase(i);
}

CPacketHandler* CPacketHandlerManager::FindPacketHandler(DWORD dwMagic)
{
	CAutoSectionLock lock(&m_csPacket);

	CPacketHandlerMap::iterator i = m_mapPacketHandler.find(dwMagic);
	if (m_mapPacketHandler.end() == i)
		return NULL;

	(*i).second->AddRef();
	return (*i).second;
}

void CPacketHandlerManager::DeleteAllPacketHandler()
{
	CAutoSectionLock lock(&m_csPacket);

	CPacketHandlerMap::iterator iterator;

	iterator = m_mapPacketHandler.begin();
	while (iterator != m_mapPacketHandler.end()) {
		(*iterator).second->Release();
		m_mapPacketHandler.erase(iterator);

		iterator = m_mapPacketHandler.begin();
	}
}
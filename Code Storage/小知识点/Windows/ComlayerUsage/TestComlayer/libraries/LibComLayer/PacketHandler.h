#pragma once

class CPacketHandlerManager : IPacketHandlerManager
{
public:
	CPacketHandlerManager(void);
private:
	virtual ~CPacketHandlerManager(void);

public:
	virtual LONG AddRef();
	virtual LONG Release();

	virtual BOOL AddPacketHandler(CPacketHandler* pPacketHandler);
	virtual void DeletePacketHandler(DWORD dwMagic);

	CPacketHandler* FindPacketHandler(DWORD dwMagic);

public:
	typedef std::map<DWORD, CPacketHandler*>	CPacketHandlerMap;

	CPacketHandlerMap m_mapPacketHandler;

	CRITICAL_SECTION m_csPacket;

	LONG			m_nRef;

	void DeleteAllPacketHandler();
};


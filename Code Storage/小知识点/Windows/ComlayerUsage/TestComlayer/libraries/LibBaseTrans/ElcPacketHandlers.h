#pragma once

class LIBBASETRANS_API CTeacherExitPacketHandler : public CPacketHandler
{
public:
	CTeacherExitPacketHandler(
		ICommunicationLayer* pComLayer) : 
	CPacketHandler(pComLayer) {}
	virtual ~CTeacherExitPacketHandler() {}
public:
	void SetClientId(CLIENTID id);

	virtual DWORD GetPacketMagic();
	virtual void ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize);

private:
	CLIENTID	m_id;
};

class LIBBASETRANS_API CCommandPacketHandler : public CPacketHandler
{
public:
	CCommandPacketHandler(
		ICommunicationLayer* pComLayer) : 
	CPacketHandler(pComLayer) {
	}
	virtual ~CCommandPacketHandler() {}
public:
	virtual DWORD GetPacketMagic();
	virtual void ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize);
};

class LIBBASETRANS_API CLiteCommandPacketHandler : public CPacketHandler
{
public:
	CLiteCommandPacketHandler(
		ICommunicationLayer* pComLayer) : 
	CPacketHandler(pComLayer) {
	}
	virtual ~CLiteCommandPacketHandler() {}
public:
	virtual DWORD GetPacketMagic();
	virtual void ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize);
protected:
	void ReportCommandArrivedEvent(BYTE* pabCommand, DWORD cbCommand);
};

class LIBBASETRANS_API CBlockPacketHandler : public CPacketHandler
{
public:
	CBlockPacketHandler(
		ICommunicationLayer* pComLayer) : 
	CPacketHandler(pComLayer) {
		m_dwLocalInterface = 0;
	}
	virtual ~CBlockPacketHandler() {}
public:
	virtual DWORD GetPacketMagic();
	virtual void ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize);

public:
	void SetLocalInterface(DWORD dwLocalInterface) {m_dwLocalInterface = dwLocalInterface;}

protected:
	DWORD m_dwLocalInterface;
};

// Quiz 

class LIBBASETRANS_API CQuizCommandPacketHandler : public CPacketHandler
{
public:
	CQuizCommandPacketHandler(
		ICommunicationLayer* pComLayer) : 
	CPacketHandler(pComLayer) {
	}
	virtual ~CQuizCommandPacketHandler() {}
public:
	virtual DWORD GetPacketMagic();
	virtual void ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize);
};

class LIBBASETRANS_API CQuizBlockPacketHandler : public CPacketHandler
{
public:
	CQuizBlockPacketHandler(
		ICommunicationLayer* pComLayer) : 
	CPacketHandler(pComLayer) {
		m_dwLocalInterface = 0;
	}
	virtual ~CQuizBlockPacketHandler() {}
public:
	virtual DWORD GetPacketMagic();
	virtual void ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize);

public:
	void SetLocalInterface(DWORD dwLocalInterface) {m_dwLocalInterface = dwLocalInterface;}

protected:
	DWORD m_dwLocalInterface;
};

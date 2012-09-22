#include "StdAfx.h"
#include "ElcPacketHandlers.h"
#include "ElcTransactions.h"

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void CTeacherExitPacketHandler::SetClientId(CLIENTID id)
{
	memcpy(&m_id, &id, sizeof(CLIENTID));
}

DWORD CTeacherExitPacketHandler::GetPacketMagic()
{
	return MAGIC_TEACHER_EXIT;
}

void CTeacherExitPacketHandler::ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize)
{
	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpcBuffer;
	if (pPacket->dwMagic != MAGIC_TEACHER_EXIT)
		return;

	PPACKET_TEACHER_EXIT pTeacherExit = (PPACKET_TEACHER_EXIT)(pPacket + 1);

	// Check if need reply
	int i=0;
	for (i=0; i<pTeacherExit->nCount; i++) {
		if (memcmp(&pTeacherExit->id[i], &m_id, sizeof(CLIENTID)) == 0)
			break;
	}

	if (i >= pTeacherExit->nCount)
		return;

	// report event
	{
		IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

		DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_TEACHER_EXIT);
		IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);

		PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
		PEVENT_TEACHER_EXIT pEvent = (PEVENT_TEACHER_EXIT)(pHeader + 1);

		pHeader->dwEvent = EV_TEACHER_EXIT;
		pHeader->dwLength = sizeof(EVENT_TEACHER_EXIT);

		pEvent->dwTeacherIp = pTeacherExit->dwTeacherIp;

		pEventMgr->PutPendingEvent(pItem);
		pEventMgr->Release();
	}

	// return exit
	{
		IBufferItem *pItem = m_pComLayer->AllocBufferItem();
		if (!pItem) 
			return;

		pItem->SetTransactionID(&pPacket->guidTransaction);
		pItem->SetDstIpAddress(CIpAddress(pTeacherExit->dwTeacherIp, m_pComLayer->GetComPort()));

		PLANGLAB_PACKET pHeader = (PLANGLAB_PACKET)pItem->GetData();
		PPACKET_TEACHER_EXIT_RET pReturn = (PPACKET_TEACHER_EXIT_RET)(pHeader + 1);

		pHeader->dwMagic = MAGIC_TEACHER_EXIT_RET;
		pHeader->dwVersion = LANGLAB_VERSION;
		pHeader->dwLength = sizeof(PACKET_TEACHER_EXIT_RET);
		memcpy(&pHeader->guidTransaction, &pPacket->guidTransaction, sizeof(GUID));

		memcpy(&pReturn->id, &m_id, sizeof(CLIENTID));
		pReturn->dwClientIp = m_pComLayer->GetLocalAddress();

		pItem->SetLength(sizeof(LANGLAB_PACKET) + pHeader->dwLength);
		m_pComLayer->AddToSendBuffer(pItem);
	}
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
DWORD CCommandPacketHandler::GetPacketMagic()
{
	return MAGIC_COMMAND;
}

void CCommandPacketHandler::ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize)
{
	if (cbSize < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpcBuffer;
	if (pPacket->dwMagic != MAGIC_COMMAND)
		return;

	if (cbSize < sizeof(LANGLAB_PACKET) + sizeof(PACKET_COMMAND))
		return;

	PPACKET_COMMAND pPacketCommand = (PPACKET_COMMAND)(pPacket + 1);
	
	if (cbSize < sizeof(LANGLAB_PACKET) + sizeof(PACKET_COMMAND) + pPacketCommand->cbCmdSize)
		return;

	CPassiveCommandTransaction* pTransaction = 
		new CPassiveCommandTransaction(m_pComLayer, &pPacket->guidTransaction);
	if (!pTransaction)
		return;

	pTransaction->ProcesssCommand(pPacketCommand);
	pTransaction->Release();
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
DWORD CLiteCommandPacketHandler::GetPacketMagic()
{
	return MAGIC_LITE_COMMAND;
}

void CLiteCommandPacketHandler::ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize)
{
	if (cbSize < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpcBuffer;
	if (pPacket->dwMagic != MAGIC_LITE_COMMAND)
		return;

	if (cbSize < sizeof(LANGLAB_PACKET) + sizeof(PACKET_LITE_COMMAND))
		return;

	PPACKET_LITE_COMMAND pPacketLiteCommand = (PPACKET_LITE_COMMAND)(pPacket + 1);
	
	if (cbSize < sizeof(LANGLAB_PACKET) + sizeof(PACKET_LITE_COMMAND) + pPacketLiteCommand->cbCmdSize)
		return;

	// report event
	ReportCommandArrivedEvent(pPacketLiteCommand->abCmdBuffer, pPacketLiteCommand->cbCmdSize);
}

void CLiteCommandPacketHandler::ReportCommandArrivedEvent(BYTE* pabCommand, DWORD cbCommand)
{
	IEventItemManager* pEventMgr = m_pComLayer->GetEventManager();

	DWORD cbEvent = sizeof(EVENT_HEADER) + sizeof(EVENT_COMMAND_ARRIVED) + cbCommand;
	cbEvent += cbCommand;

	IEventItem* pItem = pEventMgr->GetFreeEvent(cbEvent);
	if (!pItem)
		return;

	PEVENT_HEADER pHeader = (PEVENT_HEADER)pItem->GetData();
	pHeader->dwEvent = EV_COMMAND_ARRIVED;
	pHeader->dwLength = sizeof(EVENT_COMMAND_ARRIVED) + cbCommand;

	PEVENT_COMMAND_ARRIVED pEvent = (PEVENT_COMMAND_ARRIVED)(pHeader + 1);
	pEvent->cbCmdSize = cbCommand;
	memcpy(pEvent->abCmdBuffer, pabCommand, cbCommand);

	pEventMgr->PutPendingEvent(pItem);
	pEventMgr->Release();
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
DWORD CQuizCommandPacketHandler::GetPacketMagic()
{
	return MAGIC_QUIZ_COMMAND;
}

void CQuizCommandPacketHandler::ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize)
{
	if (cbSize < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpcBuffer;
	if (pPacket->dwMagic != MAGIC_QUIZ_COMMAND)
		return;

	if (cbSize < sizeof(LANGLAB_PACKET) + sizeof(PACKET_QUIZ_COMMAND))
		return;

	PPACKET_QUIZ_COMMAND pPacketCommand = (PPACKET_QUIZ_COMMAND)(pPacket + 1);
	
	if (cbSize < sizeof(LANGLAB_PACKET) + sizeof(PACKET_QUIZ_COMMAND) + pPacketCommand->cbCmdSize)
		return;

	// Command not for me
	if (pPacketCommand->dwReceiverIp != m_pComLayer->GetLocalAddress())
		return;

	CQuizPassiveCommandTransaction* pTransaction = 
		new CQuizPassiveCommandTransaction(m_pComLayer, &pPacket->guidTransaction);
	if (!pTransaction)
		return;

//	pTransaction->AddToTransactionManager();
	pTransaction->ProcessCommand(pPacketCommand);
	pTransaction->Release();
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
DWORD CQuizBlockPacketHandler::GetPacketMagic()
{
	return MAGIC_QUIZ_BLOCK;
}

void CQuizBlockPacketHandler::ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize)
{
	if (cbSize < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpcBuffer;
	if (pPacket->dwMagic != MAGIC_QUIZ_BLOCK)
		return;

	if (cbSize < sizeof(LANGLAB_PACKET) + sizeof(PACKET_QUIZ_BLOCK))
		return;

	PPACKET_QUIZ_BLOCK pPacketBlock = (PPACKET_QUIZ_BLOCK)(pPacket + 1);
	
	if (cbSize < sizeof(LANGLAB_PACKET) + sizeof(PACKET_QUIZ_BLOCK) + pPacketBlock->cbSize)
		return;

	if (pPacketBlock->dwReceiverIp != m_pComLayer->GetLocalAddress())
		return;

	CQuizBlockRecverTransaction* pTransaction = 
		new CQuizBlockRecverTransaction(m_pComLayer, &pPacket->guidTransaction);
	if (!pTransaction)
		return;

//	pTransaction->AddToTransactionManager();
	pTransaction->ProcessBlock(pPacketBlock);
	pTransaction->Release();
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
DWORD CBlockPacketHandler::GetPacketMagic()
{
	return MAGIC_BLOCK;
}

void CBlockPacketHandler::ProcessPacket(LPCVOID lpcBuffer, DWORD cbSize)
{
	if (cbSize < sizeof(LANGLAB_PACKET))
		return;

	PLANGLAB_PACKET pPacket = (PLANGLAB_PACKET)lpcBuffer;
	if (pPacket->dwMagic != MAGIC_BLOCK)
		return;

	if (cbSize < sizeof(LANGLAB_PACKET) + sizeof(PACKET_BLOCK))
		return;

	PPACKET_BLOCK pPacketBlock = (PPACKET_BLOCK)(pPacket + 1);
	
	if (cbSize < sizeof(LANGLAB_PACKET) + sizeof(PACKET_BLOCK) + pPacketBlock->cbSize)
		return;

	if (pPacketBlock->dwReceiverIp != m_pComLayer->GetLocalAddress())
		return;

	CBlockRecverTransaction* pTransaction = 
		new CBlockRecverTransaction(m_pComLayer, &pPacket->guidTransaction);
	if (!pTransaction)
		return;

	pTransaction->ProcessBlock(pPacketBlock);
	pTransaction->Release();
}
// LibComLayer.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "LibComLayer.h"
#include "langlabevent.h"
#include "CommunicationLayer.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

LIBCOMLAYER_API IEventItemManager* CreateEventManager(DWORD cbPoolSize)
{
	CEventManager* pEventMgr = new CEventManager(cbPoolSize);
	return (IEventItemManager *)pEventMgr;
}

LIBCOMLAYER_API ICommunicationLayer* CreateCommunicationLayer(
	IEventItemManager* pEventManager)
{
	CCommunicationLayer* pComLayer = 
		new CCommunicationLayer(pEventManager);
	return (ICommunicationLayer *)pComLayer;
}

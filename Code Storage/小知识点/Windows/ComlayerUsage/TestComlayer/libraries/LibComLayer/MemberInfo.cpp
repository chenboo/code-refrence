#include "stdafx.h"
#include <nb30.h>
#include <Iphlpapi.h>
#include "MemberInfo.h"

typedef struct _ASTAT_
{
    ADAPTER_STATUS adapt;
    NAME_BUFFER    NameBuff [30];
}ASTAT, * PASTAT;

typedef DWORD (WINAPI *PGETADAPTERSINFO)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);

CMemberInfo::CMemberInfo(void)
{
}

CMemberInfo::~CMemberInfo(void)
{
}

string CMemberInfo::GetKey()
{
	return GetMac();
}

string CMemberInfo::GetComputerName()
{
	char szName[MAX_COMPUTERNAME_LENGTH + 1 ];
	DWORD cbSize = sizeof(szName);
	::GetComputerName(szName, &cbSize);

	m_strValue = szName;
	return m_strValue;
}

string CMemberInfo::GetMac()
{
	BYTE abMacAddr[6];
	memset(abMacAddr, 0, 6);
	if (!GetMacAddress(abMacAddr)) {
		m_strValue = "XXXXXXXXXXXX";
		return m_strValue;
	}

	char szMac[13];
	sprintf(szMac, "%02X%02X%02X%02X%02X%02X", abMacAddr[0], abMacAddr[1], 
		abMacAddr[2], abMacAddr[3], abMacAddr[4], abMacAddr[5]);
	m_strValue = szMac;
	return m_strValue;
}

DWORD CMemberInfo::GetWksID()
{
	return 0;
}

DWORD CMemberInfo::GetReserved()
{
	return 0;
}


BOOL CMemberInfo::GetMacAddress(LPBYTE lpMacAddress)
{
	try 
	{
		HINSTANCE hInstance = LoadLibrary("iphlpapi.dll");
		if (NULL == hInstance)
			throw FALSE;

		PGETADAPTERSINFO lpfnGetAdapterInfo = (PGETADAPTERSINFO)GetProcAddress(hInstance, "GetAdaptersInfo");
		if (lpfnGetAdapterInfo == NULL) {
			FreeLibrary(hInstance);
			throw FALSE;
		}

		IP_ADAPTER_INFO AdapterInfo[16];			// Allocate information for up to 16 NICs
		DWORD dwBufLen = sizeof(AdapterInfo);		// Save the memory size of buffer

		DWORD dwStatus = lpfnGetAdapterInfo(		// Call GetAdapterInfo
			AdapterInfo,							// [out] buffer to receive data
			&dwBufLen);								// [in] size of receive data buffer
		if (dwStatus != ERROR_SUCCESS) {
			FreeLibrary(hInstance);
			throw FALSE;
		}

		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
		memcpy(lpMacAddress, pAdapterInfo->Address, 6);
		FreeLibrary(hInstance);

		return TRUE;
	} catch (...)
	{
	}

	return NetBiosGetMacAddress(lpMacAddress);
}

BOOL CMemberInfo::NetBiosGetMacAddress(LPBYTE lpMacAddress)
{
	ASTAT Adapter;

	NCB Ncb;
	UCHAR uRetCode;
	LANA_ENUM   lenum;
	int      i;

	memset( &Ncb, 0, sizeof(Ncb) );
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR *)&lenum;
	Ncb.ncb_length = sizeof(lenum);
	uRetCode = Netbios( &Ncb );
	if (0 != uRetCode)
		return FALSE;

	for(i=0; i < lenum.length ;i++) {
		memset( &Ncb, 0, sizeof(Ncb) );
		Ncb.ncb_command = NCBRESET;
		Ncb.ncb_lana_num = lenum.lana[i];

		uRetCode = Netbios( &Ncb );
		if (0 != uRetCode)
			continue;

		memset( &Ncb, 0, sizeof (Ncb) );
		Ncb.ncb_command = NCBASTAT;
		Ncb.ncb_lana_num = lenum.lana[i];

		strcpy((char *)Ncb.ncb_callname,  "*               ");
		Ncb.ncb_buffer = (BYTE *)&Adapter;
		Ncb.ncb_length = sizeof(Adapter);

		uRetCode = Netbios( &Ncb );
		if (0 != uRetCode)
			continue;

		if ( uRetCode == 0 ) {
			BYTE abPPPAdapter[] = { 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 };
			BYTE abEmptyAdatper[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
			if ((memcmp(abPPPAdapter, Adapter.adapt.adapter_address, 6) != 0) &&
				(memcmp(abEmptyAdatper, Adapter.adapt.adapter_address, 6) != 0)) {
				memcpy(lpMacAddress, Adapter.adapt.adapter_address, 6);
				return TRUE;
			}
		}
	}

	return FALSE;
}
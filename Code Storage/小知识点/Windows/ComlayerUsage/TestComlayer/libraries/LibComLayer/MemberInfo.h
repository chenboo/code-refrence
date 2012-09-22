#ifndef __MEMBERINFO_H__
#define __MEMBERINFO_H__

#include <string>

using namespace std;

class CMemberInfo
{
public:
	CMemberInfo(void);
	virtual ~CMemberInfo(void);

public:
	virtual string GetKey();
	virtual string GetComputerName();
	virtual string GetMac();
	virtual DWORD  GetWksID();
	virtual DWORD  GetReserved();

private:
	BOOL NetBiosGetMacAddress(LPBYTE lpMacAddress);
	BOOL GetMacAddress(LPBYTE lpMacAddress);

	string m_strValue;
};

#endif //__MEMBERINFO_H__
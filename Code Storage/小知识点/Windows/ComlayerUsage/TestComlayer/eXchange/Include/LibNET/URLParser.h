// URLParser.h: interface for the CURLParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_URLPARSER_H__1C429083_9996_45FE_A019_0FB3A6CB5009__INCLUDED_)
#define AFX_URLPARSER_H__1C429083_9996_45FE_A019_0FB3A6CB5009__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_PROTOCOL_LEN	32
#define MAX_HOST_LEN		256
#define MAX_PORT_LEN		8
#define MAX_URL_PATH_LEN	512

#define MAX_URL_LEN			(MAX_PROTOCOL_LEN + MAX_HOST_LEN + \
							 MAX_PORT_LEN + MAX_URL_PATH_LEN)

class LIBNET_API CURLParser  
{
// Construction & Destruction
public:
	CURLParser();

// Operations
public:
	BOOL SetURL(LPCTSTR lpszURL);
	BOOL GetURL(LPTSTR lpszURL);

// Attributes
public:
	BOOL	IsValid();

	LPCTSTR GetProtocol();
	LPCTSTR GetHostName();
	WORD	GetPort();
	LPCTSTR GetURLPath();

	void	SetProtocol(LPCTSTR lpszProtocol);
	void	SetHostName(LPCTSTR lpszHostName);
	void	SetPort(WORD wPort);
	void	SetURLPath(LPCTSTR lpszURLPath);

protected:
	TCHAR	m_szProtocol[MAX_PROTOCOL_LEN];
	TCHAR	m_szHost[MAX_HOST_LEN];
	WORD	m_wPort;
	TCHAR	m_szURLPath[MAX_URL_PATH_LEN];
};

#endif // !defined(AFX_URLPARSER_H__1C429083_9996_45FE_A019_0FB3A6CB5009__INCLUDED_)

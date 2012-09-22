#ifndef __STRING_CONVERT_H__
#define __STRING_CONVERT_H__

extern "C"
{
	BOOL GetUnicodeStringLength(LPCSTR lpMultiByte, int& cchWideChar);
	BOOL GetAnsiStringLength(LPCWSTR lpWideCharStr, int& cchMultiByte);

	// notes : need to release string buffer
	BOOL UnicodeToAnsi(LPCWSTR lpWideCharStr, LPSTR& lpMultiByte);
	BOOL AnsiToUnicode(LPCSTR lpMultiByte, LPWSTR& lpWideCharStr);

	void ReleaseStringBufferA(LPSTR& lpszBuffer);
	void ReleaseStringBufferW(LPWSTR& lpwszBuffer);
	void ReleaseStringBuffer(LPVOID& lpBuffer);

	BOOL UnicodeToAnsiCString(LPCWSTR lpWideCharStr, CString& strMultiByte);
	BOOL AnsiToUnicodeCString(LPCSTR lpMultiByte, CString& strWideChar);

	BOOL UnicodeToAnsiPtr(LPCWSTR lpWideCharStr, LPSTR lpMultiByte, int cchMaxMultiByte);
	BOOL AnsiToUnicodePtr(LPCSTR lpMultiByte, LPWSTR lpWideCharStr, int cchMaxWideChar);
}


inline BOOL GetUnicodeStringLength(LPCSTR lpMultiByte, int& cchWideChar)
{
	if (lpMultiByte == NULL) {
		cchWideChar = -1;
		return FALSE;
	}

	cchWideChar = MultiByteToWideChar(CP_ACP, 0,  lpMultiByte, -1, NULL, 0);
	if (cchWideChar <= 0)
		return FALSE;

	return TRUE;
}

inline BOOL GetAnsiStringLength(LPCWSTR lpWideCharStr, int& cchMultiByte)
{
	if (lpWideCharStr == NULL) {
		cchMultiByte = -1;
		return FALSE;
	}
	
	cchMultiByte = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, 
		lpWideCharStr, -1, NULL, 0, NULL, NULL);

	if (cchMultiByte <= 0)
		return FALSE;

	return TRUE;
}

inline BOOL UnicodeToAnsi(LPCWSTR lpWideCharStr, LPSTR& lpMultiByte)
{
	if (lpWideCharStr == NULL) {
		lpMultiByte = NULL;
		return FALSE;
	}

	int cchMultiByte = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, 
		lpWideCharStr, -1, NULL, 0, NULL, NULL);

	if (cchMultiByte <= 0)
		return FALSE;

	lpMultiByte = new char[cchMultiByte];
	if (lpMultiByte == NULL)
		return FALSE;

	BOOL bUsedDefaultChar = FALSE;
	WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
		lpWideCharStr, -1, lpMultiByte, cchMultiByte, "", &bUsedDefaultChar);

	return !bUsedDefaultChar;
}

inline BOOL AnsiToUnicode(LPCSTR lpMultiByte, LPWSTR& lpWideCharStr)
{
	if (lpMultiByte == NULL) {
		lpWideCharStr = NULL;
		return FALSE;
	}

	int cchWideChar = MultiByteToWideChar(CP_ACP, 0,  lpMultiByte, -1, 
		NULL, 0);

	if (cchWideChar <= 0)
		return FALSE;

	lpWideCharStr = new wchar_t[cchWideChar];
	if (lpWideCharStr == NULL)
		return FALSE;

	MultiByteToWideChar(CP_ACP, 0, lpMultiByte, -1, 
		lpWideCharStr, cchWideChar);

	return TRUE;
}

inline BOOL UnicodeToAnsiCString(LPCWSTR lpWideCharStr, CString& strMultiByte)
{

#ifdef _UNICODE

	strMultiByte = _T("");
	return FALSE;

#else

	if (lpWideCharStr == NULL) {
		strMultiByte = _T("");
		return FALSE;
	}

	int cchMultiByte = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, 
		lpWideCharStr, -1, NULL, 0, NULL, NULL);

	if (cchMultiByte <= 0)
		return FALSE;

	LPSTR lpMultiByte = strMultiByte.GetBuffer(cchMultiByte);
	if (lpMultiByte == NULL) {
		strMultiByte.ReleaseBuffer();
		return FALSE;
	}

	BOOL bUsedDefaultChar = FALSE;
	WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
		lpWideCharStr, -1, lpMultiByte, cchMultiByte, "", &bUsedDefaultChar);

	strMultiByte.ReleaseBuffer();

	return !bUsedDefaultChar;

#endif // _UNICODE
}

inline BOOL AnsiToUnicodeCString(LPCSTR lpMultiByte, CString& strWideChar)
{

#ifndef _UNICODE

	strWideChar = _T("");
	return FALSE;

#else

	if (lpMultiByte == NULL) {
		strWideChar = _T("");
		return FALSE;
	}

	int cchWideChar = MultiByteToWideChar(CP_ACP, 0,  lpMultiByte, -1, 
		NULL, 0);

	if (cchWideChar <= 0)
		return FALSE;

	LPWSTR lpWideCharStr = strWideChar.GetBuffer(cchWideChar);
	if (lpWideCharStr == NULL) {
		strWideChar.ReleaseBuffer();
		return FALSE;
	}

	MultiByteToWideChar(CP_ACP, 0, lpMultiByte, -1, 
		lpWideCharStr, cchWideChar);

	strWideChar.ReleaseBuffer();

	return TRUE;

#endif // _UNICODE
}

inline void ReleaseStringBufferA(LPSTR& lpszBuffer)
{
//	ASSERT(lpszBuffer != NULL);

	if (lpszBuffer) {
		delete []lpszBuffer;
		lpszBuffer = NULL;
	}
}

inline void ReleaseStringBufferW(LPWSTR& lpwszBuffer)
{
//	ASSERT(lpwszBuffer != NULL);

	if (lpwszBuffer) {
		delete lpwszBuffer;
		lpwszBuffer = NULL;
	}
}

inline void ReleaseStringBuffer(LPVOID& lpBuffer)
{
//	ASSERT(lpBuffer != NULL);

	if (lpBuffer) {
		delete lpBuffer;
		lpBuffer = NULL;
	}
}

inline BOOL UnicodeToAnsiPtr(LPCWSTR lpWideCharStr, LPSTR lpMultiByte, int cchMaxMultiByte)
{
	if (lpWideCharStr == NULL) {
		return FALSE;
	}

	int cchMultiByte = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, 
		lpWideCharStr, -1, NULL, 0, NULL, NULL);

	if (cchMultiByte <= 0 
		|| cchMaxMultiByte < cchMultiByte)
		return FALSE;

	BOOL bUsedDefaultChar = FALSE;
	WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
		lpWideCharStr, -1, lpMultiByte, cchMultiByte, "", &bUsedDefaultChar);

	return !bUsedDefaultChar;
}

inline BOOL AnsiToUnicodePtr(LPCSTR lpMultiByte, LPWSTR lpWideCharStr, int cchMaxWideChar)
{
	if (lpMultiByte == NULL) {
		return FALSE;
	}

	int cchWideChar = MultiByteToWideChar(CP_ACP, 0,  lpMultiByte, -1, 
		NULL, 0);

	if (cchWideChar <= 0
		|| cchWideChar >= cchMaxWideChar)
		return FALSE;

	MultiByteToWideChar(CP_ACP, 0, lpMultiByte, -1, 
		lpWideCharStr, cchWideChar);

	return TRUE;
}

inline BOOL UnicodeToUTF8Ptr(LPCWSTR lpWideCharStr, LPSTR lpMultiByte, int &cchMaxMultiByte)
{
	if (lpWideCharStr == NULL) {
		return FALSE;
	}

	int cchMultiByte = WideCharToMultiByte(CP_UTF8, 0, 
		lpWideCharStr, -1, NULL, 0, NULL, NULL);

	if (lpMultiByte == NULL) {
		cchMaxMultiByte = cchMultiByte;
		return cchMultiByte > 0 ? TRUE : FALSE;
	}

	if (cchMultiByte <= 0 
		|| cchMaxMultiByte < cchMultiByte)
		return FALSE;

	return !WideCharToMultiByte(CP_UTF8, 0, lpWideCharStr, -1, lpMultiByte, cchMultiByte, 0, 0);
}

inline BOOL UTF8ToUnicodePtr(LPCSTR lpMultiByte, LPWSTR lpWideCharStr, int &cchMaxWideChar)
{
	if (lpMultiByte == NULL) {
		return FALSE;
	}

	int cchWideChar = MultiByteToWideChar(CP_UTF8, 0, lpMultiByte, -1, 0, 0);

	if (lpWideCharStr == NULL) {
		cchMaxWideChar = cchWideChar;
		return cchWideChar > 0 ? TRUE : FALSE;
	}

	if (cchWideChar <= 0
		|| cchWideChar >= cchMaxWideChar)
		return FALSE;

	MultiByteToWideChar(CP_UTF8, 0, lpMultiByte, -1, 
		lpWideCharStr, cchWideChar);

	return TRUE;
}

#endif //__STRING_CONVERT_H__
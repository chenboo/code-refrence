#pragma once 

#pragma warning ( disable : 4996 )

class CElcA2W
{
public:
	CElcA2W(LPCSTR pAnsi, BOOL bInUTF8=TRUE) {
		m_uCodePage = bInUTF8 ? CP_UTF8 : CP_ACP;
		m_pStr = AnsiToWChar(pAnsi);
	}

	~CElcA2W() {
		if (m_pStr)
			delete []m_pStr;
	}

	void Convert(LPCSTR pAnsi, BOOL bInUTF8=TRUE) {
		m_uCodePage = bInUTF8 ? CP_UTF8 : CP_ACP;

		if (m_pStr)
			delete []m_pStr;

		m_pStr = AnsiToWChar(pAnsi);
	}

public:
	LPCWSTR		m_pStr;

protected:
	UINT		m_uCodePage;

	LPCWSTR AnsiToWChar(LPCSTR lpszMultiByte) {
		if (!lpszMultiByte)
			return NULL;

		int cchWChar = MultiByteToWideChar(m_uCodePage, 0, lpszMultiByte, -1, 0, 0);
		if (cchWChar == 0)
			return NULL;

		wchar_t* pWChar = new wchar_t[cchWChar];
		memset(pWChar, 0, sizeof(wchar_t) * cchWChar);

		MultiByteToWideChar(m_uCodePage, 0, lpszMultiByte, -1, pWChar, cchWChar);
		return pWChar;
	}
};

class CElcW2A
{
public:
	CElcW2A(LPCWSTR lpUncode, BOOL bToUTF8=TRUE) {
		m_uCodePage = bToUTF8 ? CP_UTF8 : CP_ACP;
		m_pStr = WCharToAnsi(lpUncode);
	}

	~CElcW2A() {
		if (m_pStr)
			delete []m_pStr;
	}

	void Convert(LPCWSTR lpUncode, BOOL bToUTF8=TRUE) {
		m_uCodePage = bToUTF8 ? CP_UTF8 : CP_ACP;

		if (m_pStr)
			delete []m_pStr;

		m_pStr = WCharToAnsi(lpUncode);
	}

public:
	LPCSTR	m_pStr;

protected:
	UINT m_uCodePage;

	LPCSTR WCharToAnsi(LPCWSTR lpszWideChar) {
		if (lpszWideChar == NULL)
			return NULL;

		int cchMultiByte = WideCharToMultiByte(m_uCodePage, 0, 
			lpszWideChar, -1, NULL, 0, NULL, NULL);

		char* pUTF8 = new char[cchMultiByte];
		memset(pUTF8, 0, cchMultiByte);

		WideCharToMultiByte(m_uCodePage, 0, lpszWideChar, -1, pUTF8, cchMultiByte, 0, 0);

		return pUTF8;
	}
};

class CElcLocNumber
{
public:
	CElcLocNumber() {}
	~CElcLocNumber() {}

public:
	CString Convert(LONG nNumber) {
		return Convert((float)nNumber, 0);
	}

	CString Convert(float fNumber, UINT NumDigits) {

		TCHAR szSDecimal[2] = { 0 };
		TCHAR szSThousand[2] = { 0 };
		TCHAR szSGrouping[5] = { 0 };	
		TCHAR szInegnumber[2] = { 0 };
		TCHAR szOutput[128] = { 0 };

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szSDecimal, 2);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, szSThousand, 2);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, szSGrouping, 5);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, szInegnumber, 2);

		NUMBERFMT numfmt;
		memset(&numfmt, 0, sizeof(numfmt));
		numfmt.NumDigits = NumDigits;
		numfmt.LeadingZero = 0;
		numfmt.lpDecimalSep = szSDecimal;
		numfmt.lpThousandSep = szSThousand;
		numfmt.NegativeOrder = (UINT)_ttoi(szInegnumber);
		if (szSGrouping[1] == _T(';'))
			numfmt.Grouping = (UINT)_ttoi(&szSGrouping[0]);
		else {
			TCHAR szTmp[3] = { 0 };
			_tcsncpy(szTmp, szSGrouping, 2);
			numfmt.Grouping = (UINT)_ttoi(szTmp);
		}

		CString strNumber;
		strNumber.Format(_T("%f"), fNumber);
		GetNumberFormat(LOCALE_USER_DEFAULT, 0, strNumber, &numfmt, szOutput, 128);

		return (CString)szOutput;
	}
};
#include "StdAfx.h"
#include "ElcSkinDoc.h"

#pragma warning (disable : 4996)


/*************************************************

                 .==.        .==.
                //`^\\      //^`\\
               // ^ ^\(\__/)/^ ^^\\
              //^ ^^ ^/6  6\ ^^ ^ \\
             //^ ^^ ^/( .. )\^ ^ ^ \\
            // ^^ ^/\| v""v |/\^ ^ ^\\
           // ^^/\/ /  `~~`  \ \/\^ ^\\
         ---------------------------------
                  HERE BE DRAGONS

*************************************************/


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CElcSkinDoc::CElcSkinDoc(void)
{
	m_pXMLDoc = NULL;
	m_dwVersion = 0x00010000;
	m_bValid = FALSE;
}

CElcSkinDoc::~CElcSkinDoc(void)
{
	ReleaseDoc();
}

CSize CElcSkinDoc::GetImageSize(const PELC_SKIN_IMAGE pSkin)
{
	CSize size(0,0);
	if (pSkin && pSkin->pImage) {
		if (pSkin->bVertical) {
			size.cx = ((Image *)pSkin->pImage)->GetWidth();
			size.cy = ((Image *)pSkin->pImage)->GetHeight() / pSkin->nCount;
		}
		else {
			size.cx = ((Image *)pSkin->pImage)->GetWidth() / pSkin->nCount;
			size.cy = ((Image *)pSkin->pImage)->GetHeight();
		}
	}

	return size;
}

CSize CElcSkinDoc::GetImageSize(UINT nImageId, LPCTSTR lpszType)
{
	CSize size(0,0);
	Image* pImage = NULL;
	if (!ImageFromIDResource(nImageId, lpszType, pImage)) {
		if (pImage)
			delete pImage;
		return size;
	}

	size.cx = pImage->GetWidth();
	size.cy = pImage->GetHeight();

	delete pImage;

	return size;
}

CSize CElcSkinDoc::GetStringSize(LPCTSTR lpszText, CFont* pFont)
{
	CSize size(0,0);

	if (!lpszText)
		return size;

	HDC hdc = GetDC(NULL);
	if (!hdc) {
		// what's error happened
		ASSERT(0);
		return size;
	}

	HGDIOBJ hFont = NULL;
	if (pFont) 
		hFont = ::SelectObject(hdc, pFont->m_hObject);
	else 
		hFont = ::SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

	::GetTextExtentExPoint(hdc, 
		lpszText,
		(int)_tcslen(lpszText),
		0,
		NULL,
		NULL,
		&size);

	if (hFont)
		::SelectObject(hdc, hFont);

	::ReleaseDC(NULL, hdc);

	return size;
}

CSize CElcSkinDoc::GetStringSize(Graphics* pGraphics, LPCTSTR lpszText, LOGFONT& lf)
{
	CSize size(0,0);

	if (!lpszText)
		return size;

	FontStyle fontstyle = FontStyleRegular;
	if (lf.lfWeight == 400) {
		fontstyle = FontStyleRegular;
	}
	else {
		if (lf.lfItalic)
			fontstyle = FontStyleBoldItalic;
		else
			fontstyle = FontStyleBold;
	}

	Font font(lf.lfFaceName, (REAL)abs(lf.lfHeight), fontstyle, UnitPixel);

	StringFormat stringformat;
	stringformat.SetAlignment(StringAlignmentNear);
	stringformat.SetLineAlignment(StringAlignmentCenter);
	stringformat.SetFormatFlags(StringFormatFlagsNoWrap);

	SizeF sizeIn(1000, 1000);
	SizeF sizeOut(0,0);

	if (pGraphics) {
		pGraphics->MeasureString(lpszText, -1, &font, sizeIn, &stringformat, &sizeOut);
	}
	else {
		HDC hdc = ::GetDC(NULL);
		Graphics graphics(hdc);
		graphics.MeasureString(lpszText, -1, &font, sizeIn, &stringformat, &sizeOut);
		::ReleaseDC(NULL, hdc);
	}

	size.cx = (int)(sizeOut.Width + 0.5f);
	size.cy = (int)(sizeOut.Height + 0.5f);

	return size;
}

int CElcSkinDoc::CetStringHeight(LPCTSTR lpszText, int nMaxWidth, CFont* pFont)
{
	return CetStringRect(lpszText, nMaxWidth, pFont).Height();
}

CRect CElcSkinDoc::CetStringRect(LPCTSTR lpszText, int nMaxWidth, CFont* pFont)
{
	CRect rect(0, 0, nMaxWidth, 0);
	if (!lpszText || _tcslen(lpszText) == 0)
		return rect;

	HDC hdc = GetDC(NULL);
	if (!hdc) {
		// what's error happened
		ASSERT(0);
		return rect;
	}

	HGDIOBJ hFont = NULL;
	if (pFont) 
		hFont = ::SelectObject(hdc, pFont->m_hObject);
	else 
		hFont = ::SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));	

	if (hFont) {
		::DrawText(hdc, 
			lpszText, 
			-1, 
			&rect, 
			DT_WORDBREAK | DT_NOPREFIX | DT_NOFULLWIDTHCHARBREAK | DT_CALCRECT);
		::SelectObject(hdc, hFont);
	}

	::ReleaseDC(NULL, hdc);

	return rect;
}

CRect CElcSkinDoc::CalcRect(const LPRECT lprcDest, const PPOINT lpTopLeft, const PSIZE pSize)
{
	CRect rect(0,0,0,0);

	if (lpTopLeft->x < 0)
		rect.left = lprcDest->right + lpTopLeft->x;
	else
		rect.left = lprcDest->left + lpTopLeft->x;
	if (lpTopLeft->y < 0)
		rect.top = lprcDest->bottom + lpTopLeft->y;
	else
		rect.top = lprcDest->top + lpTopLeft->y;

	if (pSize->cx <= 0)
		rect.right = lprcDest->right + pSize->cx;
	else
		rect.right = rect.left + pSize->cx;
	if (pSize->cy <= 0)
		rect.bottom = lprcDest->bottom + pSize->cy;
	else
		rect.bottom = rect.top + pSize->cy;

	return rect;
}

BOOL CElcSkinDoc::CreateSkinFont(const PELC_SKIN_FONT pSkinFont, CFont & ftFont)
{
	if (!pSkinFont
		|| pSkinFont->nSize == 0
		|| _tcslen(pSkinFont->szFaceName) == 0)
		return FALSE;

	LOGFONT lf = {0};
	GetDefaultFont(lf);

	_tcscpy(lf.lfFaceName, pSkinFont->szFaceName);
	CALC_FONT_HEIGHT(lf.lfHeight, pSkinFont->nSize);
	lf.lfItalic = pSkinFont->bItalic;
	lf.lfUnderline = pSkinFont->bUnderline;
	lf.lfWeight = pSkinFont->bBold ? FW_BOLD : FW_NORMAL;

	if (ftFont.GetSafeHandle())
		ftFont.DeleteObject();

	return ftFont.CreateFontIndirect(&lf);
}

BOOL CElcSkinDoc::CreateSkinFont(const PELC_SKIN_FONT pSkinFont, LOGFONT & lf)
{
	if (!pSkinFont)
		return FALSE;

	memset(&lf, 0, sizeof(lf));
	GetDefaultFont(lf);

	_tcscpy(lf.lfFaceName, pSkinFont->szFaceName);
	CALC_FONT_HEIGHT(lf.lfHeight, pSkinFont->nSize);
	lf.lfItalic = pSkinFont->bItalic;
	lf.lfUnderline = pSkinFont->bUnderline;
	lf.lfWeight = pSkinFont->bBold ? FW_BOLD : FW_NORMAL;

	return TRUE;
}

void CElcSkinDoc::ReleaseDoc()
{
	if (m_pXMLDoc)
		delete m_pXMLDoc;
	m_pXMLDoc = NULL;

	for (CPaneNCSkinArray::iterator it = m_arrPaneSkin.begin();
		it != m_arrPaneSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CPaneCSkinArray::iterator it = m_arrPaneClientSkin.begin();
		it != m_arrPaneClientSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CToolbarSkinArray::iterator it = m_arrToolbarSkin.begin();
		it != m_arrToolbarSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CButtonSkinArray::iterator it = m_arrButtonSkin.begin();
		it != m_arrButtonSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CHeaderCtrlSkinArray::iterator it = m_arrHeaderCtrlSkin.begin();
		it != m_arrHeaderCtrlSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CListSkinArray::iterator it = m_arrListSkin.begin();
		it != m_arrListSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CSplitterSkinArray::iterator it = m_arrSplitterSkin.begin();
		it != m_arrSplitterSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CMenuSkinArray::iterator it = m_arrMenuSkin.begin();
		it != m_arrMenuSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CRichMenuArray::iterator it = m_arrRichMenuSkin.begin();
		it != m_arrRichMenuSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CWindowSkinArray::iterator it = m_arrWindowSkin.begin();
		it != m_arrWindowSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CSliderSkinArray::iterator it = m_arrSliderSkin.begin();
		it != m_arrSliderSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CStatusBarSkinArray::iterator it = m_arrStatusBarSkin.begin();
		it != m_arrStatusBarSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CTabCtrlSkinArray::iterator it = m_arrTabCtrlSkin.begin();
		it != m_arrTabCtrlSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CPictureSkinArray::iterator it = m_arrPicture.begin();
		it != m_arrPicture.end();
		it ++)
	{
		delete (*it);
	}
	for (CFloatBarSkinArray::iterator it = m_arrFloatBarSkin.begin();
		it != m_arrFloatBarSkin.end();
		it ++)
	{
		delete (*it);
	}
	for (CBalloonSkinArray::iterator it = m_arrBalloonSkin.begin();
		it != m_arrBalloonSkin.end();
		it ++)
	{
		delete (*it);
	}

	m_arrPaneSkin.clear();
	m_arrPaneClientSkin.clear();
	m_arrToolbarSkin.clear();
	m_arrFloatBarSkin.clear();
	m_arrBalloonSkin.clear();
	m_arrButtonSkin.clear();
	m_arrHeaderCtrlSkin.clear();
	m_arrListSkin.clear();
	m_arrSplitterSkin.clear();
	m_arrSliderSkin.clear();
	m_arrStatusBarSkin.clear();
	m_arrTabCtrlSkin.clear();
	m_arrMenuSkin.clear();
	m_arrRichMenuSkin.clear();
	m_arrWindowSkin.clear();
}

BOOL CElcSkinDoc::LoadFile(LPCTSTR lpszFileName)
{
	if (lpszFileName == NULL || _taccess(lpszFileName, 0) != 0) {
		_TRACE(_T("Skin config file is not exist!\n"));
		ASSERT(0);
		return FALSE;
	}

	ReleaseDoc();

	TCHAR szDrive[_MAX_DRIVE] = {0};
	TCHAR szDir[_MAX_DIR] = {0};
	_tsplitpath(lpszFileName, szDrive, szDir, NULL, NULL);
	m_strPath = szDrive;
	m_strPath += szDir;
	if (m_strPath.Right(1) != _T('\\'))
		m_strPath += _T("\\");

	CElcW2A w2a(lpszFileName);
	m_pXMLDoc = new TiXmlDocument(w2a.m_pStr, true);
	
	m_bValid =  ParseDocument();

	if (!m_bValid)
		ReleaseDoc();

	return m_bValid;
}

BOOL CElcSkinDoc::ParseDocument()
{
	if (m_pXMLDoc == NULL)
		return FALSE;

	if (m_pXMLDoc->LoadFile() == false) {
		_TRACE(_T("Load skin config file failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	TiXmlElement* pRoot = m_pXMLDoc->FirstChildElement("skins");
	if (!pRoot) {
		_TRACE(_T("Skin config file format error!\n"));
		ASSERT(0);
		return FALSE;
	}

	if (!ParseHead(pRoot)
		|| !ParseSkins(pRoot))
		return FALSE;

	return TRUE;
}

BOOL CElcSkinDoc::ParseHead(TiXmlElement* pRoot)
{
	// get version
	const char* pszVer = pRoot->Attribute("version");
	if (!pszVer)
		return FALSE;

	const char* pDot = strstr(pszVer, ".");
	if (!pDot)
		return FALSE;

	char szSub[8] = { 0 };
	strncpy(szSub, pszVer, pDot-pszVer);
	m_dwVersion = (WORD)atoi(szSub);

	strcpy(szSub, pDot+1);
	m_dwVersion = (m_dwVersion<<16) + (WORD)atoi(szSub);

	if (HIWORD(m_dwVersion) > ELC_SKIN_SUPPORT_MAJOR_VERSION) {
		_TRACE(_T("Not support skin config file version!\n"));
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseFont(TiXmlElement* pElmFont, PELC_SKIN_FONT pFont)
{
	LPCSTR lpValue = pElmFont->Attribute("name");
	if (lpValue) {
		CElcA2W a2w(lpValue);
		_tcscpy(pFont->szFaceName, a2w.m_pStr);
	}
	else {
		LOGFONT lf = {0};
		GetMSShellDlgFont(lf);
		_tcscpy(pFont->szFaceName, lf.lfFaceName);
	}

	lpValue = pElmFont->Attribute("bold");
	if (lpValue && strcmp(lpValue, "yes") == 0)
		pFont->bBold = TRUE;
	else
		pFont->bBold = FALSE;

	lpValue = pElmFont->Attribute("italic");
	if (lpValue && strcmp(lpValue, "yes") == 0)
		pFont->bItalic = TRUE;
	else
		pFont->bItalic = FALSE;

	lpValue = pElmFont->Attribute("underline");
	if (lpValue && strcmp(lpValue, "yes") == 0)
		pFont->bUnderline = TRUE;
	else
		pFont->bUnderline = FALSE;

	lpValue = pElmFont->Attribute("size");
	if (lpValue)
		pFont->nSize = atoi(lpValue);
	else
		pFont->nSize = 9;

	return TRUE;
}

BOOL CElcSkinDoc::ParseImage(TiXmlElement* pElmSkin, PELC_SKIN_IMAGE pImage)
{
	LPCSTR lpValue = NULL;
	lpValue = pElmSkin->Attribute("counts");
	if (lpValue)
		pImage->nCount = atoi(lpValue);
	else
		pImage->nCount = 1;

	lpValue = pElmSkin->Attribute("arrange");
	if (!lpValue || stricmp(lpValue, "v") == 0)
		pImage->bVertical = TRUE;
	else
		pImage->bVertical = FALSE;

	pImage->stretch = 0;
	lpValue = pElmSkin->Attribute("leftsplit");
	if (lpValue)
		pImage->nLeftSplit = atoi(lpValue);
	else
		pImage->nLeftSplit = -1;
	
	lpValue = pElmSkin->Attribute("rightsplit");
	if (lpValue)
		pImage->nRightSplit = atoi(lpValue);
	else
		pImage->nRightSplit = -1;

	lpValue = pElmSkin->Attribute("topsplit");
	if (lpValue)
		pImage->nTopSplit = atoi(lpValue);
	else
		pImage->nTopSplit = -1;

	lpValue = pElmSkin->Attribute("bottomsplit");
	if (lpValue)
		pImage->nBottomSplit = atoi(lpValue);
	else
		pImage->nBottomSplit = -1;

	if (pImage->nLeftSplit > 0 && pImage->nRightSplit > 0)
		pImage->stretch |= EISR_HORIZONTAL;
	if (pImage->nTopSplit > 0 && pImage->nBottomSplit > 0)
		pImage->stretch |= EISR_VERTICAL;

	lpValue = pElmSkin->Attribute("image");
	if (lpValue) {
		CElcA2W a2w(lpValue);
		CString strPath = m_strPath + a2w.m_pStr;
		if (_taccess(strPath, 0) != 0) {
			_TRACE(_T("Image file %s is not exist\n"), strPath);
			ASSERT(0);
			pImage->pImage = NULL;
			return FALSE;
		}
		else {
			Bitmap* pBitmap = Bitmap::FromFile(strPath);
			if (pBitmap->GetLastStatus() != Ok) {
				_TRACE(_T("Load image %s failed\n"), strPath);
				ASSERT(0);
				pImage->pImage = NULL;
				return FALSE;
			}
			else {
				pImage->pImage = pBitmap;
			}
		}
	}
	else {
		_TRACE(_T("Missing image path!\n"));
		ASSERT(0);
		pImage->pImage = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseSkins(TiXmlElement* pRoot)
{
	TiXmlElement* pChildElm = NULL;

	pChildElm = pRoot->FirstChildElement("global");
	if (pChildElm && !ParseGlobal(pChildElm))
	{
		_TRACE(_T("Parse Global Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("pictures");
	if (pChildElm && !ParsePictures(pChildElm))
	{
		_TRACE(_T("Parse Pictures Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("buttons");
	if (pChildElm && !ParseButtons(pChildElm))
	{
		_TRACE(_T("Parse Buttons Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("headerctrls");
	if (pChildElm && !ParseHeaderCtrls(pChildElm))
	{
		_TRACE(_T("Parse HeaderCtrls Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("lists");
	if (pChildElm && !ParseLists(pChildElm))
	{
		_TRACE(_T("Parse Lists Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("splitters");
	if (pChildElm && !ParseSplitters(pChildElm))
	{
		_TRACE(_T("Parse Splitters Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("sliders");
	if (pChildElm && !ParseSliders(pChildElm))
	{
		_TRACE(_T("Parse Sliders Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("statusbars");
	if (pChildElm && !ParseStatusBars(pChildElm))
	{
		_TRACE(_T("Parse StatusBars Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("menus");
	if (pChildElm && !ParseMenus(pChildElm))
	{
		_TRACE(_T("Parse Menus Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("richmenus");
	if (pChildElm && !ParseRichMenus(pChildElm))
	{
		_TRACE(_T("Parse Rich-Menus Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("toolbars");
	if (pChildElm && !ParseToolbars(pChildElm))
	{
		_TRACE(_T("Parse ToolBars Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("floatbars");
	if (pChildElm && !ParseFloatBars(pChildElm))
	{
		_TRACE(_T("Parse floatBars Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("balloons");
	if (pChildElm && !ParseBalloons(pChildElm))
	{
		_TRACE(_T("Parse balloon Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("tabctrls");
	if (pChildElm && !ParseTabCtrls(pChildElm))
	{
		_TRACE(_T("Parse tabctrl Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("panes");
	if (pChildElm && !ParsePanes(pChildElm))
	{
		_TRACE(_T("Parse Panes Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	pChildElm = pRoot->FirstChildElement("windows");
	if (pChildElm && !ParseWindows(pChildElm))
	{
		_TRACE(_T("Parse Windows Skin Failed!\n"));
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseGlobal(TiXmlElement* pElm)
{
	memset(&m_global, 0, sizeof(m_global));

	TiXmlElement* pChildElm = pElm->FirstChildElement("colors");
	if (pChildElm) {
		ParseColorA(pChildElm->Attribute("mainbackground"), m_global.crMainBackgnd);
		ParseColorA(pChildElm->Attribute("panebackground"), m_global.crPaneBackgnd);
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParsePictures(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("picture");
	while (pChildElm) {
		PELC_SKIN_PICTURE pSkin = new ELC_SKIN_PICTURE;
		memset(pSkin, 0, sizeof(ELC_SKIN_PICTURE));

		if (!ParsePicture(pChildElm, pSkin)) {
			delete pSkin;
			return FALSE;
		}

		m_arrPicture.push_back(pSkin);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParsePicture(TiXmlElement* pElm, PELC_SKIN_PICTURE pSkin)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pSkin->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("background"), pSkin->crBackgnd);
	}

	// image
	TiXmlElement* pChildElm = pElm->FirstChildElement("skin");
	if (pSkin) {
		if (!ParseImage(pChildElm, &pSkin->image))
			return FALSE;
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParsePanes(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("pane");
	while (pChildElm) {
		PELC_SKIN_PNAE_NC pSkin = new ELC_SKIN_PNAE_NC;
		memset(pSkin, 0, sizeof(ELC_SKIN_PNAE_NC));
		if (!ParsePane(pChildElm, pSkin)) {
			delete pSkin;
			return FALSE;
		}

		m_arrPaneSkin.push_back(pSkin);

		pChildElm = pChildElm->NextSiblingElement();
	}

	pChildElm = pElm->FirstChildElement("clientpane");
	while (pChildElm) {
		PELC_SKIN_PNAE_CLIENT pSkin = new ELC_SKIN_PNAE_CLIENT;
		memset(pSkin, 0, sizeof(ELC_SKIN_PNAE_CLIENT));
		if (!ParseClientPane(pChildElm, pSkin)) {
			delete pSkin;
			return FALSE;
		}

		m_arrPaneClientSkin.push_back(pSkin);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParsePane(TiXmlElement* pElm, PELC_SKIN_PNAE_NC pSkin)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pSkin->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("background"), pSkin->crBackgnd);
		ParseColorA(pColors->Attribute("client"), pSkin->crClient);
	}

	// skin iamges
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pImage = pSkins->FirstChildElement("skin");
		if (!pImage || !ParseImage(pImage, &pSkin->ncimage))
			return FALSE;
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseClientPane(TiXmlElement* pElm, PELC_SKIN_PNAE_CLIENT pSkin)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pSkin->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("background"), pSkin->crBackgnd);
	}

	// skin iamges
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pImage = pSkins->FirstChildElement("skin");
		if (!pImage || !ParseImage(pImage, &pSkin->background))
			return FALSE;
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseToolbars(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("toolbar");
	while (pChildElm) {
		PELC_SKIN_TOOLBAR pSkin = new ELC_SKIN_TOOLBAR;
		memset(pSkin, 0, sizeof(ELC_SKIN_TOOLBAR));
		if (!ParseToolbar(pChildElm, pSkin)) {
			delete pSkin;
			return FALSE;
		}

		m_arrToolbarSkin.push_back(pSkin);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseToolbar(TiXmlElement* pElm, PELC_SKIN_TOOLBAR pSkin)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pSkin->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("background"), pSkin->crBackgnd);
		ParseColorA(pColors->Attribute("text"), pSkin->crText);
		ParseColorA(pColors->Attribute("border"), pSkin->crBorder);
		ParseColorA(pColors->Attribute("separator1"), pSkin->crSeparator[0]);
		ParseColorA(pColors->Attribute("separator2"), pSkin->crSeparator[1]);
	}

	// skin iamges
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pImage = pSkins->FirstChildElement("skin");
		while (pImage) {
			PELC_SKIN_IMAGE pSkinImage = NULL;
			LPCSTR lpValue = pImage->Attribute("id");
			if (stricmp(lpValue, "background") == 0)
				pSkinImage = &pSkin->background;

			if (!ParseImage(pImage, pSkinImage))
				return FALSE;

			pImage = pImage->NextSiblingElement();
		}
	}

	// substyles
	TiXmlElement* pSubStyles = pElm->FirstChildElement("substyles");
	if (!pSubStyles)
		return FALSE;
	a2w.Convert(pSubStyles->Attribute("button"));
	if (a2w.m_pStr)
		_tcscpy(pSkin->szButtonStyle, a2w.m_pStr);

	a2w.Convert(pSubStyles->Attribute("unionbutton1"));
	if (a2w.m_pStr)
		_tcscpy(pSkin->szUnionBtnStyle1, a2w.m_pStr);

	a2w.Convert(pSubStyles->Attribute("unionbutton2"));
	if (a2w.m_pStr)
		_tcscpy(pSkin->szUnionBtnStyle2, a2w.m_pStr);

	return TRUE;
}

BOOL CElcSkinDoc::ParseFloatBars(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("floatbar");
	while (pChildElm) {
		PELC_SKIN_FLOATBAR pSkin = new ELC_SKIN_FLOATBAR;
		memset(pSkin, 0, sizeof(ELC_SKIN_FLOATBAR));
		if (!ParseFloatBar(pChildElm, pSkin)) {
			delete pSkin;
			return FALSE;
		}

		m_arrFloatBarSkin.push_back(pSkin);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseFloatBar(TiXmlElement* pElm, PELC_SKIN_FLOATBAR pSkin)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pSkin->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("tiptext"), pSkin->crTip);
		ParseColorA(pColors->Attribute("tiptextshadow"), pSkin->crTipShadow);
		ParseColorA(pColors->Attribute("label"), pSkin->crLabel);
		ParseColorA(pColors->Attribute("title"), pSkin->crTitle);
	}

	TiXmlElement* pProperty = pElm->FirstChildElement("property");
	if (pProperty) {
		LPCSTR lpsValue = pProperty->Attribute("title_height");
		if (lpsValue)
			pSkin->nTitleHeight = atoi(lpsValue);

		ParseAnchor(pProperty->Attribute("drag_rect"), pSkin->drag.point, pSkin->drag.size);

		pProperty->Attribute("peek_height", &pSkin->nPeekHeight);
	}

	TiXmlElement* pControls = pElm->FirstChildElement("controls");
	TiXmlElement* pControl = pControls->FirstChildElement("control");
	while (pControl) {
		LPCSTR lpszValue = pControl->Attribute("class");
		LPCSTR lpszId = pControl->Attribute("id");

		if (!lpszValue || !lpszId) {
			return FALSE;
		}
		else if (stricmp(lpszValue, "nwbutton") == 0) {
			PELC_SKIN_NOTWND_BUTTON pButton = NULL;
			if (stricmp(lpszId, "fold") == 0)
				pButton = &pSkin->fold;
			else if (stricmp(lpszId, "pin") == 0)
				pButton = &pSkin->pin;
			else 
				return FALSE;

			if (!ParseNWButtons(pControl, pButton))
				return FALSE;
		}

		pControl = pControl->NextSiblingElement();
	}

	// skin iamges
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pImage = pSkins->FirstChildElement("skin");
		while (pImage) {
			PELC_SKIN_IMAGE pSkinImage = NULL;
			LPCSTR lpValue = pImage->Attribute("id");
			if (stricmp(lpValue, "background") == 0)
				pSkinImage = &pSkin->background;
			else if (stricmp(lpValue, "buttonbackgnd") == 0)
				pSkinImage = &pSkin->buttonbackgnd;
			else if (stricmp(lpValue, "separator") == 0)
				pSkinImage = &pSkin->separator;
			else if (stricmp(lpValue, "dropdown") == 0)
				pSkinImage = &pSkin->dropdown;
			else if (stricmp(lpValue, "tipbackground") == 0)
				pSkinImage = &pSkin->tipbackground;

			if (!ParseImage(pImage, pSkinImage))
				return FALSE;

			pImage = pImage->NextSiblingElement();
		}
	}
	
	return TRUE;
}

BOOL CElcSkinDoc::ParseBalloons(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("balloon");
	while (pChildElm) {
		PELC_SKIN_BALLOON pSkin = new ELC_SKIN_BALLOON;
		memset(pSkin, 0, sizeof(ELC_SKIN_BALLOON));
		if (!ParseBalloon(pChildElm, pSkin)) {
			delete pSkin;
			return FALSE;
		}

		m_arrBalloonSkin.push_back(pSkin);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseBalloon(TiXmlElement* pElm, PELC_SKIN_BALLOON pSkin)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pSkin->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("caption"), pSkin->crCaption);
		ParseColorA(pColors->Attribute("title"), pSkin->crTitle);
		ParseColorA(pColors->Attribute("item"), pSkin->crItem);
		ParseColorA(pColors->Attribute("hotitem"), pSkin->crHotItem);
	}

	// fonts
	TiXmlElement* pFonts = pElm->FirstChildElement("fonts");
	if (pFonts) {
		TiXmlElement* pFont = pFonts->FirstChildElement("font");
		while (pFont) {
			PELC_SKIN_FONT pSkinFont = NULL;
			LPCSTR lpValue = pFont->Attribute("id");
			if (stricmp(lpValue, "caption") == 0)
				pSkinFont = &pSkin->ftCaption;
			else if (stricmp(lpValue, "title") == 0)
				pSkinFont = &pSkin->ftTitle;
			else if (stricmp(lpValue, "item") == 0)
				pSkinFont = &pSkin->ftItem;

			if (!ParseFont(pFont, pSkinFont))
				return FALSE;

			pFont = pFont->NextSiblingElement();
		}
	}

	// skin iamges
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pImage = pSkins->FirstChildElement("skin");
		while (pImage) {
			PELC_SKIN_IMAGE pSkinImage = NULL;
			LPCSTR lpValue = pImage->Attribute("id");
			if (stricmp(lpValue, "background") == 0)
				pSkinImage = &pSkin->background;
			else if (stricmp(lpValue, "close") == 0)
				pSkinImage = &pSkin->close;
			else if (stricmp(lpValue, "icon") == 0)
				pSkinImage = &pSkin->icon;

			if (!ParseImage(pImage, pSkinImage))
				return FALSE;

			pImage = pImage->NextSiblingElement();
		}
	}
	
	return TRUE;
}

BOOL CElcSkinDoc::ParseButtons(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("button");
	while (pChildElm) {
		PELC_SKIN_BUTTON pSkin = new ELC_SKIN_BUTTON;
		memset(pSkin, 0, sizeof(ELC_SKIN_BUTTON));
		if (!ParseButton(pChildElm, pSkin)) {
			delete pSkin;
			return FALSE;
		}

		m_arrButtonSkin.push_back(pSkin);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseButton(TiXmlElement* pElm, PELC_SKIN_BUTTON pSkin)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pSkin->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("background"), pSkin->crBackgnd);
		ParseColorA(pColors->Attribute("normal"), pSkin->crTextNormal);
		ParseColorA(pColors->Attribute("hot"), pSkin->crTextHot);
		ParseColorA(pColors->Attribute("down"), pSkin->crTextDown);
		ParseColorA(pColors->Attribute("disabled"), pSkin->crTextDisabled);
		ParseColorA(pColors->Attribute("groupborder"), pSkin->crGroupBorder);
	}

	// adjust position
	TiXmlElement* pAdjustPos = pElm->FirstChildElement("adjustpos");
	if (pAdjustPos) {
		pSkin->ptAdjust.x = atoi(pAdjustPos->Attribute("x"));
		pSkin->ptAdjust.y = atoi(pAdjustPos->Attribute("y"));
	}

	// skin iamges
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pImage = pSkins->FirstChildElement("skin");
		while (pImage) {
			PELC_SKIN_IMAGE pSkinImage = NULL;
			LPCSTR lpValue = pImage->Attribute("id");
			if (stricmp(lpValue, "background") == 0)
				pSkinImage = &pSkin->background;
			else if (stricmp(lpValue, "dropdown") == 0)
				pSkinImage = &pSkin->dropdown;
			else if (stricmp(lpValue, "check") == 0)
				pSkinImage = &pSkin->check;
			else if (stricmp(lpValue, "Indeterminate") == 0)
				pSkinImage = &pSkin->Indeterminate;

			if (!ParseImage(pImage, pSkinImage))
				return FALSE;

			pImage = pImage->NextSiblingElement();
		}
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseHeaderCtrls(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("headerctrl");
	while (pChildElm) {
		PELC_SKIN_HEADERCTRL pSkin = new ELC_SKIN_HEADERCTRL;
		memset(pSkin, 0, sizeof(ELC_SKIN_HEADERCTRL));

		if (!ParseHeaderCtrl(pChildElm, pSkin)) {
			delete pSkin;
			return FALSE;
		}

		m_arrHeaderCtrlSkin.push_back(pSkin);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseHeaderCtrl(TiXmlElement* pElm, PELC_SKIN_HEADERCTRL pSkin)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pSkin->szStyle, a2w.m_pStr);

	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("text"), pSkin->crText);
		ParseColorA(pColors->Attribute("textshadow"), pSkin->crTextShadow);
	}

	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pImage = pSkins->FirstChildElement("skin");
		while (pImage) {
			PELC_SKIN_IMAGE pSkinImage = NULL;
			LPCSTR lpszValue = pImage->Attribute("id");
			if (!lpszValue)
				return FALSE;
			else if (stricmp(lpszValue, "background") == 0)
				pSkinImage = &pSkin->background;
			else if (stricmp(lpszValue, "separator") == 0)
				pSkinImage = &pSkin->separator;
			else if (stricmp(lpszValue, "sort") == 0)
				pSkinImage = &pSkin->sort;

			if (!ParseImage(pImage, pSkinImage))
				return FALSE;

			pImage = pImage->NextSiblingElement();
		}
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseLists(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("list");
	while (pChildElm) {
		PELC_SKIN_LIST pSkin = new ELC_SKIN_LIST;
		memset(pSkin, 0, sizeof(ELC_SKIN_LIST));

		if (!ParseList(pChildElm, pSkin)) {
			delete pSkin;
			return FALSE;
		}

		m_arrListSkin.push_back(pSkin);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseList(TiXmlElement* pElm, PELC_SKIN_LIST pSkin)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pSkin->szStyle, a2w.m_pStr);

	// header ctrl style
	TiXmlElement* pSubStyle = pElm->FirstChildElement("substyles");
	if (!pSubStyle)
		return FALSE;
	a2w.Convert(pSubStyle->Attribute("headerctrl"));
	if (a2w.m_pStr)
		_tcscpy(pSkin->szHeaderCtrlStyle, a2w.m_pStr);

	// body
	TiXmlElement* pBody = pElm->FirstChildElement("body");
	if (!pBody)
		return FALSE;

	TiXmlElement* pColors = pBody->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("text"), pSkin->crText);
		ParseColorA(pColors->Attribute("itema"), pSkin->crItemA);
		ParseColorA(pColors->Attribute("itemb"), pSkin->crItemB);
		ParseColorA(pColors->Attribute("textselected"), pSkin->crTextSelected);
		ParseColorA(pColors->Attribute("selectbackground"), pSkin->crSelectedBackgnd);
		ParseColorA(pColors->Attribute("border"), pSkin->crBorder);
		ParseColorA(pColors->Attribute("background"), pSkin->crBackgnd);
	}
	TiXmlElement* pSkins = pBody->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pImage = pSkins->FirstChildElement("skin");
		while (pImage) {
			PELC_SKIN_IMAGE pSkinImage = NULL;
			LPCSTR lpszValue = pImage->Attribute("id");
			if (!lpszValue)
				return FALSE;
			else if (stricmp(lpszValue, "checkbox") == 0)
				pSkinImage = &pSkin->checkbox;

			if (!ParseImage(pImage, pSkinImage))
				return FALSE;

			pImage = pImage->NextSiblingElement();
		}
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseSplitters(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("splitter");
	while (pChildElm) {
		PELC_SKIN_SPLITTER pSplitter = new ELC_SKIN_SPLITTER;
		memset(pSplitter, 0, sizeof(ELC_SKIN_SPLITTER));

		if (!ParseSplitter(pChildElm, pSplitter)) {
			delete pSplitter;
			return FALSE;
		}

		m_arrSplitterSkin.push_back(pSplitter);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseSplitter(TiXmlElement* pElm, PELC_SKIN_SPLITTER pSplitter)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pSplitter->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("background"), pSplitter->crBackgnd);
		ParseColorA(pColors->Attribute("border"), pSplitter->crBorder);
	}

	// skin iamges
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pSkin = pSkins->FirstChildElement("skin");
		if (!pSkin || !ParseImage(pSkin, &pSplitter->drag))
			return FALSE;
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseSliders(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("slider");
	while (pChildElm) {
		PELC_SKIN_SLIDER pSlider = new ELC_SKIN_SLIDER;
		memset(pSlider, 0, sizeof(ELC_SKIN_SLIDER));

		if (!ParseSlider(pChildElm, pSlider)) {
			delete pSlider;
			return FALSE;
		}

		m_arrSliderSkin.push_back(pSlider);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseSlider(TiXmlElement* pElm, PELC_SKIN_SLIDER pSlider)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pSlider->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("background"), pSlider->crBackgnd);
	}

	// skin iamges
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pSkin = pSkins->FirstChildElement("skin");
		while (pSkin) {
			PELC_SKIN_IMAGE pSkinImage = NULL;
			LPCSTR lpszId = pSkin->Attribute("id");

			if (!lpszId)
				return FALSE;
			if (stricmp(lpszId, "background") == 0) 
				pSkinImage = &pSlider->background;
			else if (stricmp(lpszId, "thumb") == 0) 
				pSkinImage = &pSlider->thumb;

			if (!ParseImage(pSkin, pSkinImage))
				return FALSE;

			pSkin = pSkin->NextSiblingElement();
		}
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseStatusBars(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("statusbar");
	while (pChildElm) {
		PELC_SKIN_STATUSBAR pStatusBar = new ELC_SKIN_STATUSBAR;
		memset(pStatusBar, 0, sizeof(ELC_SKIN_STATUSBAR));

		if (!ParseStatusBar(pChildElm, pStatusBar)) {
			delete pStatusBar;
			return FALSE;
		}

		m_arrStatusBarSkin.push_back(pStatusBar);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseStatusBar(TiXmlElement* pElm, PELC_SKIN_STATUSBAR pStatusBar)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pStatusBar->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("textnormal"), pStatusBar->crTextNormal);
		ParseColorA(pColors->Attribute("textdisabled"), pStatusBar->crTextDisabled);
		ParseColorA(pColors->Attribute("paneborder"), pStatusBar->crPaneBorder);
		ParseColorA(pColors->Attribute("background"), pStatusBar->crBackgnd);
	}

	// skin iamges
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pSkin = pSkins->FirstChildElement("skin");
		while (pSkin) {
			PELC_SKIN_IMAGE pSkinImage = NULL;
			LPCSTR lpszId = pSkin->Attribute("id");

			if (!lpszId)
				return FALSE;
			if (stricmp(lpszId, "background") == 0) 
				pSkinImage = &pStatusBar->background;

			if (!ParseImage(pSkin, pSkinImage))
				return FALSE;

			pSkin = pSkin->NextSiblingElement();
		}
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseTabCtrls(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("tabctrl");
	while (pChildElm) {
		PELC_SKIN_TABCTRL pTabCtrl = new ELC_SKIN_TABCTRL;
		memset(pTabCtrl, 0, sizeof(ELC_SKIN_TABCTRL));

		if (!ParseTabCtrl(pChildElm, pTabCtrl)) {
			delete pTabCtrl;
			return FALSE;
		}

		m_arrTabCtrlSkin.push_back(pTabCtrl);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseTabCtrl(TiXmlElement* pElm, PELC_SKIN_TABCTRL pTabCtrl)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pTabCtrl->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("textnormal"), pTabCtrl->crTextNormal);
		ParseColorA(pColors->Attribute("texthot"), pTabCtrl->crTextHot);
		ParseColorA(pColors->Attribute("textactive"), pTabCtrl->crTextActive);
		ParseColorA(pColors->Attribute("background"), pTabCtrl->crBackgnd);
	}

	// layout
	TiXmlElement* pLayout = pElm->FirstChildElement("layout");
	if (pLayout) {
		LPCSTR lpszValue = pLayout->Attribute("upward");
		if (lpszValue && stricmp(lpszValue, "yes") == 0)
			pTabCtrl->bUpward = TRUE;
	}

	// skin iamges
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pSkin = pSkins->FirstChildElement("skin");
		while (pSkin) {
			PELC_SKIN_IMAGE pSkinImage = NULL;
			LPCSTR lpszId = pSkin->Attribute("id");

			if (!lpszId)
				return FALSE;
			if (stricmp(lpszId, "background") == 0) 
				pSkinImage = &pTabCtrl->background;
			else if (stricmp(lpszId, "tabitem") == 0) 
				pSkinImage = &pTabCtrl->item;
			else if (stricmp(lpszId, "borderleft") == 0) 
				pSkinImage = &pTabCtrl->borderleft;
			else if (stricmp(lpszId, "bordercenter") == 0) 
				pSkinImage = &pTabCtrl->bordercenter;
			else if (stricmp(lpszId, "borderright") == 0) 
				pSkinImage = &pTabCtrl->borderright;
			else if (stricmp(lpszId, "dropdown") == 0) 
				pSkinImage = &pTabCtrl->dropdown;
			else if (stricmp(lpszId, "buttonclose") == 0) 
				pSkinImage = &pTabCtrl->buttonclose;
			else if (stricmp(lpszId, "buttonnew") == 0) 
				pSkinImage = &pTabCtrl->buttonnew;

			if (!ParseImage(pSkin, pSkinImage))
				return FALSE;

			pSkin = pSkin->NextSiblingElement();
		}
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseWindows(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("window");
	while (pChildElm) {
		PELC_SKIN_WINDOW pWindow = new ELC_SKIN_WINDOW;
		memset(pWindow, 0, sizeof(ELC_SKIN_WINDOW));

		if (!ParseWindow(pChildElm, pWindow)) {
			delete pWindow;
			return FALSE;
		}

		m_arrWindowSkin.push_back(pWindow);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseWindow(TiXmlElement* pElm, PELC_SKIN_WINDOW pWindow)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pWindow->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("background"), pWindow->crBackgnd);
		ParseColorA(pColors->Attribute("caption"), pWindow->crCaption);
	}

	// skins
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (pSkins) {
		TiXmlElement* pImage = pSkins->FirstChildElement("skin");
		if (!pImage)
			return FALSE;
		while (pImage) {
			PELC_SKIN_IMAGE pSkinImage = NULL;
			LPCSTR lpszValue = pImage->Attribute("id");
			if (!lpszValue)
				return FALSE;
			else if (stricmp(lpszValue, "caption_min") == 0)
				pSkinImage = &pWindow->caption_min;
			else if (stricmp(lpszValue, "caption") == 0)
				pSkinImage = &pWindow->caption;
			else if (stricmp(lpszValue, "leftborder") == 0)
				pSkinImage = &pWindow->left;
			else if (stricmp(lpszValue, "rightborder") == 0)
				pSkinImage = &pWindow->right;
			else if (stricmp(lpszValue, "bottomborder") == 0)
				pSkinImage = &pWindow->bottom;

			if (pImage) {
				if (!ParseImage(pImage, pSkinImage))
					return FALSE;
			}

			pImage = pImage->NextSiblingElement();
		}
	}

	// controls
	TiXmlElement* pControls = pElm->FirstChildElement("controls");
	if (pControls) {
		if (!ParseWindowControls(pControls, pWindow))
		return FALSE;
	}

	// client
	TiXmlElement* pClient = pElm->FirstChildElement("client");
	if (pClient) {
		if (!ParseWindowClient(pClient, &pWindow->client))
			return FALSE;
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseWindowControls(TiXmlElement* pElm, PELC_SKIN_WINDOW pWindow)
{
	TiXmlElement* pControl = pElm->FirstChildElement("control");

	while (pControl) {
		LPCSTR lpszValue = pControl->Attribute("class");
		LPCSTR lpszId = pControl->Attribute("id");

		if (!lpszValue || !lpszId) {
			return FALSE;
		}
		else if (stricmp(lpszValue, "nwbutton") == 0) {
			PELC_SKIN_NOTWND_BUTTON pButton = NULL;
			if (stricmp(lpszId, "close") == 0)
				pButton = &pWindow->close;
			else if (stricmp(lpszId, "minimize") == 0)
				pButton = &pWindow->minimize;
			else if (stricmp(lpszId, "maximize") == 0)
				pButton = &pWindow->maximize;
			else if (stricmp(lpszId, "singleclose") == 0)
				pButton = &pWindow->singleclose;

			if (!ParseNWButtons(pControl, pButton))
				return FALSE;
		}
		else if (stricmp(lpszValue, "menubar") == 0) {
			if (!ParseMenuBar(pControl, &pWindow->menubar))
				return FALSE;
		}

		pControl = pControl->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseNWButtons(TiXmlElement* pElm, PELC_SKIN_NOTWND_BUTTON pNWButton)
{
	TiXmlElement* pPosElm = pElm->FirstChildElement("position");
	if (!pPosElm || !ParseAnchor(pPosElm->Attribute("rect"), pNWButton->pos.point, pNWButton->pos.size))
		return FALSE;

	TiXmlElement* pSkinElm = pElm->FirstChildElement("skin");
	if (!pSkinElm || !ParseImage(pSkinElm, &pNWButton->image))
		return FALSE;

	return TRUE;
}

BOOL CElcSkinDoc::ParseMenuBar(TiXmlElement* pElm, PELC_SKIN_MENUBAR pMenuBar)
{
	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("textnormal"), pMenuBar->crTextNormal);
		ParseColorA(pColors->Attribute("texthot"), pMenuBar->crTextHot);
		ParseColorA(pColors->Attribute("textdown"), pMenuBar->crTextDown);
	}

	// skins
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (!pSkins)
		return FALSE;
	TiXmlElement* pImage = pSkins->FirstChildElement("skin");
	if (!pImage)
		return FALSE;
	while (pImage) {
		PELC_SKIN_IMAGE pSkinImage = NULL;
		LPCSTR lpszValue = pImage->Attribute("id");
		if (!lpszValue)
			return FALSE;
		else if (stricmp(lpszValue, "background") == 0)
			pSkinImage = &pMenuBar->background;
		else if (stricmp(lpszValue, "itembk") == 0)
			pSkinImage = &pMenuBar->itembk;

		if (pImage) {
			if (!ParseImage(pImage, pSkinImage))
				return FALSE;
		}

		pImage = pImage->NextSiblingElement();
	}

	// menu style
	TiXmlElement* pSubStyles = pElm->FirstChildElement("substyles");
	if (!pSubStyles)
		return FALSE;

	CElcA2W a2w(pSubStyles->Attribute("menu"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pMenuBar->szMenuStyle, a2w.m_pStr);

	return TRUE;
}

BOOL CElcSkinDoc::ParseMenus(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("menu");
	while (pChildElm) {
		PELC_SKIN_MENU pMenu = new ELC_SKIN_MENU;
		memset(pMenu, 0, sizeof(ELC_SKIN_MENU));

		if (!ParseMenu(pChildElm, pMenu)) {
			delete pMenu;
			return FALSE;
		}

		m_arrMenuSkin.push_back(pMenu);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseMenu(TiXmlElement* pElm, PELC_SKIN_MENU pMenu)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pMenu->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("textnormal"), pMenu->crTextNormal);
		ParseColorA(pColors->Attribute("texthot"), pMenu->crTextHot);
		ParseColorA(pColors->Attribute("textdisabled"), pMenu->crTextDisabled);
		ParseColorA(pColors->Attribute("select"), pMenu->crSelected);
		ParseColorA(pColors->Attribute("selectborder"), pMenu->crSelectedBorder);
		ParseColorA(pColors->Attribute("separator"), pMenu->crSeparator);
		ParseColorA(pColors->Attribute("border"), pMenu->crBorder);
		ParseColorA(pColors->Attribute("background"), pMenu->crBackgnd);
	}

	// skins
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (!pSkins)
		return FALSE;
	TiXmlElement* pImage = pSkins->FirstChildElement("skin");
	if (!pImage)
		return FALSE;
	while (pImage) {
		PELC_SKIN_IMAGE pSkinImage = NULL;
		LPCSTR lpszValue = pImage->Attribute("id");
		if (!lpszValue)
			return FALSE;
		else if (stricmp(lpszValue, "check") == 0)
			pSkinImage = &pMenu->check;
		else if (stricmp(lpszValue, "radio") == 0)
			pSkinImage = &pMenu->radio;
		else if (stricmp(lpszValue, "arrow") == 0)
			pSkinImage = &pMenu->arrow;

		if (pImage) {
			if (!ParseImage(pImage, pSkinImage))
				return FALSE;
		}

		pImage = pImage->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseRichMenus(TiXmlElement* pElm)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("richmenu");
	while (pChildElm) {
		PELC_SKIN_RICH_MENU pMenu = new ELC_SKIN_RICH_MENU;
		memset(pMenu, 0, sizeof(ELC_SKIN_RICH_MENU));

		if (!ParseRichMenu(pChildElm, pMenu)) {
			delete pMenu;
			return FALSE;
		}

		m_arrRichMenuSkin.push_back(pMenu);

		pChildElm = pChildElm->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseRichMenu(TiXmlElement* pElm, PELC_SKIN_RICH_MENU pRichMenu)
{
	CElcA2W a2w(pElm->Attribute("style"));
	if (!a2w.m_pStr)
		return FALSE;
	_tcscpy(pRichMenu->szStyle, a2w.m_pStr);

	// colors
	TiXmlElement* pColors = pElm->FirstChildElement("colors");
	if (pColors) {
		ParseColorA(pColors->Attribute("textnormal"), pRichMenu->crTextNormal);
		ParseColorA(pColors->Attribute("texthot"), pRichMenu->crTextHot);
		ParseColorA(pColors->Attribute("textdisabled"), pRichMenu->crTextDisabled);
		ParseColorA(pColors->Attribute("separator"), pRichMenu->crSeparator);
	}

	// skins
	TiXmlElement* pSkins = pElm->FirstChildElement("skins");
	if (!pSkins)
		return FALSE;
	TiXmlElement* pImage = pSkins->FirstChildElement("skin");
	if (!pImage)
		return FALSE;
	while (pImage) {
		PELC_SKIN_IMAGE pSkinImage = NULL;
		LPCSTR lpszValue = pImage->Attribute("id");
		if (!lpszValue)
			return FALSE;
		else if (stricmp(lpszValue, "check") == 0)
			pSkinImage = &pRichMenu->check;
		else if (stricmp(lpszValue, "select") == 0)
			pSkinImage = &pRichMenu->select;
		else if (stricmp(lpszValue, "background") == 0)
			pSkinImage = &pRichMenu->backgnd;

		if (pImage) {
			if (!ParseImage(pImage, pSkinImage))
				return FALSE;
		}

		pImage = pImage->NextSiblingElement();
	}

	return TRUE;
}

BOOL CElcSkinDoc::ParseWindowClient(TiXmlElement* pElm, PELC_SKIN_CLIENT pClient)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement("colors");
	if (pChildElm) {
		ParseColorA(pChildElm->Attribute("text"), pClient->crText);
		ParseColorA(pChildElm->Attribute("background"), pClient->crBackgnd);
	}

	pChildElm = pElm->FirstChildElement("imagepane");
	if (pChildElm) {
		TiXmlElement* pSubElm = pChildElm->FirstChildElement("colors");
		if (pSubElm) {
			ParseColorA(pSubElm->Attribute("text"), pClient->pane.crText);
			ParseColorA(pSubElm->Attribute("background"), pClient->pane.crBackgnd);

			LPCSTR lpszValue = pSubElm->Attribute("fill");
			if (lpszValue && stricmp(lpszValue, "yes") == 0)
				pClient->pane.bFillBackgnd = TRUE;
		}
		
		TiXmlElement* pSkinElm = pChildElm->FirstChildElement("skin");
		if (pSkinElm && !ParseImage(pSkinElm, &pClient->pane.image))
			return FALSE;
	}

	return TRUE;
}

PELC_SKIN_GLOBAL CElcSkinDoc::GetGlobalSkin()
{
	return &m_global;
}

PELC_SKIN_PNAE_NC CElcSkinDoc::GetNCPaneSkin(LPCTSTR lpStyle)
{
	for (CPaneNCSkinArray::iterator it = m_arrPaneSkin.begin();
		it != m_arrPaneSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_PNAE_CLIENT CElcSkinDoc::GetClientPaneSkin(LPCTSTR lpStyle)
{
	for (CPaneCSkinArray::iterator it = m_arrPaneClientSkin.begin();
		it != m_arrPaneClientSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_TOOLBAR CElcSkinDoc::GetToolbarSkin(LPCTSTR lpStyle)
{
	for (CToolbarSkinArray::iterator it = m_arrToolbarSkin.begin();
		it != m_arrToolbarSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_FLOATBAR CElcSkinDoc::GetFloatBarSkin(LPCTSTR lpStyle)
{
	for (CFloatBarSkinArray::iterator it = m_arrFloatBarSkin.begin();
		it != m_arrFloatBarSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_BALLOON CElcSkinDoc::GetBalloonSkin(LPCTSTR lpStyle)
{
	for (CBalloonSkinArray::iterator it = m_arrBalloonSkin.begin();
		it != m_arrBalloonSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_PICTURE CElcSkinDoc::GetPicture(LPCTSTR lpStyle)
{
	for (CPictureSkinArray::iterator it = m_arrPicture.begin();
		it != m_arrPicture.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_BUTTON CElcSkinDoc::GetButtonSkin(LPCTSTR lpStyle)
{
	for (CButtonSkinArray::iterator it = m_arrButtonSkin.begin();
		it != m_arrButtonSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_HEADERCTRL CElcSkinDoc::GetHeaderCtrlSkin(LPCTSTR lpStyle)
{
	for (CHeaderCtrlSkinArray::iterator it = m_arrHeaderCtrlSkin.begin();
		it != m_arrHeaderCtrlSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_LIST CElcSkinDoc::GetListSkin(LPCTSTR lpStyle)
{
	for (CListSkinArray::iterator it = m_arrListSkin.begin();
		it != m_arrListSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_SPLITTER CElcSkinDoc::GetSplitterSkin(LPCTSTR lpStyle)
{
	for (CSplitterSkinArray::iterator it = m_arrSplitterSkin.begin();
		it != m_arrSplitterSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_SLIDER CElcSkinDoc::GetSliderSkin(LPCTSTR lpStyle)
{
	for (CSliderSkinArray::iterator it = m_arrSliderSkin.begin();
		it != m_arrSliderSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_STATUSBAR CElcSkinDoc::GetStatusBarSkin(LPCTSTR lpStyle)
{
	for (CStatusBarSkinArray::iterator it = m_arrStatusBarSkin.begin();
		it != m_arrStatusBarSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_TABCTRL CElcSkinDoc::GetTabCtrlSkin(LPCTSTR lpStyle)
{
	for (CTabCtrlSkinArray::iterator it = m_arrTabCtrlSkin.begin();
		it != m_arrTabCtrlSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_MENU CElcSkinDoc::GetMenuSkin(LPCTSTR lpStyle)
{
	for (CMenuSkinArray::iterator it = m_arrMenuSkin.begin();
		it != m_arrMenuSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_RICH_MENU CElcSkinDoc::GetRichMenuSkin(LPCTSTR lpStyle)
{
	for (CRichMenuArray::iterator it = m_arrRichMenuSkin.begin();
		it != m_arrRichMenuSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

PELC_SKIN_WINDOW CElcSkinDoc::GetWindowSkin(LPCTSTR lpStyle)
{
	for (CWindowSkinArray::iterator it = m_arrWindowSkin.begin();
		it != m_arrWindowSkin.end();
		it ++)
	{
		if (_tcsicmp((*it)->szStyle, lpStyle) == 0)
			return (*it);
	}

	return NULL;
}

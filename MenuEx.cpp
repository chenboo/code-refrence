#include "stdafx.h"
#include "MenuEx.h"

IMPLEMENT_DYNAMIC( CMenuEx, CMenu )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMenuEx::CMenuEx()
{
	m_bPopupMenu=FALSE;					// initialize this menu flag
										// FALSE indicate this menu is top-level menu
										// TRUE means this menu is popupmenu

	m_nSeparator = 10;					// separator menu item 's height

	// initialize color attribute
	m_crBackground = ::GetSysColor(COLOR_MENU);	
	m_crText =::GetSysColor(COLOR_MENUTEXT);
	m_crTextSelected =::GetSysColor(COLOR_MENUTEXT);
	m_crLeft=RGB(236,233,216);
	m_crSelectedBroder=RGB(49,106,197);
	m_crSelectedFill=RGB(200,220,253);

	// no normal imagelist
	m_pImageList=NULL;					
	m_pDisabledImageList=NULL;			
	m_pHotImageList=NULL;		
	
	// default menu image size
	m_szImage=CSize(20,20);				
}
/////////////////////////////////////////////////////////////////////////////
CMenuEx::~CMenuEx()
{	
	// destroy menu and free all variable
	DestroyMenu();	
}
/////////////////////////////////////////////////////////////////////////////

BOOL CMenuEx::DestroyMenu()
{	
	// delete all imagelist
	if(m_pImageList!=NULL)
		m_pImageList->DeleteImageList();

	if(m_pDisabledImageList!=NULL)
		m_pDisabledImageList->DeleteImageList();

	if(m_pHotImageList!=NULL)
		m_pHotImageList->DeleteImageList();		


	for(int i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		MENUITEM *pMenuItem = m_MenuItemArr.GetAt(i);
		
		// destroy submenu
		if(pMenuItem->bIsSubMenu)
			pMenuItem->pSubMenu->DestroyMenu(); 		
		
		// remove menu item from array
		m_MenuItemArr.RemoveAt(i);
		
		// free menuitem struct
		delete pMenuItem;
	}	
	return CMenu::DestroyMenu();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMenuEx::LoadMenu(UINT nIDResource)
{
	// load menu from menu editor created resouce
	CMenu Menu;
	Menu.LoadMenu(nIDResource);

	// empty all variable
	if(m_hMenu!=NULL)	
		DestroyMenu();	
		
	if(m_bPopupMenu)
	{
		// create popupmenu
		if(!CreatePopupMenu()) 
			return FALSE;	
	}
	else
	{
		// create top-level menu  
		if(!CreateMenu()) 
			return FALSE; 
	}		
	return LoadMenu(Menu);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMenuEx::LoadMenu(HMENU hMenu)
{
	// load menu from global HMENU handle
	CMenu Menu;
	Menu.Attach(hMenu);
	
	// empty all variable
	if(m_hMenu!=NULL)	
		DestroyMenu();	
	
	if(m_bPopupMenu)
	{
		// create popupmenu
		if(!CreatePopupMenu()) 
			return FALSE;	
	}
	else
	{
		// create top-level menu  
		if(!CreateMenu()) 
			return FALSE; 
	}	
	return LoadMenu(Menu);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMenuEx::LoadMenu(LPCTSTR lpszResourceName)
{
	// load menu from resource string
	CMenu Menu;
	Menu.LoadMenu(lpszResourceName);
	
	// empty all variable
	if(m_hMenu!=NULL)	
		DestroyMenu();	
	
	if(m_bPopupMenu)
	{
		// create popupmenu
		if(!CreatePopupMenu()) 
			return FALSE;	
	}
	else
	{
		// create top-level menu  
		if(!CreateMenu()) 
			return FALSE; 
	}	
	return LoadMenu(Menu);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMenuEx::LoadMenu(CMenu& Menu)
{	
	UINT nID;
	UINT nIndex;	
	UINT nState;
	CString strMenu;

	// fill this menu use temp menu
	for(nIndex = 0; nIndex < Menu.GetMenuItemCount(); nIndex++)
	{		
		nID=Menu.GetMenuItemID(nIndex);					// get menu ID
		nState=Menu.GetMenuState(nIndex,MF_BYPOSITION);	// get menu state
		Menu.GetMenuString(nIndex,strMenu,MF_BYPOSITION);// get menu name
		
		if(nID==0)
		{	
			// insert separator
			if(!AppendMenu(MF_SEPARATOR))
				return FALSE;
		}
		else if(nID==-1)
		{
			// bypass popupmenu's item number	
			nState&=0x00FFL;	
			
			// insert popupmenu
			if(!AppendMenu(MF_POPUP|nState,(UINT)Menu.GetSubMenu(nIndex)->Detach(),strMenu))
				return FALSE;
		}
		else
		{		
			// insert normal menu item
			if(!AppendMenu(MF_STRING|nState,nID,strMenu))
				return FALSE;
		}
	}		
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMenuEx::AppendMenu(UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem )
{	
	// append ownerdrawn style menu item to this menu
	// get last position of this menu
	UINT nIndex=m_MenuItemArr.GetSize();
	
	// insert menu item to last position
	// means append menu item
	return InsertMenu(nIndex,nFlags|MF_BYPOSITION,nIDNewItem,lpszNewItem);	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMenuEx::InsertMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem , LPCTSTR lpszNewItem )
{
	// insert menu item to specific position or command ID
	LPMENUITEM pMenuItem;
	UINT nIndex;
	CSize  size;	
	
	// mainframe's device context,use to get text size
	CDC	 *pDC =AfxGetMainWnd()->GetDC();

	if(nFlags & MF_BYPOSITION)
	{
		// insert menu item by position
		pMenuItem=new MENUITEM;

		if(nFlags&MF_SEPARATOR)
		{
			// insert separator
			pMenuItem->strText = "";						// no name
			pMenuItem->nID = 0;								// separator's ID
			pMenuItem->itemSize=CSize(0,m_nSeparator);		// separator's default size
			pMenuItem->pImageList=NULL;						// separator has no image
			pMenuItem->pDisabledImageList=NULL;
			pMenuItem->pHotImageList=NULL;
			pMenuItem->nImageIndex=-1;
			pMenuItem->bIsSubMenu=FALSE;					// separator must not be popupmenu
			pMenuItem->pSubMenu=NULL;				
			
			// insert menu item to this menu
			if(!CMenu::InsertMenu(nPosition,nFlags,nIDNewItem,lpszNewItem))
			{
				delete pMenuItem;
				AfxGetMainWnd()->ReleaseDC(pDC);
				return FALSE;
			}
			
			if(m_bPopupMenu)
			{			
				// modify menu item to ownerdrawn style
				// if this menu is not a top-level menu
				if(!CMenu::ModifyMenu(nPosition,MF_SEPARATOR|MF_BYPOSITION|MF_OWNERDRAW,0,(LPCTSTR)pMenuItem))
				{
					delete pMenuItem;
					AfxGetMainWnd()->ReleaseDC(pDC);
					return FALSE;
				}
			}			
			// insert menu item to array
			m_MenuItemArr.InsertAt(nPosition,pMenuItem);
		}
		else if(nFlags&MF_POPUP)
		{
			// insert popupmenu
			CMenuEx* pSubMenu=new CMenuEx;

			// create new popupmenu
			pSubMenu->SetPopupMenu(TRUE);

			// load popupmenu from specific handle
			pSubMenu->LoadMenu((HMENU)nIDNewItem);
			size=pDC->GetTextExtent(lpszNewItem);

			pMenuItem->strText = lpszNewItem;
			pMenuItem->nID = -1;	
			
			// specified menu item size,do not include image's size
			pMenuItem->itemSize.cx=size.cx+25;
			pMenuItem->itemSize.cy=2;
			
			// specified all imagelist for new menu item
			pMenuItem->pImageList=m_pImageList;		
			pMenuItem->pDisabledImageList=m_pDisabledImageList;
			pMenuItem->pHotImageList=m_pHotImageList;
			pMenuItem->nImageIndex=-1;
			pMenuItem->bIsSubMenu=TRUE;
			pMenuItem->pSubMenu=pSubMenu;		// popupmenu's pointor				
			
			if(!CMenu::InsertMenu(nPosition,nFlags,(UINT)(pSubMenu->m_hMenu),lpszNewItem))
			{
				delete pMenuItem;
				AfxGetMainWnd()->ReleaseDC(pDC);
				return FALSE;
			}
			
			if(m_bPopupMenu)
			{			
				if(!CMenu::ModifyMenu(nPosition,nFlags|MF_POPUP|MF_BYPOSITION|MF_OWNERDRAW,(UINT)(pSubMenu->m_hMenu),(LPCTSTR)pMenuItem))
				{
					delete pMenuItem;
					AfxGetMainWnd()->ReleaseDC(pDC);
					return FALSE;
				}
			}			
			m_MenuItemArr.InsertAt(nPosition,pMenuItem);
		}
		else // if(nFlags & MF_STRING)
		{
			// insert normal menu item
			size=pDC->GetTextExtent(lpszNewItem);

			pMenuItem->strText = lpszNewItem;
			pMenuItem->nID = nIDNewItem;			
			pMenuItem->itemSize.cx=size.cx+10;
			pMenuItem->itemSize.cy=2;		
			pMenuItem->pImageList=m_pImageList;		
			pMenuItem->pDisabledImageList=m_pDisabledImageList;
			pMenuItem->pHotImageList=m_pHotImageList;
			pMenuItem->nImageIndex=-1;
			pMenuItem->bIsSubMenu=FALSE;
			pMenuItem->pSubMenu=NULL;					
			
			if(!CMenu::InsertMenu(nPosition,nFlags,nIDNewItem,lpszNewItem))
			{
				delete pMenuItem;
				AfxGetMainWnd()->ReleaseDC(pDC);
				return FALSE;
			}			
			if(m_bPopupMenu)
			{			
				if(!CMenu::ModifyMenu(nPosition,MF_BYPOSITION|MF_OWNERDRAW,nIDNewItem,(LPCTSTR)pMenuItem))
				{
					delete pMenuItem;
					AfxGetMainWnd()->ReleaseDC(pDC);
					return FALSE;
				}
			}
			m_MenuItemArr.InsertAt(nPosition,pMenuItem);
		}			
		return TRUE;
	}
	else if(nFlags & MF_BYCOMMAND)		// insert menu item by command ID
	{		
		for(nIndex = 0; nIndex < (UINT)m_MenuItemArr.GetSize(); nIndex++)
		{
			pMenuItem=m_MenuItemArr.GetAt(nIndex);
			
			// insert menu item with position
			// if the position match command ID
			if(pMenuItem->nID == nPosition)
				return InsertMenu(nIndex,MF_BYPOSITION,nIDNewItem,lpszNewItem);					
		}

		// do not found menu item in this menu
		CMenuEx* pSubMenu=FindPopupMenuFromID(nPosition);

		if(pSubMenu==NULL)
		{
			// not found 
			AfxGetMainWnd()->ReleaseDC(pDC);
			return FALSE;
		}
		else
		{
			// insert menu item to submenu
			return pSubMenu->InsertMenu(nPosition,nFlags,nIDNewItem ,lpszNewItem);
		}		
	}	

	AfxGetMainWnd()->ReleaseDC(pDC);
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////

BOOL CMenuEx::RemoveMenu(UINT nPosition, UINT nFlags)
{	
	UINT nIndex;
	LPMENUITEM pMenuItem;

	// remove menu item by position
	if(nFlags & MF_BYPOSITION)
	{
		// position out of range of menu item array
		if(nPosition>(UINT)(m_MenuItemArr.GetSize()-1))
			return FALSE;

		// remove menu item from menu item array
		m_MenuItemArr.RemoveAt(nPosition);
				
		// remvoe menu item from this menu
		if(!CMenu::RemoveMenu(nPosition,nFlags))
			return FALSE;
	}
	else if(nFlags & MF_BYCOMMAND)
	{
		// remove menu item by command ID
		for(nIndex = 0; nIndex < (UINT)m_MenuItemArr.GetSize(); nIndex++)
		{
			pMenuItem = m_MenuItemArr.GetAt(nIndex);
			
			// remove menu item by position match command ID
			if(pMenuItem->nID == nPosition)			
				return RemoveMenu(nIndex,MF_BYPOSITION);						
		}	
		
		// found menu item from submenu of this menu
		CMenuEx* pSubMenu=FindPopupMenuFromID(nPosition);		
		if(pSubMenu==NULL)					
			return FALSE;		// not found
		else
			return pSubMenu->RemoveMenu(nIndex,MF_BYPOSITION);	// remove menu item from submenu		
	}
	
	return FALSE;	
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMenuEx::ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem)
{
	UINT nIndex;
	LPMENUITEM pMenuItem;
	CDC	 *pDC =AfxGetMainWnd()->GetDC();	
	CSize  size;

	// modify menu item by position
	if(nFlags & MF_BYPOSITION)
	{
		if(!CMenu::ModifyMenu(nPosition,nFlags,nIDNewItem,lpszNewItem))
		{	
			// fail to modify menu
			AfxGetMainWnd()->ReleaseDC(pDC);
			return FALSE;
		}				
		pMenuItem=GetMenuItem(nPosition);
		
		if(pMenuItem==NULL)
			return FALSE;

		if(nFlags&MF_SEPARATOR)
		{
			// modify menu item to separator
			pMenuItem->strText = "";
			pMenuItem->nID = 0;		
			pMenuItem->itemSize=CSize(0,m_nSeparator);
			pMenuItem->pImageList=m_pImageList;		
			pMenuItem->pDisabledImageList=m_pDisabledImageList;
			pMenuItem->pHotImageList=m_pHotImageList;
			pMenuItem->nImageIndex=-1;
			pMenuItem->bIsSubMenu=FALSE;
			pMenuItem->pSubMenu=NULL;			
		}
		else if(nFlags&MF_POPUP)
		{
			// modify menu item to popupmenu item
			CMenuEx* pSubMenu=new CMenuEx;
			pSubMenu->SetPopupMenu(TRUE);
			pSubMenu->LoadMenu((HMENU)nIDNewItem);

			pMenuItem->strText = lpszNewItem;
			pMenuItem->nID = -1;
			size=pDC->GetTextExtent(lpszNewItem);
			pMenuItem->itemSize.cx=size.cx+25;
			pMenuItem->itemSize.cy=2;		
			pMenuItem->pImageList=m_pImageList;		
			pMenuItem->pDisabledImageList=m_pDisabledImageList;
			pMenuItem->pHotImageList=m_pHotImageList;
			pMenuItem->nImageIndex=-1;
			pMenuItem->bIsSubMenu=TRUE;
			pMenuItem->pSubMenu=pSubMenu;					
		}
		else // MF_STRING
		{
			// modify menu item to normal menu item
			pMenuItem->strText = lpszNewItem;
			pMenuItem->nID = nIDNewItem;

			size=pDC->GetTextExtent(lpszNewItem);
			pMenuItem->itemSize.cx=size.cx+10;
			pMenuItem->itemSize.cy=2;	
			pMenuItem->pImageList=m_pImageList;		
			pMenuItem->pDisabledImageList=m_pDisabledImageList;
			pMenuItem->pHotImageList=m_pHotImageList;			
			pMenuItem->nImageIndex=-1;
			pMenuItem->bIsSubMenu=FALSE;
			pMenuItem->pSubMenu=NULL;			
		}			
		return TRUE;		
	}
	else if(nFlags & MF_BYCOMMAND)
	{
		// modify menu item by command ID
		for(nIndex = 0; nIndex < (UINT)m_MenuItemArr.GetSize(); nIndex++)
		{
			pMenuItem=m_MenuItemArr.GetAt(nIndex);	
			
			// modify ID matched menu item by position
			if(pMenuItem->nID == nPosition)						
				return ModifyMenu(nIndex,MF_BYPOSITION,nIDNewItem,lpszNewItem);							
		}
		
		// walk through all items, looking for ID matched popupmenu
		CMenuEx* pSubMenu=FindPopupMenuFromID(nPosition);
		
		if(pSubMenu==NULL)					
			return FALSE;		// not found	
		else		
			return pSubMenu->ModifyMenu(nPosition,nFlags,nIDNewItem,lpszNewItem);			
	}		
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CMenuEx* CMenuEx::GetSubMenu(int nPosition)
{
	// get submenu by position based zero
	// return NULL if do not exist
	if((UINT)nPosition>(m_MenuItemArr.GetSize()-1))
		return NULL;
	
	return m_MenuItemArr.GetAt(nPosition)->pSubMenu;
}
/////////////////////////////////////////////////////////////////////////////
CMenuEx* CMenuEx::FindPopupMenuFromID(UINT nID)
{
	ASSERT_VALID(this);
	
	// walk through all items, looking for ID match
	UINT nItems = GetMenuItemSize();
	for (int iItem = 0; iItem < (int)nItems; iItem++)
	{
		CMenuEx* pPopup = GetSubMenu(iItem);
		if (pPopup != NULL)
		{
			// recurse to child popup
			pPopup = FindPopupMenuFromID(nID);
			// check popups on this popup
			if (pPopup != NULL)
				return pPopup;
		}
		else if (GetMenuItemID(iItem) == nID)
		{
			// it is a normal item inside this popup			
			return this;
		}
	}
	// not found
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	// chech this is ownerdrawn menu or not
	if(lpDIS->CtlType!=ODT_MENU)
		return;	

	// drawn device context for draw menu
	CDC* pDC=CDC::FromHandle(lpDIS->hDC);
	LPMENUITEM lpMenuItem=(LPMENUITEM)lpDIS->itemData;

	// rect region of device context
	CRect rect=CRect(lpDIS->rcItem);	
	
	// draw background of menu item
	DrawBackground(pDC,rect);	
	
	// draw selected 	
	DrawSelected(pDC,rect,lpDIS);		
	
	// draw menu text
	DrawMenuText(pDC,rect,lpDIS);
	
	//draw menu imagelist
	DrawMenuImage(pDC,rect,lpDIS);		
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::DrawBackground(CDC* pDC,CRect rect)
{
	CBrush BGBrush(m_crBackground);
	CBrush LeftBrush(m_crLeft);
	CRect rcLeft=rect;	

	rcLeft.right = rcLeft.left+m_szImage.cx+2;
	
	pDC->FillRect(rect,&BGBrush);
	pDC->FillRect(rcLeft,&LeftBrush);
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::DrawMenuImage(CDC *pDC, CRect rect,LPDRAWITEMSTRUCT lpDIS)
{
	CRect rcImage=rect;
	LPMENUITEM lpMenuItem=(LPMENUITEM)lpDIS->itemData;
	
	rcImage.left+=1;
	rcImage.right=rcImage.left+m_szImage.cx;

	rcImage.top+=1;
	rcImage.bottom=rcImage.top+m_szImage.cy;	

	// menu item style is checked
	// draw "¡Ì" instead of image
	if(lpDIS->itemState & ODS_CHECKED)
	{	
		CString check("¡Ì");
		CPen* pBorderPen=new CPen(PS_SOLID,1,m_crSelectedBroder);
		CBrush* pFillBrush=new CBrush(m_crSelectedFill);
		CPen* pOldPen= pDC->SelectObject(pBorderPen);
		CBrush* pOldBrush= pDC->SelectObject(pFillBrush);
	
		pDC->Rectangle(rcImage);		
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		
		CSize size=pDC->GetTextExtent(check);
		rcImage.left=rcImage.left+(rcImage.Width()-size.cx)/2;
		rcImage.top=rcImage.top+(rcImage.Height()-size.cy)/2-4;

		pDC->SetBkMode(TRANSPARENT);

		if(lpDIS->itemState & ODS_GRAYED)
		{	
			// draw grayed "¡Ì"
			pDC->DrawState(rcImage.TopLeft(), rcImage.Size(),check, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
		}
		else
		{	
			// set text color for different text style
			if(lpDIS->itemState & ODS_SELECTED)			
				pDC->SetTextColor(m_crTextSelected);			
			else			
				pDC->SetTextColor(m_crText);
			
			// draw normal "¡Ì"
			pDC->DrawText(check,rcImage,DT_EXPANDTABS|DT_VCENTER|DT_SINGLELINE);
		}		
		pDC->SetBkMode(OPAQUE);
	}
	else
	{
		// draw imagelist of menu item
		// if menu item has image(nImageIndex!=-1)
		if(lpMenuItem->nImageIndex !=-1)
		{			
			CImageList* pImageList=lpMenuItem->pImageList;			
			
			if((lpDIS->itemState&ODS_GRAYED)&&(lpMenuItem->pDisabledImageList!=NULL))
			{
				// draw disabled image
				pImageList=lpMenuItem->pDisabledImageList;
			}
			else if((lpDIS->itemState&ODS_SELECTED)&&(lpMenuItem->pHotImageList!=NULL))
			{	
				// draw selected image
				pImageList=lpMenuItem->pHotImageList;
			}							
			
			// draw NO.nImageIndex image of imagelist
			pImageList->Draw(pDC,lpMenuItem->nImageIndex,rcImage.TopLeft(),ILD_TRANSPARENT);		
		}
	}	
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::DrawMenuText(CDC *pDC, CRect rect,LPDRAWITEMSTRUCT lpDIS)
{
	CRect rcText=rect;
	LPMENUITEM lpMenuItem=(LPMENUITEM)lpDIS->itemData;	
	rcText.left = rcText.left+m_szImage.cx+2+8;	
	
	// not use background color when draw text
	pDC->SetBkMode(TRANSPARENT);
	
	if(lpMenuItem->nID==0)
	{
		// draw separator
		rcText.top =rcText.Height()/2+rcText.top-1;
		rcText.bottom =rcText.top +1;
		pDC->FillSolidRect(rcText,RGB(197,194,184));		
	}
	else
	{	
		// calculate text position
		CSize size=pDC->GetTextExtent(lpMenuItem->strText);		
		rcText.top=rcText.top+(rcText.Height()-size.cy)/2;

		if(lpDIS->itemState & ODS_GRAYED)
		{	
			// draw grayed text
			pDC->DrawState(rcText.TopLeft(), rcText.Size(), lpMenuItem->strText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
		}
		else
		{
			// set text color
			if(lpDIS->itemState & ODS_SELECTED)			
				pDC->SetTextColor(m_crTextSelected);			
			else			
				pDC->SetTextColor(m_crText);				
			
			// draw normal text
			pDC->DrawText(lpMenuItem->strText,rcText,DT_LEFT | DT_EXPANDTABS | DT_VCENTER);
		}
	}	
	pDC->SetBkMode(OPAQUE);
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::DrawSelected(CDC *pDC, CRect rect,LPDRAWITEMSTRUCT lpDIS)
{
	CRect rcText=rect;
	LPMENUITEM lpMenuItem=(LPMENUITEM)lpDIS->itemData;

	// separator,grayed menu item,disabled menu item
	// can not be selected
	if((lpMenuItem->nID==0)||(lpDIS->itemState & ODS_GRAYED)||(lpDIS->itemState & ODS_DISABLED))
		return;	
	// draw selected recatangle use special color
	// if current menu item is selected
	if(lpDIS->itemState & ODS_SELECTED )
	{
		CPen* pBorderPen=new CPen(PS_SOLID,1,m_crSelectedBroder);
		CBrush* pFillBrush=new CBrush(m_crSelectedFill);		
		CPen* pOldPen= pDC->SelectObject(pBorderPen);
		CBrush* pOldBrush= pDC->SelectObject(pFillBrush);
		
		pDC->Rectangle(rect);		// draw selected rectangle

		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::SetImageList(CImageList* pImageList)
{
	if(pImageList==NULL)
		return;		

	IMAGEINFO info;
	LPMENUITEM lpMenuItem;	
	m_pImageList =pImageList;
	
	// get imagelist size
	pImageList->GetImageInfo(0,&info);	
	m_szImage.cx=info.rcImage.right-info.rcImage.left;	
	m_szImage.cy=info.rcImage.bottom-info.rcImage.top;

	for(int i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		lpMenuItem=m_MenuItemArr.GetAt(i);

		// set normal imagelist for menu item
		lpMenuItem->pImageList=pImageList;
		
		// set submenu imagelist
		if(lpMenuItem->bIsSubMenu)		
			lpMenuItem->pSubMenu->SetImageList(pImageList);				
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::SetDisabledImageList(CImageList* pImageList)
{
	if(pImageList==NULL)
		return;
	
	LPMENUITEM lpMenuItem;
	// update disabled imagelist
	m_pDisabledImageList =pImageList;
	
	if(m_pImageList==NULL&&m_pHotImageList==NULL)
	{		
		IMAGEINFO info;
		
		// only set disabled imagelist
		// get imagelist size
		pImageList->GetImageInfo(0,&info);		
		m_szImage.cx=info.rcImage.right-info.rcImage.left;	
		m_szImage.cy=info.rcImage.bottom-info.rcImage.top;
	}
	
	for(int i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		lpMenuItem=m_MenuItemArr.GetAt(i);
		lpMenuItem->pDisabledImageList=pImageList;
		
		// set submenu disabled imagelis
		if(lpMenuItem->bIsSubMenu)		
			lpMenuItem->pSubMenu->SetDisabledImageList(pImageList);
				
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::SetHotImageList(CImageList* pImageList)
{
	if(pImageList==NULL)
		return;
	
	LPMENUITEM lpMenuItem;
	m_pHotImageList =pImageList;
	
	if(m_pImageList==NULL&&m_pDisabledImageList==NULL)
	{		
		IMAGEINFO info;		

		// only hot imagelist is set
		// get imagelist size
		pImageList->GetImageInfo(0,&info);			
		m_szImage.cx=info.rcImage.right-info.rcImage.left;	
		m_szImage.cy=info.rcImage.bottom-info.rcImage.top;
	}

	for(int i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		lpMenuItem=m_MenuItemArr.GetAt(i);
		lpMenuItem->pHotImageList=pImageList;
		
		// set submenu hot imagelis
		if(lpMenuItem->bIsSubMenu)		
			lpMenuItem->pSubMenu->SetHotImageList(pImageList);		
	}	
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::SetImageIndex(const UINT* nIDResource,UINT nIDCount)
{
	// set imagelist index from specific table "nIDResource[nIDCount]"
	// only can SetImageIndex after set one imagelist
	if(m_pImageList==NULL&&m_pHotImageList==NULL&&m_pDisabledImageList==NULL)
		return;

	LPMENUITEM lpMenuItem;
	for(UINT nIndex=0;nIndex<(UINT)m_MenuItemArr.GetSize();nIndex++)
	{
		lpMenuItem=m_MenuItemArr.GetAt(nIndex);

		// call submenu 's loadtoolbar
		if(lpMenuItem->bIsSubMenu)		
			lpMenuItem->pSubMenu->SetImageIndex(nIDResource,nIDCount);		
		else
		{
			for(UINT i=0;i<nIDCount;i++)
			{
				// if table's commandID equal menu item's command ID
				// set nImageIndex to table's index
				if(lpMenuItem->nID==nIDResource[i])
					lpMenuItem->nImageIndex=i;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::LoadToolBar(const CToolBar* pToolBar)
{
	
	// load all imagelist from pToolBar
	SetImageList(pToolBar->GetToolBarCtrl().GetImageList());
	SetDisabledImageList(pToolBar->GetToolBarCtrl().GetDisabledImageList());
	SetHotImageList(pToolBar->GetToolBarCtrl().GetHotImageList());
	
	// pToolBar don't have imagelist
	if(m_pImageList==NULL&&m_pDisabledImageList==NULL&&m_pHotImageList==NULL)
		return;

	LPMENUITEM pItem;

	for(UINT nIndex=0;nIndex<(UINT)m_MenuItemArr.GetSize();nIndex++)
	{
		pItem=m_MenuItemArr.GetAt(nIndex);		
		
		// load submenu toolbar
		if(pItem->bIsSubMenu)		
			pItem->pSubMenu->LoadToolBar(pToolBar);		
		else
		{	
			for(UINT i=0;i<(UINT)pToolBar->GetToolBarCtrl().GetButtonCount();i++)
			{				 
				if(pItem->nID==pToolBar->GetItemID(i))
				{
					// if toolbar's commandID equal menu item's command ID
					// set nImageIndex to toolbar's index
					pItem->nImageIndex=i;

					// set all imagelist to toolbar's imagelist
					pItem->pImageList=m_pImageList;
					pItem->pDisabledImageList=m_pDisabledImageList;
					pItem->pHotImageList=m_pHotImageList;						
				}
			}
		}		
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::SetTextColor(COLORREF color)
{
	LPMENUITEM lpMenuItem;
	// set normal menu text color
	m_crText =color;
	for(int i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		lpMenuItem=m_MenuItemArr.GetAt(i);
		
		// set submenu text color
		if(lpMenuItem->bIsSubMenu)
			lpMenuItem->pSubMenu->SetTextColor(color);
	}
}
/////////////////////////////////////////////////////////////////////////////

void CMenuEx::SetBackgroundColor(COLORREF color)
{
	LPMENUITEM lpMenuItem;
	// set menu background color
	m_crBackground =color;
	for(int i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		lpMenuItem=m_MenuItemArr.GetAt(i);
		if(lpMenuItem->bIsSubMenu)
			lpMenuItem->pSubMenu->SetBackgroundColor(color);
	}
}
/////////////////////////////////////////////////////////////////////////////

void CMenuEx::SetTextSelectedColor(COLORREF color)
{
	LPMENUITEM lpMenuItem;
	// set selected menu text color
	m_crTextSelected =color;
	for(int i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		lpMenuItem=m_MenuItemArr.GetAt(i);
		if(lpMenuItem->bIsSubMenu)
			lpMenuItem->pSubMenu->SetTextSelectedColor(color);	
	}
}
/////////////////////////////////////////////////////////////////////////////

void CMenuEx::SetLeftColor(COLORREF color)
{
	LPMENUITEM lpMenuItem;
	// set menu left side color
	m_crLeft =color;
	for(int i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		lpMenuItem=m_MenuItemArr.GetAt(i);
		if(lpMenuItem->bIsSubMenu)
			lpMenuItem->pSubMenu->SetLeftColor(color);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::SetSelectedBroderColor(COLORREF color)
{
	LPMENUITEM lpMenuItem;
	// set selected rectangle border color
	m_crSelectedBroder =color;
	for(int i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		lpMenuItem=m_MenuItemArr.GetAt(i);
		if(lpMenuItem->bIsSubMenu)
			lpMenuItem->pSubMenu->SetSelectedBroderColor(color);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::SetSelectedFillColor(COLORREF color)
{
	LPMENUITEM lpMenuItem;
	// set selected rectangle fill color
	m_crSelectedFill =color;
	for(int i = 0; i < m_MenuItemArr.GetSize(); i++)
	{
		lpMenuItem=m_MenuItemArr.GetAt(i);
		if(lpMenuItem->bIsSubMenu)
			lpMenuItem->pSubMenu->SetSelectedFillColor(color);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::SetPopupMenu(BOOL bPopupMenu)
{
	// set top-level menu indicator	
	m_bPopupMenu=bPopupMenu;
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::UpdatePopupMenu()
{	
	// top-level menu do not need update popupmenu
	if(!m_bPopupMenu)
		return;
	
	LPMENUITEM pMenuItem;
	UINT nID,nState,nIndex;
	CSize  size;
	CString strMenu;
	UINT nSize=GetMenuItemCount();	
	CDC	 *pDC =AfxGetMainWnd()->GetDC();

	for(nIndex=0;nIndex<nSize;nIndex++)
	{
		nID=GetMenuItemID(nIndex);					// get menu ID
		nState=GetMenuState(nIndex,MF_BYPOSITION);	// get menu state
		GetMenuString(nIndex,strMenu,MF_BYPOSITION);// get menu name
		pMenuItem=GetMenuItem(nIndex);	
		
		if((pMenuItem==NULL)||(pMenuItem->nID!=nID))
		{
			pMenuItem=new MENUITEM;			
			
			if(nID==0)
			{	
				// insert separator
				pMenuItem->strText = "";						// no name
				pMenuItem->nID = 0;								// separator's ID
				pMenuItem->itemSize=CSize(0,m_nSeparator);		// separator's default size
				pMenuItem->pImageList=NULL;						// separator has no image
				pMenuItem->pDisabledImageList=NULL;
				pMenuItem->pHotImageList=NULL;
				pMenuItem->nImageIndex=-1;
				pMenuItem->bIsSubMenu=FALSE;					// separator must not be popupmenu
				pMenuItem->pSubMenu=NULL;				
				
				// modify menu item to ownerdrawn style
				// if this menu is not a top-level menu
				CMenu::ModifyMenu(nIndex,MF_SEPARATOR|MF_BYPOSITION|MF_OWNERDRAW,0,(LPCTSTR)pMenuItem);
				
				// insert menu item to array
				m_MenuItemArr.InsertAt(nIndex,pMenuItem);					
			}
			else if(nID==-1)
			{
				// bypass popupmenu's item number	
				nState&=0x00FFL;	
				// insert popupmenu
				CMenuEx* pSubMenu=new CMenuEx;				
				// create new popupmenu
				pSubMenu->SetPopupMenu(TRUE);
				
				// load popupmenu from specific handle
				pSubMenu->LoadMenu((HMENU)(CMenu::GetSubMenu(nIndex)->Detach()));
				size=pDC->GetTextExtent(strMenu);
				
				pMenuItem->strText = strMenu;
				pMenuItem->nID = -1;					
				// specified menu item size,do not include image's size
				pMenuItem->itemSize.cx=size.cx+25;
				pMenuItem->itemSize.cy=2;				
				pMenuItem->pImageList=m_pImageList;		
				pMenuItem->pDisabledImageList=m_pDisabledImageList;
				pMenuItem->pHotImageList=m_pHotImageList;
				pMenuItem->nImageIndex=-1;
				pMenuItem->bIsSubMenu=TRUE;
				pMenuItem->pSubMenu=pSubMenu;		// popupmenu's pointor				
				
				CMenu::ModifyMenu(nIndex,nState|MF_POPUP|MF_BYPOSITION|MF_OWNERDRAW,(UINT)(pSubMenu->m_hMenu),(LPCTSTR)pMenuItem);
				
				m_MenuItemArr.InsertAt(nIndex,pMenuItem);
			}
			else
			{					
				// insert normal menu item
				size=pDC->GetTextExtent(strMenu);
				
				pMenuItem->strText = strMenu;
				pMenuItem->nID = nID;			
				pMenuItem->itemSize.cx=size.cx+10;
				pMenuItem->itemSize.cy=2;		
				pMenuItem->pImageList=m_pImageList;		
				pMenuItem->pDisabledImageList=m_pDisabledImageList;
				pMenuItem->pHotImageList=m_pHotImageList;
				pMenuItem->nImageIndex=-1;
				pMenuItem->bIsSubMenu=FALSE;
				pMenuItem->pSubMenu=NULL;
				
				CMenu::ModifyMenu(nIndex,nState|MF_BYPOSITION|MF_OWNERDRAW,nID,(LPCTSTR)pMenuItem);
				
				m_MenuItemArr.InsertAt(nIndex,pMenuItem);					
			}			
		}
		else if((nID!=-1)&&((pMenuItem->strText==strMenu)||!(nState & MF_OWNERDRAW)))
		{	
			// system don't auto insert popup menu item
		    pMenuItem->strText=strMenu;
			CMenu::ModifyMenu(nIndex,nState|MF_BYPOSITION|MF_OWNERDRAW,nID,(LPCTSTR)pMenuItem);
		}
	}
	
	for(nIndex=nSize;nIndex<GetMenuItemSize();nIndex++)
	{
		pMenuItem=m_MenuItemArr.GetAt(nIndex);
		m_MenuItemArr.RemoveAt(nIndex);
		delete pMenuItem;
	}	
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::InitPopupMenu(CMenu* pSubMenu,UINT nIndex,BOOL bSystem)
{
	//update pSubMenu's item to owerdrawn style
	if(bSystem)
		return;
	
	((CMenuEx*)pSubMenu)->UpdatePopupMenu();	
}
/////////////////////////////////////////////////////////////////////////////
void CMenuEx::MeasureItem(LPMEASUREITEMSTRUCT  lpMIS)
{
	// check measure ownerdraw menu or other controler
	if(lpMIS->CtlType!=ODT_MENU)		
		return;

	MENUITEM *lpMenuItem =(LPMENUITEM)lpMIS->itemData;
	
	// initialize width and height with itemSize
	lpMIS->itemWidth=lpMenuItem->itemSize.cx;
	lpMIS->itemHeight=lpMenuItem->itemSize.cy;

	// not separator,may be popupmenu or normal menu item
	if(lpMenuItem->nID!=0)
	{		
		IMAGEINFO info;

		if(lpMenuItem->pImageList!=NULL)
		{	
			// first add normal imagelist size
			lpMenuItem->pImageList->GetImageInfo(0,&info);				
			lpMIS->itemWidth+=(info.rcImage.right-info.rcImage.left);
			lpMIS->itemHeight+=(info.rcImage.bottom-info.rcImage.top);			
		}
		else if(lpMenuItem->pDisabledImageList!=NULL)
		{
			// or add disabled imagelist size
			lpMenuItem->pDisabledImageList->GetImageInfo(0,&info);				
			lpMIS->itemWidth+=(info.rcImage.right-info.rcImage.left);
			lpMIS->itemHeight+=(info.rcImage.bottom-info.rcImage.top);
		}
		else if(lpMenuItem->pHotImageList!=NULL)
		{
			// or add hot imagelist size
			lpMenuItem->pHotImageList->GetImageInfo(0,&info);				
			lpMIS->itemWidth+=(info.rcImage.right-info.rcImage.left);
			lpMIS->itemHeight+=(info.rcImage.bottom-info.rcImage.top);
		}
		else
		{		
			// add default size,if do not have imagelist
			lpMIS->itemWidth+=20;
			lpMIS->itemHeight+=20;
		}		
	}		
}
/////////////////////////////////////////////////////////////////////////////
UINT  CMenuEx::GetMenuItemSize() const 
{
	// get menu item counter
	return m_MenuItemArr.GetSize();
}
/////////////////////////////////////////////////////////////////////////////
LPMENUITEM CMenuEx::GetMenuItem(UINT nPosition)
{
	// get menu item by position
	if(nPosition>(UINT)m_MenuItemArr.GetUpperBound())
		return NULL;

	return m_MenuItemArr.GetAt(nPosition);
}
/////////////////////////////////////////////////////////////////////////////




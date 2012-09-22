#include "stdafx.h"
#include "Dialog.h"
#include "DialogDlg.h"
#include "afxcmn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


CDialogDlg::CDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DIR, m_TreeCtrl);
	DDX_Control(pDX, IDC_LIST_DIR, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CDialogDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CDialogDlg::OnBnClickedCancel)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_DIR, &CDialogDlg::OnTvnItemexpandedTreeDir)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DIR, &CDialogDlg::OnTvnSelchangedTreeDir)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CDialogDlg::OnHdnItemclickListDir)
	ON_NOTIFY(NM_CLICK, IDC_LIST_DIR, &CDialogDlg::OnNMClickListDir)
END_MESSAGE_MAP()

// CDialogDlg message handlers

BOOL CDialogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_ImageList.Create(32, 32, ILC_COLOR32, 10, 30);
	m_ListCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);

	//DWORD dwExStyle = LVS_EX_CHECKBOXES;
	//m_ListCtrl.SetExtendedStyle(dwExStyle);

	m_ListCtrl.SetExtendedStyle(m_ListCtrl.GetExtendedStyle() | LVS_EX_CHECKBOXES);

	DWORD dwStyle = GetWindowLong(m_TreeCtrl.m_hWnd, GWL_STYLE);
	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	SetWindowLong(m_TreeCtrl.m_hWnd, GWL_STYLE, dwStyle);

	m_hRoot = m_TreeCtrl.InsertItem(_T("ÎÒµÄµçÄÔ"));
	GetLogicalDrives(m_hRoot);
	GetDriveDir(m_hRoot);
	m_TreeCtrl.Expand(m_hRoot, TVE_EXPAND);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDialogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CDialogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDialogDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CDialogDlg::GetDriveDir(HTREEITEM hParent)
{
	CString strText;
	HTREEITEM hChild = m_TreeCtrl.GetChildItem(hParent);
	
	while(hChild)
	{
		strText = m_TreeCtrl.GetItemText(hChild);

		if(strText.Right(1) != _T("\\"))
		{
			strText += _T("\\");
		}

		strText += _T("*.*");
		CFileFind file;
		BOOL bContinue = file.FindFile(strText);

		while(bContinue)
		{
			bContinue = file.FindNextFile();
			if(file.IsDirectory() && !file.IsDots())
			{
				m_TreeCtrl.InsertItem(file.GetFileName(), hChild);
			}
		}

		hChild = m_TreeCtrl.GetNextItem(hChild, TVGN_NEXT);
	}
}

void CDialogDlg::GetLogicalDrives(HTREEITEM hParent)
{
	//get the length of drive strings
	int nBufferLength = GetLogicalDriveStrings(0, NULL);

	TCHAR* pBuffer = new TCHAR[nBufferLength + 1];
	GetLogicalDriveStrings(nBufferLength, pBuffer);

	TCHAR* pBegin = pBuffer;
	size_t nLength = _tcslen(pBuffer);

	while(nLength > 0)
	{
		TRACE(_T("%s\n"), pBuffer);
		m_TreeCtrl.InsertItem(pBuffer, hParent);
		pBuffer += nLength + 1;
		nLength = _tcslen(pBuffer);
	}
	
	delete [] pBegin;
	pBegin = NULL;
}

CString CDialogDlg::GetFullPath(HTREEITEM hCurrent)
{
	CString strTemp;
	CString strReturn;
	while(hCurrent != m_hRoot)
	{
		strTemp = m_TreeCtrl.GetItemText(hCurrent);

		if(strTemp.Right(1) != _T("\\"))
		{
			strTemp += _T("\\");
		}

		strReturn = strTemp + strReturn;
		hCurrent = m_TreeCtrl.GetParentItem(hCurrent);
	}

	return strReturn;
}

void CDialogDlg::OnTvnItemexpandedTreeDir(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	TVITEM item = pNMTreeView->itemNew;
	if(item.hItem == m_hRoot)
	{
		return;
	}

	HTREEITEM hChild = m_TreeCtrl.GetChildItem(item.hItem);

	while(hChild)
	{
		AddSubDir(hChild);
		hChild = m_TreeCtrl.GetNextItem(hChild,TVGN_NEXT);
	}

	*pResult = 0;
}

void CDialogDlg::AddSubDir(HTREEITEM hParent)
{
	HTREEITEM hChild = m_TreeCtrl.GetChildItem(hParent);

	if(hChild)
	{
		return;
	}

	CString strPath = GetFullPath(hParent);

	if(strPath.Right(1) != _T("\\"))
	{
		strPath += _T("\\");
	}

	CFileFind file;
	strPath += _T("*.*");
	BOOL bContinue = file.FindFile(strPath);

	while(bContinue)
	{
		bContinue = file.FindNextFile();
		if(file.IsDirectory() && !file.IsDots())
		{
			m_TreeCtrl.InsertItem(file.GetFileName(), hParent);
		}
	}
}

void CDialogDlg::OnTvnSelchangedTreeDir(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_ListCtrl.DeleteAllItems();
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	TVITEM item = pNMTreeView->itemNew;
	if(item.hItem == m_hRoot)
	{
		return;
	}

	CString str = GetFullPath(item.hItem);
	if(str.Right(1) != _T("\\"))
	{
		str += _T("\\");
	}

	int nIndex = 0;
	CFileFind file;
	CString strFile;
	str += _T("*.*");
	BOOL bHasfile = file.FindFile(str);

	while(bHasfile)
	{
		bHasfile = file.FindNextFile();
		if(!file.IsDirectory() && !file.IsDots())
		{
			strFile = str;
			SHFILEINFO info;
			nIndex = str.Find(_T("*.*"));
			strFile.Delete(nIndex, 3);

			SHGetFileInfo(strFile + file.GetFileName(), 0, &info, sizeof(&info), SHGFI_DISPLAYNAME | SHGFI_ICON);

			nIndex = m_ImageList.Add(info.hIcon);
			m_ListCtrl.InsertItem(nIndex, info.szDisplayName, nIndex);
		}
	}

	*pResult = 0;
}

void CDialogDlg::OnHdnItemclickListDir(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	//TRACE(_T("HdnItemclickListDir\n"));
	//MessageBox(_T("OnHdnItemclickListDir"));
	*pResult = 0;
}

void CDialogDlg::OnNMClickListDir(NMHDR *pNMHDR, LRESULT *pResult)
{
	//TRACE(_T("NMClickListDir\n"));
	//MessageBox(_T("OnNMClickListDir"));
	*pResult = 0;
}

// TestBaseUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestBaseUI.h"
#include "TestBaseUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_HSLIDER_CHANGE WM_USER + 100
#define WM_VSLIDER_CHANGE WM_USER + 101

#define WM_SKIN_SLIDER_NOTIFY	(WM_USER + 102)
#define WM_TABCTRL_NOTIFY		(WM_USER + 103)

// CAboutDlg dialog used for App About

class CAboutDlg : public CElcSkinDialog
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

CAboutDlg::CAboutDlg() : CElcSkinDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CElcSkinDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CElcSkinDialog)
END_MESSAGE_MAP()


// CTestBaseUIDlg dialog




CTestBaseUIDlg::CTestBaseUIDlg(CWnd* pParent /*=NULL*/)
	: CElcSkinDialog(CTestBaseUIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	LOGFONT lf = {0};
	GetDefaultFont(lf);

	m_ftText.CreateFontIndirect(&lf);
	lf.lfWeight = 700;
	m_ftBold.CreateFontIndirect(&lf);
}

void CTestBaseUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CElcSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_1, m_wndPngStatic);
	DDX_Control(pDX, IDC_STATIC_2, m_wndText);
}

BEGIN_MESSAGE_MAP(CTestBaseUIDlg, CElcSkinDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(WM_HSLIDER_CHANGE, OnHSliderChanged)
	ON_COMMAND(WM_VSLIDER_CHANGE, OnVSliderChanged)
	
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CTestBaseUIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestBaseUIDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTestBaseUIDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_STUD_VOICE, &CTestBaseUIDlg::OnBnClickedStudVoice)
	ON_BN_CLICKED(IDC_BUTTON6, &CTestBaseUIDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CTestBaseUIDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CTestBaseUIDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CTestBaseUIDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CTestBaseUIDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CTestBaseUIDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CTestBaseUIDlg::OnBnClickedButton12)
	ON_BN_CLICKED(200, &CTestBaseUIDlg::OnBnClickedButton13)
	ON_BN_CLICKED(201, &CTestBaseUIDlg::OnBnClickedButton14)
	ON_BN_CLICKED(4005, &CTestBaseUIDlg::OnBnClickedButton15)
	ON_BN_CLICKED(4007, &CTestBaseUIDlg::OnBnClickedButton16)
	ON_BN_CLICKED(4008, &CTestBaseUIDlg::OnBnClickedButton17)
	ON_BN_CLICKED(4009, &CTestBaseUIDlg::OnBnClickedButton18)

	ON_BN_CLICKED(5000, &CTestBaseUIDlg::OnBnClickedButton5000)

	ON_MESSAGE(WM_SKIN_SLIDER_NOTIFY, OnSliderNotify)
END_MESSAGE_MAP()


// CTestBaseUIDlg message handlers

BOOL CTestBaseUIDlg::OnInitDialog()
{
	CElcSkinDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	CRect rc;
	COLORREF crBk = ::GetSysColor(COLOR_BTNFACE);

	// 水平滑杆
	rc.top = 10;
	rc.bottom = 50;
	rc.left = 10;
	rc.right = 110;
	// 创建滑杆
	m_wndHSlider.Create(rc, this, 1000, WM_HSLIDER_CHANGE, TRUE);
	//设置滑块图片和滑杆的大小。
	m_wndHSlider.SetSliderBar(IDB_SLIDERBAR, 10);
	// 设置电平最大值
	m_wndHSlider.SetLevelMax(100);
	// 设置滑杆的范围
	m_wndHSlider.SetRange(10);
	// 设置当前位置
	m_wndHSlider.SetPos(5);
	m_wndHSlider.SetFillPos(5);
	m_wndHSlider.SetBkColor(crBk);
	m_wndHSlider.SetAutoFill(TRUE);


	rc.top = 50;
	rc.bottom = 71;
	rc.left = 10;
	rc.right = 110;

	// 创建滑杆
	m_wndHSlider2.Create(rc, this, 1000, WM_HSLIDER_CHANGE, TRUE);
	//设置滑块图片和滑杆的大小。
	m_wndHSlider2.SetSliderBar(IDB_SLIDERBAR, 10);
	// 设置滑杆的范围
	m_wndHSlider2.SetRange(20);
	// 设置当前位置
	m_wndHSlider2.SetPos(20);
	// 

	m_wndHSlider2.SetSliderImage(IDB_SLIDER_BK, 5, 5);
	// 使用背景图后，不支持电平显示
	m_wndHSlider2.EnableBkImage();
	m_wndHSlider2.SetBkColor(crBk);
	m_wndHSlider2.ShowWindow(SW_HIDE);

	m_wndSkinSlider.Create(WS_CHILD|WS_VISIBLE, rc, this, 100);
	m_wndSkinSlider.SetNotifyWindow(GetSafeHwnd(), WM_SKIN_SLIDER_NOTIFY);
	m_wndSkinSlider.ShowLevel(TRUE);
	//m_wndSkinSlider.SetRange(10);

	
	// 垂直滑杆
	rc.top = 10;
	rc.bottom = 110;
	rc.left = 120;
	rc.right = 140;
	m_wndVSlider.Create(rc, this, 1000, WM_VSLIDER_CHANGE, FALSE);
	m_wndVSlider.SetSliderBar(IDB_BITMAP1, 10);
	m_wndVSlider.SetLevelMax(300);
	m_wndVSlider.SetRange(10);
	m_wndVSlider.SetPos(0);
	m_wndVSlider.SetBkColor(crBk);
	m_wndVSlider.SetAutoFill(TRUE);

	


	// 演示电平
	m_nHVolume = 0;
	m_nVVolume = 0;

	srand( (unsigned)time( NULL ) );
	//SetTimer(1, 100, NULL);
	//SetTimer(2, 100, NULL);
	SetTimer(3, 100, NULL);


	CFont ftTitle;

	CClientDC dc(this);
	ftTitle.CreatePointFont(90, _T("宋体"), &dc);

	// List 1
	GetDlgItem(IDC_LIST1)->GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_wndList1.SetFont(&ftTitle);
	m_wndList1.SetBkColor(RGB(216,233,253));
	m_wndList1.SetTextColor(RGB(0,0,0));

	m_wndList1.Create(rc, this, 0, WS_CHILD|WS_VISIBLE, RLS_HAS_GRID);
	m_wndList1.AddItem(_T("张三"), -1);
	m_wndList1.AddItem(_T("张三 1"), -1);
	m_wndList1.AddItem(_T("张三 2"), -1);
	m_wndList1.AddItem(_T("张三 3"), -1);

	//GetNCSkinBinder()->BindWindow(m_wndList1.GetSafeHwnd(), ELC_SKIN_STYLE_PANE_CHILD);
	GetSkinWndBinder()->BindWindow(&m_wndList1, ELC_SKIN_STYLE_WINDOW_NCSKIN_WND);

	// List 2
	GetDlgItem(IDC_LIST2)->GetWindowRect(&rc);
	ScreenToClient(&rc);


	m_wndList2.SetFont(&ftTitle);
	m_wndList2.SetBkColor(RGB(216,233,253));
	m_wndList2.SetTextColor(RGB(0,0,0));

	HICON hUnCheck = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_UNCHECK), IMAGE_ICON, 16, 16, 0);
	HICON hCheck = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_CHECK), IMAGE_ICON, 16, 16, 0);

	m_wndList2.SetRadioIcon(hCheck, hUnCheck);

	m_wndList2.Create(rc, this, 0, WS_CHILD|WS_VISIBLE, RLS_HAS_GRID | RLS_HAS_RADIO);
	m_wndList2.AddItem(_T("张三"), -1);
	m_wndList2.AddItem(_T("张三 1"), -1);
	m_wndList2.AddItem(_T("张三 2"), -1);
	m_wndList2.AddItem(_T("张三 3"), -1);

	// List 3
	GetDlgItem(IDC_LIST3)->GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_wndList3.SetFont(&ftTitle);
	m_wndList3.SetBkColor(RGB(216,233,253));
	m_wndList3.SetTextColor(RGB(0,0,0));

//	HICON hUnCheck = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_UNCHECK), IMAGE_ICON, 16, 16, 0);
//	HICON hCheck = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_CHECK), IMAGE_ICON, 16, 16, 0);

	m_wndList3.SetRadioIcon(hCheck, hUnCheck);

	CBitmap bmpBase;
	imgList.Create(16, 16, ILC_MASK | ILC_COLOR24, 0, 1);
	bmpBase.Attach(::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_TEST), 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION));
	imgList.Add(&bmpBase, RGB(255,0,255));

	m_wndList3.SetImageList(&imgList);

	m_wndList3.Create(rc, this, 0, WS_CHILD|WS_VISIBLE, RLS_HAS_GRID | RLS_HAS_RADIO);
	m_wndList3.AddItem(_T("张三"), 0);
	m_wndList3.AddItem(_T("张三 1"), 1);
	m_wndList3.AddItem(_T("张三 2"), 2);
	m_wndList3.AddItem(_T("张三 3"), 1);

	// List 4

	GetDlgItem(IDC_LIST4)->GetWindowRect(&rc);
	ScreenToClient(&rc);

	m_wndList4.SetFont(&ftTitle);
	m_wndList4.SetBkColor(RGB(216,233,253));
	m_wndList4.SetTextColor(RGB(0,0,0));

	m_wndList4.SetRadioIcon(hCheck, hUnCheck);

	m_wndList4.SetImageList(&imgList);

	m_wndList4.Create(rc, this, 0, WS_CHILD|WS_VISIBLE|WS_VSCROLL, RLS_HAS_GRID | RLS_HAS_RADIO);
	m_wndList4.AddItem(_T("英文"), 0, _T("张三"), 3);
	m_wndList4.AddItem(_T("中文"), 1, _T("李四"), 3);
	m_wndList4.AddItem(_T("加拿大"), 2, _T("王二"), 3);


	m_btn1.Create(NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, 200);
	m_btn1.SetBitmap(IDB_BITMAP3);
	CSize size = m_btn1.GetBitmapSize();
	m_btn1.MoveWindow(660,40,size.cx, size.cy);

	m_btn2.Create(_T("test"), WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, CRect(0,0,0,0), this, 201);
	//m_btn2.SetBitmap(IDB_BITMAP4);
	//size = m_btn2.GetBitmapSize();
	//m_btn2.MoveWindow(660,80,size.cx, size.cy);
	m_btn2.MoveWindow(660,80,100, 30);
	

	m_wndTab.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(10, 160, 310, 195), this, 0);
	m_wndTab.SetImageList(&imgList);
	//m_wndTab.SetBkColor(RGB(120,80,140));
	//m_wndTab.SetTabLayout(ELCTAB_LAYOUT_UP);
	//m_wndTab.AppendButton(300, IDB_ELCBN_BTNCLOSE);
	m_wndTab.AddItem(_T("item 1"));
	m_wndTab.AddItem(_T("item 2"));
	m_wndTab.AddItem(_T("item 3"));
	//m_wndTab.SetBorder(ELCTABCTRL_BORDER_ALL);
	m_wndTab.ShowCloseButton(TRUE);
	m_wndTab.ShowNewButton(TRUE);
	m_wndTab.SetNewButtonTip(_T("new"));
	m_wndTab.SetCloseButtonTip(_T("Close"));
	m_wndTab.SetDropdownButtonTip(_T("Active files"));

	m_wndBar.LoadSkin(_T("style3"));
	m_wndBar.Create(this, CRect(330, 150, 690, 210), WS_CHILD|WS_VISIBLE, 10);
	m_wndBar.AddLabel(4005, _T("我是文字标签")); 
	m_wndBar.AddButton(4007, IDR_RT_PNG1, _T("RT_PNG"));
	m_wndBar.SetButtonText(4007, _T("图标文字"));
	//m_wndBar.AddButton(4008, IDR_RT_PNG1, _T("RT_PNG"));
	m_wndBar.AddButton(4008, IDB_BITMAP1, RT_BITMAP);
	//m_wndBar.AddButton(4009, 0, _T("RT_PNG"), _T("文字按钮"), ELCBDT_DROPDOWN);
	//m_wndBar.AddButton(4009, IDR_MAINFRAME, RT_ICON);
	m_wndBar.AddFourStateButton(4009, IDR_RT_PNG2);

	m_wndBar.AddUnionButton(500, IDR_RT_PNG1, _T("1"), 501, IDR_RT_DROPDOWN, 0);
	

	m_btn4.Create(_T("点我一下"), WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, 4005);
	m_btn4.SetIconImage(IDR_RT_PNG2, _T("RT_PNG"), ELCBIM_FOUR_STATE);
	size = m_btn4.GetIdealSize();

	m_wndPngStatic.SetImage(IDR_RT_PNG1, _T("RT_PNG"));

	BindPictureCtrl(IDC_STATIC_3, IDR_RT_PNG1);

	ELC_UNION_BUTTON left, right;
	left.nId = 1000;
	left.nResId = IDR_RT_PNG1;
	left.strCaption = _T("1");
	left.strSkinStyle = ELC_SKIN_STYLE_BUTTON_SUB_TOOLBAR;
	left.strType = RT_PNG;
	left.mode = ELCBIM_SINGLE;

	right.nId = 1000;
	right.nResId = IDR_RT_PNG1;
	right.strCaption = _T("2");
	right.strSkinStyle = ELC_SKIN_STYLE_BUTTON_SUB_TOOLBAR;
	right.strType = RT_PNG;
	right.mode = ELCBIM_SINGLE;

	m_wndUnionButton.Create(&left, &right, WS_CHILD | WS_VISIBLE, this, 0);

	size = m_wndUnionButton.GetIdeaSize();

	m_wndUnionButton.MoveWindow(640,110,size.cx, size.cy);

	return TRUE;  //
}

void CTestBaseUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CElcSkinDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestBaseUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CElcSkinDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestBaseUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestBaseUIDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (1 == nIDEvent) {
		int RANGE_MIN = 0;
		int RANGE_MAX = m_nHVolume * 5;
		if (m_nHVolume == 0)
			m_wndHSlider.SetLevel(0);
		else {
			int rand100 = (((double) rand() / 
				(double) RAND_MAX) * RANGE_MAX + RANGE_MIN);

			m_wndHSlider.SetLevel(rand100);
		}
	}
	else if (2 == nIDEvent) {
		int RANGE_MIN = 0;
		int RANGE_MAX = m_nVVolume * 10;

		if (m_nVVolume==0) {
			m_wndVSlider.SetLevel(0);
		}
		else {
			int rand100 = (((double) rand() / 
				(double) RAND_MAX) * RANGE_MAX + RANGE_MIN);

			m_wndVSlider.SetLevel(rand100);
		}
	}
	else if (3 == nIDEvent) {
		int nValue = rand() % 100;
		m_wndSkinSlider.SetLevel(nValue);
	}

	CElcSkinDialog::OnTimer(nIDEvent);
}

void CTestBaseUIDlg::OnHSliderChanged()
{
	int nPos = m_wndHSlider.GetPos();
	CString strText;
	strText.Format(_T("%d"), nPos);
	GetDlgItem(IDC_EDIT2)->SetWindowText(strText);
	m_nHVolume = nPos;

	//m_wndHSlider.SetFillPos(nPos);
}

void CTestBaseUIDlg::OnVSliderChanged()
{
	int nPos = m_wndVSlider.GetPos();
	CString strText;
	strText.Format(_T("%d"), nPos);
	GetDlgItem(IDC_EDIT3)->SetWindowText(strText);
	m_nVVolume = nPos;
}

void CTestBaseUIDlg::OnBnClickedButton1()
{
	CBlendMsgWnd* pMsgWnd = new CBlendMsgWnd;
	pMsgWnd->SetIcon(m_hIcon);
	pMsgWnd->SetTitle( _T("请您发言..."));
	pMsgWnd->SetDecs(_T("请使用麦克风进行发言。"));
	pMsgWnd->SetAnimateDelay(500);
	pMsgWnd->SetShowDelay(3000);

	CFont ftTitle, ftDesc;

	CClientDC dc(this);
	ftTitle.CreatePointFont(240, _T("宋体"), &dc);

	LOGFONT logFont;
	ftTitle.GetLogFont(&logFont);
	logFont.lfWeight = FW_BOLD;

	ftTitle.DeleteObject();

	ftTitle.CreateFontIndirect(&logFont);
	
	ftDesc.CreatePointFont(90, _T("宋体"), &dc);

	pMsgWnd->SetDescFont(&ftDesc);

	pMsgWnd->SetTitleFont(&ftTitle);

	if (pMsgWnd->Create(NULL)) {
		pMsgWnd->ShowWindow(SW_SHOW);
		pMsgWnd->UpdateWindow();
	}

	
}

void CTestBaseUIDlg::OnBnClickedButton2()
{
	CBlendSelectWnd* pMsgWnd = new CBlendSelectWnd;
	pMsgWnd->SetIcon(m_hIcon);

	HICON hIconClose = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_CLOSE), IMAGE_ICON, 16, 16, 0);
	HICON hIconCloseDown = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_CLOSEDOWN), IMAGE_ICON, 16, 16, 0);
	HICON hUnCheck = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_UNCHECK), IMAGE_ICON, 16, 16, 0);
	HICON hCheck = (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_CHECK), IMAGE_ICON, 16, 16, 0);

	pMsgWnd->SetCloseIcon(hIconClose, hIconCloseDown);
	pMsgWnd->SetRadioIcon(hCheck, hUnCheck);

	pMsgWnd->SetTitle(_T("同声翻译..."));

	pMsgWnd->SetAnimateDelay(500);
	pMsgWnd->SetShowDelay(0);

	CFont ftTitle, ftDesc;

	CClientDC dc(this);
	ftTitle.CreatePointFont(240, _T("宋体"), &dc);

	LOGFONT logFont;
	ftTitle.GetLogFont(&logFont);
	logFont.lfWeight = FW_BOLD;

	ftTitle.DeleteObject();

	ftTitle.CreateFontIndirect(&logFont);
	
	ftDesc.CreatePointFont(90, _T("宋体"), &dc);

	pMsgWnd->SetOptionFont(&ftDesc);
	pMsgWnd->SetTitleFont(&ftTitle);

	if (!pMsgWnd->Create(CRect(0,0,500,100), NULL, 0, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS))
		return;

	pMsgWnd->AddItem(_T("英语"));
	pMsgWnd->AddItem(_T("中文"));
	pMsgWnd->AddItem(_T("德语"));
	pMsgWnd->AddItem(_T("葡萄牙语"));
	pMsgWnd->AddItem(_T("法语"));
	pMsgWnd->AddItem(_T("意大利语"));

	pMsgWnd->SetWindowCenter();

	pMsgWnd->ShowWindow(SW_SHOW);
	pMsgWnd->UpdateWindow();
}

void CTestBaseUIDlg::OnBnClickedButton3()
{
}

void CTestBaseUIDlg::OnBnClickedStudVoice()
{
	/*
	STUDV_PARAMS params = {0};
	params.pTeacherFrame = 0;
	params.pCallback = 0;

	params.pParent = this;
	params.dwVoiceMask = 0xFFFFFFFF;
	params.bModelless = FALSE;
	params.ptLeftTop = CPoint(0,0);

	ElcBase_CreateStudentVoiceDlg(&params);
	*/
}

void OnELCBNCallbackPro(UINT nID, DWORD dwUser)
{
	CString strText;
	strText.Format(_T("Track item %d"), nID);
	MessageBox((HWND)dwUser, strText, _T("test balloon"), MB_OK);
}


void CTestBaseUIDlg::OnBnClickedButton6()
{
	BALLOON_INFO balloon;
	balloon.strCaption = _T("Language Lab");
	balloon.strTitle = _T("Message Box");
	balloon.strCheckBox = _T("Do not prompt again.");
	balloon.bChecked = TRUE;

	//balloon.bShowCaptonIcon = FALSE;
	//balloon.nTitleIcon = IDR_RT_PNG1;
	
	balloon.pMouseClicked = (PMOUSECLICKED)OnELCBNCallbackPro;
	balloon.dwUser = (DWORD)GetSafeHwnd();
	CElcBalloon* pWndPop = CElcBalloon::CreateBalloon(balloon);

	pWndPop->AddTrackItem(1, _T("单击这里显示消息１"));
	pWndPop->AddTrackItem(2, _T("单击这里..."));
	pWndPop->AddTrackItem(4, _T("单击这里..."));
	pWndPop->AddTrackItem(3, _T("单击这里不再提示。"));

	if (pWndPop) 
	{
		pWndPop->Popup(this);
	}
}

void CTestBaseUIDlg::OnBnClickedButton7()
{
	BALLOON_INFO balloon;
	balloon.strCaption = _T("Language Lab");
	balloon.strTitle = _T("Message Box");

	balloon.nTitleIcon = IDR_RT_PNG1;

	balloon.pMouseClicked = (PMOUSECLICKED)OnELCBNCallbackPro;
	balloon.dwUser = (DWORD)GetSafeHwnd();
	CElcBalloon* pWndPop = CElcBalloon::CreateBalloon(balloon);

	pWndPop->AddTrackItem(1, _T("单击这里显示消息１"));
	pWndPop->AddTrackItem(2, _T("单击这里..."));
	pWndPop->AddTrackItem(4, _T("单击这里..."));
	pWndPop->AddTrackItem(3, _T("单击这里不再提示。"));

	if (pWndPop) 
	{
		pWndPop->Popup(this);
	}
}

void CTestBaseUIDlg::OnBnClickedButton8()
{
	BALLOON_INFO balloon;
	balloon.strCaption = _T("Language Lab");
	//balloon.strTitle = _T("Message Box");

	balloon.bShowCaptonIcon = FALSE;
	balloon.nTitleIcon = IDR_RT_PNG1;

	balloon.pMouseClicked = (PMOUSECLICKED)OnELCBNCallbackPro;
	balloon.dwUser = (DWORD)GetSafeHwnd();
	CElcBalloon* pWndPop = CElcBalloon::CreateBalloon(balloon);

	pWndPop->AddTrackItem(1, _T("If the function succeeds, the return value is the height of the text in logical units."));

	if (pWndPop) 
	{
		pWndPop->Popup(this);
	}
}

void CTestBaseUIDlg::OnBnClickedButton9()
{
	BALLOON_INFO balloon;
	balloon.strCaption = _T("Language Lab");
	balloon.strTitle = _T("Message Box");

	balloon.strCheckBox = _T("Do not prompt again.");

	balloon.bShowCaptonIcon = FALSE;
	balloon.nTitleIcon = IDR_RT_PNG1;

	balloon.pMouseClicked = (PMOUSECLICKED)OnELCBNCallbackPro;
	balloon.dwUser = (DWORD)GetSafeHwnd();
	CElcBalloon* pWndPop = CElcBalloon::CreateBalloon(balloon);

	pWndPop->AddTrackItem(1, _T("If the function succeeds, the return value is the height of the text in logical units."));

	if (pWndPop) 
	{
		pWndPop->Popup(this);
	}
}

void CTestBaseUIDlg::OnBnClickedButton10()
{
	
}

void CTestBaseUIDlg::OnBnClickedButton11()
{
	CString strText;
	GetDlgItem(IDC_EDIT2)->GetWindowText(strText);
	int nPos = _ttoi(strText);
	if (m_wndHSlider.GetSafeHwnd())
	{
		m_wndHSlider.SetPos(nPos);
		m_wndSkinSlider.SetPos(nPos);
		//m_wndHSlider.SetFillPos(nPos);
		//m_wndHSlider.SetLevel(100);
	}
}

void CTestBaseUIDlg::OnBnClickedButton12()
{
	CString strText;
	GetDlgItem(IDC_EDIT3)->GetWindowText(strText);
	int nPos = _ttoi(strText);
	if (m_wndVSlider.GetSafeHwnd())
	{
		m_wndVSlider.SetPos(nPos);
		m_wndVSlider.SetFillPos(nPos);
		//m_wndVSlider.SetLevel(100);
	}
}

void CTestBaseUIDlg::OnBnClickedButton13()
{
}

void CTestBaseUIDlg::OnBnClickedButton14()
{
}

int g_test1 = 0;
void CTestBaseUIDlg::OnBnClickedButton15()
{
	if (g_test1 == 0) {
		m_wndBar.SetButtonText(4007, _T("停止"));
		m_wndBar.EnableButton(4007, TRUE);
	}
	else if (g_test1 == 1) {
		m_wndBar.SetButtonText(4007, _T("投影广播"));
		m_wndBar.SetButtonDropType(4007, ELCBDT_DROPDOWN);
	}
	else if (g_test1 == 2) {
		m_wndBar.SetButtonsLayout(ELCBL_VERTICAL);	
	}
	else if (g_test1 == 3) {
		m_wndBar.SetButtonDropType(4007, ELCBDT_NORMAL);
		m_wndBar.SetButtonsLayout(ELCBL_HORIZONTAL);
	}
	else if (g_test1 == 4) {
		m_wndBar.EnableButton(4007, FALSE);
	}

	g_test1 ++;
	if (g_test1 > 4)
		g_test1 = 0;
}

void CTestBaseUIDlg::OnBnClickedButton16()
{
	if (m_wndFloatBar.GetSafeHwnd()) {
		m_wndFloatBar.ShowBar();
		return;
	}

	m_wndFloatBar.CreateBar(this);
	m_wndFloatBar.AddButton(5000, _T("按钮1fsdafsdfas df s"), IDR_RT_STOP, _T("RT_PNG"), _T("第一个按钮，屏幕广播"), ELCBDT_DROPDOWN);
	m_wndFloatBar.AddButton(100, _T("按钮 2"), IDR_RT_PEN, _T("RT_PNG"), _T("第二个按钮，网络影院"), ELCBDT_DROPDOWN);
	m_wndFloatBar.AddSeparator(102);
	m_wndFloatBar.AddButton(200, _T("按钮 3"), IDR_RT_OPTION, _T("RT_PNG"), _T("第三个按钮"));
	m_wndFloatBar.AddButton(300, _T("按钮 4"), IDR_RT_PEN, _T("RT_PNG"), _T("第一个按钮，屏幕广播"));
	m_wndFloatBar.AddSeparator(302);
	m_wndFloatBar.AddButton(400, NULL, IDR_RT_OPTION, _T("RT_PNG"), _T("第一个按钮，屏幕广播"));
	m_wndFloatBar.AddButton(500, _T("按钮 555"), IDR_RT_STOP, _T("RT_PNG"), _T("第一个按钮，屏幕广播"));
	m_wndFloatBar.AddButton(600, _T("按钮 6"), IDR_RT_PHONE, _T("RT_PNG"), _T("第一个按钮，好长好长好长好长好长好长好长好长好长好长好长好长好长好长好长好长的提示！"));
	m_wndFloatBar.AddSeparator(602);
	m_wndFloatBar.AddButton(700, _T("按钮 77700007"), IDR_RT_OPTION, _T("RT_PNG"), _T("最后一个按钮，好长好长好长好长好长好长好长好长的文字呀"));
	m_wndFloatBar.AddImage(800, IDR_RT_PHONE);
	m_wndFloatBar.AddLabel(900, _T("test"));

	m_wndFloatBar.SetButtonCheck(200, TRUE);
	m_wndFloatBar.EnableButton(300, FALSE);

	m_wndFloatBar.SetTitle(_T("Tomlie"));
	m_wndFloatBar.SetTitleIcon(IDR_RT_RED);
	
	m_wndFloatBar.ShowBar();
}

void CTestBaseUIDlg::OnBnClickedButton5000()
{
	m_wndFloatBar.SetBarAutoHide(!m_wndFloatBar.IsBarAutoHide());
	m_wndFloatBar.SetButtonCheck(5000, !m_wndFloatBar.IsBarAutoHide());
}

BOOL g_bTabLayout = 0;
void CTestBaseUIDlg::OnBnClickedButton17()
{
	m_wndTab.ShowBorder(!m_wndTab.IsBorderVisible());
	g_bTabLayout = !g_bTabLayout;
	m_wndTab.SetTabLayout((ELCTAB_LAYOUT)g_bTabLayout);

	CElcRichMenu* pSkinMenu = new CElcRichMenu();

	pSkinMenu->AddItem(1, _T("item 1"), _T("item 1 desc"), IDR_RT_PNG1);
	pSkinMenu->AddItem(2, _T("item 2"), _T("It provides member functions for creating and destroying a menu"), IDR_RT_PNG1);
	pSkinMenu->AddItem(3, _T("item 3"), _T("item 3 desc"), IDR_RT_PNG1);
	pSkinMenu->AddItem(4, _T("item 4"), _T("item 3 desc"), IDR_RT_PNG1);
	pSkinMenu->AddItem(5, _T("item 5"), _T("item 3 desc"), IDR_RT_PNG1);

	pSkinMenu->CheckItem(2, 1);
	pSkinMenu->EnableItem(3, 0);
	pSkinMenu->CheckItem(3, 1);
	//pSkinMenu->SetIconSize(16, 16);

	CRect rect;
	m_wndBar.GetButton(4008)->GetWindowRect(rect);
	pSkinMenu->Popup(rect.left, rect.bottom, ELCRMA_LEFT_TOP, this);

	/*
	CSkinRichMenu* pSkinMenu = new CSkinRichMenu();

	pSkinMenu->AddItem(1, _T("Item 1"), _T("Item descript"), IDR_RT_PNG1, RT_PNG, 1, 1);
	pSkinMenu->AddItem(2, _T("Item 1"), _T("Item descript"), IDR_RT_STOP);
	pSkinMenu->AddItem(3, _T("Item 1"), _T("Item descript"), IDR_RT_PEN);

	pSkinMenu->SetIconSize(CSize(32,32));
	pSkinMenu->SetMenuWidth(200);

	pSkinMenu->SetCapFont(&m_ftBold);
	pSkinMenu->SetDescFont(&m_ftText);

	pSkinMenu->SetBkColor(RGB(250,250,250));
	pSkinMenu->SetTextColor(RGB(101,99,99));

	pSkinMenu->SetDescRow(2);

	CRect rect;
	m_wndBar.GetButton(4008)->GetWindowRect(rect);
	pSkinMenu->ShowPopMenu(rect.left, rect.bottom, LEFT_TOP);
	*/
}

void CTestBaseUIDlg::OnBnClickedButton18()
{
	/*
	if (CreateTransparentWnd(&m_wndTransparentBar, 200, CRect(100, 100, 400, 150), this)) {
		m_wndTransparentBar.AddButton(100, IDR_RT_PNG2);
		m_wndTransparentBar.AddButton(101, IDR_RT_PNG2);
		m_wndTransparentBar.AddButton(102, IDR_RT_PNG2);
	}
	*/

	if (m_wndSkinDlg.GetSafeHwnd()) {
		m_wndSkinDlg.DestroyWindow();
	}

	m_wndSkinDlg.Create(CSkinDlgTest::IDD, this);

	CSkinWndBinder* pBinder = GetSkinWndBinder();
	pBinder->BindWindow(m_wndSkinDlg.GetSafeHwnd(), ELC_SKIN_STYLE_WINDOW_DLG);
	CElcSkinFrameBase* pFrame = pBinder->GetSkinFrame(m_wndSkinDlg.GetSafeHwnd());
	pFrame->AdjustImagePanePos(IDC_IMAGE_PANE);

	UINT nIDs[] = {IDOK, IDCANCEL, IDC_BUTTON1, IDC_BUTTON2};
	pFrame->BindButton(nIDs, _countof(nIDs));

	pFrame->BindButton(IDC_CHECK1, ELC_SKIN_STYLE_BUTTON_CHECKBOX);
	pFrame->BindButton(IDC_STATIC_3, ELC_SKIN_STYLE_BUTTON_GROUPBOX);

	m_wndSkinDlg.CenterWindow(this);
	m_wndSkinDlg.ShowWindow(SW_SHOW);
}

LRESULT CTestBaseUIDlg::OnSliderNotify(WPARAM wParam, LPARAM lParam)
{
	CString strText;
	strText.Format(_T("%d"), lParam);
	GetDlgItem(IDC_EDIT2)->SetWindowText(strText);

	return 0;
}
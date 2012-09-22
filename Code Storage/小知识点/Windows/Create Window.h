//创建窗口的不同

//1、使用create函数:
virtual BOOL Create(
   LPCTSTR lpszClassName,
   LPCTSTR lpszWindowName,
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID,
   CCreateContext* pContext = NULL
);


UINT nID = 1;
CWnd* pParentWnd = this;
LPCTSTR lpszClassName = NULL;
CCreateContext* pContext = NULL;
RECT rect = RECT(0, 0, 200, 300);
DWORD dwStyle = WS_CHILD | WS_VISIBLE;
LPCTSTR lpszWindowName = _T("My Window");


//例如:
	m_wnd.Create(NULL,
		_T("peng"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CRect(100, 100, 300, 300),
		this,
		0,
		0);
				
//创建后窗口将具有child分格,且父窗口为this指针所指向的窗口

//2、使用createEx函数:

virtual BOOL CreateEx(
   DWORD dwExStyle,
   LPCTSTR lpszClassName,
   LPCTSTR lpszWindowName,
   DWORD dwStyle,
   const RECT& rect,
   CWnd* pParentWnd,
   UINT nID,
   LPVOID lpParam = NULL
);


//例如:
	m_wnd.CreateEx(NULL,
		lpszClassName,
		_T("peng"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CRect(100,100,300,300),
		this,
		0,
		0);
		
//创建后窗口不具有child风格，虽然有this指针,该窗口的父窗口为NULL,而owner窗口为this指针所指的窗口,
//同时rect所代表的矩形是以屏幕的左上角坐标为基点的。



//创建各式各样的按钮:

CButton myButton1, myButton2, myButton3, myButton4;

// Create a push button.//普通下压按钮
myButton1.Create(_T("My button"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
   CRect(10,10,100,30), pParentWnd, 1);

// Create a radio button.//单选按钮
myButton2.Create(_T("My button"), WS_CHILD|WS_VISIBLE|BS_RADIOBUTTON, 
   CRect(10,40,100,70), pParentWnd, 2);

// Create an auto 3-state button.//三态按钮
myButton3.Create(_T("My button"), WS_CHILD|WS_VISIBLE|BS_AUTO3STATE, 
   CRect(10,70,100,100), pParentWnd, 3);

// Create an auto check box.//选择按钮
myButton4.Create(_T("My button"), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, 
   CRect(10,100,100,130), pParentWnd, 4);
 

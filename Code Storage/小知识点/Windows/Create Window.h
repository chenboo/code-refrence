//�������ڵĲ�ͬ

//1��ʹ��create����:
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


//����:
	m_wnd.Create(NULL,
		_T("peng"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CRect(100, 100, 300, 300),
		this,
		0,
		0);
				
//�����󴰿ڽ�����child�ָ�,�Ҹ�����Ϊthisָ����ָ��Ĵ���

//2��ʹ��createEx����:

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


//����:
	m_wnd.CreateEx(NULL,
		lpszClassName,
		_T("peng"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CRect(100,100,300,300),
		this,
		0,
		0);
		
//�����󴰿ڲ�����child�����Ȼ��thisָ��,�ô��ڵĸ�����ΪNULL,��owner����Ϊthisָ����ָ�Ĵ���,
//ͬʱrect������ľ���������Ļ�����Ͻ�����Ϊ����ġ�



//������ʽ�����İ�ť:

CButton myButton1, myButton2, myButton3, myButton4;

// Create a push button.//��ͨ��ѹ��ť
myButton1.Create(_T("My button"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
   CRect(10,10,100,30), pParentWnd, 1);

// Create a radio button.//��ѡ��ť
myButton2.Create(_T("My button"), WS_CHILD|WS_VISIBLE|BS_RADIOBUTTON, 
   CRect(10,40,100,70), pParentWnd, 2);

// Create an auto 3-state button.//��̬��ť
myButton3.Create(_T("My button"), WS_CHILD|WS_VISIBLE|BS_AUTO3STATE, 
   CRect(10,70,100,100), pParentWnd, 3);

// Create an auto check box.//ѡ��ť
myButton4.Create(_T("My button"), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, 
   CRect(10,100,100,130), pParentWnd, 4);
 

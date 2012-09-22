//树型控件的使用:

//1、设置树型控件的风格。

	DWORD dwStyle = GetWindowLong(m_TreeCtrl.m_hWnd, GWL_STYLE);
	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	SetWindowLong(m_TreeCtrl.m_hWnd, GWL_STYLE, dwStyle);
	
//TVS_HASBUTTONS、TVS_HASLINES等风格有很多视具体情况而选。

//2、为树型控件添加图像列表

//树控件 (Tree Control) (CTreeCtrl) 中的每项都可以有一对与之关联的位图化图像。
//这些图像出现在项标签的左边。
//如果选择了该项，就会显示一个图像；如果没有选择该项，就会显示另一个图像。
//例如，某项可能在选定时显示一个打开的文件夹，而在未选定时显示关闭的文件夹。

//创建图像列表并把它加入到树形控件中的代码如下:
	HICON hIcon[2];
	CTreeCtrl& m_TreeCtrl = GetTreeCtrl();
	m_ImageList.Create(16, 16, ILC_COLOR8 | ILC_MASK, 2, 1);

	hIcon[0] = AfxGetApp()->LoadIcon(IDI_ICON2);
	hIcon[1] = AfxGetApp()->LoadIcon(IDI_ICON1);

	for(int n = 0; n < 2; n++)
	{
		m_ImageList.Add(hIcon[n]);
	}

	m_TreeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

//3、添加节点
//在树形控件中每一个结点都有一个句柄（HTREEITEM），同时添加结点时必须提供的参数是该结点的父结点句柄，利用

HTREEITEM InsertItem(
   LPCTSTR lpszItem,
   int nImage,
   int nSelectedImage,
   HTREEITEM hParent = TVI_ROOT,
   HTREEITEM hInsertAfter = TVI_LAST
);

//可以添加一个结点，pszItem为显示的字符，hParent代表父结点的句柄，
//nImage为结点没被选中时所使用图片序号，nSelectedImage为结点被选中时所使用图片序号
//图片序列号为图像列表中图片的序列号。
//当前添加的结点会排在hInsertAfter表示的结点的后面，返回值为当前创建的结点的句柄
//如:
	hchid = m_TreeCtrl.InsertItem(_T("ok la"), 1, 1, hParent);

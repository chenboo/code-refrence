//���Ϳؼ���ʹ��:

//1���������Ϳؼ��ķ��

	DWORD dwStyle = GetWindowLong(m_TreeCtrl.m_hWnd, GWL_STYLE);
	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	SetWindowLong(m_TreeCtrl.m_hWnd, GWL_STYLE, dwStyle);
	
//TVS_HASBUTTONS��TVS_HASLINES�ȷ���кܶ��Ӿ��������ѡ��

//2��Ϊ���Ϳؼ����ͼ���б�

//���ؼ� (Tree Control) (CTreeCtrl) �е�ÿ�������һ����֮������λͼ��ͼ��
//��Щͼ����������ǩ����ߡ�
//���ѡ���˸���ͻ���ʾһ��ͼ�����û��ѡ�����ͻ���ʾ��һ��ͼ��
//���磬ĳ�������ѡ��ʱ��ʾһ���򿪵��ļ��У�����δѡ��ʱ��ʾ�رյ��ļ��С�

//����ͼ���б��������뵽���οؼ��еĴ�������:
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

//3����ӽڵ�
//�����οؼ���ÿһ����㶼��һ�������HTREEITEM����ͬʱ��ӽ��ʱ�����ṩ�Ĳ����Ǹý��ĸ������������

HTREEITEM InsertItem(
   LPCTSTR lpszItem,
   int nImage,
   int nSelectedImage,
   HTREEITEM hParent = TVI_ROOT,
   HTREEITEM hInsertAfter = TVI_LAST
);

//�������һ����㣬pszItemΪ��ʾ���ַ���hParent�������ľ����
//nImageΪ���û��ѡ��ʱ��ʹ��ͼƬ��ţ�nSelectedImageΪ��㱻ѡ��ʱ��ʹ��ͼƬ���
//ͼƬ���к�Ϊͼ���б���ͼƬ�����кš�
//��ǰ��ӵĽ�������hInsertAfter��ʾ�Ľ��ĺ��棬����ֵΪ��ǰ�����Ľ��ľ��
//��:
	hchid = m_TreeCtrl.InsertItem(_T("ok la"), 1, 1, hParent);

//CTypedPtrArray的使用

//1>声明
CTypedPtrArray<CPtrArray, CRichButton*>		m_BtnArray;

//2>删除所有

CDrawbar::~CDrawbar()
{
	int nCount = (int)m_BtnArray.GetCount();

	for(int i = 0; i < nCount; i++)
	{
		delete m_BtnArray.GetAt(i);
	}

	m_BtnArray.RemoveAll();
	m_CheckBtnArray.RemoveAll();
}

//3>添加
m_BtnArray.Add(pButton);

//CTypedPtrArray��ʹ��

//1>����
CTypedPtrArray<CPtrArray, CRichButton*>		m_BtnArray;

//2>ɾ������

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

//3>���
m_BtnArray.Add(pButton);

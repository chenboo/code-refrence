#include <vector>

BOOL CreateMultipleDirectory(const CString& szPath)
{
	//���Ҫ������Ŀ¼�ַ���
	CString strDir(szPath);
	//ȷ����'\'��β�Դ������һ��Ŀ¼

	if (strDir.GetAt(strDir.GetLength() - 1) != _T('\\'))
	{
		strDir.AppendChar(_T('\\'));
	}

	//���ÿһ��Ŀ¼�ַ���
	std::vector<CString> vPath;
	CString strTemp;//һ����ʱ����,���Ŀ¼�ַ���
	bool bSuccess = false;//�ɹ���־
	
	//����Ҫ�������ַ���
	for (int i = 0; i < strDir.GetLength(); ++i)
	{
		if (strDir.GetAt(i) != _T('\\')) 
		{//�����ǰ�ַ�����'\\'
			strTemp.AppendChar(strDir.GetAt(i));
		}
		else 
		{//�����ǰ�ַ���'\\'
			vPath.push_back(strTemp);//����ǰ����ַ�����ӵ�������
			strTemp.AppendChar(_T('\\'));
		}
	}

	//�������Ŀ¼������,����ÿ��Ŀ¼
	std::vector<CString>::const_iterator vIter;
	for (vIter = vPath.begin(); vIter != vPath.end(); vIter++) 
	{
		//���CreateDirectoryִ�гɹ�,����true,���򷵻�false
		bSuccess = CreateDirectory(*vIter, NULL) ? true : false;    
	}

	return bSuccess;
}

//ע:ʹ��CreateDirectoryһ��ֻ�ܴ���һ���ļ��У������Ҫ����C:\Peng\OK, ���C:\Peng�ļ���
//���������޷�����OK�ļ��С�

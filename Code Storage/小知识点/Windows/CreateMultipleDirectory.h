#include <vector>

BOOL CreateMultipleDirectory(const CString& szPath)
{
	//存放要创建的目录字符串
	CString strDir(szPath);
	//确保以'\'结尾以创建最后一个目录

	if (strDir.GetAt(strDir.GetLength() - 1) != _T('\\'))
	{
		strDir.AppendChar(_T('\\'));
	}

	//存放每一层目录字符串
	std::vector<CString> vPath;
	CString strTemp;//一个临时变量,存放目录字符串
	bool bSuccess = false;//成功标志
	
	//遍历要创建的字符串
	for (int i = 0; i < strDir.GetLength(); ++i)
	{
		if (strDir.GetAt(i) != _T('\\')) 
		{//如果当前字符不是'\\'
			strTemp.AppendChar(strDir.GetAt(i));
		}
		else 
		{//如果当前字符是'\\'
			vPath.push_back(strTemp);//将当前层的字符串添加到数组中
			strTemp.AppendChar(_T('\\'));
		}
	}

	//遍历存放目录的数组,创建每层目录
	std::vector<CString>::const_iterator vIter;
	for (vIter = vPath.begin(); vIter != vPath.end(); vIter++) 
	{
		//如果CreateDirectory执行成功,返回true,否则返回false
		bSuccess = CreateDirectory(*vIter, NULL) ? true : false;    
	}

	return bSuccess;
}

//注:使用CreateDirectory一次只能创建一个文件夹，即如果要创建C:\Peng\OK, 如果C:\Peng文件夹
//不存在则无法创建OK文件夹。

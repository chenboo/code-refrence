//LibTinyXml��ʹ��
//LibTinyXml����Ҫ���þ���������XML�ļ����н���
//�Լ�����ΪXML�ļ������ƺ�С��ǿ��ֻ���������ļ�������ͷ�ļ�
//tinystr.h, tinyxml.h�Լ��ĸ�Դ�ļ�tinystr.cpp, tinyxml.cpp, 
//tinyxmlerror.cpp, tinyxmlparser.cpp
//�ٸ�����˵�����ʹ�ã�
����
#include < iostream>
#include "tinyxml.h"
#include "tinystr.h"
#include < string>
#include < windows.h>
#include < atlstr.h>
using namespace std;

CString GetAppPath()
{
	//��ȡӦ�ó����Ŀ¼
	TCHAR modulePath[MAX_PATH];
	GetModuleFileName(NULL, modulePath, MAX_PATH);
	CString strModulePath(modulePath);
	strModulePathstrModulePath = strModulePath.Left
		(strModulePath.ReverseFind(_T('\\')));
	return strModulePath;  
}

bool CreateXmlFile(string& szFileName)
{	
	//����xml�ļ�,szFilePathΪ�ļ������·��,�������ɹ�����true,����false
	try
	{
		//����һ��XML���ĵ�����
		TiXmlDocument *myDocument = new TiXmlDocument();
		//����һ����Ԫ�ز����ӡ�
		TiXmlElement *RootElement = new TiXmlElement("Persons");
		myDocument->LinkEndChild(RootElement);
		//����һ��PersonԪ�ز����ӡ�
		TiXmlElement *PersonElement = new TiXmlElement("Person");
		RootElement->LinkEndChild(PersonElement);
		//����PersonԪ�ص����ԡ�
		PersonElement->SetAttribute("ID", "1");
		//����nameԪ�ء�ageԪ�ز����ӡ�
		TiXmlElement *NameElement = new TiXmlElement("name");
		TiXmlElement *AgeElement = new TiXmlElement("age");
		PersonElement->LinkEndChild(NameElement);
		PersonElement->LinkEndChild(AgeElement);
		//����nameԪ�غ�ageԪ�ص����ݲ����ӡ�
		TiXmlText *NameContent = new TiXmlText("������");
		TiXmlText *AgeContent = new TiXmlText("22");
		NameElement->LinkEndChild(NameContent);
		AgeElement->LinkEndChild(AgeContent);
		CString appPath = GetAppPath();
		string seperator = "\\";
		string fullPath = appPath.GetBuffer(0) +seperator+szFileName;
		myDocument->SaveFile(fullPath.c_str());//���浽�ļ�
	}

	catch (string& e)
	{
		return false;
	}

	return true;
}

bool ReadXmlFile(string& szFileName)
{
	//��ȡXml�ļ���������
	try
	{
		CString appPath = GetAppPath();
		string seperator = "\\";
		string fullPath = appPath.GetBuffer(0) +seperator+szFileName;
		//����һ��XML���ĵ�����
		TiXmlDocument *myDocument = new TiXmlDocument(fullPath.c_str());
		myDocument->LoadFile();
		//��ø�Ԫ�أ���Persons��
		TiXmlElement *RootElement = myDocument->RootElement();
		//�����Ԫ�����ƣ������Persons��
		cout < <  RootElement->Value() < <  endl;
		//��õ�һ��Person�ڵ㡣
		TiXmlElement *FirstPerson = RootElement->FirstChildElement();
		//��õ�һ��Person��name�ڵ��age�ڵ��ID���ԡ�
		TiXmlElement *NameElement = FirstPerson->FirstChildElement();
		TiXmlElement *AgeElement = NameElement->NextSiblingElement();
		TiXmlAttribute *IDAttribute = FirstPerson->FirstAttribute();
		//�����һ��Person��name���ݣ��������ǣ�age���ݣ�����ID���ԣ�����
		cout < <  NameElement->FirstChild()->Value() < <  endl;
		cout < <  AgeElement->FirstChild()->Value() < <  endl;
		cout < <  IDAttribute->Value()< <  endl;
	}  
	catch (string& e)
	{
		return false;
	}

	return true;
}

int main()
{
	string fileName = "info.xml";
	CreateXmlFile(fileName);
	ReadXmlFile(fileName);
}

//�����γɵ�XML�ļ��������£�
// < Persons> 
// < Person ID="1"> 
// < name>������< /name> 
// < age>20< /age> 
// < /Person> 
// < Person ID="2"> 
// < name>�׾���< /name> 
// < age>18< /age> 
// < /Person> 
// < /Persons> 

//������OK��



//����һ������
//index htm
void CXMLCREATE::CreateindexXml(CString str)
{
	TiXmlDocument xmlDoc;
//<HTML>
	TiXmlElement htm("HTML");

	//<HEAD>
		TiXmlElement head("HEAD");
		//<META>
		TiXmlElement Meta("META");
		Meta.SetAttribute("NAME","ҳ�����");
		Meta.SetAttribute("CONTENT","ҳ�����");
		//</META>
		//<TITLE>
		TiXmlElement title("TITLE");
		TiXmlText text("ҳ�����");
		title.InsertEndChild(text);
		//</TITLE>
		head.InsertEndChild(Meta);
		head.InsertEndChild(title);
	//</HEAD>

	//<FRAMESET>
		TiXmlElement frame("FRAMESET");
		frame.SetAttribute("ROWS","60,1,*");
		frame.SetAttribute("framespacing","0");
		frame.SetAttribute("border","0");
		frame.SetAttribute("frameborder","0");
		//<frame1>
		TiXmlElement frame1("FRAME");
		frame1.SetAttribute("SRC","FRAME\\icons.htm");
		frame1.SetAttribute("NAME","ICONS");
		frame1.SetAttribute("NORRESIZE SCROLLING", "NO");
		//<frame2>
		TiXmlElement frame2("FRAME");
		frame2.SetAttribute("SRC","FRAME\\blank.htm");
		frame2.SetAttribute("NAME","BLANK");
		frame2.SetAttribute("NORRESIZE SCROLLING", "NO");
		//<frame3>
		TiXmlElement frame3("FRAME");
		frame3.SetAttribute("SRC","FRAME\\OverView.htm");
		frame3.SetAttribute("NAME","VIEW");
		frame.InsertEndChild(frame1);
		frame.InsertEndChild(frame2);
		frame.InsertEndChild(frame3);
	//</FRAMESET>

	htm.InsertEndChild(head);
	htm.InsertEndChild(frame);
//</HTML>

	xmlDoc.InsertEndChild(htm);

	char* pch = CString2utfA(str);
	xmlDoc.SaveFile(pch,TRUE);
	delete[] pch;
}
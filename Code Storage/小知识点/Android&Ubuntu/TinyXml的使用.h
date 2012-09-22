//LibTinyXml的使用
//LibTinyXml的主要作用就是用来对XML文件进行解析
//以及保存为XML文件，堪称很小很强大，只包含六个文件，两个头文件
//tinystr.h, tinyxml.h以及四个源文件tinystr.cpp, tinyxml.cpp, 
//tinyxmlerror.cpp, tinyxmlparser.cpp
//举个例子说明如何使用：
方法
#include < iostream>
#include "tinyxml.h"
#include "tinystr.h"
#include < string>
#include < windows.h>
#include < atlstr.h>
using namespace std;

CString GetAppPath()
{
	//获取应用程序根目录
	TCHAR modulePath[MAX_PATH];
	GetModuleFileName(NULL, modulePath, MAX_PATH);
	CString strModulePath(modulePath);
	strModulePathstrModulePath = strModulePath.Left
		(strModulePath.ReverseFind(_T('\\')));
	return strModulePath;  
}

bool CreateXmlFile(string& szFileName)
{	
	//创建xml文件,szFilePath为文件保存的路径,若创建成功返回true,否则false
	try
	{
		//创建一个XML的文档对象。
		TiXmlDocument *myDocument = new TiXmlDocument();
		//创建一个根元素并连接。
		TiXmlElement *RootElement = new TiXmlElement("Persons");
		myDocument->LinkEndChild(RootElement);
		//创建一个Person元素并连接。
		TiXmlElement *PersonElement = new TiXmlElement("Person");
		RootElement->LinkEndChild(PersonElement);
		//设置Person元素的属性。
		PersonElement->SetAttribute("ID", "1");
		//创建name元素、age元素并连接。
		TiXmlElement *NameElement = new TiXmlElement("name");
		TiXmlElement *AgeElement = new TiXmlElement("age");
		PersonElement->LinkEndChild(NameElement);
		PersonElement->LinkEndChild(AgeElement);
		//设置name元素和age元素的内容并连接。
		TiXmlText *NameContent = new TiXmlText("周星星");
		TiXmlText *AgeContent = new TiXmlText("22");
		NameElement->LinkEndChild(NameContent);
		AgeElement->LinkEndChild(AgeContent);
		CString appPath = GetAppPath();
		string seperator = "\\";
		string fullPath = appPath.GetBuffer(0) +seperator+szFileName;
		myDocument->SaveFile(fullPath.c_str());//保存到文件
	}

	catch (string& e)
	{
		return false;
	}

	return true;
}

bool ReadXmlFile(string& szFileName)
{
	//读取Xml文件，并遍历
	try
	{
		CString appPath = GetAppPath();
		string seperator = "\\";
		string fullPath = appPath.GetBuffer(0) +seperator+szFileName;
		//创建一个XML的文档对象。
		TiXmlDocument *myDocument = new TiXmlDocument(fullPath.c_str());
		myDocument->LoadFile();
		//获得根元素，即Persons。
		TiXmlElement *RootElement = myDocument->RootElement();
		//输出根元素名称，即输出Persons。
		cout < <  RootElement->Value() < <  endl;
		//获得第一个Person节点。
		TiXmlElement *FirstPerson = RootElement->FirstChildElement();
		//获得第一个Person的name节点和age节点和ID属性。
		TiXmlElement *NameElement = FirstPerson->FirstChildElement();
		TiXmlElement *AgeElement = NameElement->NextSiblingElement();
		TiXmlAttribute *IDAttribute = FirstPerson->FirstAttribute();
		//输出第一个Person的name内容，即周星星；age内容，即；ID属性，即。
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

//操作形成的XML文件内容如下：
// < Persons> 
// < Person ID="1"> 
// < name>周星星< /name> 
// < age>20< /age> 
// < /Person> 
// < Person ID="2"> 
// < name>白晶晶< /name> 
// < age>18< /age> 
// < /Person> 
// < /Persons> 

//结束了OK。



//另外一个例子
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
		Meta.SetAttribute("NAME","页面浏览");
		Meta.SetAttribute("CONTENT","页面浏览");
		//</META>
		//<TITLE>
		TiXmlElement title("TITLE");
		TiXmlText text("页面浏览");
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
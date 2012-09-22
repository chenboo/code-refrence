//Vector 常用法

//包含的头文件:
	struct STDevice_Valid
	{
		CString		strDeviceID;
		CString		strDeviceName;
		BOOL		bInvalid;

		STDevice_Valid()
		{
			strDeviceID = _T("");
			strDeviceName = _T("");
			bInvalid = FALSE;
		}
	};

	typedef vector<STDevice_Valid> DeviceArry;

	DeviceArry deviceArray;
	STDevice_Valid	devicesel;
	devicesel.strDeviceName = _T("deviceName");
	devicesel.strDeviceID = _T("deviceID");
	devicesel.bInvalid = FALSE;

//一、往vector里面加入数据:

//1、往vector的尾部加入数据:
	deviceArray.push_back(devicesel);	
//2、往vector的某一位置加入数据:
	deviceArray.insert(deviceArray.begin() + 1,devicesel);
	
//二、往vector里面删除数据:

//1、往vector的尾部删除数据:
	deviceArray.pop_back();	
//2、往vector的某一位置删除数据:
	deviceArray.erase(deviceArray.begin());
	
	
//三、访问vector里面的数据:

	//使用两种方法来访问vector。
	//1、   vector::at()
	//2、   vector::operator[]
	//operator[]主要是为了与C语言进行兼容。它可以像C语言数组一样操作。
	//但at()是我们的首选，因为at()进行了边界检查，如果访问超过了vector的范围，将抛出一个例外。
	//由于operator[]容易造成一些错误，所有我们很少用它，下面进行验证一下：
	//分析下面的代码:

	vector<int> v;
	v.reserve(10);//先储备10个单位的空间
	for(int i = 0; i < 7; i++)
	{
		v.push_back(i);
	}
	try
	{
		int iVal1 = v[7]; // not bounds checked - will not throw
		int iVal2 = v.at(7); // bounds checked - will throw if out of range
	}
	catch(const exception& e)
	{
		cout << e.what();
	}

//四、遍历vector里面的数据:
	DeviceArray::iterator it;	
	for(it = deviceArray.begin(); it != deviceArray.end(); it++)
	{
		(*it).strDeviceID = _T("DeviceID");
		(*it).strDeviceName = _T("DeviceName");
		(*it).bInvalid = FALSE;
	}

	



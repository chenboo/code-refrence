//Vector ���÷�

//������ͷ�ļ�:
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

//һ����vector�����������:

//1����vector��β����������:
	deviceArray.push_back(devicesel);	
//2����vector��ĳһλ�ü�������:
	deviceArray.insert(deviceArray.begin() + 1,devicesel);
	
//������vector����ɾ������:

//1����vector��β��ɾ������:
	deviceArray.pop_back();	
//2����vector��ĳһλ��ɾ������:
	deviceArray.erase(deviceArray.begin());
	
	
//��������vector���������:

	//ʹ�����ַ���������vector��
	//1��   vector::at()
	//2��   vector::operator[]
	//operator[]��Ҫ��Ϊ����C���Խ��м��ݡ���������C��������һ��������
	//��at()�����ǵ���ѡ����Ϊat()�����˱߽��飬������ʳ�����vector�ķ�Χ�����׳�һ�����⡣
	//����operator[]�������һЩ�����������Ǻ������������������֤һ�£�
	//��������Ĵ���:

	vector<int> v;
	v.reserve(10);//�ȴ���10����λ�Ŀռ�
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

//�ġ�����vector���������:
	DeviceArray::iterator it;	
	for(it = deviceArray.begin(); it != deviceArray.end(); it++)
	{
		(*it).strDeviceID = _T("DeviceID");
		(*it).strDeviceName = _T("DeviceName");
		(*it).bInvalid = FALSE;
	}

	



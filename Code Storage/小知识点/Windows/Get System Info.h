//���ܼ���ƽ���õñȽ϶�Ļ�ȡϵͳ��Ϣ�ĺ���

//��ȡ����Ĵ�С(����������������)

CRect rcWorkArea;
SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);


//�����Ļ��Ⱥ͸߶�
int nFullWidth = GetSystemMetrics(SM_CXSCREEN);
int nFullHeight = GetSystemMetrics(SM_CYSCREEN);
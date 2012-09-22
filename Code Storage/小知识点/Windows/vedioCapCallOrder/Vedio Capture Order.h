//��Ƶ�ɼ�����˳��

//��Ҫ�õ�����ģ��:

//1��IDShowCaptureModule(������Ƶ�Ĳɼ�)
//2��IDDrawRendererModule(������Ƶ����Ⱦ)


//һ����ʼ����Ƶ�ɼ�ģ��

//ͨ��CXIVideoChannel�ĺ���
//1��SetRendererCallback ���������ڻص������ڵĹ���renderwnd��һЩ����
//��Ҫ�ǽӿ�CIRendererCallback�ṩ��һЩ������

class CIRendererCallback
{
public:
	enum STATUS {
		STATUS_NORMAL,
		STATUS_DEVICE_NOT_SIGNAL,
		STATUS_DEVICE_NOT_SELECTED,
		STATUS_DEVICE_NOT_PRESENT,
		STATUS_DEVICE_NOT_SUPPORTED,
		STATUS_CAN_NOT_DISPLAY
	};

public:
	virtual IVoIPSink * GetInputSink(PVOID pvParam) = NULL;
	virtual BOOL SetRenderer(int cx, int cy, DWORD dwColorSpace, PVOID pvParam) = NULL;
	virtual void SetStatus(STATUS status, PVOID pvParam) = NULL;
};

//2��SetDevice���������ú������������Ϣ
//�ڸú����л����IDShowCaptureModuleģ��ĳ�ʼ����
//����


//������ʼ����Ƶ��Ⱦģ��(��CRenderWnd�н���)
//��CXIVideoChannel���нӿڵĻص�ʱ�ڵ���SetRendererʱ
//�����IDDrawRendererModule(��Ⱦģ��)�ĳ�ʼ����
//����GetInput�����л�õ�CRenderwnd��ָ�룬CRenderWnd����ʵ����IVoIPSink�ӿ�
//������Ƶ�ɼ�ģ����ýӿں���
//virtual void PutFrame(const BYTE * pbFrame, int cbFrame, const BYTE * pbFrameInfo, int cbFrameInfo);
//���õ�����Ƶ��Ϣ������Ⱦģ���У���Ⱦģ����Զ�������Ⱦ



//CXIAudioChannel�� CXIVedioChannel���Ƕ�Ӧһ��������Ƶ�豸�Ĳɼ��豸
//���ǻ����Щ�豸�л�ȡ��Ϣ����Ƶ��������Ƶ
//

//CXIVedioChannel�������ݴ������Ĳ�����������Ƶ�ɼ�ģ�飬������Ҫ����һ�����Խ�����Ƶ
//���ݲ����ܹ���ʾ��������Ⱦģ��
//���ģ�����CRendererWnd

//CXIAudioChannel�������ݴ������Ĳ�����������Ƶ�ɼ�ģ�飬
//������������������������һ���豸���������������û���ر��д
//��������Ⱦģ�飬����Ĭ�Ͻ����˴��������뵽������


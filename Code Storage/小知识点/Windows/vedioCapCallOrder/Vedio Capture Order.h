//视频采集调用顺序：

//主要用到两个模块:

//1、IDShowCaptureModule(用于视频的采集)
//2、IDDrawRendererModule(用于视频的渲染)


//一、初始化视频采集模块

//通过CXIVideoChannel的函数
//1、SetRendererCallback 函数，用于回调主窗口的关于renderwnd的一些函数
//主要是接口CIRendererCallback提供的一些函数。

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

//2、SetDevice函数，给该函数传入相关信息
//在该函数中会进行IDShowCaptureModule模块的初始化和
//设置


//二、初始化视频渲染模块(在CRenderWnd中进行)
//在CXIVideoChannel进行接口的回调时在调用SetRenderer时
//会进行IDDrawRendererModule(渲染模块)的初始化。
//调用GetInput函数中会得到CRenderwnd的指针，CRenderWnd函数实现了IVoIPSink接口
//会让视频采集模块调用接口函数
//virtual void PutFrame(const BYTE * pbFrame, int cbFrame, const BYTE * pbFrameInfo, int cbFrameInfo);
//将得到的视频信息放入渲染模块中，渲染模块会自动进行渲染



//CXIAudioChannel、 CXIVedioChannel就是对应一个个音视频设备的采集设备
//它们会从这些设备中获取信息，音频或者是视频
//

//CXIVedioChannel里面会根据传进来的参数，创建视频采集模块，它所需要的是一个可以接受视频
//数据并且能够显示出来的渲染模块
//这个模块就是CRendererWnd

//CXIAudioChannel里面会根据传进来的参数，创建音频采集模块，
//由于我们听到的声音都来自一个设备即耳机，因此这里没有特别的写
//声音的渲染模块，里面默认进行了处理，即接入到耳机。


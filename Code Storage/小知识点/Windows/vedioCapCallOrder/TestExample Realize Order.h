//视频采集单路采集的测试程序调用步骤：

//设置输出目录:..\..\..\RunImage\$(PlatformName)\$(ConfigurationName)

//中间目录: ..\..\..\Intermediate\$(PlatformName)\$(ConfigurationName)\$(ProjectName)

//附加包含目录: ../../../Include;../../../eXchange/Include

//添加附件库的目录: "$(OutDir)\"

//需要链接的库: LibProjectord.lib


//在stdafx.h文件中加入如下语句:


#include "LibVoIP/BaseModules.h"
using namespace VoIP;
#include "CodeSnippets/teacherframe.h"
#include "../../Libraries/LibProjector/Export.h"

//启动单通道视频:
//在相应函数加入如下语句:

	if (PRJT_SingleChannel_IsCreated()) 
	{
		PRJT_SingleChannel_Destroy();
	}
	else
	{
		PRJT_SINGLE_CHANNEL_PARAMS params = {0};
		params.hNotifyWnd = GetSafeHwnd();
		params.nNotifyMsg = WM_PROJECTOR_NOTIFY;

		params.bEmbedFrameMode = FALSE;
		params.pWndParent = this;

		params.bHasVideo = TRUE;
		params.bHasAudio = TRUE;

		PRJT_SingleChannel_Create(&params);
	}
	
//其中WM_PROJECTOR_NOTIFY消息为单通道窗口关闭时发送给子窗口的消息

//在主窗口加入对此消息的相应函数:


LRESULT Cxxxxx::OnProjectorNotify(WPARAM wParam, LPARAM lParam)
{
	PRJT_SingleChannel_Destroy();
	return 0;
}

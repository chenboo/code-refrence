//��Ƶ�ɼ���·�ɼ��Ĳ��Գ�����ò��裺

//�������Ŀ¼:..\..\..\RunImage\$(PlatformName)\$(ConfigurationName)

//�м�Ŀ¼: ..\..\..\Intermediate\$(PlatformName)\$(ConfigurationName)\$(ProjectName)

//���Ӱ���Ŀ¼: ../../../Include;../../../eXchange/Include

//��Ӹ������Ŀ¼: "$(OutDir)\"

//��Ҫ���ӵĿ�: LibProjectord.lib


//��stdafx.h�ļ��м����������:


#include "LibVoIP/BaseModules.h"
using namespace VoIP;
#include "CodeSnippets/teacherframe.h"
#include "../../Libraries/LibProjector/Export.h"

//������ͨ����Ƶ:
//����Ӧ���������������:

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
	
//����WM_PROJECTOR_NOTIFY��ϢΪ��ͨ�����ڹر�ʱ���͸��Ӵ��ڵ���Ϣ

//�������ڼ���Դ���Ϣ����Ӧ����:


LRESULT Cxxxxx::OnProjectorNotify(WPARAM wParam, LPARAM lParam)
{
	PRJT_SingleChannel_Destroy();
	return 0;
}

#ifndef __ELCBASEUI_EXPORT_H__
#define __ELCBASEUI_EXPORT_H__

#ifdef LIBELCBASEUI_EXPORTS
#define LIBELCBASEUI_API __declspec(dllexport)
#else
#define LIBELCBASEUI_API __declspec(dllimport)
#endif




#include <afxtempl.h>

#include "gdiplus.h"
using namespace Gdiplus;

#include "ElcSkinStyle.h"
#include "ElcUICommon.h"
#include "ElcUISnippets.h"

#include "ElcSkinDoc.h"
#include "ElcSkinPainter.h"
#include "NCSkinBinding.h"
#include "ElcSkinBackgndWnd.h"
#include "ElcNWCtrlContainer.h"
#include "ElcSkinFrameBase.h"
#include "ElcSkinFrame.h"
#include "ElcSkinDialog.h"
#include "ElcSkinWnd.h"
#include "SkinWndBinder.h"

#include "BarButton.h"
#include "BarButtonEx.h"
#include "ElcButton.h"
#include "ElcUnionButton.h"
#include "BlendMsgWnd.h"
#include "BlendSelectWnd.h"
#include "RadioListBox.h"
#include "VolumeSlider.h"
#include "ElcSlider.h"
#include "ElcStatusBar.h"
#include "ElcRichMenu.h"
#include "RollLabel.h"
#include "Floatbar.h"
#include "ElcTabCtrl.h"
#include "ElcBaseBar.h"
#include "ElcToolBar.h"
#include "ElcToolBarEx.h"
#include "ElcFloatBar.h"
#include "ElcBalloon.h"
#include "ElcSplitter.h"
#include "ElcRichList.h"
#include "ElcStatic.h"
#include "ButtonEx.h"
#include "Splash.h"
#include "SimpleSplitter.h"
#include "SplitterControl.h"
#include "GradientCaption.h"
#include "BtnST.h"
#include "ColourPickerCB.h"
#include "ThumbListCtrl.h"
#include "ElcHeaderCtrl.h"
#include "ElcReportListCtrl.h"
#include "ElcListCtrl.h"

#ifdef __ELC_BASE_STUDVOICE_SUPPORT__

#pragma warning ( disable : 4311 4312 )

#define STUD_VT_SPEAK			0x00000001
#define STUD_VT_SCENE			0x00000002
#define STUD_VT_TRANSLATE		0x00000004


typedef struct tagSTUDV_ITEM
{
	TCHAR szFriendName[32];
	PMEMBERINFO pMemberInfo;
}STUDV_ITEM, *PSTUDV_ITEM;

class IStudentVoiceDialogCallback
{
public:
	virtual void OnStudentVoiceStart(DWORD dwVoiceType, int nCount, PSTUDV_ITEM pItems) = 0;
};

class IStudentCheckDialogCallback
{
public:
	virtual void OnStudentCheckChanged(UINT64 dwFuncMask, BOOL bCheck, int nCount, PMEMBERINFO* ppMemberInfo) = 0;
};

class IStudentDemoDialogCallback
{
public:
	virtual void OnStudentDemoStart(PMEMBERINFO pDemoMemberInfo, int nCount, PMEMBERINFO* ppMemberInfo) = 0;
};

class IElcCommonDialog
{
public:
	virtual void Destroy() = 0;
};

#endif // __ELC_BASE_STUDVOICE_SUPPORT__

extern "C"
{
	#ifdef __ELC_BASE_STUDVOICE_SUPPORT__

	typedef struct tagSTUDV_PARAMS
	{
		ITeacherFrame* pTeacherFrame;
		IStudentVoiceDialogCallback* pCallback;
		CString strMsgBoxTitle;
		CWnd* pParent;
		DWORD dwVoiceMask; 
		BOOL bModelless;
		POINT ptLeftTop;

		UINT64 dwFuncMask;
	}STUDV_PARAMS, *PSTUDV_PARAMS;

	typedef struct tagSTUDC_PARAMS
	{
		ITeacherFrame* pTeacherFrame;
		IStudentCheckDialogCallback* pCallback;
		CWnd* pParent;
		BOOL bModelless;
		POINT ptLeftTop;
		UINT64 dwFuncMask;
		PMEMBERINFO pExcludeMember;
	}STUDC_PARAMS, *PSTUDC_PARAMS;

	typedef struct tagSTUDD_PARAMS
	{
		ITeacherFrame* pTeacherFrame;
		PMEMBERINFO pDemoMemberInfo;
		CWnd* pParent;
		int nRecvCount;
		PMEMBERINFO* ppRecvMemberInfo;
	}STUDD_PARAMS, *PSTUDD_PARAMS;

	LIBELCBASEUI_API BOOL ElcBase_CreateStudentVoiceDlg(PSTUDV_PARAMS pParams);
	LIBELCBASEUI_API BOOL ElcBase_CreateStudentCheckDlg(PSTUDC_PARAMS pParams, int nCount = 0, PMEMBERINFO* ppMemberInfo = NULL);
	LIBELCBASEUI_API BOOL ElcBase_CreateStudentDemoDlg(PSTUDD_PARAMS pParams);

	#endif // __ELC_BASE_STUDVOICE_SUPPORT__

	LIBELCBASEUI_API BOOL SetCtrlPos(UINT nPosId, CWnd* pWndCtrl, CWnd* pParent);
	LIBELCBASEUI_API BOOL GetStudentImage(CBitmap& bitmap);

	LIBELCBASEUI_API BOOL LoadImageFile(LPCTSTR lpszFile, CBitmap* pBitmap, CSize& size);
	LIBELCBASEUI_API BOOL LoadImageFromBuffer(LPBYTE lpBuffer, int cbBuffer, CBitmap* pBitmap, CSize& size);

	LIBELCBASEUI_API LPVOID LoadImageFromResource(UINT nRedId, LPCTSTR lpszType);
	LIBELCBASEUI_API void ReleaseImage(LPVOID pImage);

	LIBELCBASEUI_API void ElcBase_ProcessDllIdle();
	LIBELCBASEUI_API BOOL ElcBase_FilterDllMsg(MSG* pMsg);

	LIBELCBASEUI_API CElcSkinDoc* GetSkinDoc();
	LIBELCBASEUI_API CElcSkinPainter* GetPainter();
	LIBELCBASEUI_API CNCSkinBinding* GetNCSkinBinder();
	LIBELCBASEUI_API CSkinWndBinder* GetSkinWndBinder();

	LIBELCBASEUI_API BOOL CreateTransparentWnd(CWnd* pWnd, BYTE btAlpha, const RECT& rect, CWnd* pParent);
}

#endif //__ELCBASEUI_EXPORT_H__
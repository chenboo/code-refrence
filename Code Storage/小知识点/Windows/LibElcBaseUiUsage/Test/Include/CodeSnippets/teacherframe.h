#ifndef __TEACHERFRAME_H__
#define __TEACHERFRAME_H__

#include "Protocol.h"
#include "ADCCommon.h"
//#include "features.h"

class IBaseView;

// Teacher Frame
class ITeacherFrameNotify
{
public:
	virtual void OnMemberConnected(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData) {}
	virtual void OnMemberConnected(PMEMBERINFO pMemberInfo, DWORD dwUserData) {}

	virtual void OnMemberDisconnected(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData) {}
	virtual void OnMemberDisconnected(PMEMBERINFO pMemberInfo, DWORD dwUserData) {}

	virtual void OnMemberAdded(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData) {}
	virtual void OnMemberAdded(PMEMBERINFO pMemberInfo, DWORD dwUserData) {}

	virtual void OnMemberRemoved(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData) {}
	virtual void OnMemberRemoved(PMEMBERINFO pMemberInfo, DWORD dwUserData) {}
	virtual void OnAllMembersRemoved(DWORD dwUserData) {}

	virtual void OnMemberCheckChanged(PMEMBERINFO pMemberInfo, DWORD dwUserData) {}
	virtual void OnMemberGroupIdChanged(PMEMBERINFO pMemberInfo, DWORD dwUserData) {}
	virtual void OnMemberBatteryChanged(PMEMBERINFO pMemberInfo, DWORD dwUserData) {}

	virtual void OnMemberNameChanged(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData) {}
	virtual void OnMemberNameChanged(PMEMBERINFO pMemberInfo, DWORD dwUserData) {}

	virtual void OnMemberHandUp(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData) {}
	virtual void OnMemberHandUp(PMEMBERINFO pMemberInfo, DWORD dwUserData) {}

	virtual void OnMemberHandDown(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData) {}
	virtual void OnMemberHandDown(PMEMBERINFO pMemberInfo, DWORD dwUserData) {}

	virtual void OnMemberStateChanged(LPCTSTR lpszName, BYTE* abMac, DWORD dwMemberIp, DWORD dwUserData, UINT64 dwOldState, UINT64 dwNewState) {}
	virtual void OnMemberStateChanged(PMEMBERINFO pMemberInfo, DWORD dwUserData, UINT64 dwOldState, UINT64 dwNewState) {}

	virtual void OnFunctionChanged(int nFunction, UINT64 dwCommand, BOOL bStart) {}

	virtual void OnFrameExit(DWORD dwUserData) {}
};

enum FUNCTION_GUID {
	FUNCID_BROADCAST,
	FUNCID_VOICE_TEACH,
	FUNCID_MONITOR,
	FUNCID_DEMO,
	FUNCID_VOICE_CHAT,
	FUNCID_MOVIE,
	FUNCID_RECORD,
	FUNCID_PLAYBACK,
	FUNCID_OPTIONS,
	FUNCID_FILE_CAST,
	FUNCID_CLASS_MODEL,
	FUNCID_GROUP_TEACH,
	FUNCID_GROUP_CHAT,
	FUNCID_EXTENSION,
	FUNCID_GROUP_CALL,
	FUNCID_VRECORD,
	FUNCID_QUIZ,
	FUNCID_APP_FORCE,
	FUNCID_WEB_FORCE,
	FUNCID_VWEBSITE,
	FUNCID_QUIZINSTANT,
	FUNCID_NONE
};

class IMainFrame
{
public:
	virtual BOOL IsTeacherFrame() = 0;
	
	virtual void ShowMainFloatbar(UINT64 dwFunctionCmd, BOOL bShow) = 0;

	// Get audio device's id
	// return GUID_NULL if not found
	virtual GUID GetVoiceDevice(ELC_AUDIO_MIXER_TYPE type) = 0;

	//////////////////////////////////////////
	// get / set profile data
	virtual UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault) = 0;
	virtual BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue) = 0;
	virtual CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault) = 0;
	virtual BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue) = 0;

	// send command packet
	virtual void SendReliableCommand(DWORD* pdwDestIpAddr, DWORD dwCount, LPVOID lpvCommand, DWORD cbSize) = 0;
};

class ITeacherFrame : public IMainFrame
{
public:
	enum NOTIFYMASK {
		ELC_MEMBER_CONNECTED			= 0x00000001,
		ELC_MEMBER_DISCONNECTED			= 0x00000002,
		ELC_MEMBER_ADDED				= 0x00000004,
		ELC_MEMBER_REMOVED				= 0x00000008,
		ELC_MEMBER_REMOVED_ALL			= 0x00000010,
		ELC_MEMBER_NAMECHANGED			= 0x00000020,
		ELC_MEMBER_HANDUP				= 0x00000040,
		ELC_MEMBER_HANDDOWN				= 0x00000080,
		ELC_MEMBER_STATE				= 0x00000100,
		ELC_MEMBER_CHECK_CHANGED		= 0x00000200,
		ELC_MEMBER_GROUPID_CHANGED		= 0x00000400,
		ELC_MEMBER_BATTERY_CHANGED		= 0x00000800,
		ELC_MEMBER_AUDIOPLUG_CHANGED	= 0x00001000,
		ELC_FUNCTION_CHANGED			= 0x00002000,
		ELC_EXIT						= 0x00004000,
		ELC_AUDIO_CHANGED				= 0x00008000,
		ELC_VOICE_METER					= 0x00010000,
	};

public:
	// Event notify
	virtual BOOL RegisterCallback(DWORD dwMask, ITeacherFrameNotify *pCallback, DWORD dwUserData) = 0;
	virtual void UnRegisterCallback(ITeacherFrameNotify *pCallback) = 0;

	virtual int GetAllMemberInfoPtrs(PMEMBERINFO* ppMemberInfo) = 0;

	virtual void ShowContextMenu(IBaseView* pView, CPoint point) = 0;

	virtual void OnMemberDblclicked(IBaseView* pView, PMEMBERINFO pMemberInfo) = 0;
	virtual void OnViewChanged(BASEVIEW_TYPE type) = 0;

	virtual void SetMemberCheck(PMEMBERINFO* ppMemberInfo, int nCount, BOOL bCheck) = 0;
	virtual void SetMemberGroupId(PMEMBERINFO* ppMemberInfo, int nCount, DWORD dwGroupId) = 0;
	virtual void SetThumbnailSize(DWORD dwGroupId, int cxThumbnail, int cyThumbnail) = 0;
	virtual void SetSubmitEnable(PMEMBERINFO pMemberInfo, BOOL bEnable) = 0;
	virtual void SetSubmitEnableRemove(PMEMBERINFO pMemberInfo) = 0;
	virtual void SetSubmitEnableRemoveAll() = 0;
	virtual CString GetSubmitPath(LPCTSTR lpszStudentName) = 0;

	virtual CString GetGroupName(DWORD dwGroupId) = 0;

	//
	virtual DWORD GetAddress(DWORD nFuncID) = 0;
	virtual WORD GetPort(DWORD nFuncID) = 0;
	virtual void PutAddress(DWORD dwIpAddress) = 0;
	virtual void PutPort(WORD wPort) = 0;
	virtual void PutAddressPort(DWORD dwIpAddress, WORD wPort) = 0;
	virtual void PutFunctionAddressPort(DWORD nFuncID) = 0;

	virtual int  GetFrameValueInt(TCHAR* pszEntry) = 0;
	virtual CString GetFrameValueString(TCHAR* pszEntry) = 0;
};

// Student Frame
class IStudentFrameNotify
{
public:
	virtual void OnConnected(LPCTSTR lpszTeacherName, DWORD dwTeacherIp, DWORD dwUserData) {}
	virtual void OnDisconnected(DWORD dwUserData) {}
};

class IStudentFrame : public IMainFrame
{
public:
	enum NOTIFYMASK {
		ELC_CONNECTED =		0x00000001,
		ELC_DISCONNECTED =	0x00000002,
		ELC_AUDIO_CHANGED =	0x00000004,
		ELC_NAME_CHANGED =	0x00000008,
	};

public:
	// Event notify
	virtual BOOL RegisterCallback(DWORD dwMask, IStudentFrameNotify *pCallback, DWORD dwUserData) = 0;
	virtual void UnRegisterCallback(IStudentFrameNotify *pCallback) = 0;

	virtual BOOL BeginRemoteControl(PCONTROLPARAMS pControlParams) = 0;
	virtual void StopRemoteControl() = 0;

	virtual void OnFileReceived(CString strFilePath) = 0;
};


//////////////////////////////////////////////
// base view 
class IBaseView : public ITeacherFrameNotify
{
public:
	virtual CWnd* GetWindowPtr() = 0;
	virtual int GetViewType() = 0;
	virtual void DestroyMe() = 0;

	virtual void SetBackgndColor(COLORREF color) = 0;
	
	// if ppMemberInfo == 0, return the counts
	virtual int GetCheckedMember(PMEMBERINFO* ppMemberInfo) = 0;
	virtual void SetCheckedMember(PMEMBERINFO* ppMemberInfo, int nCount) = 0;
	
	// if ppMemberInfo == 0, return the counts
	virtual int GetSelectedMember(PMEMBERINFO* ppMemberInfo) = 0;
	virtual void SetSelectedMember(PMEMBERINFO* ppMemberInfo, int nCount) = 0;

	virtual PMEMBERINFO GetFirstSelectedMember() = 0;
	
	virtual PMEMBERINFO GetFocusMember() = 0;
	virtual void SetFocusMember(PMEMBERINFO pMemberInfo) = 0;
	virtual void EditFocusedMember() = 0;

	// if ppMemberInfo == 0, return the counts
	virtual int GetAllMembers(PMEMBERINFO* ppMemberInfo) = 0;
	
	virtual void OnGroupChanged(DWORD dwGroupId) = 0;
	virtual void OnGroupLeaderChanged(PMEMBERINFO pOldLeader, PMEMBERINFO pNewLeader) = 0;
};



#endif //_TEACHERFRAME_H__
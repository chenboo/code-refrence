#ifndef __SYSTEMDIRECTORY_H__
#define __SYSTEMDIRECTORY_H__

#define WINDIR						0x0001
#define WINSYSDIR					0x0002
#define WINTEMPDIR					0x0003
#define FOLDER_PROGRAMS				0x0004
#define FOLDER_COMMON_FILES			0x0005
#define FOLDER_INSTALL				0x0006
#define FOLDER_MYDOCUMNETS			0x0007
#define FOLDER_DESKTOP				0x0008
#define FLODER_STARTMEMU			0x0009
#define FOLDER_STARTUP				0x000a

#define MAX_SYSDIR_NUMS				0x000a

class CSystemDirectoryNet
{
public:
	CSystemDirectoryNet();
	virtual ~CSystemDirectoryNet();

public:
	WORD GetSysDirID(CString strName);
	CString GetSysDirName(WORD wSysDirID);

	CString GetSysDirValueByID(WORD wSysDirID);
	CString GetSysDirValueByName(CString strName);

	CString TranslateSystemPath(CString strPath);

	CString Get_WinDir();
	CString Get_WinSysDir();
	CString Get_WinTempDir();
	CString Get_DesktopDir();
	CString Get_ProgramsDir();
	CString Get_MydocumnetsDir();
	CString Get_StartmenuDir();
	CString Get_Startup();
	CString Get_CommonfilesDir();
	CString Get_InstallDir();

	CString GetCurrentVersonPath(CString strKeyName);
	CString GetShellFolders(CString strKeyName);

	BOOL IsSystemDefinedPath(CString strName);
};

#endif //__SYSTEMDIRECTORY_H__

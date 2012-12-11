; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CWaveView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "wave.h"
LastPage=0

ClassCount=5
Class1=CMainFrame
Class2=CWaveApp
Class3=CAboutDlg
Class4=CWaveDoc
Class5=CWaveView

ResourceCount=2
Resource1=IDR_MAINFRAME
Resource2=IDD_ABOUTBOX

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CWaveApp]
Type=0
BaseClass=CWinApp
HeaderFile=Wave.h
ImplementationFile=Wave.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Wave.cpp
ImplementationFile=Wave.cpp
LastObject=CAboutDlg

[CLS:CWaveDoc]
Type=0
BaseClass=CDocument
HeaderFile=WaveDoc.h
ImplementationFile=WaveDoc.cpp
LastObject=CWaveDoc

[CLS:CWaveView]
Type=0
BaseClass=CView
HeaderFile=WaveView.h
ImplementationFile=WaveView.cpp
LastObject=ID_PLAY
Filter=C
VirtualFilter=VWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_APP_ABOUT
Command3=ID_STOP
Command4=ID_PLAY
CommandCount=4

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_MRU_FILE1
Command3=ID_APP_EXIT
Command4=ID_EDIT_PLAY
Command5=ID_EDIT_STOP
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_APP_ABOUT
CommandCount=8

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14


// CWavCtrDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CWavCtr.h"
#include "CWavCtrDlg.h"
#include<mmsystem.h> 
#pragma comment(lib,"winmm.lib ") 
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
 



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
int N=0;//�������Ͳ���ͼ1:20�ı���ת����N�Ǽ�����

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCWavCtrDlg �Ի���




CCWavCtrDlg::CCWavCtrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCWavCtrDlg::IDD, pParent)
	, time(_T(""))
	, WavInfo(_T(""))
	, FileOpen(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCWavCtrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, CS_Progress);
}

BEGIN_MESSAGE_MAP(CCWavCtrDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CCWavCtrDlg::OnBnClickedOk)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, &CCWavCtrDlg::OnNMReleasedcaptureSlider1)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SOP, &CCWavCtrDlg::OnBnClickedSop)
	ON_BN_CLICKED(IDC_BUTTON1, &CCWavCtrDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CCWavCtrDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CCWavCtrDlg ��Ϣ�������

BOOL CCWavCtrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	wavWnd.wavSize=0;
	wavWnd.sample_time=0;
	wavWnd.sample_Hz=0;
	wavWnd.sample_Hz=0;
	wavWnd.sample_channel=0;
	wavWnd.sample_BperSec=0;
	wavWnd.IsPlaying=false;
	wavWnd.FileName=_T("");
	wavWnd.FilePath=_T("");
	wavWnd.data_ptr=NULL;
	wavWnd.current_time=0;
	FileOpen=false;
	((CWnd *)(this->GetDlgItem(IDC_SOP)))->EnableWindow(FALSE);
	((CWnd *)(this->GetDlgItem(IDC_BUTTON1)))->EnableWindow(FALSE);



	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCWavCtrDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCWavCtrDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CCWavCtrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCWavCtrDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CFileDialog cfd(TRUE);
	cfd.m_ofn.lpstrTitle=_T("���ļ�");
	cfd.m_ofn.lpstrFilter=_T("Wav��Ƶ�ļ���*.wav��\0*.wav\0");
	if(IDOK==cfd.DoModal())
	{
		wavWnd.FileName=cfd.GetFileName();
		wavWnd.FilePath=cfd.GetPathName();
		AnalyWav();
		N=0;
	}

}

void CCWavCtrDlg::AnalyWav(void)
{
	delete [] wavWnd.m_pData;

	wavWnd.wavFile.Open(wavWnd.FilePath,CFile::modeRead,0);
	bool riff=false;
	bool wave=false;
	bool fmt=false;
	//if(FileOpen) delete[] wavWnd.data_ptr;
	unsigned char*pBuf=new unsigned char[58];

	wavWnd.wavFile.Read((void*)pBuf,58);

	//��֤wav�ļ�����
	if(pBuf[0]=='R'&&pBuf[1]=='I'&&pBuf[2]=='F'&&pBuf[3]=='F') riff=true;
	if(pBuf[8]=='W'&&pBuf[9]=='A'&&pBuf[10]=='V'&&pBuf[11]=='E') wave=true;
	if(pBuf[12]=='f'&&pBuf[13]=='m'&&pBuf[14]=='t') fmt=true;

	//��֤��ϣ���ȡwavͷ�ļ���Ϣ
	if(riff&&wave&&fmt)
	{
		wavWnd.sample_channel=pBuf[23]*256+pBuf[22];                                 //������Ŀ
		wavWnd.sample_Hz=pBuf[24]+pBuf[25]*256+pBuf[26]*256*256+pBuf[27]*256*256*256;
		wavWnd.sample_bit=pBuf[35]*256+pBuf[34];                                     //ÿ����������λ
		wavWnd.sample_BperSec=pBuf[31]*256*256*256+pBuf[30]*256*256+pBuf[29]*256+pBuf[28];

		int pos=35;
		int temp;
		temp=pBuf[16]+pBuf[17]*256+pBuf[18]*256*256+pBuf[19]*256*256*256;
		pos=pos+temp-16;
		while((pBuf[pos]!='d')&&(pBuf[pos+1]!='a')&&(pBuf[pos+2]!='t')&&(pBuf[pos+3]!='a')) pos++;
		pos+=4;
		wavWnd.wavSize=pBuf[pos]+pBuf[pos+1]*256+pBuf[pos+2]*256*256+pBuf[pos+3]*256*256*256;
		wavWnd.sample_time=(wavWnd.wavSize*8.0)/(wavWnd.sample_Hz*wavWnd.sample_channel*wavWnd.sample_bit*1.0);
		pos+=4;

		wavWnd.wavFile.Seek(pos,CFile::begin);
		pos=wavWnd.wavFile.GetLength()-pos;
		if(wavWnd.sample_bit==16)
		{
			short *temp=new short[pos+1];
			temp[pos]=NULL;
			wavWnd.wavFile.Read((void*)temp,pos);
			wavWnd.data_ptr=temp;
			wavWnd.m_pData = temp;
		}
		else if(wavWnd.sample_bit==8)
		{
			BYTE *temp=new BYTE[pos+1];
			temp[pos]=NULL;
			wavWnd.wavFile.Read((void*)temp,pos);
			wavWnd.data_ptr=temp;
		}
		wavWnd.wavFile.Seek(0,CFile::begin);
		wavWnd.IsPlaying=true;
		wavWnd.back=false;
		wavWnd.current_time=0;
		
		SetTimer(1,1000,NULL);                                  //����ļ�ʱ�������ڿ��ƾ�̬�ı���͹�����
		SetTimer(2,100,NULL);
		wavWnd.wavFile.Close();
		WavInfo=_T("������")+wavWnd.FileName;
		
		CTimeSpan ts(wavWnd.sample_time);
		CTimeSpan tc(wavWnd.current_time);
		time=tc.Format(_T("%H:%M:%S"));
		time+="/";
		CString a;
		a=ts.Format(_T("%H:%M:%S"));
		time=time+a;

		WavInfo+=_T("ʱ�䣺");
		WavInfo+=a;
		WavInfo+=_T("Ƶ�ʣ�");
		a=_T("");
		a.Format(_T("%d"),wavWnd.sample_Hz);
		WavInfo+=a;
		WavInfo+=_T("  ");
		a=_T("");
		if(wavWnd.sample_channel==1) WavInfo+=_T("��");
		else if(wavWnd.sample_channel==2) WavInfo+=_T("˫");
		WavInfo+=_T("���� ");
		a=_T("");
		a.Format(_T("%d"),wavWnd.sample_bit);
		WavInfo+=a;
		WavInfo+=_T("λ");

		CS_Progress.SetRange(0,wavWnd.sample_time);
		CS_Progress.SetPos(wavWnd.current_time);
		CRect r;
		GetWindowRect(r);
		CS_Progress.SetPageSize(r.Width()/wavWnd.sample_time);
		((CWnd *)(this->GetDlgItem(IDC_SOP)))->EnableWindow(TRUE);
		((CWnd *)(this->GetDlgItem(IDC_BUTTON1)))->EnableWindow(TRUE);
		SetDlgItemText(IDC_SOP,_T("��ͣ"));

		PlaySound(wavWnd.FilePath,0,SND_ASYNC|SND_NODEFAULT);




	}
	else
	{
		AfxMessageBox(_T("wav�ļ���ʽ����ȷ"));
		wavWnd.wavFile.Close();
	}
	wavWnd.reOpen=true;
}

int CCWavCtrDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if ( !wavWnd.Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,0,NULL))
		return -1;

	return 0;
}

void CCWavCtrDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CRect r;
	GetClientRect(r);

	if (wavWnd.GetSafeHwnd()) {

		wavWnd.MoveWindow(10,35,r.Width(),r.Height()*3/4);
	}
	// TODO: Add your message handler code here
}

void CCWavCtrDlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	wavWnd.current_time=CS_Progress.GetPos();
	wavWnd.Current_pos=wavWnd.current_time*20;
	N=wavWnd.current_time;
	wavWnd.back=true;
	wavWnd.IsPlaying=true;
	SetDlgItemText(IDC_SOP,_T("��ͣ"));
	*pResult = 0;
}

void CCWavCtrDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case 1:
	{
	SetDlgItemText(IDC_WAVINFO,WavInfo);
	if(wavWnd.IsPlaying)
	{
		if(wavWnd.current_time<=wavWnd.sample_time&&wavWnd.current_time>=0)
		{
			if(wavWnd.P_Direction) {
				int temp=wavWnd.Current_pos/20;
				if(temp>N) 
				{
					wavWnd.current_time++;
					N=temp;
				}
			}
			else
			{
				int temp=wavWnd.Current_pos/20;
				if(temp<N)
				{
					wavWnd.current_time--;
					N=temp;
				}
			}
			
		}

		CTimeSpan ts(wavWnd.sample_time);
		CTimeSpan tc(wavWnd.current_time);

		if(tc>ts) wavWnd.current_time=wavWnd.sample_time;
		if(wavWnd.current_time<0) wavWnd.current_time=0;
		
		

		tc=CTimeSpan(wavWnd.current_time);
		time=tc.Format(_T("%H:%M:%S"));
		time+="/";
		time+=ts.Format(_T("%H:%M:%S"));
		SetDlgItemText(IDC_TIMEINFO,time);
		
		CS_Progress.SetPos(wavWnd.current_time);
	}


		CRect rT;
		wavWnd.GetClientRect(&rT);
		CClientDC dc((CWnd*)&wavWnd);
		CPen penR(PS_SOLID,2,RGB(255,0,0));
		dc.MoveTo(wavWnd.current_x,rT.bottom);
		dc.LineTo(wavWnd.current_x,rT.top);
	}
	case 2:
		{
		CString a,b;
		if(wavWnd.sample_channel==1)
		{
			a.Format(_T("%d"),wavWnd.current_yL);
			a=_T("��������")+a;
		}
		else
		{
			a.Format(_T("%d"),wavWnd.current_yL);
			b.Format(_T("%d"),wavWnd.current_yR);
			a=_T("��������")+a;
			a+=_T("\n");
			b=_T("��������")+b;
			a=a+b;
		}
		SetDlgItemText(IDC_POSINFO,a);

	}

			

	}

	CDialog::OnTimer(nIDEvent);
}

void CCWavCtrDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	KillTimer(1);
	KillTimer(2);

	CDialog::OnClose();
}

void CCWavCtrDlg::OnBnClickedSop()
{
	// TODO: Add your control notification handler code here
	if(wavWnd.sample_time) wavWnd.IsPlaying=!wavWnd.IsPlaying;
	if(wavWnd.IsPlaying) 
	{
		SetDlgItemText(IDC_SOP,_T("��ͣ"));
		wavWnd.Current_pos=wavWnd.current_time*20;
		N=wavWnd.current_time;
	}
	if(!wavWnd.IsPlaying)
	{
		SetDlgItemText(IDC_SOP,_T("��ʼ"));
		wavWnd.Current_pos=wavWnd.current_time*20;
		N=wavWnd.current_time;
	}

}

void CCWavCtrDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	wavWnd.P_Direction=!wavWnd.P_Direction;
}

void CCWavCtrDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	PlaySound(NULL,NULL,SND_ASYNC);
}

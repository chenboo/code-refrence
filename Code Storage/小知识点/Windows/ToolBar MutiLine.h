VC��ʵ�ֶ��й����� 
        VC��ʵ�ֹ������Ǽ�����׾ٵ��£�Ȼ��Ҫʵ�ֶ��й�����ȴ��Ҫһ���ı�̣����ļ�Ҫ����ʵ��һ���򵥵Ķ��й����� �ľ��巽����

         ���й�������ʵ�ַ�������CBRS_SIZE_FIXED��ʽ������������Ȼ������TBBS_WRAPPED����һ��ĩβ�İ�ť��ʽ��ע�⣬����ʱһ��Ҫʹ��CBRS_SIZE_FIXED��ʽ����Ȼ�ǲ���ʹ��������ť���еġ�

        �ȴ���һ�������ĵ���MFCӦ�ó���Ȼ����CMainFrame�������һ��CToolBar���󣬱�������ͨ���Ļ�ͼ������Ϊ�У����ö�������Ϊm_wndDrawingBar��Ȼ������Դ�ļ�����ӻ�ͼ������λͼ�����Ĳ���λͼ����ʱ��Ĭ��IDΪIDB_BITMAP1���ðɣ����е�׼���������Ѿ���ɣ����濪ʼʵ�ֶ��й�������

       ��CMainFrame������ӳ�Ա����CreateDrawingBar()���������

        protected:
        void CreateDrawingBar();

       ʵ�ָú����������������

       // ���ɶ��й���������
       void CMainFrame::CreateDrawingBar()
       {
        //��������ťҪ���У�����ʱҪʹ��CBRS_SIZE_FIXED��ʽ����Ч
         if(!m_wndDrawingBar.CreateEx   (                    this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS) || 
!m_wndDrawingBar.SetButtons(NULL,16) )
         {
        TRACE0("δ�ܴ������й�����\n");
        return;      // δ�ܴ���
        }

         CImageList img;

        img.Create( IDB_BITMAP1, 16, 0, RGB(255,0,0) );
        m_wndDrawingBar.GetToolBarCtrl().SetHotImageList(&img);  
        m_wndDrawingBar.GetToolBarCtrl().SetImageList(&img);
        m_wndDrawingBar.GetToolBarCtrl().SetDisabledImageList(&img);
          img.Detach();

        // ���ð�ť��ʽʱҪʹ��TBBS_WRAPPED�������е�λ��
        m_wndDrawingBar.SetButtonInfo(0,ID_FILE_NEW,TBSTYLE_BUTTON,0);
        m_wndDrawingBar.SetButtonInfo(1,ID_FILE_NEW,TBSTYLE_BUTTON|TBBS_WRAPPED,1); //����
        m_wndDrawingBar.SetButtonInfo(2,ID_FILE_NEW,TBSTYLE_BUTTON,2);
        m_wndDrawingBar.SetButtonInfo(3,ID_FILE_NEW,TBSTYLE_BUTTON|TBBS_WRAPPED,3);
        m_wndDrawingBar.SetButtonInfo(4,ID_FILE_NEW,TBSTYLE_BUTTON,4);
        m_wndDrawingBar.SetButtonInfo(5,ID_FILE_NEW,TBSTYLE_BUTTON|TBBS_WRAPPED,5);
        m_wndDrawingBar.SetButtonInfo(6,ID_FILE_NEW,TBSTYLE_BUTTON,6);
        m_wndDrawingBar.SetButtonInfo(7,ID_FILE_NEW,TBSTYLE_BUTTON|TBBS_WRAPPED,7);
        m_wndDrawingBar.SetButtonInfo(8,ID_FILE_NEW,TBSTYLE_BUTTON,8);
        m_wndDrawingBar.SetButtonInfo(9,ID_FILE_NEW,TBSTYLE_BUTTON|TBBS_WRAPPED,9);
        m_wndDrawingBar.SetButtonInfo(10,ID_FILE_NEW,TBSTYLE_BUTTON,10);
        m_wndDrawingBar.SetButtonInfo(11,ID_FILE_NEW,TBSTYLE_BUTTON|TBBS_WRAPPED,11);
        m_wndDrawingBar.SetButtonInfo(12,ID_FILE_NEW,TBSTYLE_BUTTON,12);
        m_wndDrawingBar.SetButtonInfo(13,ID_FILE_NEW,TBSTYLE_BUTTON|TBBS_WRAPPED,13);
        m_wndDrawingBar.SetButtonInfo(14,ID_FILE_NEW,TBSTYLE_BUTTON,14);
        m_wndDrawingBar.SetButtonInfo(15,ID_FILE_NEW,TBSTYLE_BUTTON|TBBS_WRAPPED,15);         }

       ������CMainFrame���OnCreate�����е���CreateDrawingBar�����������������

        // �������й�����
          CreateDrawingBar();
         ע�⣺�������Ӧ����return���֮ǰ���á�


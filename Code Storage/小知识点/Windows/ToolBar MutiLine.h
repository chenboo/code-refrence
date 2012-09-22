VC中实现多行工具栏 
        VC中实现工具栏是件轻而易举的事，然而要实现多行工具栏却需要一定的编程，本文简要介绍实现一个简单的多行工具栏 的具体方法。

         多行工具栏的实现方法是用CBRS_SIZE_FIXED样式创建工具栏，然后再用TBBS_WRAPPED设置一行末尾的按钮样式。注意，创建时一定要使用CBRS_SIZE_FIXED样式，不然是不会使工具栏按钮换行的。

        先创建一个基于文档的MFC应用程序，然后在CMainFrame类中添加一个CToolBar对象，本文中以通常的绘图工具栏为列，将该对象命名为m_wndDrawingBar。然后在资源文件中添加绘图工具栏位图，本文采用位图载入时的默认ID为IDB_BITMAP1。好吧，所有的准备工作都已经完成，下面开始实现多行工具栏。

       在CMainFrame类中添加成员函数CreateDrawingBar()，语句如下

        protected:
        void CreateDrawingBar();

       实现该函数，具体代码如下

       // 生成多行工具栏函数
       void CMainFrame::CreateDrawingBar()
       {
        //工具栏按钮要换行，创建时要使用CBRS_SIZE_FIXED样式才有效
         if(!m_wndDrawingBar.CreateEx   (                    this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED|CBRS_LEFT|CBRS_TOOLTIPS) || 
!m_wndDrawingBar.SetButtons(NULL,16) )
         {
        TRACE0("未能创建多行工具栏\n");
        return;      // 未能创建
        }

         CImageList img;

        img.Create( IDB_BITMAP1, 16, 0, RGB(255,0,0) );
        m_wndDrawingBar.GetToolBarCtrl().SetHotImageList(&img);  
        m_wndDrawingBar.GetToolBarCtrl().SetImageList(&img);
        m_wndDrawingBar.GetToolBarCtrl().SetDisabledImageList(&img);
          img.Detach();

        // 设置按钮样式时要使用TBBS_WRAPPED标明换行的位置
        m_wndDrawingBar.SetButtonInfo(0,ID_FILE_NEW,TBSTYLE_BUTTON,0);
        m_wndDrawingBar.SetButtonInfo(1,ID_FILE_NEW,TBSTYLE_BUTTON|TBBS_WRAPPED,1); //换行
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

       现在在CMainFrame类的OnCreate函数中调用CreateDrawingBar函数，具体代码如下

        // 创建多行工具栏
          CreateDrawingBar();
         注意：以上语句应该在return语句之前调用。


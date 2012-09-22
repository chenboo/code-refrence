//about the OnNcLButtonUp message

//�ڴ��ڴ�������״̬ʱ������ͨ���϶����������ƶ����ڣ�������ڱ������ϰ�����������
//ϵͳ�Ჶ����꣬����ֱ���ſ��������󣬴����ǲ����յ������Ϣ�ġ�
//������ڴ������״̬�������ڿͻ���������ꡢ�Ƶ��������Ϸſ���
//���ߵ���˵����ȶ������յ�WM_NCLBUTTONUP��Ϣ��

//����ͨ�����������һ����깳����ʵ�֣���OnNcLButtonDown����ʱ���ɿ�����յ�lbuttonup��Ϣ�������򲻻���Ӧ
//ͨ�����ӣ����յ�lbuttonup��Ϣ����Ӧ�ó�����OnNcLButtonUp��Ϣ���Ӷ����������⡣
//����Ϊԭ�Ľ��ܣ�


	//WM_NCLBUTTONUP message patch
	
	//If you spend time investigating what happens when you click and release the left button over the title bar, 
	//you will find out that instead of getting "non-client left button up" message, you just get "left button up". 
	//One could actually work with it, if the message found it's way into the application. It does not.
	
	
	//I was in a middle of writing an application when I discovered this. 
	//I looked all over the internet for articles regarding WM_NCLBUTTONUP problem, 
	//but the only thing I could find were questions about the problem. 
	//After some more investigating I have come up with a patch that could be adopted by each application requiring such notification.
	
	//The patch consists of installing a "windows hook" that will intercept all mouse messages for this application before they enter into the message pump. 
	//To do that you need to call SetWindowsHookEx(...) function, soon after the main window is created. Here is the call: 

	hMHook = SetWindowsHookEx( 
                     // hook type: 
                WH_MOUSE,
                     // hook procedure: 
                (HOOKPROC) MouseHookProc,
                     // handle to application instance: 
                AfxGetInstanceHandle(),
                     // thread identifier: 
                AfxGetThread()->m_nThreadID
            );


	//It is very important that you supply handle to application instance and thread identifier, 
	//otherwise every application running on your computer will attempt to hook 
	//it's mouse messages through your program and it could be disastrous. 
	//By supplying these two parameters you will insure that only messages from your application will end up in your callback function. 

	//Equally important is a call to remove the hook before your application terminates. 
	//The UnhookWindowsHookEx(...) function removes a hook procedure installed in a hook chain. 
	//Most likely you will call it somewhere in OnDestroy(), like this:
	
	 if(hMHook != NULL)
      UnhookWindowsHookEx(hMHook);
	  
	  
	 //The callback function is where you will receive WM_NCLBUTTONDOWN message and the next 
	 //time you receive WM_LBUTTONUP message you will post WM_NCLBUTTONUP directly into the application message pump. 
	 //Therefore, no special handling will be required to service these messages. 
	 //You will simply write your code inside of OnNcLButtonUp(...), just like you would for any other message. 

	//Here is the callback code:
	
	// ////////////////////////////////////////////////////
	// handle to the mouse hook
		HHOOK hMHook = NULL;

	// status of non-client left button down
		BOOL bNcLButtonDown = FALSE;  

// /////////////////////////////////////////////////////
// Mouse hook process

LRESULT CALLBACK MouseHookProc( int nCode, 
                                WPARAM wParam,
                                LPARAM lParam) 
{ 
  if(nCode == HC_ACTION) 
  { 
    // get a pointer to the mouse hook struct. 
    PMOUSEHOOKSTRUCT mhs = (PMOUSEHOOKSTRUCT) lParam; 
    
    // intercept messages for left button down and up
    switch(wParam) 
    { 
        case WM_NCLBUTTONDOWN: 
         {
             // get the pointer to the main window 
             CWnd *pWnd =  AfxGetMainWnd(); 

             // if the message is from your window and 
             // the hit test indicates title bar 
             if((mhs->hwnd == pWnd->GetSafeHwnd()) 
                 && (mhs->wHitTestCode == HTCAPTION))
             { 
                 // then indicate non-client left button down 
                 bNcLButtonDown = TRUE; 
                
                //  there is no problem with this message 
                // so you don't have to do anything else 
             } 
        } 
        break; 
        
        case WM_NCLBUTTONUP: 
            // you will get this message if you double-click 
            //     on the title bar 
            // reset the status 
            bNcLButtonDown = FALSE;  
            break;

        case WM_LBUTTONUP: 
          {
              //  get the pointer to the main window 
              CWnd *pWnd = AfxGetMainWnd(); 

              // if the message is from your window and 
              // non-client left button is down
              if((mhs->hwnd == pWnd->GetSafeHwnd()) 
                  && (bNcLButtonDown == TRUE)) 
              {
                 // then post WM_NCLBUTTONUP message directly 
                 // into your window message pump 
                 // Note: I'm hardcoding HTCAPTION because the 
                 // left button was down, and while it is down,
                 // the mouse does not move in respect to the 
                 // window, but it does in respect to the screen,
                 // so the mouse should still be over the caption
                 // bar of your window when you release the button.
                 pWnd->PostMessage(WM_NCLBUTTONUP, HTCAPTION, 
                                MAKELONG(mhs->pt.x,mhs->pt.y));

                 // reset non-client left button down 
                 bNcLButtonDown = FALSE;
              }
          } 
          break; 

        default: 
            break; 
    } 
  } 
  // let the messages through to the next hook 
 return CallNextHookEx(hMHook, nCode, wParam, lParam); 
}


	//I am including two sample projects. The "nclbxExample" is technical, and the "AlphaDialogExample" is more practical. 
	//The "nclbxExample" is better documented so you can see how and were I have implemented the code. 





//The WM_NCLBUTTONUP message is posted when the user releases the left mouse button while the cursor is within the nonclient area of a window. 
//This message is posted to the window that contains the cursor. If a window has captured the mouse, this message is not posted. 

//�����ڰ������Ի��һ����ť��ʱ�����ĳ�ֲ��������簴�˹رհ�ť�������˺ܶ෽�����ػ����Ϣ�������Ǻܳɹ���
//����żȻ���֣���WM_NCLBUTTONDOWN��ֱ��return 0�����ܴ���WM_NCLBUTTONUP�ˣ��ǳ��򵥣�
//����������������Ĵ����ϻ�����ť��Ҳ�������ˣ���һ�����ӣ��ػ�WM_NCLBUTTONDOWN��ֱ��return 0��
//�ػ�WM_NCHITTEST��������ʱ����ǲ��������ǵģ���ť����rect�У�����ڣ�return HTCLOSE�������ùرհ�ťΪ������
//�ػ�WM_NCLBUTTONUP����MSDN֪����nHittest = (INT) wParam����������switch(nHittest)��case HTCLOSE���رհ�ť������PostMessage(WM_CLOSE,0,0)
//������CODEGURU�Ͽ�����λ���ѷ�����һ����WM_NCLBUTTONUP��Patch�ĳ���Ҳ��ʵ�ֽػ�WM_NCLBUTTONUP�Ĺ��ܣ�



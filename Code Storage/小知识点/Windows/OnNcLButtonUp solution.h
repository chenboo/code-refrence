//about the OnNcLButtonUp message

//在窗口处于正常状态时（可以通过拖动标题栏来移动窗口），如果在标题栏上按下鼠标左键，
//系统会捕获鼠标，所以直到放开鼠标左键后，窗口是不会收到鼠标消息的。
//如果窗口处于最大化状态，或者在客户区按下鼠标、移到标题栏上放开，
//或者点击菜单栏等都可以收到WM_NCLBUTTONUP消息。

//可以通过给程序添加一个鼠标钩子来实现，当OnNcLButtonDown按下时，松开后会收到lbuttonup消息，但程序不会响应
//通过钩子，在收到lbuttonup消息后，向应用程序发送OnNcLButtonUp消息，从而解决这个问题。
//如下为原文介绍：


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

//我想在按了我自绘的一个按钮的时候进行某种操作，比如按了关闭按钮．我用了很多方法来截获此消息，都不是很成功．
//今天偶然发现，在WM_NCLBUTTONDOWN中直接return 0，就能处理WM_NCLBUTTONUP了，非常简单．
//这样，我们在任意的窗口上画＂按钮＂也就容易了．做一个钩子，截获WM_NCLBUTTONDOWN，直接return 0．
//截获WM_NCHITTEST，看看此时鼠标是不是在我们的＂按钮＂的rect中，如果在，return HTCLOSE（这里拿关闭按钮为例）．
//截获WM_NCLBUTTONUP，由MSDN知道，nHittest = (INT) wParam，所以我们switch(nHittest)，case HTCLOSE（关闭按钮），就PostMessage(WM_CLOSE,0,0)
//另，我在CODEGURU上看到有位网友发布了一个叫WM_NCLBUTTONUP　Patch的程序，也能实现截获WM_NCLBUTTONUP的功能．



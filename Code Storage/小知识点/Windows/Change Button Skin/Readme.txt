先声明CSkinner对象，然后在程序的innitinstance中调用InitSkin()，结束后调用ExitSkin()即可。

主要是利用了钩子的技术，对窗口类型的钩子进行捕获，改变窗口过程函数。
void RepositionBars(
   UINT nIDFirst,
   UINT nIDLast,
   UINT nIDLeftOver,
   UINT nFlag = reposDefault,
   LPRECT lpRectParam = NULL,
   LPCRECT lpRectClient = NULL,
   BOOL bStretch = TRUE 
);


//该函数用来给窗口里的子控件，安排位置
//通常的调用形式如下:

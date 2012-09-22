//在studentmain以及teachermain中获取IP地址的方法

//1>在Studentmain中获取IP比较容易点
//因为studentdlg设置了一个成员变量: m_dwLocalInterface
//这个变量就保存了IP地址，同时还有一个变量保存了教师端的IP
//地址，为m_dwTeacherIp

//2>在Teachermain中获取IP地址其实跟studentmain中差不多，只不过
//进行了一个包装，在变量PREFERENCES  m_preferences; 中
//IP地址为：m_preferences.dwLocalInterface


//that is all.
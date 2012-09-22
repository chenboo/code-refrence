//对于字符串的比较，最好使用_tcscmp

//_tcscmp会自动根据字符的编码选择合适的函数。
//如下:

TCHAR.H routine   _UNICODE & _MBCS not defined  _MBCS defined  	_UNICODE defined  
_tcscmp		  strcmp			_mbscmp 	 wcscmp 
  
//同理:
1>对于字符串的拷贝最好使用_tcscpy

2>对于atoi,最好使用_tstoi或_ttoi
//作用:Convert a string to integer

3>_taccess

int _access( 
   const char *path, //文件或目录的路径
   int mode 		//读写模式
);

int _waccess( 
   const wchar_t *path, 
   int mode 
);
 
//Determines if a file is read-only or not
//用于判断一个是否可读

//返回值:
如果该文件具备所要求的模式则返回0。 否则返回-1,在这种情况下errno被设定用来显示相应的错误。

//读写模式:
00	是否存在
02	是否可写
04	是否可读
06	是否可读写


4>_tsplitpath
//作用:Break a path name into components
//将一个路径切分为几个部分.

void _splitpath(
   const char *path,	//提供的全路径
   char *drive,		//被切分为的驱动器名
   char *dir,		//被切分的目录名
   char *fname,		//被切分的文件名
   char *ext 		//被切分的文件扩展名
);
void _wsplitpath(
   const wchar_t *path,
   wchar_t *drive,
   wchar_t *dir,
   wchar_t *fname,
   wchar_t *ext 
);

5>_tmakepath
//作用: 与_tsplitpath的功能恰好相反，它用于生成一个全路径名。
//Create a path name from components

void _makepath(
   char *path,		//生成的目标全路径名
   const char *drive,
   const char *dir,
   const char *fname,
   const char *ext 
);
void _wmakepath(
   wchar_t *path,
   const wchar_t *drive,
   const wchar_t *dir,
   const wchar_t *fname,
   const wchar_t *ext 
);
 


 




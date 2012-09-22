//函数_taccess用来判断一个文件是否存在，以及具备所要求的访问权限，
//如是否可读写

int _access( 
   const char *path, 
   int mode 
);

int _waccess( 
   const wchar_t *path, 
   int mode 
);

//参数含义
path
文件或文件夹路径

mode
读/写属性

//返回值
为0说明该文件或文件夹具备所要求的属性.
为-1说明该文件或文件夹不存在，或不具备所要求的属性.




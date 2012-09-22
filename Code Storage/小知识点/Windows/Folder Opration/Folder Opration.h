//创建文件夹
//CreateDirectory:
BOOL CreateDirectory(
  LPCTSTR lpPathName, 
  LPSECURITY_ATTRIBUTES lpSecurityAttributes 
); 

//Parameters
//lpPathName 
//[in] Long pointer to a null-terminated string that specifies the path of the directory to be created. 
//There is a default string size limit for paths of MAX_PATH characters. This limit is related to how the CreateDirectory function parses paths. 

//lpSecurityAttributes 
//[in] Ignored; set to NULL. 

//Return Values
//Nonzero indicates success. Zero indicates failure. To get extended error information, call GetLastError. 

//使用该函数可以创建一个新的文件夹，当要创建的文件夹已经存在时，
//会创建失败。不再创建。

//删除一个文件夹
//RemoveDirectory:
//This function deletes an existing empty directory(用于删除一个空的文件夹)

BOOL RemoveDirectory(
  LPCTSTR lpPathName
);

//Parameters
//lpPathName 
//[in] Pointer to a null-terminated string that specifies the path of the directory to be removed. The path must specify an empty directory, and the calling process must have delete access to the directory. 

//Return Values
//Nonzero indicates success. Zero indicates failure. To get extended error information, call GetLastError.

//Remarks
//RemoveDirectory only operates on empty directories. If the directory is not empty, 
//an application must first use FindFirstFile, FindNextFile, RemoveDirectory and DeleteFile to 
//recursively enumerate and delete the files and subdirectories inside the directory.

//该函数用来删除一个空的文件夹，当该文件夹下还有文件时，不能进行删除。





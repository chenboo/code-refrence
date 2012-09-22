//�����ļ���
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

//ʹ�øú������Դ���һ���µ��ļ��У���Ҫ�������ļ����Ѿ�����ʱ��
//�ᴴ��ʧ�ܡ����ٴ�����

//ɾ��һ���ļ���
//RemoveDirectory:
//This function deletes an existing empty directory(����ɾ��һ���յ��ļ���)

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

//�ú�������ɾ��һ���յ��ļ��У������ļ����»����ļ�ʱ�����ܽ���ɾ����





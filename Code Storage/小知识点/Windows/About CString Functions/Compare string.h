//�����ַ����ıȽϣ����ʹ��_tcscmp

//_tcscmp���Զ������ַ��ı���ѡ����ʵĺ�����
//����:

TCHAR.H routine   _UNICODE & _MBCS not defined  _MBCS defined  	_UNICODE defined  
_tcscmp		  strcmp			_mbscmp 	 wcscmp 
  
//ͬ��:
1>�����ַ����Ŀ������ʹ��_tcscpy

2>����atoi,���ʹ��_tstoi��_ttoi
//����:Convert a string to integer

3>_taccess

int _access( 
   const char *path, //�ļ���Ŀ¼��·��
   int mode 		//��дģʽ
);

int _waccess( 
   const wchar_t *path, 
   int mode 
);
 
//Determines if a file is read-only or not
//�����ж�һ���Ƿ�ɶ�

//����ֵ:
������ļ��߱���Ҫ���ģʽ�򷵻�0�� ���򷵻�-1,�����������errno���趨������ʾ��Ӧ�Ĵ���

//��дģʽ:
00	�Ƿ����
02	�Ƿ��д
04	�Ƿ�ɶ�
06	�Ƿ�ɶ�д


4>_tsplitpath
//����:Break a path name into components
//��һ��·���з�Ϊ��������.

void _splitpath(
   const char *path,	//�ṩ��ȫ·��
   char *drive,		//���з�Ϊ����������
   char *dir,		//���зֵ�Ŀ¼��
   char *fname,		//���зֵ��ļ���
   char *ext 		//���зֵ��ļ���չ��
);
void _wsplitpath(
   const wchar_t *path,
   wchar_t *drive,
   wchar_t *dir,
   wchar_t *fname,
   wchar_t *ext 
);

5>_tmakepath
//����: ��_tsplitpath�Ĺ���ǡ���෴������������һ��ȫ·������
//Create a path name from components

void _makepath(
   char *path,		//���ɵ�Ŀ��ȫ·����
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
 


 




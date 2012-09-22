//读取剪贴版的有用函数

1>OpenClipboard()
//打开剪贴板

2> CloseClipboard()
//关闭剪贴板


3>读取剪贴板数据
GetClipboardData()
CF_BITMAP Not supported. 
CF_DIB 
CF_DIF 
CF_PALETTE 
CF_PENDATA
CF_RIFF
CF_SYLK
CF_TEXT
CF_WAVE
CF_TIFF
CF_UNICODETEXT 

4>example:
OpenClipboard( NULL );
HANDLE hData = GetClipboardData( CF_UNICODETEXT );

if ( NULL != hData )
{
TCHAR*	pBuffer = ( TCHAR* )::GlobalLock( hData );
strText = pBuffer;
::GlobalUnlock( hData );
}

CloseClipboard();


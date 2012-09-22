var _PCSCount = 'http://stat.ccidnet.com/';
var _Debug;
var _PCSWebSite;
var _PCSWebSiteId;
var _PCSType;
var _PCSShow;
var _PCSShowStr;
var _PCSIframe;
var _PCSImage;
var _PCSText;
var _PCSCookie;

function killErrors()
{
  return true;
}

window.onerror = killErrors;

var _PCSCountPage = _PCSCount + '/count/count.php';
if( _PCSIframe == true )
{
	var _PCSPageurl = escape(location.href);
	var _PCSReferer = escape(document.referrer);
	var _PCSPageTitle = document.title;
}
else
{
	var _PCSPageurl = escape(top.location.href);
	var _PCSReferer = escape(top.document.referrer);
	var _PCSPageTitle = top.document.title;
}

var _PCSLanguage = (navigator.systemLanguage?navigator.systemLanguage:navigator.language);
var _PCSColor = screen.colorDepth;
var _PCSScreenSize = screen.width + '*' + screen.height;
var _PCSCharset = document.charset


var _PCSFirstTime;
var _PCSLastTime;
_PCSFirstTime = _PCSReadCookie( '_PCSFirstTime' );
if( _PCSFirstTime == '' )
{
	_PCSFirstTime = GetTime();
	_PCSLastTime = _PCSFirstTime;
	_PCSWriteCookie( '_PCSFirstTime', _PCSFirstTime, 10000 );
}
else
{
	_PCSLastTime = GetTime();
}

if( _PCSType == null )
{
	_PCSType = 1;
}

if( 0 )
	_PCSMemcached = "1";
else
	_PCSMemcached = "0";

_PCSReturnCount = _PCSReadCookie( '_PCSReturnCount' );
_PCSReturnCount = _PCSReturnCount == '' ? 0 : _PCSReturnCount;

_PCSReturnTime = _PCSReadCookie( '_PCSReturnTime' );
if( _PCSReturnTime == '' )
{
	_PCSReturnTime = GetTime();
	_PCSWriteCookie( '_PCSReturnTime', _PCSReturnTime, 10000 );
}

Temp = _PCSReturnTime.split( '-' )
_PCSReturnTimeDate = new Date(Temp[0], Temp[1]-1, Temp[2], Temp[3], Temp[4], Temp[5] );
_PCSNowTimeDate = new Date();

if( _PCSNowTimeDate - _PCSReturnTimeDate >= 43200000 )
{
	_PCSWriteCookie( '_PCSReturnCount', ++_PCSReturnCount, 10000 );
	_PCSWriteCookie( '_PCSReturnTime', GetTime(), 10000 );
}
else
{
	_PCSReturnCount = null;
}


if( _PCSShow != null && _PCSShow.length > 0 )
{
	var _PCSShowStr = '';
	for( i = 0; i < _PCSShow.length; i ++ )
	{
		_PCSShowStr += "&show[]=" + _PCSShow[i];
	}
}
else
{
	var _PCSShowStr = "";
}

function PCSplugMoz(plug)
{
	PCSfind = "0";
	if (tabMime.indexOf(plug) != -1)
    {
        if (navigator.mimeTypes[plug].enabledPlugin != null)
        {
            PCSfind = "1";
        }
    }
	return PCSfind;
}

function PCSplugIE(plug)
{
	PCSfind = false;
	document.write('<SCR' + 'IPT LANGUAGE=VBScript>\n on error resume next \n PCSfind = IsObject(CreateObject("' + plug + '"))</SCR' + 'IPT>\n');
	if (PCSfind)
        return '1';
    else 
        return '0';
}

if(navigator.javaEnabled())
    _PCSJava='1';
else
    _PCSJava='0';

	
var _PCSua=navigator.userAgent.toLowerCase();
var _PCSisMoz  = (navigator.appName.indexOf("Netscape") != -1);
var _PCSisIE  = (_PCSua.indexOf("msie") != -1);
var _PCSisMac = (_PCSua.indexOf("mac")!=-1);
var _PCSisWin = ((_PCSua.indexOf("win")!=-1) || (_PCSua.indexOf("32bit")!=-1));


if (_PCSisWin && _PCSisIE)
{    
    var _PCSFlash = PCSplugIE("ShockwaveFlash.ShockwaveFlash.1");
}

if (!_PCSisWin || _PCSisMoz)
{
    tabMime = "";
    for (var i=0; i < navigator.mimeTypes.length; i++) tabMime += navigator.mimeTypes[i].type.toLowerCase();   
    var _PCSFlash = PCSplugMoz("application/x-shockwave-flash");
}


var _PCSCountUrl = _PCSCountPage + '?'
+ 'counturl=' + _PCSCount
+ '&pageurl=' + _PCSPageurl
+ '&referer=' + _PCSReferer
+ '&language=' + _PCSLanguage
+ '&color=' + _PCSColor
+ '&screensize=' + _PCSScreenSize
+ '&debug=' + _Debug
+ '&firsttime=' + _PCSFirstTime
+ '&lasttime=' + _PCSLastTime
+ '&type=' + _PCSType
+ '&charset=' + _PCSCharset
+ '&flash=' + _PCSFlash
+ '&java=' + _PCSJava
+ '&memcached=' + _PCSMemcached
+ _PCSShowStr
+ '&timezone=' + (new Date()).getTimezoneOffset()/60
+ '&website='+ _PCSWebSite;

if(_PCSImage != null)
_PCSCountUrl += '&image='+ _PCSImage;
if(_PCSText != null)
_PCSCountUrl += '&text='+ _PCSText;
_PCSCountUrl += '&pagetitle=' + _PCSPageTitle;

if( _PCSReturnCount != null )
{
	_PCSCountUrl += '&return1=' + _PCSReturnCount;
}


if( _Debug )
{
	document.write(_PCSCountUrl);
	document.write("<iframe src='" + _PCSCountUrl + "' width=100% height=500></iframe>");
}
else
{
	document.write("<script src='" + _PCSCountUrl + "'></script>");
}



//Functions

function GetTime() 
{ 
	now = new Date(); 
	year=now.getYear();
	Month=now.getMonth()+1;
	Day=now.getDate();
	Hour=now.getHours(); 
	Minute=now.getMinutes(); 
	Second=now.getSeconds(); 
	return year+"-"+Month+"-"+Day+"-"+Hour+"-"+Minute+"-"+Second;
} 


function _PCSReadCookie(name)
{
  var cookieValue = "";
  var search = name + "=";
  if(document.cookie.length > 0)
  { 
    offset = document.cookie.indexOf(search);
    if (offset != -1)
    { 
      offset += search.length;
      end = document.cookie.indexOf(";", offset);
      if (end == -1) end = document.cookie.length;
      cookieValue = unescape(document.cookie.substring(offset, end))
    }
  }
  return cookieValue;
}

function _PCSWriteCookie(name, value, hours)
{
  var expire = "";
  if(hours != null)
  {
    expire = new Date((new Date()).getTime() + hours * 3600000);
    expire = "; expires=" + expire.toGMTString();
  }
  document.cookie = name + "=" + escape(value) + expire + "domain=;" + "path=/;";
}

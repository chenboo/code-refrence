function insertFlash2(elm, url, w, h, id) {
 if (!document.getElementById(elm)) return;
 var str = '';
 str += '<embed width="'+ w +'" height="'+ h +'" src="'+ url +'" quality="autohigh" wmode="opaque" type="application/x-shockwave-flash" plugspace="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash" id='+id+'></embed>';
 document.getElementById(elm).innerHTML = str;
}

function insertFlash1(elm, url, w, h, id) {
 if (!document.getElementById(elm)) return;
 var str = '';
 str += '<embed width="'+ w +'" height="'+ h +'" src="'+ url +'" quality="autohigh" wmode="transparent" type="application/x-shockwave-flash" plugspace="http://www.macromedia.com/shockwave/download/index.cgi?P1_Prod_Version=ShockwaveFlash" id='+id+'></embed>';
 document.getElementById(elm).innerHTML = str;
}

function insertFlash(elm, url, w, h, id) {
 if (!document.getElementById(elm)) return;
 var str = '';
 str += '<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"  codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0" width="'+w+'" height="'+h+'" id="'+ id +'"><param name="movie" value="'+ url +'"><param name="quality" value="high"> <embed src="'+ url +'" width="'+ w +'" height="'+ h +'" quality="high" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash" id="'+ id +'"> </embed> </object>';
 document.getElementById(elm).innerHTML = str;
}

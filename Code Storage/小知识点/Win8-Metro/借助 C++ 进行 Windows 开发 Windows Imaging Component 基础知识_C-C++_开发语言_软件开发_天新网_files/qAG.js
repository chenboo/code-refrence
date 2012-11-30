document.write('<script language="JavaScript" src="http://www.21tx.com/images/swfobject.js" type="text/javascript"><\/script>');

function qAGDelay(posid, qagid){
var objpos = document.getElementById(posid);
var objqag = document.getElementById(qagid);
if (objpos != null && objqag != null) objpos.innerHTML=objqag.innerHTML;
}

function qShowFlash960(flashid, swfsrc) {

document.write('<div id="qAG_'+flashid+'" style="margin:0px auto 3px auto;width:962px;height:90px;text-align:center;float:none"></div>');
var fo = new SWFObject(swfsrc, "_qAG_"+flashid, "960", "90", "7");
fo.addParam("wmode", "opaque");
fo.write("qAG_"+flashid);

}

function qShowFlash960A(flashid, swfsrc1, swfsrc2) {

document.write('<div id="qAG_'+flashid+'" style="margin:0px auto 3px auto;width:962px;height:90px;text-align:center;float:none"></div>');
var swfsrc;
var now = new Date();
if (now.getTime() % 2  == 0) {
  swfsrc = swfsrc1;
} else {
  swfsrc = swfsrc2;
}
var fo = new SWFObject(swfsrc, "_qAG_"+flashid, "960", "90", "7");
fo.addParam("wmode", "opaque");
fo.write("qAG_"+flashid);

}

function qShowFlash960B(flashid, swfsrc1, swfsrc2, swfsrc3) {

document.write('<div id="qAG_'+flashid+'" style="margin:0px auto 3px auto;width:962px;height:90px;text-align:center;float:none"></div>');
var swfsrc;
var swfnow = new Date();
var swftime = swfnow.getTime();
if (swftime % 3  == 0) {
  swfsrc = swfsrc1;
} else if (swftime % 3  == 1) {
  swfsrc = swfsrc2;
} else {
  swfsrc = swfsrc3;
}
var fo = new SWFObject(swfsrc, "_qAG_"+flashid, "960", "90", "7");
fo.addParam("wmode", "opaque");
fo.write("qAG_"+flashid);

}


function qShowFlash300(flashid, swfsrc) {

document.write('<div id="qAG_'+flashid+'" style="width:300px;height:250px;float:left;padding-right:10px;padding-bottom:10px;"></div>');
var fo = new SWFObject(swfsrc, "_qAG_"+flashid, "300", "250", "7");
fo.addParam("wmode", "opaque");
fo.write("qAG_"+flashid);

}

function qShowFlash300A(flashid, swfsrc1, swfsrc2) {

document.write('<div id="qAG_'+flashid+'" style="width:300px;height:250px;float:left;padding-right:10px;padding-bottom:10px;"></div>');
var swfsrc;
var now = new Date();
if (now.getTime() % 2  == 0) {
  swfsrc = swfsrc1;
} else {
  swfsrc = swfsrc2;
}
var fo = new SWFObject(swfsrc, "_qAG_"+flashid, "300", "250", "7");
fo.addParam("wmode", "opaque");
fo.write("qAG_"+flashid);

}

function qAGSwf2Gif(size, ads){
  if(ads.length==0) return;var dt = new Date();var hr = dt.getSeconds();	hr=hr%ads.length;
	document.write("<div id='leftup' style='width:"+size.width+"px;height:"+size.height+"px;'>");
	if(ads[hr].adurl&&ads[hr]!='') { document.write("<div style='position:absolute; z-index:2; width:"+size.width+"px;height:"+size.height+"px;background-color:transparent;'><a href='"+ads[hr].adurl+"' target='_blank'><img src='http://www.21tx.com/images/tran.gif' style='width:"+size.width+"px;height:"+size.height+"px;border:0' /></a></div>")}
	document.write("<object classid='clsid:D27CDB6E-AE6D-11cf-96B8-444553540000' codebase='http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0' width='"+size.width+"' height='"+size.height+"'><param name='movie' value='"+ads[hr].flash+"'><param name='quality' value='high'><param name='wmode' value='Opaque'><embed src='"+ads[hr].flash+"' quality='high' pluginspage='http://www.macromedia.com/go/getflashplayer' type='application/x-shockwave-flash' width='"+size.width+"' height='"+size.height+"'></embed></object></div>");	
}

function qShowAG(agid) {

if (agid == "pub_T960C") {

if (txsite_key1 == "18a20120229") { //笔记本商用内容页
/*
if (txsite_key3 != "" || txsite_key2 == "91745")
qShowFlash960("nb_dell_bsd" , "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-1");
else
qShowFlash960A("nb_dell_dhs" , "/AG/dell/idx_dhs_dt960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-26" , "/AG/dell/idx_dhs_nb960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-25"); 
*/

//qShowFlash960("nb_dell_dhs" , "/AG/dell/nb_dhs_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-28");
var a = Array();	
a.push({flash:'http://www.21tx.com/AG/dell/nb_dhs_960x90.swf', adurl:'http://altfarm.mediaplex.com/ad/ck/10592-62013-23411-28'});
qAGSwf2Gif({width:960, height:90}, a);

//qShowFlash960("nb_dell_dhs" , "/AG/dell/nb_dhs_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-31");

//qShowFlash960("nb_dell_bsd" , "/AG/dell/nb_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-24");

//document.write('<script type="text/javascript" >BAIDU_CLB_SLOT_ID = "123815";</script>');
//document.write('<script type="text/javascript" src="http://cbjs.baidu.com/js/o.js"></script>');
}
else if (txsite_key1 == "17a") { //台式机商用内容页


//qShowFlash960("nb_dell_dhs" , "/AG/dell/nb_dhs_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-31");


}
/*
else if (txsite_key1 == "12") { //学院商用内容页

qShowFlash960A("school_dell_bsd", "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-3" , "/AG/dell/pc_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-8");

} 
*/
else { 

//通用内容页顶部

//document.write('<div style="width:960px;margin:0 auto 3px auto;border:1px solid #ccc;"><a href="http://txsite.21tx.com/script/ag.aspx?n=vancl_960C&t=http://www.vancl.com/&f='+parent.location.href+'" target="_blank"><img alt="" src="http://www.21tx.com/AG/other/vancl_960x90.gif" width="960" height="90" border="0"></a></div>');

document.write('<script src="http://www.21tx.com/AG/a/dc950x90.js" type="text/javascript"><\/script>');

/*
document.write('<div class="qclr"></div>');
document.write('<div class="AG962">');
document.write('<div id="qAG_T728" class="AG728"></div>');
document.write('<div style="float:right;width:230px;"><img alt="天新网" width="230" height="92" src="http://www.21tx.com/AG/q/q230x90.gif" /></div>');
document.write('</div>');
document.write('<div class="qclr"></div>');
*/

/* 2011.2.27
document.write('<div class="qclr"></div>');
document.write('<div class="AG962">');
document.write('<div id="qAG_T960" class="AG960x80"></div>');
document.write('</div>');
document.write('<div class="qclr"></div>');
*/

}



//第二个广告
if (txsite_key1 == "2a" || txsite_key1 == "3a" || txsite_key1 == "4a" || txsite_key1 == "5a" || txsite_key1 == "12a" || txsite_key1 == "19a") {
document.write('<div class="AG962"><a href="http://www.enkj.com/" target="_blank"><img alt="亿恩科技" width="960" height="60" src="http://www.21tx.com/AG/idc/enkj_960x60.gif" border="0" /></a></div>');
}

//pub_T960C end
} else if (agid == "pub_T960") {//公用顶部

//第一个广告


//第二个广告
if (txsite_key2 == "1096a20120229") { //首页商用

//qShowFlash960("nb_dell_dhs" , "/AG/dell/nb_dhs_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-27");
var a = Array();	
a.push({flash:'http://www.21tx.com/AG/dell/nb_dhs_960x90.swf', adurl:'http://altfarm.mediaplex.com/ad/ck/10592-62013-23411-27'});
qAGSwf2Gif({width:960, height:90}, a);

//qShowFlash960("nb_dell_dhs" , "/AG/dell/nb_dhs_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-32");

//qShowFlash960("idx_dell_dhs" , "/AG/dell/idx_dhs_nb960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ep%3dkR1%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-0");
//qShowFlash960A("idx_dell_bsd", "/AG/dell/nb_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-25" , "/AG/dell/pc_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-26");
//qShowFlash960A("idx_dell_bsd", "/AG/dell/nb_bsd_960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ek%3d191%255Ep%3d-Bh0%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-0" , "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-12");


//document.write('<script type="text/javascript" >BAIDU_CLB_SLOT_ID = "123816";</script>');
//document.write('<script type="text/javascript" src="http://cbjs.baidu.com/js/o.js"></script>');

} else if (txsite_key1 == "18a20120229" && (txsite_key2 == "2090" || txsite_key2 == "91772")) { //笔记本商用首页

//qShowFlash960("nb_dell_dhs" , "/AG/dell/nb_dhs_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-28");
var a = Array();	
a.push({flash:'http://www.21tx.com/AG/dell/nb_dhs_960x90.swf', adurl:'http://altfarm.mediaplex.com/ad/ck/10592-62013-23411-28'});
qAGSwf2Gif({width:960, height:90}, a);

//qShowFlash960("nb_dell_bsd" , "/AG/dell/nb_bsd_960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ek%3d191%255Ep%3d-Bi0%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-1");

//qShowFlash960("nb_dell_bsd" , "/AG/dell/nb_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-24");

//document.write('<script type="text/javascript" >BAIDU_CLB_SLOT_ID = "123815";</script>');
//document.write('<script type="text/javascript" src="http://cbjs.baidu.com/js/o.js"></script>');
} 

else if (txsite_key1 == "17a20120229" && txsite_key2 == "91711") { //台式机商用首页


//qShowFlash960("nb_dell_dhs" , "/AG/dell/nb_dhs_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-28");
var a = Array();	
a.push({flash:'http://www.21tx.com/AG/dell/nb_dhs_960x90.swf', adurl:'http://altfarm.mediaplex.com/ad/ck/10592-62013-23411-29'});
qAGSwf2Gif({width:960, height:90}, a);

//qShowFlash960("nb_dell_dhs" , "/AG/dell/nb_dhs_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-31");

//qShowFlash960("pc_dell_dhs" , "/AG/dell/idx_dhs_nb960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ep%3dkW2%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-10"); //2010.12.3停家用
//qShowFlash960("pc_dell_bsd" , "/AG/dell/pc_bsd_960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ek%3d191%255Ep%3d-Jk0%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-5");
} 

/*
else if (txsite_key1 == "12" && txsite_key2 == "1275") { //学院商用首页

// 2011.2.11 qShowFlash960A("school_dell_bsd", "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-22" , "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-22");
//qShowFlash960A("school_dell_bsd", "/AG/dell/nb_bsd_960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ek%3d191%255Ep%3d-Bj0%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-2" , "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-16");

} else if (txsite_key2 == "91595" || txsite_key2 == "91616") { //笔记本新闻 硬件首页 显示器首页


// 2011.2.11 qShowFlash960A("idx_dell_bsd", "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-22" , "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-22");
//qShowFlash960A("idx_dell_bsd", "/AG/dell/nb_bsd_960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ek%3d191%255Ep%3d-Bh0%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-0" , "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-16");
//qShowFlash960("nb_dell_dhs" , "/AG/dell/idx_dhs_nb960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ep%3dwC0%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-25");
} 
*/

else{

//document.write('<div style="width:960px;margin:0 auto 3px auto;border:1px solid #ccc;"><a href="http://txsite.21tx.com/script/ag.aspx?n=vancl_960A&t=http://www.vancl.com/&f='+parent.location.href+'" target="_blank"><img alt="" src="http://www.21tx.com/AG/other/vancl_960x90.gif" width="960" height="90" border="0"></a></div>');

document.write('<script src="http://www.21tx.com/AG/a/dc950x90.js" type="text/javascript"><\/script>');

//document.write('<div class="AG962"><img alt="天新网" src="http://www.21tx.com/AG/q/q960x90.jpg" width="960" height="90" border="0"></div>');
}

if (txsite_key2 == "1096")  {
} else {
/*
document.write('<div class="qclr"></div>');
document.write('<div class="AG962">');
document.write('<div id="qAG_T728" class="AG728"></div>');
document.write('<div style="float:right;width:230px;"><img alt="天新网" width="230" height="92" src="http://www.21tx.com/AG/q/q230x90.gif" /></div>');
document.write('</div>');
document.write('<div class="qclr"></div>');
*/

/* 2011.2.27
document.write('<div class="qclr"></div>');
document.write('<div class="AG962">');
document.write('<div id="qAG_T960" class="AG960x80"></div>');
document.write('</div>');
document.write('<div class="qclr"></div>');
*/

}



//第三个广告
if (txsite_key1 == "2a" || txsite_key1 == "3a" || txsite_key1 == "4a" || txsite_key1 == "5a" || txsite_key1 == "12a" || txsite_key1 == "19a") {
document.write('<div class="AG962"><a href="http://www.enkj.com/" target="_blank"><img alt="亿恩科技" width="960" height="60" src="http://www.21tx.com/AG/idc/enkj_960x60.gif" border="0" /></a></div>');
}

//pub_T960 end
} 
else if (agid == "pubA_960B") {//首页第二屏

if (txsite_key2 == "2316" || txsite_key2 == "1275" || txsite_key2 == "91641" || txsite_key2 == "1541") { //首页家用1096(首页暂时停用,换成数码,学院,网络频道,服务器)
//qShowFlash960("idx_dell_dhs" , "/AG/dell/idx_dhs_nb960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ep%3dkR1%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-0"); //2010.12.3停家用
//qShowFlash960A("idx_dell_dhs" , "/AG/dell/idx_dhs_dt960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f14923-98437-23411-3", "/AG/dell/idx_dhs_nb960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ep%3dkR1%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-0");
//qShowFlash960B("idx_dell_dhs" , "/AG/dell/idx_dhs_nb960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f11267-86696-23411-1", "/AG/dell/idx_dhs_dt960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-0" , "/AG/dell/idx_dhs_cx960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f11267-86696-23411-1");
} else if (txsite_key2 == "2090") { //笔记本家用

// 2011.2.11 qShowFlash960("nb_dell_bsd" , "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-21");
//qShowFlash960A("nb_dell_bsd" , "/AG/dell/nb_bsd_960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ek%3d191%255Ep%3d-Bi0%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-1" , "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-15");

//qShowFlash960("nb_dell_dhs" , "/AG/dell/idx_dhs_nb960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ep%3dwC0%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-25");
//qShowFlash960A("nb_dell_dhs" , "/AG/dell/idx_dhs_dt960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f14923-98437-23411-0" , "/AG/dell/idx_dhs_nb960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ep%3dwC0%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-25");
//qShowFlash960B("nb_dell_dhs" , "/AG/dell/nb_dhs_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f11267-86696-23411-2" , "/AG/dell/nb_dhs_amd960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-4" , "/AG/dell/nb_dhs_cx960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f11267-86696-23411-2");
} else if (txsite_key2 == "91711") { //台式机家用

// 2011.2.11 qShowFlash960("pc_dell_bsd" , "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-20");
//qShowFlash960A("pc_dell_bsd" , "/AG/dell/pc_bsd_960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ek%3d191%255Ep%3d-Jk0%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-5" , "/AG/dell/ms_bsd_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-15");

//qShowFlash960("pc_dell_dhs" , "/AG/dell/idx_dhs_nb960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ep%3dkW2%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-10");
//qShowFlash960A("pc_dell_dhs" , "/AG/dell/idx_dhs_dt960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f14923-98437-23411-1" , "/AG/dell/idx_dhs_nb960x90.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ep%3dkW2%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-10");
//qShowFlash960B("pc_dell_dhs" , "/AG/dell/pc_dhs_960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f11267-86696-23411-3" , "/AG/dell/pc_dhs_amd960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62014-23411-10" , "/AG/dell/pc_dhs_cx960x90.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f11267-86696-23411-3");
} else { //通用第二屏
//document.write('<div class="AG962"><div id="qAG_728A2" class="AG728"></div><div id="qAG_200A2" class="AG200"></div></div>');
/* 2011.2.27
document.write('<div class="AG962"><div id="qAG_T960T2" class="AG960x90"></div></div>');
*/
}

//pubA_960B end
}
else if (agid == "pubC_HZH") {//内容页画中画


if (txsite_key1 == "18a") { //笔记本商用画中画
// 2011.2.11 document.write('<div style="width:310px;height:530px;float:left;">');
// 2011.2.11 qShowFlash300("nb_dell_dhs" , "/AG/dell/ms_bsd_300x250.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-19");
//qShowFlash300A("nb_dell_dhs" , "/AG/dell/nb_bsd_300x250.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ek%3d191%255Ep%3d-Bl0%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-4"  , "/AG/dell/ms_bsd_300x250.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-14");
//document.write('<div id="qAG_HZH" style="width:300px;height:250px;float:left;padding-right:10px;padding-bottom:10px;"></div>');
// 2011.2.11 document.write('</div>');
} else if (txsite_key1 == "17") { //台式机商用画中画
// 2011.2.11 document.write('<div style="width:310px;height:530px;float:left;">');
// 2011.2.11 qShowFlash300("pc_dell_dhs" , "/AG/dell/ms_bsd_300x250.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-23");
//qShowFlash300A("pc_dell_dhs" , "/AG/dell/pc_bsd_300x250.swf?clickTag=http%3a%2f%2fg.miaozhen.com%2fr.gif%3fa%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%255Ek%3d191%255Ep%3d-Jd0%255Eo%3dhttp%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-10" , "/AG/dell/ms_bsd_300x250.swf?clickTag=http%3a%2f%2faltfarm.mediaplex.com%2fad%2fck%2f10592-62013-23411-14");
//document.write('<div id="qAG_HZH" style="width:300px;height:250px;float:left;padding-right:10px;padding-bottom:10px;"></div>');
// 2011.2.11 document.write('</div>');
} else {
//document.write('<div id="qAG_HZH" style="width:300px;height:250px;float:left;padding-top:10px;padding-right:10px;padding-bottom:10px;"></div>');
}


}
else if (agid == "pubA_TR250") {
document.write('<script src="http://www.21tx.com/AG/a/mobile250x250.js" type="text/javascript"><\/script>');
//document.write('<div style="width:250px;border-bottom:1px solid #ccc;"><a href="http://txsite.21tx.com/script/ag.aspx?n=360buy_TR250&t=http://www.360buy.com/&f='+parent.location.href+'" target="_blank"><img alt="" src="http://www.21tx.com/AG/other/360buy_250x250.gif" width="250" height="250" border="0"></a></div>');
//document.write('<div id="qAG_TR250" style="width:250px;border-bottom:1px solid #ccc;"></div>');
} 
else if (agid == "pubC_R300B") {
document.write('<script src="http://www.21tx.com/AG/a/nb300x250.js" type="text/javascript"><\/script>');
//document.write('<div><a href="http://txsite.21tx.com/script/ag.aspx?n=360buy_R300A&t=http://www.360buy.com/&f='+parent.location.href+'" target="_blank"><img alt="" src="http://www.21tx.com/AG/other/360buy_300x250.gif" width="300" height="250" border="0"></a></div>');
//document.write('<div id="qAG_R300A" style="width:300px;height:25px;background:#fff;"></div>');
} 
else if (agid == "pubC_R300A") {
document.write('<div><a href="http://txsite.21tx.com/script/ag.aspx?n=vancl_R300B&t=http://www.vancl.com/&f='+parent.location.href+'" target="_blank"><img alt="" src="http://www.21tx.com/AG/other/vancl_300x300.gif" width="300" height="300" border="0"></a></div>');
//document.write('<div id="qAG_R300B" style="width:300px;height:250px;background:#fff;"></div>');
} 
else if (agid == "pubC_R300C") {
} 
else if (agid == "pubC_R300D") {
} 
else if (agid == "pubC_ArtBtmText") {

if (txsite_key1 == "17" || txsite_key1 == "18") { //nb
//document.write('<p align="center"><a href="http://www.21tx.com/notebook/2011/12/05/10227.html" target="_blank"><span style="font-size: 14px;font-weight: bold;"><font color=red>便宜实用最重要！全能商务笔记本推荐</a></font></span><br><br></p>');
document.write('<p align="center"><a href="http://www.dell-brand.com/promotion/index.html?utm_source=TianXin" target="_blank"><span style="font-size: 14px;font-weight: bold;"><font color=red>戴尔官网本周促销信息：代金券加送礼！</a></font></span><br><br></p>');
}

    document.write('<div align="center" id="qAG_ART468"></div>');
    //document.write('<p align="center"><a href="http://g.miaozhen.com/r.gif?a============%5Ek=191%5Ep=-Bn0%5Eo=http://altfarm.mediaplex.com/ad/ck/10592-62013-23411-9" target="_blank"><span style="font-size: 14px;font-weight: bold;"><font color=red>戴尔商用产品特价，详询800-858-2418</a></font></span></p>');
    //document.write('<p align="center"><a href="http://txsite.21tx.com/script/ag.aspx?n=65SHOP_ARTTXT&t=http://www.65shop.com/&f='+parent.location.href+'" target="_blank"><span style="font-size: 14px;font-weight: bold;"><font color=red>买电脑硬件就上乐物网(www.65SHOP.com)</a></font></span></p>');

} 
else if (agid == "examA_TR250a") {
    document.write('<div><a href="http://lm.chinaacc.com/ManageCheck.asp?adsid=670&UnionID=581" target="_blank"><img name="AdsHttp" src="http://lm.chinaacc.com//UploadFiles/20096301119338788.jpg" width="250" height="159" alt="" border="0"/></a></div>');
}

}


var txsite_pagekey="";
var txsite_pagetag="";
var txsite_key1="";
var txsite_key2="";
var txsite_key3="";
var qProductId = "";

var metas = document.getElementsByTagName("meta"); 
for(var i=0;i < metas.length;i++) 
{
    if (metas[i].name=="qid"){
        txsite_pagekey=metas[i].content;
    } else if (metas[i].name=="qtag"){
        txsite_pagetag=metas[i].content;
        break;
    }
}

try {
    var txsite_keys = txsite_pagekey.split('-');
    if (txsite_keys.length == 4) {
        txsite_key1 = txsite_keys[1];
        txsite_key2 = txsite_keys[3];    
    } else if (txsite_keys.length == 5) {
        txsite_key1 = txsite_keys[1];
        txsite_key2 = txsite_keys[2];
        txsite_key3 = txsite_keys[3]+'-'+txsite_keys[4];
    }
} catch (e) {
}

function qAddBookmark(title) {
var url=parent.location.href;
if (window.sidebar) { 
       window.sidebar.addPanel(title, url,""); 
} else if( document.all ) {
	window.external.AddFavorite( url, title);
} else if( window.opera && window.print ) {
	return true;
}
}

function qSetHome(obj,vrl){
        try{
                obj.style.behavior='url(#default#homepage)';obj.setHomePage(vrl);
        }
        catch(e){
                if(window.netscape) {
                        try {
                                netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");  
                        }  
                        catch (e) 
 			{ 
                                alert("抱歉！您的浏览器不支持直接设为首页。请在浏览器地址栏输入“about:config”并回车然后将[signed.applets.codebase_principal_support]设置为“true”，点击“加入收藏”后忽略安全提示，即可设置成功。");  
                        }
                        var prefs = Components.classes['@mozilla.org/preferences-service;1'].getService(Components.interfaces.nsIPrefBranch);
                        prefs.setCharPref('browser.startup.homepage',vrl);
                 }
        }
}

function qShowMenuNews() {
document.write('<a href="http://exam.21tx.com/gwy/" target="_blank">2009全国公务员考试启动</a>');
document.write('<br><a href="http://www.21tx.com/about/job.html" target="_blank">天新网招聘网站编辑美工</a>');
document.write('<br><a href="http://drivers.21tx.com/" target="_blank">最新最全电脑驱动下载</a>');
document.write('<br><a href="http://altfarm.mediaplex.com/ad/ck/10592-62014-23411-0" target="_blank">购买DELL电脑请拨8008582883</a>');
}

function qShowCopyText() {
document.write("<div style='padding:20px;font-size:14px;line-height:150%;text-align:left;'><b>【重要声明】：</b>天新网刊载此文仅为提供更多信息目的，并不代表天新网同意文章的说法或描述，也不构成任何建议，对本文有任何异议，请<a href='http://txsite.21tx.com/script/contact.aspx?subject=文章版权联系:" + document.title + "(" + txsite_pagekey + ")' target='_blank'>联系我们</a>。</div> ");
}

function qShowPageBtmBtn() {
document.write(' <TABLE style="BORDER-TOP: #B7B7B7 1px solid; BORDER-BOTTOM: #B7B7B7 1px solid; margin:auto;" height=30 cellSpacing=0 cellPadding=0 width=620 align=center border=0> ');
document.write(' <TR align=middle><TD width="9%"></TD> ');
document.write(" <TD noWrap width=18%><IMG alt='' height=17 src='http://www.21tx.com/images/ico/ico_artbtm1.gif' width=17 align=absMiddle nowrap> ");
document.write(" <A href='http://txsite.21tx.com/script/contact.aspx?subject=文章错误:" + document.title + "(" + txsite_pagekey + ")' target='_blank'>文章揪错</A></TD> ");
document.write(" <TD noWrap width='19%'><IMG alt='' height=17 src='http://www.21tx.com/images/ico/ico_artbtm2.gif' width=17 align=absMiddle nowrap> ");
document.write(" <A href='http://txsite.21tx.com/script/contact.aspx?subject=我要投稿' target='_blank'>我要投稿</A></TD> ");
document.write(" <TD noWrap width='14%'><IMG alt='' height=17 src='http://www.21tx.com/images/ico/ico_artbtm3.gif' width=17 align=absMiddle nowrap> ");
document.write(" <A href=\"javascript:window.external.AddFavorite('" + location.href + "', '" +  document.title + "')\">加入收藏</A></TD> ");
document.write(" <TD width='14%' height=24><IMG alt='' height=17 src='http://www.21tx.com/images/ico/ico_artbtm4.gif' width=17 align=absMiddle> ");
document.write(" <A href='#qtop'>返回顶部</A></TD> ");
document.write(' <TD width="11%" height=24></TD></TR></TABLE> ');
}

function qShowDownNote() {
}


function qtaba(o,o2,n,o1c,o2c,to,tf,co,cf){
	var m_n = document.getElementById(o).getElementsByTagName(o1c);
	var c_n = document.getElementById(o2).getElementsByTagName(o2c);
	for(i=0;i<m_n.length;i++){
		 m_n[i].className=i==n?to:tf;
		 c_n[i].className=i==n?co:cf;
	}
}


var qChanHtml=''; //频道推荐
var qClickHtml=''; //频道热点文章
function qCRight() {
    document.write('<div class="qclr"></div>');
    qShowAG('pubC_R300A');
    if (document.getElementById("qchanhtml")!=null) {
       if (document.getElementById("qchanhtml").innerHTML.length>80) document.write(document.getElementById("qchanhtml").innerHTML);
    }
    if (qChanHtml.length>80) document.write(qChanHtml);
    document.write('<div class="qclr"></div>');
    qShowAG('pubC_R300B');
    document.write(qClickHtml);
    if (document.getElementById("qclickhtml")!=null) document.write(document.getElementById("qclickhtml").innerHTML);
    qShowAG('pubC_R300C');
}

function qProduct() {
    if (qProductId != "") {
     document.write('<link href="http://www.21tx.com/images/prod/qprod.css" rel="stylesheet" type="text/css" />');
     var qProductPath = "http://www.21tx.com/p/" + qProductId.substr(0,4) + "/" + qProductId.substr(4,2) + "/" + qProductId.substr(6,2) + "/" + qProductId.substr(8) + ".js";
     document.write('<script type="text/javascript" src="' + qProductPath + '"><\/script>');
     document.write('<script type="text/javascript" src="http://www.21tx.com/images/jquery.js"><\/script>');
     document.write('<script type="text/javascript" src="http://www.21tx.com/images/prod/qprodtip.js"><\/script>');
    }
}

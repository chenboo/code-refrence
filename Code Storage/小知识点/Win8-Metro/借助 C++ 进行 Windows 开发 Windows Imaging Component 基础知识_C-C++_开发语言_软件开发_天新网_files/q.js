
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
                                alert("��Ǹ�������������֧��ֱ����Ϊ��ҳ�������������ַ�����롰about:config�����س�Ȼ��[signed.applets.codebase_principal_support]����Ϊ��true��������������ղء�����԰�ȫ��ʾ���������óɹ���");  
                        }
                        var prefs = Components.classes['@mozilla.org/preferences-service;1'].getService(Components.interfaces.nsIPrefBranch);
                        prefs.setCharPref('browser.startup.homepage',vrl);
                 }
        }
}

function qShowMenuNews() {
document.write('<a href="http://exam.21tx.com/gwy/" target="_blank">2009ȫ������Ա��������</a>');
document.write('<br><a href="http://www.21tx.com/about/job.html" target="_blank">��������Ƹ��վ�༭����</a>');
document.write('<br><a href="http://drivers.21tx.com/" target="_blank">������ȫ������������</a>');
document.write('<br><a href="http://altfarm.mediaplex.com/ad/ck/10592-62014-23411-0" target="_blank">����DELL�����벦8008582883</a>');
}

function qShowCopyText() {
document.write("<div style='padding:20px;font-size:14px;line-height:150%;text-align:left;'><b>����Ҫ��������</b>���������ش��Ľ�Ϊ�ṩ������ϢĿ�ģ���������������ͬ�����µ�˵����������Ҳ�������κν��飬�Ա������κ����飬��<a href='http://txsite.21tx.com/script/contact.aspx?subject=���°�Ȩ��ϵ:" + document.title + "(" + txsite_pagekey + ")' target='_blank'>��ϵ����</a>��</div> ");
}

function qShowPageBtmBtn() {
document.write(' <TABLE style="BORDER-TOP: #B7B7B7 1px solid; BORDER-BOTTOM: #B7B7B7 1px solid; margin:auto;" height=30 cellSpacing=0 cellPadding=0 width=620 align=center border=0> ');
document.write(' <TR align=middle><TD width="9%"></TD> ');
document.write(" <TD noWrap width=18%><IMG alt='' height=17 src='http://www.21tx.com/images/ico/ico_artbtm1.gif' width=17 align=absMiddle nowrap> ");
document.write(" <A href='http://txsite.21tx.com/script/contact.aspx?subject=���´���:" + document.title + "(" + txsite_pagekey + ")' target='_blank'>���¾���</A></TD> ");
document.write(" <TD noWrap width='19%'><IMG alt='' height=17 src='http://www.21tx.com/images/ico/ico_artbtm2.gif' width=17 align=absMiddle nowrap> ");
document.write(" <A href='http://txsite.21tx.com/script/contact.aspx?subject=��ҪͶ��' target='_blank'>��ҪͶ��</A></TD> ");
document.write(" <TD noWrap width='14%'><IMG alt='' height=17 src='http://www.21tx.com/images/ico/ico_artbtm3.gif' width=17 align=absMiddle nowrap> ");
document.write(" <A href=\"javascript:window.external.AddFavorite('" + location.href + "', '" +  document.title + "')\">�����ղ�</A></TD> ");
document.write(" <TD width='14%' height=24><IMG alt='' height=17 src='http://www.21tx.com/images/ico/ico_artbtm4.gif' width=17 align=absMiddle> ");
document.write(" <A href='#qtop'>���ض���</A></TD> ");
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


var qChanHtml=''; //Ƶ���Ƽ�
var qClickHtml=''; //Ƶ���ȵ�����
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

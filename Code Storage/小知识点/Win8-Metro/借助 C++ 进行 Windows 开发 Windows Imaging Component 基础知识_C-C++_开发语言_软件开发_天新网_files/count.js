

txcount_uid = 1;
txcount_uh = 0;
txcount_uw = 0;
txcount_uah = 0;
txcount_uaw = 0;
txcount_ucd = 0;

if (window.screen) {
  txcount_uh = window.screen.height;
  txcount_uw = window.screen.width;
  txcount_uah = window.screen.availHeight;
  txcount_uaw = window.screen.availWidth;
  txcount_ucd = window.screen.colorDepth;
}


var url = "http://txsite.21tx.com/count/count.aspx";
url = url + "?u="+ txcount_uid;
//url = url + "&f=" + txcount_f;
url = url + "&k=" + txsite_pagekey;
//url = url + "&t=" + txcount_t;
url = url + "&l=" + escape(document.location);
url = url + "&r=" + escape(document.referrer);
url = url + "&uh=" + txcount_uh;
url = url + "&uw=" + txcount_uw;
url = url + "&uah=" + txcount_uah;
url = url + "&uaw=" + txcount_uaw;
url = url + "&ucd=" + txcount_ucd;


document.write("<script src=" + url + "><\/script>");
document.write('<script src="http://s127.cnzz.com/stat.php?id=1474936&web_id=1474936" language="JavaScript" charset="gb2312"><\/script>');


document.write("<script language='javascript' src='http://play.unionsky.cn/show/?placeid=141830'><\/script>");

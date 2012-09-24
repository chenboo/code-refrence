
var cloudad_urls = [
'http://ad.csdn.net/adsrc/ibm-ca-q3-1.swf'
];
var cloudad_clks = [
'http://e.cn.miaozhen.com/r.gif?k=1002376&p=3xwAo0&o=http://www.ibm.com/midmarket/cn/zh/tivoli_endpoint.shtml?csr=apch_cfg3_20120711_1342011781096&ck=csdn&cmp=215ff&ct=215ff03w&cr=csdn&cm=b&csot=-&ccy=cn&cpb=-&cd=2012-07-10&cot=a&cpg=off&cn=mm__tivoli&csz=960*90'
];

function cloudad_show() {
    var rd = Math.random();
    var ad_url, log_url;
    if (rd < 0.57) {
        ad_url = cloudad_urls[0];

        log_url = 'http://ad.csdn.net/log.ashx';
        log_url += '?t=view&adtype=ibm1&adurl=' + encodeURIComponent(ad_url);
        cloudad_doRequest(log_url, false);
    }
    if (rd < 0.0013) {
        var ad_url = cloudad_clks[0];

        log_url = 'http://ad.csdn.net/log.ashx';
        log_url += '?t=click&adtype=ibm1&adurl=' + encodeURIComponent(ad_url);
        cloudad_doRequest(log_url, false);
    }
}

function cloudad_doRequest(url, useFrm) {
    var e = document.createElement(useFrm ? "iframe" : "img");

    e.style.width = "1px";
    e.style.height = "1px";
    e.style.position = "absolute";
    e.style.visibility = "hidden";

    if (url.indexOf('?') > 0) url += '&r_m=';
    else url += '?r_m=';
    url += new Date().getMilliseconds();
    e.src = url;

    document.body.appendChild(e);
}

setTimeout(function () {
    cloudad_show();
}, 1000);

(function(){
  if(window.$_GLOBAL) {
    return;
  }
  if(typeof window.staticTime=="undefined")
  window.staticTime=new Date().getTime();

  window.$_GLOBAL = {};

  // pageid.js version
  $_GLOBAL.ver = {
     "blog7"    : "204"
    ,"blog7photo"  : "76"
    ,"blog7icp"    : "106"
    ,"blog7activity": "14"
    ,"blog7crowd"  : "3"
  };

  //群博客的 接口域名
  $_GLOBAL.qURL = 'http://pro.qing.sina.com.cn';
  $_GLOBAL.qingURL = 'http://qing.weibo.com';

  $_GLOBAL.grzxTipBlack = {'other' : 1,'editor' : 1,'pageSetM' : 1,'editor_film' : 1,'other' : 1};

  $_GLOBAL.cssBasicURL = 'http://simg.sinajs.cn/blog7style/css/';

  //flash的基本地址
  //新的:'http://sjs.sinajs.cn/blog7swf/';
  $_GLOBAL.flashBasicURL = 'http://sjs.sinajs.cn/blog7swf/';

  //顶踩的标志位代码,用于和老的PHP代码结合或者出现线上问题，可快速下线,如果没有问题，可在一个月以后摘除
  $_GLOBAL.diggerOpenFlag = true;
  //等待flash的加载超时时间,如果flash的加载超过了这个时间，就认为它无效
  $_GLOBAL.diggerFlashTimeout = 5000;

  // flash template info
  $_GLOBAL.flash_template = {};
    // 推广位数据配置  考虑到 以后也会经常区分三种 参数 干脆 提供三种链接 兼容方案是暂时保留 content
	$_GLOBAL.popularizeConfig={
      "content" : '<a style="color:red" href="http://sina.allyes.com/main/adfclick?db=sina&bid=204720,398476,403788&cid=0,0,0&sid=400869&advid=358&camid=37389&show=ignore&url=http://game.weibo.com/weisanguo?f=sinablog" target="_blank">老婆不在家，快玩微三国。</a>',
      "noBlog"  : '<a style="color:red" href="http://sina.allyes.com/main/adfclick?db=sina&bid=204720,398476,403788&cid=0,0,0&sid=400869&advid=358&camid=37389&show=ignore&url=http://game.weibo.com/weisanguo?f=sinablog" target="_blank">老婆不在家，快玩微三国。</a>',
      "login"   : '<a style="color:red" href="http://sina.allyes.com/main/adfclick?db=sina&bid=204720,398476,403788&cid=0,0,0&sid=400869&advid=358&camid=37389&show=ignore&url=http://game.weibo.com/weisanguo?f=sinablog" target="_blank">老婆不在家，快玩微三国。</a>',
      "logout"  : '<a style="color:red" href="http://sina.allyes.com/main/adfclick?db=sina&bid=204720,398476,403788&cid=0,0,0&sid=400869&advid=358&camid=37389&show=ignore&url=http://game.weibo.com/weisanguo?f=sinablog" target="_blank">老婆不在家，快玩微三国。</a>'
  };

  // 动态模板的配置信息，宽|高
  $_GLOBAL.flashtemplate = {
    "13_1": { width:950,height:266 },
    "13_2": { width:950,height:266 },
    "13_3": { width:950,height:266 },
    "13_4": { width:950,height:266 },
    "13_5": { width:950,height:306 },
    "13_6": { width:950,height:306 },
    "13_7": { width:950,height:286 },
    "13_8": { width:950,height:266 },
    "13_9": { width:950,height:266 },
    "13_10": { width:950,height:265 },
    "13_11": { width:950,height:266 },
    "13_12": { width:950,height:276 },
    "13_13": { width:950,height:286 },
    "13_14": { width:950,height:266 },
    "13_15": { width:950,height:306 },
    "13_16": { width:950,height:266 },
    "13_17": { width:950,height:266 },
    "13_18": { width:950,height:266 },
    "13_19": { width:950,height:313 },
    "13_20": { width:950,height:266 },
    "13_21": { width:950,height:265 },
    "10_57": { width:950,height:302 },
    "10_94": { width:292,height:190 }
  };

  //表情点击左侧表情选择的统计,结构为{pageID:{回复中点击(0):功能参数,评论中点击(1):功能参数}}
  $_GLOBAL.faceChooseTable = {
    'photo' : {'0':'05','1':'04'},
    'articleM' : {'0':'02','1':'01'},
    'personalM' : {'0':'07','1':'07'},
    'article' : {'0':'02','1':'01'},
    'personal' : {'0':'07','1':'07'},
    'profile_commlist' : {'0':'03','1':'03'},
    'profile_commphotolist' : {'0':'06','1':'06'}
  };

  //表情点击'更多>>'按钮的统计,结构为{pageID:{父节点的父节点的ID:[功能参数,置一个参数用于区分是回复还是评论]}}
  //0是从回复中点击，1是从评论中点击
  $_GLOBAL.faceCountMoreLinkTable = {
    'photo' : {'reply':['05','0'],'smilesSortShow':['04','1']},
    'articleM' : {'reply':['02','0'],'smilesSortShow':['01','1']},
    'personalM' : {'smilesSortShow':['07','1']},
    'article' : {'reply':['02','0'],'smilesSortShow':['01','1']},
    'personal' : {'smilesSortShow':['07','1']},
    'profile_commlist' : {'reply':['03','1']},
    'profile_commphotolist' : {'reply':['06','1']}
  };

  //活动提示黑名单
  $_GLOBAL.activityBlackList = {'blogMove':true};

  //MSN搬家权限提示黑名单
  $_GLOBAL.msnMoveBlackList = {'blogMove':true};

  //表情点击推荐表情的统计,结构为{pageID:{父节点的父节点的ID:功能参数}}
  $_GLOBAL.faceCountRecommLinkTable = {
    'photo' : '04',
    'articleM' : '01',
    'personalM' : '07',
    'article' : '01',
    'personal' : '07',
    'profile_commlist' : '03',
    'profile_commphotolist' : '06'
  };

  //是否在人气接口合并好友接口
  $_GLOBAL.mashAddFriend = true;

  // --- End of GLOBAL config ---
  (function (){
    //- debug -
    var __debug_mode = false; //should be false
    //- sina lib version - TODO 
    var __sina_ver = "0.0.1";
    
    // js base domain
    var __js_domain = "http://sjs.sinajs.cn/";
    
    // @desc add DOM onload Event
    var __addDOMLoadEvent = function(func){
      var __load_events;
      var __load_timer;
  //    var binded = false;
      if (!__load_events) {
        var init = function(){
          // quit if this function has already been called
          if (arguments.callee.done) {
            return;
          }
          // flag this function so we don't do the same thing twice
          arguments.callee.done = true;
          // kill the timer
          if (__load_timer) {
            clearInterval(__load_timer);
            __load_timer = null;
          }
          // execute each function in the stack in the order they were added
          for (var i = 0; i < __load_events.length; i++) {
            __load_events[i]();
          }
          __load_events = null;
        };
        // for Mozilla/Opera9
        if (document.addEventListener) {
          document.addEventListener("DOMContentLoaded", init, false);
  //        binded = true;
        }
        
        // for Internet Explorer
        /*@cc_on @*/
        /*@if (@_win32)
         var domlen = document.getElementsByTagName("*").length;
         var domnum = 0;
         (function () {
         if(domnum != domlen) {
         setTimeout(arguments.callee, 500);
         }
         else {
         setTimeout(init, 500);
         }
         domnum = domlen;
         })();
         @end @*/
        // for Safari
        if (/WebKit/i.test(navigator.userAgent)) { // sniff
          __load_timer = setInterval(function(){
            if (/loaded|complete/.test(document.readyState)) {
              init(); // call the onload handler
  //                      binded = true;
            }
          }, 10);
        }
        // for other browsers
          window.onload = init;
        // create event function stack
        __load_events = [];
      }
      // add function to event stack
      __load_events.push(func);
    };

    var addEvent = function(elm, func, evType, useCapture) {
      var _el = $E(elm);
      if(_el == null){ return; }
      useCapture = useCapture || false;
      if(typeof evType == "undefined"){
        evType = "click";
      }
      if(_el.addEventListener){
        _el.addEventListener(evType, func, useCapture);
        return true;
      }else if(_el.attachEvent){
        var r = _el.attachEvent("on" + evType, func);
        return true;
      }else{
        _el['on' + evType] = func;
      }
    };
    var __addFocusEvent = function(fFocusFunction){
      //TODO not available during the dev
      if(window.excute != null && typeof window.excute == "function"){
        excute();
        excute = null;
      }
    };
    if(__debug_mode){
      document.write('<s'+'cript type="text/javascript" src="' + __js_domain + 'bind2/debug/debug_base.js"></s'+'cript>');
      document.write('<s'+'cript type="text/javascript" src="' + __js_domain + 'bind2/debug/debug.js"></s'+'cript>');
    }
    
    window.__load_js = function(){
      var url = "";
      if(__debug_mode == true){
        url =   __js_domain + "bind2/index.php?product=" + scope.$PRODUCT_NAME + "&pageid=" + scope.$pageid
              + "&rnd=" + new Date().getTime();
      }else{
        // 取得 JS 版本号
        var jsver =  (typeof $_GLOBAL != "undefined" && $_GLOBAL.ver != null)
                ? $_GLOBAL.ver[scope.$PRODUCT_NAME] || "-1"
                : "-1";
        url =   __js_domain + scope.$PRODUCT_NAME + "/" + scope.$pageid + ".js?"
            + jsver + ".js";
      }
      function render_page(){
        __addDOMLoadEvent(main);
        addEvent(document.body, __addFocusEvent, "focus");
        addEvent(window, __addFocusEvent, "scroll");
        addEvent(document.body, __addFocusEvent, "mousemove");
        addEvent(document.body, __addFocusEvent, "mouseover");
      }
      function report_error(time){
        var err_log_url = "http://control.blog.sina.com.cn/admin/article/ria_debug.php?type=js_err_log&page=" + scope.$PRODUCT_NAME
          + "_" + scope.$pageid + "&code=404&desc=&usetime=" + (new Date().getTime() - startTime);
        new Image().src = err_log_url;
        clearTimeout(loadCheck);loadCheck = null;
        report_error = function (){};
      }
      if(__debug_mode == true){
        window.__render_page = render_page;
        document.write('<s'+'cript src="' + url +'" charset="utf-8"></s'+'cript>');
      } else {
        var js = document.createElement("script");
        js.src = url;
        js.charset = "utf-8";
        js.onload = js.onreadystatechange = function () {
          if (js && js.readyState && js.readyState != "loaded" && js.readyState != "complete") {
            return;
          }
          js.onload = js.onreadystatechange = js.onerror = null;
          js.src = "";
          js.parentNode.removeChild(js);
          js = null;
          clearTimeout(loadCheck);loadCheck = null;
          if(typeof main != "undefined"){ render_page(); } else { report_error(); }
        };
        js.onerror = function () {
          js.onload = js.onreadystatechange = js.onerror = null;
          js.src = "";
          js.parentNode.removeChild(js);
          js = null;
          clearTimeout(loadCheck); loadCheck = null;
          report_error();
        };      
        var startTime = new Date().getTime();
        document.getElementsByTagName("head")[0].appendChild(js);
        var loadCheck = setTimeout(function () {
          report_error(-1);
        }, 30000);
      }
    };
    window.__render_page = function(){};
    
    if (typeof scope.$setDomain == "undefined" || scope.$setDomain == true) {
      document.domain = "sina.com.cn";
    }
    if(/\((iPhone|iPad|iPod)/i.test(navigator.userAgent) == false){return;}
    document.addEventListener('mouseover', function(e){
      var ele = e.target;
      do{
        if(ele.tagName == 'A'){
          ele.target = '_self';
          return;
        }
        if(ele.tagName == 'DIV'){return;}
        ele = ele.parentNode;
      }while(ele);
    },false);
  })();

  scope.testboot={};//千万别删除，此代码测试发博文页面的Boot.js是否加载
})();
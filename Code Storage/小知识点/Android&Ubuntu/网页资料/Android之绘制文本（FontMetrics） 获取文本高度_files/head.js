<!--
// flash,scroller,pollfun,default
// 显示flash
function Flash(file, w, h)
{
		document.write("<div style=\"padding:5px\"><object classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\" codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,29,0\" width=\"");
		document.write(w);
		document.write("\" height=\"");
		document.write(h);
		document.write("\"><param name=\"movie\" value=\"");
		document.write(urlPrefix+file);
		document.write("\"> <param name=\"quality\" value=\"high\"><param name=\"wmode\" value=\"opaque\"><embed src=\"");
		document.write(file);
		document.write("\" loop=\"false\" menu=\"false\" quality=\"high\" width=\"");
		document.write(w);
		document.write("\" height=\"");
		document.write(h);
		document.write("\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\"></embed></object></div>");
}

//***

// 提交调查检查
function pollCheck(o,checkMax){
	var checkNum = 0;
	var f = o.form;
	for(var i=0; i<f.elements.length; i++){
		var c = f.elements[i];
		if (c.name==o.name && c.checked){
			checkNum++;
		}
	}
	if (checkNum>checkMax){
		alert("最多选择"+ checkMax +"项!");
		o.checked = false;
	}
}

// 查看调查
function ViewPoll(id) {
	window.open(urlPrefix+"pollView.aspx?act=result&id="+id, "iwmsPoll","width=500,height=400,left=0,top=0,scrollbars=1,status=1,resizable=1");
}


//***

// 友情链接滚动脚本
//alert(navigator.userAgent);
var rollspeed=30
var myInter;
var ff=navigator.userAgent.indexOf("Firefox")>=0;
function MarqueeV(){
	var ooRollV=document.getElementById("oRollV");
	var ooRollV1=document.getElementById("oRollV1");
	var ooRollV2=document.getElementById("oRollV2");
	if (ooRollV.scrollTop>=(ff ? ooRollV.scrollHeight/2 : ooRollV1.scrollHeight)){
		ooRollV.scrollTop=1;	
	}else{
		ooRollV.scrollTop++;
	}
}
function StartRollV() {
	var ooRollV=document.getElementById("oRollV");
	var ooRollV1=document.getElementById("oRollV1");
	var ooRollV2=document.getElementById("oRollV2");
	if (ooRollV) {
		if (parseInt(ooRollV.style.height)>=ooRollV1.scrollHeight) {
			ooRollV.style.height = ooRollV1.scrollHeight;
			return;
		}
		ooRollV2.innerHTML=ooRollV1.innerHTML;
		myInter=setInterval(MarqueeV,rollspeed);
		ooRollV.onmouseover=function() {clearInterval(myInter)};
		ooRollV.onmouseout=function() {myInter=setInterval(MarqueeV,rollspeed)};
	}
}
function MarqueeH(){
	var ooRollH=document.getElementById("oRollH");
	var ooRollH1=document.getElementById("oRollH1");
	var ooRollH2=document.getElementById("oRollH2");
	if(ooRollH2.offsetLeft-ooRollH.scrollLeft<=0) {
		ooRollH.scrollLeft-=ooRollH1.offsetWidth;
	}else{
		ooRollH.scrollLeft++;
	}
}
function StartRollH() {
	var ooRollH=document.getElementById("oRollH");
	var ooRollH1=document.getElementById("oRollH1");
	var ooRollH2=document.getElementById("oRollH2");
	if (ooRollH) {
		if (parseInt(ooRollH.style.width)>=ooRollH2.offsetLeft) {
			oRollH.style.width = oRollH2.offsetLeft;
			return;
		}
		ooRollH2.innerHTML=ooRollH1.innerHTML;
		myInter=setInterval(MarqueeH,rollspeed);
		ooRollH.onmouseover=function() {clearInterval(myInter)};
		ooRollH.onmouseout=function() {myInter=setInterval(MarqueeH,rollspeed)};
	}
}

// 友情链接高度
function linkSize()
{
	var o1,o2;
	with(document){
		o1 = document.getElementById("siteNav");
		o2 = document.getElementById("oRollV");
	}
	if (o1&&o2){
		if (parseInt(o2.style.height)+25<parseInt(o1.clientHeight)){
			o2.style.height = (parseInt(o1.clientHeight) - 35) + "px";
		}
	}
}


//***

/*公告滚动脚本
* Pausing up-down scroller- ? Dynamic Drive (www.dynamicdrive.com)
* This notice MUST stay intact for legal use
* Visit http://www.dynamicdrive.com/ for this script and 100s more.
*/
function pausescroller(content, divId, divHeight, delay){
	content=this.arrayInit(content);
	if (content.length==0){
		return;
	}else if(content.length==1){
		content[1]=content[0];
	}
	this.content=content; //message array content
	this.tickerid=divId; //ID of ticker div to display information
	this.delay=delay; //Delay between msg change, in miliseconds.
	this.mouseoverBol=0; //Boolean to indicate whether mouse is currently over scroller (and pause it if it is)
	this.hiddendivpointer=1; //index of message array for hidden div
	var o_c=document.createElement("DIV");
	o_d=document.createElement("DIV");
	o_d.id=divId;
	with(o_d.style){
		position="relative";
		overflow="hidden";
	height=divHeight+"px";
	}
	var o_d1=document.createElement("DIV");
	with(o_d1){
		id=divId+"1";
		className="innerDiv bulletin";
		style.position="absolute";
		style.textAlign="center";
		style.width="100%";
		innerHTML=content[0];
	}
	var o_d2=document.createElement("DIV");
	with(o_d2){
		id=divId+"2";
		className="innerDiv bulletin";
		style.position="absolute";
		style.textAlign="center";
		style.width="100%";
		style.visibility="hidden";
		innerHTML=content[1];
	}
	o_d.appendChild(o_d1);
	o_d.appendChild(o_d2);
	o_c.appendChild(o_d);
	document.write(o_c.innerHTML);
	var scrollerinstance=this;
	setTimeout(function(){scrollerinstance.initialize()}, 300);
}

// initialize()- Initialize scroller method.
pausescroller.prototype.initialize=function(){
	this.tickerdiv=document.getElementById(this.tickerid);
	this.visiblediv=document.getElementById(this.tickerid+"1");
	this.hiddendiv=document.getElementById(this.tickerid+"2");
	this.visibledivtop=parseInt(pausescroller.getCSSpadding(this.tickerdiv));
	//set width of inner DIVs to outer DIV's width minus padding (padding assumed to be top padding x 2)
	this.visiblediv.style.width=this.hiddendiv.style.width=this.tickerdiv.offsetWidth-(this.visibledivtop*2)+"px";
	this.getinline(this.visiblediv, this.hiddendiv);
	this.hiddendiv.style.visibility="visible";
	var scrollerinstance=this;
	document.getElementById(this.tickerid).onmouseover=function(){scrollerinstance.mouseoverBol=1};
	document.getElementById(this.tickerid).onmouseout=function(){scrollerinstance.mouseoverBol=0};
	if (window.attachEvent){//Clean up loose references in IE
		window.attachEvent("onunload", function(){scrollerinstance.tickerdiv.onmouseover=scrollerinstance.tickerdiv.onmouseout=null});
	}
	setTimeout(function(){scrollerinstance.animateup()}, this.delay);
};

// animateup()- Move the two inner divs of the scroller up and in sync
pausescroller.prototype.animateup=function(){
	var scrollerinstance=this;
	if (parseInt(this.hiddendiv.style.top)>(this.visibledivtop+5)){
		this.visiblediv.style.top=parseInt(this.visiblediv.style.top)-5+"px";
		this.hiddendiv.style.top=parseInt(this.hiddendiv.style.top)-5+"px";
		setTimeout(function(){scrollerinstance.animateup()}, 50);
	}else{
		this.getinline(this.hiddendiv, this.visiblediv);
		this.swapdivs();
		setTimeout(function(){scrollerinstance.setmessage()}, this.delay)
	};
};

// swapdivs()- Swap between which is the visible and which is the hidden div
pausescroller.prototype.swapdivs=function(){
	var tempcontainer=this.visiblediv;
	this.visiblediv=this.hiddendiv;
	this.hiddendiv=tempcontainer;
};

pausescroller.prototype.getinline=function(div1, div2){
	div1.style.top=this.visibledivtop+"px";
	div2.style.top=Math.max(div1.parentNode.offsetHeight, div1.offsetHeight)+"px";
};

// setmessage()- Populate the hidden div with the next message before it's visible
pausescroller.prototype.setmessage=function(){
	var scrollerinstance=this;
	if (this.mouseoverBol==1){ //if mouse is currently over scoller, do nothing (pause it)
		setTimeout(function(){scrollerinstance.setmessage()}, 100);
	}else{
		var i=this.hiddendivpointer;
		var ceiling=this.content.length;
		this.hiddendivpointer=(i+1>ceiling-1)? 0 : i+1;
		this.hiddendiv.innerHTML=this.content[this.hiddendivpointer];
		this.animateup();
	}
};

pausescroller.getCSSpadding=function(tickerobj){ //get CSS padding value, if any
	if (tickerobj.currentStyle){
		return tickerobj.currentStyle["paddingTop"];
	}else if (window.getComputedStyle){ //if DOM2
		return window.getComputedStyle(tickerobj, "").getPropertyValue("padding-top");
	}else{
		return 0;
	}
};

// iwms公告数组处理函数
pausescroller.prototype.arrayInit=function(arr){
	if (arr.length==0 || typeof(arr[0])!="object"){
		return arr;
	}
	if (arr[0].length==3){
		for(var i=0;i<arr.length;i++){
		var c=document.createElement("DIV");
		var a=document.createElement("A");
		a.target="_blank";
		a.href=arr[i][1];
		a.innerHTML=arr[i][0];
		a.className="bulletin";
		var s=document.createElement("SPAN");
		s.className="gray";
		s.innerHTML="&nbsp; ["+ arr[i][2] +"]";
		c.appendChild(a);
		c.appendChild(s);
		arr[i] = c.innerHTML;
		}
	}else{
		for(var i=0;i<arr.length;i++){
		var c=document.createElement("DIV");
		var a=document.createElement("A");
		a.style.fontWeight="bold";
		a.target="_blank";
		a.href=arr[i][1];
		a.innerHTML=arr[i][0];
		var s=document.createElement("DIV");
		s.className="gray";
		s.style.textAlign="right";
		s.style.marginBottom="10px";
		s.innerHTML="&nbsp; ["+ arr[i][2] +"]";
		var b=document.createElement("DIV");
		b.style.textAlign="left";
		b.innerHTML=arr[i][3];
		c.appendChild(a);
		c.appendChild(b);
		c.appendChild(s);
		arr[i] = c.innerHTML;
		}
	}
	return arr;
};
-->
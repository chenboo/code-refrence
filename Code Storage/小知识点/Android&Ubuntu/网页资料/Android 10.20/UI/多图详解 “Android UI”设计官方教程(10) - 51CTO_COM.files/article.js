// JavaScript Document
function email_test(str) 
{ 
var i,flag=0; 
var at_symbol=0; 
//��@������λ�� 
var dot_symbol=0; 
//��.������λ�� 
//if(char_test(str.charAt(0))==0 ) 
//return (1); 
//���ַ���������ĸ 

for (i=1;i<str.length;i++) 
if(str.charAt(i)=='@') 
{ 
at_symbol=i; 
break; 
} 
//��⡰@����λ�� 

if(at_symbol==str.length-1 || at_symbol==0) 
return(2); 
//û���ʼ����������� 

if(at_symbol<3) 
return(3); 
//�ʺ����������ַ� 

if(at_symbol>19 ) 
return(4); 
//�ʺŶ���ʮ�Ÿ��ַ� 

for(i=1;i<at_symbol;i++) 
if(char_test(str.charAt(i))==0 && spchar_test(str.charAt(i))==0) 
return (5); 
for(i=at_symbol+1;i<str.length;i++) 
if(char_test(str.charAt(i))==0 && spchar_test(str.charAt(i))==0) 
return (5); 
//�����������������ַ� 

for(i=at_symbol+1;i<str.length;i++) 
if(str.charAt(i)=='.') dot_symbol=i; 
for(i=at_symbol+1;i<str.length;i++) 
if(dot_symbol==0 || dot_symbol==str.length-1) 
//�򵥵ļ����û�С�.������ȷ�����������Ƿ�Ϸ� 
return (6); 

return (0); 
//�ʼ����Ϸ� 
}
function mail_process(stringin){ 
var num=email_test(stringin); 
var str=""; 
if (num!=0) 
{
switch (num) 
{
case 1: 
str="���ַ���������ĸ������Ϊ�գ��뷵�����"; 
break; 
case 2: 
str="��������д�ʼ��������ĵ�ַ�ˣ��뷵�����"; 
break; 
case 3: 
str="�����ʺ�̫�̣��������������ַ�!�뷵�����"; 
break; 
case 4: 
str="�����ʺ�̫�������ܶ���ʮ�Ÿ��ַ�!�뷵�����"; 
break; 
case 5: 
str="��ʹ���˷Ƿ��ַ�!�뷵�����"; 
break; 
case 6: 
str="�����ʼ��������ĵ�ַ���Ϸ�!�뷵�����"; 
break; 
default: 
str="����email��ַ���Ϸ�!�뷵�����"; 
} 
alert(str); 
return false;
} 
}
function cmouseover(divName,over,counts,oclass){
	if(signover==1){
		for(var i=0;i<counts;i++){
			var divClassName = document.getElementById(divName+'top_'+i);
			
				divClassName.className = divClassName.className.substr(0,divClassName.className.length-2)+"_b";
		}
		document.getElementById(divName+'top_'+over).className = divClassName.className.substr(0,divClassName.className.length-2)+"_a";
		for(i=0;i<counts;i++){
			document.getElementById(divName+'_'+i).style.display = "none";
		}
		document.getElementById(divName+'_'+over).style.display = "";
	}
	signover = 0;
	return false;
}
function change_con(up,down){
	if(up && down){
		document.getElementById("con_"+up).style.display="";
		document.getElementById("jiantou_"+up).style.display="";
		document.getElementById("con_"+down).style.display="none";
		document.getElementById("jiantou_"+down).style.display="none";
	}
}
function change_read(up,down){
	if(up && down){
		document.getElementById("com_"+up).style.display="";
		document.getElementById("bt_com_"+up).className="right_4_2";
		document.getElementById("com_"+down).style.display="none";
		document.getElementById("bt_com_"+down).className="right_4_3";
	}
}
function change_top(tnum){
	if(tnum){
		for(var i=1;i<5;i++){
			document.getElementById("bt_top_"+i).className="right_4_3";
			document.getElementById("div_top_"+i).style.display="none";
		}
		document.getElementById("bt_top_"+tnum).className="right_4_2";
		document.getElementById("div_top_"+tnum).style.display="";
	}
}
function change_spec(tnum){
	if(tnum){
		for(var i=1;i<4;i++){
			document.getElementById("bt_spec_"+i).className="right_4_3";
			document.getElementById("spec_"+i).style.display="none";
		}
		document.getElementById("bt_spec_"+tnum).className="right_4_2";
		document.getElementById("spec_"+tnum).style.display="";
	}
}
function change_down(tnum){
	for(var i=0;i<3;i++){
		document.getElementById("download_"+i).style.display="none";
	}
	document.getElementById("download_"+tnum).style.display="";
}
function FormMenu(targ,selObj,restore){ 
	window.open(selObj.options[selObj.selectedIndex].value);
	if (restore) selObj.selectedIndex=0;
}
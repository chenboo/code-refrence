function char_test(chr) 
//�ַ���⺯�� 
{ 
var i; 
var smallch="abcdefghijklmnopqrstuvwxyz"; 
var bigch="ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
for(i=0;i<26;i++) 
if(chr==smallch.charAt(i) || chr==bigch.charAt(i)) 
return(1); 
return(0); 
} 

function spchar_test(chr) 
//���ֺ������ַ���⺯�� 
{ 
var i; 
var spch="_-.0123456789"; 
for (i=0;i<13;i++) 
if(chr==spch.charAt(i)) 
return(1); 
return(0); 
} 

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
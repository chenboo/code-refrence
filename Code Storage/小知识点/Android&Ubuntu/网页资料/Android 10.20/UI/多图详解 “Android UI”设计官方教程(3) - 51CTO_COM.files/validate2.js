function validate(theform) {
	
	if( theform.msg.value == '' || theform.msg.value == ''){
		alert('��������������');
		theform.msg.focus();
		return false;
	}
	
	if(theform.password.value == '' && theform.nouser.checked==false ){
			alert('������½��ѡ����������!');
			
			return false;
		}
	if(theform.username.value == '' && theform.nouser.checked==false){
			alert('������½��ѡ����������!');
			
			return false;
		}
	if( theform.authnum.value == ''){
		alert('��������֤��');
		theform.authnum.focus();
		return false;
	}

	
}
/* modify by kelly 2010-10-20
function big(o)
{
	var zoom=parseInt(o.style.zoom, 10)||100;zoom+=window.event.wheelDelta/12;
	if (zoom>0) o.style.zoom=zoom+'%';
	return false; 
}
*/

var clickCount = 0;
function clearCommentContent(oObject){
	clickCount++;
	if (clickCount == 1){
		oObject.value = "";
	}
}
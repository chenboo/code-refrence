
document.write('<form action="http://www.51cto.com/php/sendfeedback.php" target="_self" method="post" name="feedback" id="feedback" onSubmit="return commentSubmit(this)" style="padding: 0px; margin: 0px;">');
document.write('<textarea class="txtinput" name="msg" id="msg" style="height: 88px;"></textarea><br /><table width="99%" border="0" cellspacing="0" cellpadding="0"><tr><td height="6" colspan="6">');
document.write('<input type="hidden" name="artID" value="269370" />');
document.write('<input type="hidden" name="referer" value="http://mobile.51cto.com/symbian-269370.htm" />');
document.write('<input type="hidden" name="quick" value="0" />');
document.write('<input type="hidden" name="author2" value="51CTO����" />');
document.write('</td></tr><tr><td width="9%">');
document.write('��֤�룺</td><td width="11%"><input name="authnum" onClick="displaysecunum();" type="text" class="test3" />');
document.write('</td><td width="51%"><img id="secunum" style="vertical-align:middle;" onclick="refimg()" style="display:none"/>');
document.write('<span style="display:none;" id="spanfont">���ͼƬ��ˢ����֤��</span>');
document.write('<span style="display:inline;" id="clickfont">������������֤��</span>');
document.write('</td><td width="4%"><input name="nouser" type="checkbox" value="1" onClick="author2username2()"></td><td width="12%">��������');
document.write('</td><td width="13%"><input type="image" name="Submit32" src="http://www.51cto.com/php/tijiao.gif" />');
document.write('</td></tr><tr><td height="6" colspan="6"></td></tr></table></form>');
function displaysecunum(){
				var displaystr = document.getElementById("secunum").style.display;
				if(displaystr == 'none' || !displaystr)
				{
					refimg();
					document.getElementById("clickfont").style.display = 'none';
					document.getElementById("secunum").style.display = 'inline';
					document.getElementById("spanfont").style.display = 'inline';
				}
			}
function author2username2(){
				if( islogin==1 )
				{
					if (this.checked==true)
					{
						document.getElementById("author2").value=document.getElementById("username2").value;document.getElementById("username2").value="51CTO����";
					}else
					{
						document.getElementById("username2").value=document.getElementById("author2").value
					}
				}
			}
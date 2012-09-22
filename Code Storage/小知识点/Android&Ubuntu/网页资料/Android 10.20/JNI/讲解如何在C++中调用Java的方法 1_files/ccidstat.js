			var sdomain=location.host;
			var sitenum="";
			if(sdomain=="eco.ccidnet.com"){
						sitenum="100040";
				}else if(sdomain=="project.ccidnet.com"){
						sitenum="100026";
				}
			if(sitenum!=""){
				document.write ("<script language=\"javascript\" type=\"text/javascript\">");
				document.write ("var _PCSWebSite=\""+sitenum+"\";");	/*这个变量存频道(栏目)编号*/
				document.write ("var _PCSImage=\"countlogo1.gif\";")
				document.write ("<\/script>");
				document.write ("<script language=\"javascript\" type=\"text/javascript\" src=\"http:\/\/stat.ccidnet.com\/count\/count.js\" ><\/script>");
			  sitenum="100039";
			}
			document.write ("<script language=\"javascript\" type=\"text/javascript\" src=\"http:\/\/image.ccidnet.com\/nav\/count_JS\/ccid\/urlstat.js\" ><\/script>");

document.write("<input type=\"hidden\" name=\"clientDate\" id=\"clientDate\" ");
document.write("value=\"");
document.write((new Date()).toGMTString().replace("UTC","GMT"));
document.write("\"/>");

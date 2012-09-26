/* Create Chen博客的CSS */
/* http://technology.cnblogs.com */
/* 2012-7-26 Updated */

body{
	background:white;
}

#main{
	margin-bottom: 60px;
}
a{
	color:#06D;
}
a:hover {
	_color: #F60;
	color:#06D;
	text-decoration:underline;
}
div.postTitle a{
	color:#06D;
}
div.postTitle a:hover{
	color:red !important;
	_border-bottom:#F60 2px solid;
	text-decoration:none;
}
div.postTitle a:visited{
	color:#06D;
}
#topBackground{
	background: url(http://pic002.cnblogs.com/images/2011/70278/2011052216374836.jpg) no-repeat center top;
	top: 0;
	height: 400px;
	position: fixed;
	width: 100%;
	z-index:-1;
}
#header{
	height:90px;
}
#footer{
	bottom: 0px;
	height: 200px;
	position: fixed;
	width: 100%;
	background: url(http://pic002.cnblogs.com/images/2011/70278/2011052216374836.jpg) no-repeat center bottom;
	z-index: -1000;
	color: transparent;
	display:block !important;
}
/*导航条*/
div#navigator {
	font-family: "Microsoft Yahei",Tahoma,Arial,Helvetica,STHeiti;
	position: absolute;
	top: 10px;
	right: 20px;
	float: right;
	height: 76px;
	height: 80px;
background:none;
	_background: url(http://pic002.cnblogs.com/images/2011/70278/2011051911433598.png) no-repeat center center;
}
#navList {
	position: static !important;
	margin-top: 75px;
}
#navList li a:hover{
	color:white;
	border-bottom:2px solid red;
	text-decoration:none;
}
/*博客标题*/
div#blogTitle {
	background:none;
height: 85px;
background: url(http://pic002.cnblogs.com/images/2011/70278/2011051911433598.png) no-repeat bottom right;
margin-right: 30px;
}
div#blogTitle .title {
	font-family: Georgia, 'Serif', 'Arial', 'Helvetica';
	font-style: italic;
	font-size: 28px;
	font-weight:bold;
	padding-top: 23px;
}
/*子标题*/
div#blogTitle .subtitle {
	color: #ffffff;
	margin: 10px 0px 0px 50px;
	font-size: 11pt;
}
div.postTitle {
	border-bottom: 1px solid #CCC;
}
div#mainContent{
	margin-left: 280px;
	margin-top: 25px;
}
div.forFlow{
	padding: 20px 30px 50px;
	background: white;
}
div#sideBar{
	border:none !important;
	background-color:transparent !important;
}
/*搜索的框框和按钮*/
.btn_my_zzk{
	background: url(http://pic002.cnblogs.com/images/2011/70278/2011060320275124.png) repeat-x 0 0;
	height: 25px;
	width: 70px;
	background-position: 0 -36px;
	border: 1px solid #787878;
	color: #000;
	cursor: pointer;
	font-size: 13px;
	font-family: "Microsoft Yahei",Tahoma,Arial,Helvetica,STHeiti;
	-webkit-border-radius: 4px;
}
.input_my_zzk{
	outline: none;
	width: 150px;
	height: 25px;
	padding-left: 10px;
	color: #333;
	border: 1px solid #BBB;
	-webkit-border-radius: 4px;
}
/*不显示随笔发布日期*/
.dayTitle {
display:none;
}
/*评论文本框*/
#tbCommentBody{
	background: url("http://pic002.cnblogs.com/images/2011/70278/2011060318304574.png") no-repeat scroll 0 0;
	border: 1px solid #AEBACD;
	font-family: Microsoft Yahei;
	font-size: 14px;
	line-height: 20px;
	padding: 7px;
	margin-bottom: 10px;
	resize: none;
	width: 580px;
	height: 200px;
	outline: none;
}

/*发表评论Lable上面的div*/
div.postDesc{
	padding-bottom:0px;
}
/*发表评论的Lable, 绿色背景*/
div.feedback_area_title{
	width:100%;
	margin:0px;
	background-color:green;
	color:#ffffff;
	text-align:center;
}
/*每条评论的div*/
.feedbackItem{
	position: relative;
	_border: 1px solid #BBB;
	margin-bottom: 8px;
	margin-top: 4px;
	_background-color: #FCFEC5;
	min-height:85px;
	color: #000;
}
div.feedbackItem .feedbackListSubtitle
{
	margin-left:57px;
}
div.feedbackItem .comment_vote
{
	margin-top:10px;
}
div.feedbackItem div.feedbackCon {
	color: #555;
	padding: 7px 7px 7px 30px;
	margin-left: 30px;
	_background: url('http://pic002.cnblogs.com/images/2011/70278/2011082418163061.gif') no-repeat 0px 0px;
}
#author_profile a:hover {
	text-decoration:none;
}
.feedbackAvatar{
	border: 1px solid #BBB;
	width:50px;max-height: 50px;
	position:absolute;
	top: 15px;
}
/*提交按钮*/
#btn_comment_submit{
	cursor: pointer;
	width:55px;height:25px;border:none;
	background-color:#2DAEBF;
	-webkit-border-radius: 10px;color:white;
	font-weight: bold;
	-webkit-box-shadow: 0 1px 3px rgba(0, 0, 0, 0.5);
	text-shadow: 0 -1px 1px rgba(0, 0, 0, 0.25);
	font-size: 10pt;
}
/*投票相关的支持按钮*/
.diggit{
	width: 125px;
	height: 50px;
	background: url('http://pic002.cnblogs.com/images/2011/70278/2011050520390113.gif') no-repeat;
	cursor: pointer;
	margin-top: 0px;
	padding-top: 9px;
	padding-left: 38px;
}
.diggnum{
	font-family:Georgia, Serif;
}
.buryit {
	display:none;
}
span#digg_count{
	font-size: 20px;
	color:green;
}
/*提示推荐的Label*/
.diggword {
	margin-top: 0px;
}

/*引用的文字*/
blockquote{
	color: #000;
	font-family:"Microsoft Yahei", Tahoma, Arial, Helvetica, STHeiti;
	background-color:#FCFEC5;
	border:1px solid #BBB;
	_border-radius:8px;
	margin:5px 0px 5px 0px;
	padding-left: 20px;
	line-height: 1.3;
	word-wrap: break-word;
}

.copy {
	border: #c0c0c0 1px solid;
	padding: 10px;
	line-height: 15pt;
	margin-top: 15px;
	margin-bottom: 15px;
	background: #dbeaff;
	font-size: 9pt;
}
.quote {
	border: #c0c0c0 1px dashed;
	padding: 10px;
	line-height: 12pt;
	margin-top: 15px;
	font-family: Verdana, "Lucida Grande", Geneva, Arial, sans-serif;
	margin-bottom: 15px;
	background: #e9e9e9;
	font-size: 9pt;
}
.copy img {
	width: 25px; height: 25px
}
.cuscomment {
	border: #c0c0c0 1px dashed;
	padding: 5px 10px 10px 10px;
	line-height: 20pt;
	margin-top: 15px;
	margin-bottom: 15px;
	font-size: 9pt;
}
.fastcomment {
	padding: 5px;
	margin-top: 5px;
	color: #ffffff;
	margin-right: 5px;
}
.fastcomment:hover {
	border: #ffffff 1px outset;
	background: #ffffff;
	color: #000000;
}
.postTitle, .entrylistPosttitle, .feedback_area_title{
	font-family: "Microsoft Yahei","微软雅黑",Tahoma,Arial,Helvetica,STHeiti;
	font-size: 16px;
	font-weight: 500;
	line-height: 1.4em;
}
.postBody {
	font-size: 15px;
	font-family:"Trebuchet MS", Georgia,"Times New Roman",Times,sans-serif;
	line-height: 180%;
}
.postBody p{
	margin: 12px auto 12px auto;
}
.postBody a {
	border-bottom: #3d81ee 1px dashed; COLOR: #06C;
}
.postBody a:hover {
	color: red;
	text-decoration:none;
}
.postBody h1 {
	border-radius: 3px;
	padding-top:5px;
	padding-bottom:5px;
	padding-left:20px;
	background: #1977B0;
	margin-top: 15px;
	margin-bottom: 5px;
	color: white;
	font-size: 18px;
	font-weight: bold;
	letter-spacing: 1px;
}
.postBody h2 {
	color: #1977B0;
	padding-left: 20px;
	font-size: 18px;
}

.postBody h3 {
	color: #1977B0;
	padding-left: 25px;
	font-size: 16px;
}
.postBody h4 {
	color: #1977B0;
	padding-left: 30px;
}
.postBody h5 {
	color: #1977B0;
	padding-left: 25px;
}
.postBody img {
	margin-bottom: 0px;
}
div.postCon {
	color: #444; font-size: 13px
}
div.postCon a {
	border-bottom: #3d81ee 1px dashed;
	color: #06C; font-size: 13px
}
div.postCon a:hover {
	color: #f00;
	text-decoration:none;
}
#MySignature {
	margin-top: 30px;
}
#bottom a{
	color:#ffffff;
}
/*代码部分*/
.syntaxhighlighter{
	font-size:14px !important;
	line-height: 1.4 !important;
	border:#c0c0c0 1px solid !important;
	margin-top: 2px !important;
	margin-bottom: 2px !important;
	border-radius:5px;
	word-wrap: break-word;
}
/*主页文章简要信息*/
.c_b_p_desc {
	font-family:"Microsoft Yahei",Tahoma,Arial,Helvetica,STHeiti;
}
/*摘要右边的图片*/
.desc_img {
	height: 120px;
	width: 120px;
}
.code{
	font-size: 14px;
	font-family: "Microsoft Yahei",Tahoma,Arial,Helvetica,STHeiti;
	background-color: #FCFEC5;
	border: 1px solid #BBB;
	margin: 5px 0px 5px 0px;
	padding-left: 20px;
	word-wrap: break-word;
}
.post table{
	margin-left: 30px;
	border: 1px solid #333;
	border-collapse: collapse;
	border-spacing: 0;
	line-height:1.3;
	font-size: 12px;
}
.post tr, td, th{
	border: 1px solid #333;
	padding: 5px 8px;
	text-align: center;
	white-space: nowrap;
}
.post th{
	background: #3398CC none repeat scroll 0 0;
	color:#fff;
}
::selection
{
	background: rgb(255,255,158);
}
::-moz-selection
{
	background: rgb(255,255,158);
}
/*scroll bar*/
::-webkit-scrollbar {
    width:17px;
    height:17px;
    background-color:#f0f0f0;
    border:none
}
::-webkit-scrollbar-thumb {
    background-color:#cdcdcd;
    border:1px solid #f0f0f0
}
::-webkit-scrollbar-thumb:hover {
    background-color:#dadada
}
::-webkit-scrollbar-thumb:active {
    background-color:#606060
}
::-webkit-scrollbar-thumb:disabled {
    background-color:#f9f9f9
}
::-webkit-scrollbar-corner {
    background-color:#f0f0f0
}
::-webkit-scrollbar-button {
    background-color:#f0f0f0;
    background-image:url(http://pic002.cnblogs.com/images/2012/70278/2012081922455213.png);
    background-repeat:no-repeat
}
::-webkit-scrollbar-button:vertical {
    height:33px
}
::-webkit-scrollbar-button:horizontal {
    width:33px
}
::-webkit-scrollbar-button:horizontal:increment {
    background-position:0 -444px
}
::-webkit-scrollbar-button:horizontal:decrement {
    background-position:12px -425px
}
::-webkit-scrollbar-button:vertical:increment {
    background-position:-1px -391px
}
::-webkit-scrollbar-button:vertical:decrement {
    background-position:-1px -358px
}
::-webkit-scrollbar-button:hover {
    background-color:#dadada
}
::-webkit-scrollbar-button:horizontal:increment:hover {
    background-position:0 -548px
}
::-webkit-scrollbar-button:horizontal:decrement:hover {
    background-position:12px -529px
}
::-webkit-scrollbar-button:vertical:increment:hover {
    background-position:-1px -495px
}
::-webkit-scrollbar-button:vertical:decrement:hover {
    background-position:-1px -462px
}
::-webkit-scrollbar-button:active {
    background-color:#606060
}
::-webkit-scrollbar-button:horizontal:increment:active {
    background-position:0 -652px
}
::-webkit-scrollbar-button:horizontal:decrement:active {
    background-position:12px -633px
}
::-webkit-scrollbar-button:vertical:increment:active {
    background-position:-1px -599px
}
::-webkit-scrollbar-button:vertical:decrement:active {
    background-position:-1px -566px
}
::-webkit-scrollbar-button:disabled {
    background-color:#f9f9f9
}
::-webkit-scrollbar-button:horizontal:increment:disabled {
    background-position:0 -756px
}
::-webkit-scrollbar-button:horizontal:decrement:disabled {
    background-position:12px -737px
}
::-webkit-scrollbar-button:vertical:increment:disabled {
    background-position:-1px -703px
}
::-webkit-scrollbar-button:vertical:decrement:disabled {
    background-position:-1px -670px
}
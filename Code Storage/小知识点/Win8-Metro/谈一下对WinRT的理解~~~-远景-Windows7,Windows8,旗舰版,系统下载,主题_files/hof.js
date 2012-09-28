/* jQuery Easing v1.3 - http://gsgd.co.uk/sandbox/jquery/easing/ */
jQuery.easing["easeOutCubic"] = function (x, t, b, c, d) {
	return c * ((t = t / d - 1) * t * t + 1) + b;
};

jQuery(function ($) {
	$(".pb_login").css("padding-right", "60px");
	$("#um div").css("padding-right", "50px");
	
	$("body").prepend("<div id='hof'><div class='bd'></div><div class='hof_wrapper'><div class='hof_inner'><table><tr><td><img src='http://pbcdn.com/public/javascript/hof/images/hof_logo.png' alt='远景名人堂 | Hall of Fame' class='hof_logo'/><p><img src='http://pbcdn.com/public/javascript/hof/images/hof_comingsoon.jpg' alt='coming soon' /></p><p class='words'>献给那些曾经、现在和将来<br />为景友们作出卓越贡献的人们</p></td></tr></table></div><a href='hof.html' id='btn_hof' /></div><span class='close' title='关闭'>×</span></div>");
	
	$("#btn_hof").click(function () {
		$("#cproIframe4holder,#cproIframe5holder").css("z-index", "998");
		$("html").addClass("hof");
		$("#hof .close").css({
			"top": "-65px",
			"right": "-65px"
		}).animate({
			"top": "-10px",
			"right": "-10px"
		}, 400, function () {
			$(this).removeAttr("style");
		});
		$("#hof .hof_inner").animate({
			"height": $(window).height()
		}, 1300, "easeOutCubic", function(){
			$(window).bind("resize", resizeFun);
		});
		return false;
	}).hover(function(){
		$(this).animate({"bottom":"-58px"}, 200);
	}, function(){
		if (!$("html").hasClass("hof")) $(this).animate({"bottom":"-53px"}, 200);
	}).mousedown(function(){
		$(this).animate({"bottom":"-61px"}, 200);
	});
	
	$("#hof .close").click(function () {
		$("html").removeClass("hof");
		$("#btn_hof").animate({"bottom":"-53px"}, 200);
		$("#hof .close").show().stop().animate({
			"top": "-65px",
			"right": "-65px"
		}, 400, function () {
			$(this).removeAttr("style");
		});
		$("#hof .hof_inner").animate({
			"height": 0
		}, 1300, "easeOutCubic", function(){
			$("#hof .close").attr("style", "").removeAttr("style");
		});
		$(window).unbind("resize", resizeFun);
		return false;
	}).hover(function(){
		if ($("html").hasClass("hof")) $(this).animate({"top":0,"right":0}, 200);
	}, function(){
		if ($("html").hasClass("hof")) $(this).animate({"top":"-10px","right":"-10px"}, 200);
	}).mousedown(function(){
		$(this).animate({"top":"-2px","right":"-2px"}, 200);
	});

	$("body").mousedown(function (e) {
		if ($("html").hasClass("hof") && e.which == 2) return false;
	});
	var resizeFun = function () {
		if ($("html").hasClass("hof")) $("#hof .hof_inner").css("height", $(window).height());
	};
});
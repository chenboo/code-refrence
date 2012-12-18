var users = Array();

$(document).ready(function() {
    $('[name=userLink]').each(function() {
        var username = $(this).text();
        $(this).qtip({
        	prerender: false,
        	content: '<img width="32" height="32" src="http://www.developer.nokia.com/Community/Piazza/images/loading.gif" />',
        	position: {
        		my: "bottom right",
        		at: "top left"
        	},
        	show: {
        		solo: true,
        		ready: false
        	},
        	hide: {
        		fixed: true,
        		delay: 750
        	},
        	style: {
        	    classes: "ui-tooltip-blue ui-tooltip-rounded ui-tooltip-shadow nd-ui-tooltip ",
        		tip: true
        	},
        	events: {
        		show: function (event, api) { 
        		    ajaxUrl = 'http://www.developer.nokia.com/Community/Wiki/Special:UserContributions?username=' + username + '&function=?'; 
        		    var popup = api;
        		    if (users[username]) { 
        		        popup.set('content.text', generateUserPopup(username));
        		    } 
        		    else { 
        		        $.getJSON(ajaxUrl, function(data) { 
        		            users[username] = data; 
        		            popup.set('content.text', generateUserPopup(username));
        		        }); 
        		    } 
        		}
        	}
        }
        );
    });
});

function generateUserPopup(username) {
    
    // Check to see is the user is authenticated before displaying the send PM
    sendMessage = '';
    if ($.cookie('fnlogged')) {
        sendMessage = '<li><a href="http://www.developer.nokia.com/Community/Discussion/private.php?do=newpm&username=' + username + '">Send private message</a></li>';
    }
    
    var content = '<span class="roundCorner" style="height: 78px; width: 78px; float: right;"><div class="roundCornerTl"></div><div class="roundCornerBr"></div><img class="roundImage tl br" style="float: right" width="78" height="78" src="http://www.developer.nokia.com/dynamic/profile/photo.html?size=78&username=' + username + '" alt="' + username + '" /></span>\
    <ul class="userPopup">\
    <li><a href="http://www.developer.nokia.com/Profile/?u=' + username + '">View profile</a></li>\
    ' + sendMessage + '\
    <li>Articles: ' + users[username].articles + '</li>\
    <li>Edits: ' + users[username].modifications + '</li>\
    </ul>';
    return content;
}

// Add popup to talk/comments
mw.loader.using( ['mediawiki.util', 'jquery.ui', 'ext.jqueryqtip'], function () {
    // alert("skin nokiadev.js loaded");
    
    // shift the talkhere div outside the main body div (MW1.18 version changed)
    $(".talkhere-tomove").appendTo("#content");
    $(".talkhere-tomove").show();
    
    if (mw.config.get( 'wgIsMainPage' )) {
        $(".talkhere-tomove").hide();
        $("#talkhere").hide();
        
    }

    // jQuery(function( $ ) {
        //$("#fncontent table tr:nth-child(even)").addClass('even');

        // move a non-empty TOC across otherwise blast it
        if ($("#bodyContent #toc li").size() > 0) {
            $("#contents-list").html($("#bodyContent #toc ul").html());
            $(".tocnumber").remove();
            $(".toctoggle").remove();
            $("#mw-panel2").show();
        } else {
            $("#mw-panel2").remove();        
            $("#mw-panel2-wrapper").remove();        
        }

        $("#toc").remove();

        // Stripe the tables
        $(".wikitable tr:even").addClass("even");

        if (talkHistory) {
            $('#ca-talk a').text('Read Comments');
            $('#ca-talk').qtip({
                prerender: false,
                content: '<img class="roundImage tl br" style="float: right" width="78" height="78" src="http://www.developer.nokia.com/dynamic/profile/photo.html?size=78&username=' + talkHistory["username"] + '" alt="' + talkHistory["username"] + '" />\
                </span>\
                <ul class="userPopup">\
                    <li>Last comment by <a href="http://www.developer.nokia.com/Profile/?u=' + talkHistory["username"] + '">' + talkHistory["username"] + '</a></li>\
                    <li>' + talkHistory["lastmod"] + '</li>\
                </ul>',
                position: {
            		my: "bottom middle",
            		at: "top middle"
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
            	}
            });
        }    
    
        // Remove the <div style="clear:both;"></div> from after the editor
        $(".wikiEditor-ui + div").css("clear", "none");
    // });

    /* a copy of the collapsbile menu code from the Vector skin, but set up for the 2nd mw-panel*/
    // $j( document ).ready( function() {
    	if( !wgVectorEnabledModules.collapsiblenav ) {
    		return true;
    	}
    	$j( '#mw-panel2' ).addClass( 'collapsible-nav' );
    	// Always show the first portal
    	$j( '#mw-panel2 > div.portal:first' )
    		.addClass( 'expanded' )
    		.find( 'div.body' )
    		.show();
    	// Remember which portals to hide and show
    	$j( '#mw-panel2 > div.portal:not(:first)' )
    		.each( function() {
    			if ( $j.cookie( 'vector-nav-' + $j(this).attr( 'id' ) ) == 'true' ) {
    				$j(this)
    					.addClass( 'expanded' )
    					.find( 'div.body' )
    					.show();
    			} else {
    				$j(this).addClass( 'collapsed' );
    			}
    		} );
    	// Toggle the selected menu's class and expand or collapse the menu
    	$j( '#mw-panel2 > div.portal > h5' ).click( function() {
    		$j.cookie( 'vector-nav-' + $j(this).parent().attr( 'id' ), $j(this).parent().is( '.collapsed' ) );
    		$j(this)
    			.parent()
    			.toggleClass( 'expanded' )
    			.toggleClass( 'collapsed' )
    			.find( 'div.body' )
    			.slideToggle( 'fast' );
    		return false;
    	} );
    // } );
} );
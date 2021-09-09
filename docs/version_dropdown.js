//hello
versions=['devel'];

version='devel';
function changeVersion(newVersion) {
	newURL = window.location.href.replace('/'+version+'/','/'+newVersion+'/');
	if ( newURL != window.location.href ) {
		window.location.href = newURL;
	} else {
		console.log('are you in development?');
	}
}

$(document).ready(function() {
	let content = '<div class="version"><div class="dropdown"><div class="entry">▸ '+version+'</div><div class="dropdown-content">';

	for (let i in versions) {
		if (versions[i] == version) {
			continue;
		}
		content += '<a onclick="changeVersion(\''+versions[i]+'\')">' + versions[i] + '</a>';
	}
	content += '</div></div></div>';
	versionDiv = $('.version');
	versionDiv.replaceWith(content);
	$('.version .dropdown').css({
		"position":"relative",
		"display": "inline-block",
		"width": "100px",
	});
	$('.version .dropdown a').css({
		"display":"block",
		"background":"#2980b9",
		"margin":"1px",
		"color":"hsla(0,0%,100%,.5)",
	});
	$('.version .dropdown-content').css({
		"display":"block",
		"position":"absolute",
		"background":"#49a0d9",
		"width": "100px",
		"z-index":"1",
	});
	$('.version .dropdown-content').hide();
	menuIsShown = false;
	entryItem = $('.version .dropdown .entry');
	contentItem = $('.version .dropdown .dropdown-content');
	showMenu = function() {
		entryItem.text('▾ ' + version);
		contentItem.show(300);
		menuIsShown = true;
	};

	hideMenu = function() {
		entryItem.text('▸ ' + version);
		contentItem.hide(300);
		menuIsShown = false;
	};

	$('.version .dropdown .entry').click(function() {
		if ( menuIsShown == true ) {
			hideMenu();
		} else {
			showMenu();
		}
	});

	window.onclick = function(event) {
		if ( !event.target.matches('.entry') ) {
			hideMenu();
		}
	}


});

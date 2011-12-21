// Transcript highlighting functions
// Customized to DHi Video Player environment -- GPL
// --------------------------------------------------- //

	var highlightedLine = 0;
	
	function goToLine(num) {
		$("#tLine"+highlightedLine).toggleClass('highlighted');
		highlightedLine = num;
		$("#tLine"+highlightedLine).toggleClass('highlighted');
		$("#rolling_transcript").scrollTop(0);
		var pos = $("#tLine"+num).position().top;
		$("#rolling_transcript").scrollTop(pos);
	}
	
	$(function() {
		$('a.lightbox').lightBox(); // Select all links with lightbox class
	});
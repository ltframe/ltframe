$(function(){
	 $("input[type='text']").bind('click',  function(event) {
	 			$(this).val('');
	 });	

	$("#minwindow").bind('mousedown',function() {
		MinWindow();
	});
	$("#windowbar").bind('mousedown',function() {
		DragWindow();
	});
})
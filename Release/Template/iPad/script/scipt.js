// JavaScript Document


 /*!***********************Jquery_folder_Effect *****************************
 * jQuery JavaScript Library
 * Copyright 2011, Zhang Ziyang
 */
$(document).ready(function(){	   
	
		$(".folders").toggle(function(){						  
			$(this).siblings(".icon").fadeTo(300,0.2);
			$("#dock").fadeTo(300,0.2);
			$(this).children(".iconName").hide();
			$(".slide").slideDown(300);
			$(".pack>.icon").fadeIn(500);
			
		},function(){				  
			$(this).siblings(".icon").fadeTo(300,1);
			$("#dock").fadeTo(300,1);
			$(".slide").slideUp(300);
			$(".pack>.icon").fadeOut(500);
			$(this).children(".iconName").fadeIn(500);
			}

		); 
		
		$(".task").toggle(function(){		
			$(".iconSet").animate({"margin-top":-90},200);	
			$("#dock").fadeTo(100,0.2);	
			$(".icon").fadeTo(100,0.2);
		},function(){		
		    $(".iconSet").animate({"margin-top":0},200);	
			$("#dock").fadeTo(100,1);
			$(".icon").fadeTo(100,1);
			});
		

		
})	





 /*!***********************Jquery_dialogue *****************************
 * jQuery JavaScript Library
 * Copyright 2011, Zhang Ziyang
 */
$(document).ready(function(){
	$("#black,#screen").addClass("zoomIn");
		var w = screen.width/2-143;
		var h = screen.height/2-99;			   
						   
	$(".dialogue").click(function(){
		$("#black").fadeTo(100,0.7,showLgoin);
	}); 
	
	$(".cancel").click(function(){
	    $("#black").fadeTo(100,1);
		$(".floatBox").fadeOut(100);	
		}); 
	
	function showLgoin(){                          //Jquery_ShowBoX_Animation
			$(".floatBox").css({"left":w,"top":h})		
		                  .fadeIn(100)
						  }
	$(".editBtn").toggle(function(){
			$(".editList").fadeIn(300);
	},function(){
			$(".editList").fadeOut(300);
	}); 

}); 

 /*!***********************Jquery_PhotoZoom *****************************
 * jQuery JavaScript Library
 * Copyright 2011, Zhang Ziyang
 */

$(document).ready(function(){
						
$("img.image").click(function(){
							  
							  var src = $(this).attr("src");		
							  var img = $("<img/>")
							  
							  .attr({ 
										src: src,
										title: "image",
										alt: "image"
									  })
							  .addClass("amplifier");
							  
							  $("#backList").show();
							  $("#album").hide();	
							  $("#black").append(img);

						   })
$("#backList").click(function(){
							 $(".amplifier").remove();
							 $("#album").show();
							 $("#backList").hide();
							  
							    
					})
 })
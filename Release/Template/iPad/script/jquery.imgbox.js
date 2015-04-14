/*
 * imgBox - jQuery Plugin
 * Yet another lightbox alternative
 *
 * Copyright (c) 2009 jQueryGlobe
 * Examples and documentation at: http://jqueryglobe.com/article/imgbox/
 * 
 * Version: 1.0.0 (21/10/2009)
 * Requires: jQuery v1.3+
 * 
 * Dual licensed under the MIT and GPL licenses:
 *   http://www.opensource.org/licenses/mit-license.php
 *   http://www.gnu.org/licenses/gpl.html
 */

(function($) {
	$.fn.fixPNG = function() {
		return this.each(function () {
			var image = $(this).css('backgroundImage');

			if (image.match(/^url\(["']?(.*\.png)["']?\)$/i)) {
				image = RegExp.$1;
				$(this).css({
					'backgroundImage': 'none',
					'filter': "progid:DXImageTransform.Microsoft.AlphaImageLoader(enabled=true, sizingMethod=" + ($(this).css('backgroundRepeat') == 'no-repeat' ? 'crop' : 'scale') + ", src='" + image + "')"
				}).each(function () {
					var position = $(this).css('position');
					if (position != 'absolute' && position != 'relative')
						$(this).css('position', 'relative');
				});
			}
		});
	};

	var elem, opts, preloader, orig_pos, final_pos, busy = false, nr, zindex = 90, titleh = 0, shadow = 20, margin = 20, fx = $.extend($('<div/>')[0], { prop: 0 });

	$.fn.imgbox = function(settings) {
		return this.unbind('click.pb').bind('click.pb', function() {
			$.imgbox( $(this), settings );
			return false;
		});
	};

	$.imgbox = function(e, o) {
		if (busy) {
			return false;
		}

		elem	= e;
		opts	= $.extend({}, $.fn.imgbox.defaults, o);
		nr		= jQuery.data(elem[0]);

		if ($('#imgbox-wrap-' + nr).length) {
		    zoomOut();
			return false;
		}

		hideActivity();

		if (opts.overlayShow) {
			$('#imgbox-overlay')
				.unbind().stop().hide()
				.css({
					'height'	: $(document).height(),
					'opacity'	: opts.overlayOpacity
				})
				.show();
		}

		preloader = new Image;
		preloader.src = $(elem).attr('href');

		if (preloader.complete == false) {
			showActivity();

			$(preloader).unbind().one('load', function() {
				hideActivity();
				zoomIn();
			});

		} else {
			zoomIn();
		}
	};

	$.fn.imgbox.defaults = {
		padding				:	10,
		alignment			:	'auto', // auto OR center
		allowMultiple		:	true,
		autoScale			:	true,
		speedIn				:	500,
		speedOut			:	500,
		easingIn			:	'swing',
		easingOut			:	'swing',
		zoomOpacity			:	true,
		overlayShow			:	true,
		overlayOpacity		:	0.5,
		hideOnOverlayClick	:	true,
		hideOnContentClick	:	true
	};

	function zoomIn() {
		busy = true;

		if (opts.allowMultiple == false) {
			$('.imgbox-wrap').remove();
			$('.imgbox-bg-wrap').remove();

		} else {
			zindex = zindex + 2;
		}

		final_pos = getZoomTo();

		var title = $(elem).attr('title') || '';

		$('<div id="imgbox-wrap-' + nr + '" class="imgbox-wrap"></div>')
			.css({
				'z-index'	:	zindex,
				'padding'	:	opts.padding
			})
			.append('<img class="imgbox-img" id="imgbox-img-' + nr + '" src="' + preloader.src + '" alt="' + title + '" />')
			.appendTo('body');

		$('<div id="imgbox-bg-' + nr + '" class="imgbox-bg-wrap"><div class="imgbox-bg imgbox-bg-n"/><div class="imgbox-bg imgbox-bg-ne"/><div class="imgbox-bg imgbox-bg-e"/><div class="imgbox-bg imgbox-bg-se"/><div class="imgbox-bg imgbox-bg-s"/><div class="imgbox-bg imgbox-bg-sw"/><div class="imgbox-bg imgbox-bg-w"/><div class="imgbox-bg imgbox-bg-nw"/></div>').appendTo('body');

		if ($.browser.msie && parseInt($.browser.version.substr(0, 1)) < 7) {
			$('#imgbox-bg-' + nr).find('.imgbox-bg').fixPNG();
		}

		titleh = 0;

		if (title.length > 0) {
			$('<div id="imgbox-tmp" class="imgbox-title" />').html(title).css('width', final_pos.width).appendTo('body');

			titleh = $('#imgbox-tmp').outerHeight();

			final_pos.height	+= titleh;
			final_pos.top		-= titleh > margin + shadow ? margin : margin * 0.5;

			$('#imgbox-tmp').remove();
			$('#imgbox-wrap-' + nr).append('<div class="imgbox-title">' + title + '</div>');
		}

		if (opts.speedIn > 0) {
			var pos	= getThumbPos();

			orig_pos = {
				top		:	pos.top		- opts.padding,
				left	:	pos.left	- opts.padding,
				width	:	pos.width,
				height	:	pos.height
			};

			$('#imgbox-wrap-' + nr).css(orig_pos).show();
			$('#imgbox-bg-' + nr).css({
				top			: orig_pos.top,
				left		: orig_pos.left,
				width		: orig_pos.width	+ (opts.padding * 2),
				height		: orig_pos.height	+ (opts.padding * 2),
				'z-index'	: zindex - 1
			}).show();

			if (opts.zoomOpacity) {
				final_pos.opacity = 1;
			}

		  	fx.prop = 0;

			$(fx).animate({ prop: 1 }, {
				 duration	: opts.speedIn,
				 easing		: opts.easingIn,
				 step		: draw,
				 complete	: _finish
			});

		} else {
			$('#imgbox-img-' + nr ).css('height', (final_pos.height - titleh) + 'px');
			$('#imgbox-wrap-' + nr).css(final_pos).fadeIn('normal',  _finish );
	
			$('#imgbox-bg-' + nr).css({
				top			: final_pos.top,
				left		: final_pos.left,
				width		: final_pos.width	+ (opts.padding * 2),
				height		: final_pos.height	+ (opts.padding * 2),
				'z-index'	: zindex - 1
			}).fadeIn('normal');
		}
	};

	function draw(pos) {
		var width	= Math.round(orig_pos.width		+ (final_pos.width	- orig_pos.width)	* pos);
		var height	= Math.round(orig_pos.height	+ (final_pos.height	- orig_pos.height)	* pos);

		var top		= Math.round(orig_pos.top	+ (final_pos.top	- orig_pos.top)		* pos);
		var left	= Math.round(orig_pos.left	+ (final_pos.left	- orig_pos.left)	* pos);

		$('#imgbox-wrap-' + nr).css({
			'width'		: width		+ 'px',
			'height'	: height	+ 'px',
			'top'		: top		+ 'px',
			'left'		: left		+ 'px'
		});

		$('#imgbox-bg-' + nr).css({
			'width'		: Math.round(width	+ opts.padding * 2 ) + 'px',
			'height'	: Math.round(height	+ opts.padding * 2 ) + 'px',
			'top'		: top	+ 'px',
			'left'		: left	+ 'px'
		});

		$('#imgbox-img-' + nr ).css('height',  Math.round( height - ( ( ((height - Math.min(orig_pos.height, final_pos.height)) * 100) / (Math.max(orig_pos.height - final_pos.height, final_pos.height - orig_pos.height) ) * titleh / 100))) + 'px');

		if (typeof final_pos.opacity !== 'undefined') {
			var opacity = pos < 0.3 ? 0.3 : pos;

			$('#imgbox-wrap-' + nr).css('opacity', opacity);

			if ($.browser.msie == false) {
				$('#imgbox-bg-' + nr).css('opacity', opacity);
			}
		}
	};

	function _finish() {
		if (opts.overlayShow && opts.hideOnOverlayClick) {
			$('#imgbox-overlay').bind('click', {elem: elem, nr : nr, opts : opts, titleh : titleh}, clickHandler);
		}

		$('#imgbox-wrap-' + nr)
			.css('filter', '')
			.bind('click', {elem: elem, nr : nr, opts : opts, titleh : titleh}, clickHandler)
			.append('<a href="javascript:;" class="imgbox-close"></a>')
			.children('.imgbox-title')
			.show();

		if ($.browser.msie && parseInt($.browser.version.substr(0, 1)) < 7) {
			$('#imgbox-wrap-' + nr).find('.imgbox-close').fixPNG();
		}

		busy = false;
	};

	function clickHandler(e) {
		e.stopPropagation();

		if (e.target.className == 'imgbox-close' || (e.data.opts.hideOnOverlayClick && e.target.id == 'imgbox-overlay') || (e.data.opts.hideOnContentClick && e.target.className == 'imgbox-img' && ($(this).css('z-index') == zindex || $('.imgbox-img').length == 1))) {
			elem	= e.data.elem;
			nr		= e.data.nr;
			opts	= e.data.opts;
			titleh	= e.data.titleh;
			zoomOut();

		} else if ($(this).css('z-index') < zindex) {
			$(this).next('.imgbox-bg-wrap').css('z-index', ++zindex);
			$(this).css('z-index', ++zindex);
		}
	};

	function zoomOut() {
		if (busy) {
			return false;
		}

		busy = true;

		$('#imgbox-wrap-' + nr)
			.children('.imgbox-close, .imgbox-title')
			.remove();

		if (opts.speedOut > 0) {
			var pos	= getThumbPos();

			orig_pos = {
				top		:	pos.top		- opts.padding,
				left	:	pos.left	- opts.padding,
				width	:	pos.width,
				height	:	pos.height
			};

			var pos = $('#imgbox-wrap-' + nr).position();

			final_pos = {
				top		:	pos.top ,
				left	:	pos.left,
				width	:	$('#imgbox-wrap-' + nr).width(),
				height	:	$('#imgbox-wrap-' + nr).height()
			};

			if (opts.zoomOpacity) {
				final_pos.opacity = 0;
			}

			setTimeout(function() {
				$('#imgbox-wrap-' + nr).css('z-index', 90);
				$('#imgbox-bg-' + nr).css('z-index', 90);
		    }, opts.speedOut * 0.5);

		    fx.prop = 1;

		    $(fx).animate({ prop: 0 }, {
				 duration	: opts.speedIn,
				 easing		: opts.easingIn,
				 step		: draw,
				 complete	: _clean_up
			});

		} else {

			if (opts.overlayShow) {
				_clean_up();

			} else {
				$('#imgbox-bg-' + nr).fadeOut('fast');
				$('#imgbox-wrap-' + nr).fadeOut('fast', _clean_up );
			}
		}
	};

	function _clean_up() {
		$('#imgbox-bg-' + nr).stop().remove();
		$('#imgbox-wrap-' + nr).remove();

        zindex = zindex > 90 ? zindex - 2 : 90;

		if (opts.overlayShow) {
			$('#imgbox-overlay').unbind().stop().fadeOut(200);
		}

		busy = false;	
	};

	function getZoomTo() {
		var wiew	= getViewport();
		var to		= {width : preloader.width, height : preloader.height};

		var horizontal_space	= (opts.padding + shadow + margin ) * 2;
		var vertical_space		= (opts.padding + shadow + margin ) * 2;

		if (opts.autoScale && (to.width > (wiew[0] - horizontal_space) || to.height > (wiew[1] - vertical_space))) {
			var ratio = Math.min(Math.min( wiew[0] - horizontal_space, to.width) / to.width, Math.min( wiew[1] - vertical_space, to.height) / to.height);

			to.width	= Math.round(ratio * to.width);
			to.height	= Math.round(ratio * to.height);
		}

		if (opts.alignment == 'center') {
			to.top	= wiew[3] + ((wiew[1] - to.height - opts.padding * 2) * 0.5);
			to.left	= wiew[2] + ((wiew[0] - to.width  - opts.padding * 2) * 0.5);

		} else {
			var pos	= getThumbPos();

			to.top	= pos.top  - ( ( to.height - pos.height  ) * 0.5) - opts.padding ;
			to.left	= pos.left - ( ( to.width  - pos.width   ) * 0.5) - opts.padding ;

			to.top	= to.top 	> wiew[3] + margin + shadow ? to.top	:  wiew[3] + margin + shadow; 
			to.left	= to.left 	> wiew[2] + margin + shadow ? to.left	:  wiew[2] + margin + shadow;

			to.top	= to.top	> wiew[1] + wiew[3] - ( to.height	+ vertical_space )	? wiew[1] + wiew[3] - ( to.height	+ ( margin	+ shadow + opts.padding * 2 ) ) : to.top;
			to.left	= to.left	> wiew[0] + wiew[2] - ( to.width	+ horizontal_space )	? wiew[0] + wiew[2] - ( to.width	+ ( margin  + shadow + opts.padding * 2 ) ) : to.left;
		}

		if ( opts.autoScale == false ) {
			to.top	= to.top 	> wiew[3] + shadow + margin ? to.top	:  wiew[3] + shadow + margin;
			to.left	= to.left 	> wiew[2] + shadow + margin ? to.left	:  wiew[2] + shadow + margin;
		}

		to.top 	= parseInt(to.top);
		to.left	= parseInt(to.left);

		return to;
	};

	function getViewport() {
		return [ $(window).width(), $(window).height(), $(document).scrollLeft(), $(document).scrollTop() ];
	};

	function getThumbPos() {
		var thumb	= $(elem).find('img').eq(0);
		var pos		= thumb.offset();

		pos.top		+= parseFloat( thumb.css('paddingTop') );
		pos.left	+= parseFloat( thumb.css('paddingLeft') );

		pos.top		+= parseFloat( thumb.css('border-top-width') );
		pos.left	+= parseFloat( thumb.css('border-left-width') );

		pos.width	= thumb.width();
		pos.height	= thumb.height();

		return pos;
	};

	function showActivity() {
		var pos	= getThumbPos( elem );

		$('#imgbox-loading').css(pos).show();
	};

	function hideActivity() {
		$(preloader).unbind();
		$('#imgbox-loading').hide();
	};

	function cancelLoading() {
	    hideActivity();

	    if (opts.overlayShow) {
			$('#imgbox-overlay').unbind().stop().fadeOut(200);
		}
	};

	function init() {
		$('<div id="imgbox-loading"><div></div></div><div id="imgbox-overlay"></div>').appendTo('body');

		$('#imgbox-loading')
			.click(cancelLoading)
			.find('div')
			.css('opacity', 0.4);
	};

	$(document).ready(function() {
		init();
	});

})(jQuery);
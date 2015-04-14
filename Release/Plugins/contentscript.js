var supportedMimeTypes = ['video/mp4', 'video/x-ms-wmv'];
var supportedVideoExtensions = ['.mp4', '.wmv', '.mp4v', '.m4v'];

var getSupportedMediaSource = function(videoElement)
{
    // If the video element source is supported, then we replace the tag.

    if (videoElement.src
        && isWMPSupported(videoElement))
    {
        return videoElement.src;
    }

    // "If all src videos are [supported], the video tag is replaced otherwise
    // it is not. Basically if there is at least one video that WMP is not able
    // to play we should not replace the Video tag."

    var sources = videoElement.getElementsByTagName("source");
    var supportedSource = null;

    for (var i=0; i<sources.length; i++)
    {
        if (sources[i].src)
        {
            if (isWMPSupported(sources[i]))
            {
                supportedSource = sources[i].src;
            }
            else
            {
                return null;
            }
        }
    }

    return supportedSource;
}

var isWMPSupported = function(videoOrSourceElement)
{
    if (videoOrSourceElement.type)
    {
        var type = videoOrSourceElement.type.toLowerCase();
        var index = type.indexOf(';');

        if (index != -1)
            type = type.slice(0, index);


        for (var i=0; i<supportedMimeTypes.length; i++)
        {
            if (supportedMimeTypes[i] == type)
                return true;
        }
    }
    else if (videoOrSourceElement.src)
    {
        var src = videoOrSourceElement.src.toLowerCase();
        var lastIndex = src.lastIndexOf(".");

        if (lastIndex != -1)
            src = src.slice(lastIndex);


        for (var i=0; i<supportedVideoExtensions.length; i++)
        {
            if (supportedVideoExtensions[i] == src)
                return true;
        }
    }

    return false;
}

var createControlFromVideo = function(videoElement)
{
    var supportedMediaSource = getSupportedMediaSource(videoElement);

    if (!supportedMediaSource)
    {
        return null;
    }
    else
    {
    }

    var control = document.createElement("object");
    control.type = "application/x-ms-wmp";

    // assign height/width
    var width = videoElement.width;
    if (width <= 0) 
    {
        // try to use clientWidth if the video element doesn't have explicit width
        if (videoElement.clientWidth > 0)
        {
            width = videoElement.clientWidth;

            // adjust for padding
            if (videoElement.style.paddingRight != "")
            {
                width -= parseInt(videoElement.style.paddingRight);
            }
            if (videoElement.style.paddingLeft != "")
            {
                width -= parseInt(videoElement.style.paddingLeft);
            }
        }
        else
        {
            // default to 320 if we can't get width or clientWidth
            width = 320;
        }

        width += 'px';
    }
    control.width = width;

    var height = videoElement.height;
    if (height <= 0) 
    {
        // try to use clientHeight if the video element doesn't have explicit height
        if (videoElement.clientHeight > 0)
        {
            height = videoElement.clientHeight;

            // adjust for padding
            if (videoElement.style.paddingTop != "")
            {
                height -= parseInt(videoElement.style.paddingTop);
            }
            if (videoElement.style.paddingBottom != "")
            {
                height -= parseInt(videoElement.style.paddingBottom);
            }
        }
        else
        {
            // default to 240 if we can't get height or clientHeight
            height = 240;
        }

        height += 'px';
    }
    control.height = height;

    // following standard attributes need to be assigned only when they are present in the video tag

    if (videoElement.id != "") { control.id = videoElement.id; }
    if (videoElement.dir != "") { control.dir = videoElement.dir; }
    if (videoElement.class != "") { control.class = videoElement.class; }
    if (videoElement.title != "") { control.title = videoElement.title; }
    if (videoElement.draggable) { control.draggable = true; }
    if (videoElement.lang != "") { control.lang = videoElement.lang; }
    if (videoElement.spellcheck) { control.spellcheck = true; }
    if (videoElement.style.cssText != "") { control.style.cssText = videoElement.style.cssText; }

    // controls attribute - boolean for video tag. 
    // for WMP, uiMode => "full" shows controls, "none" shows only video window
    var controls = (videoElement.controls == true) ? "full" : "none";
    control.setAttribute("uiMode", controls);

    var autostart = videoElement.autoplay;  // boolean - maps to object.autostart property
    var paramAutoStart = document.createElement("param");
    paramAutoStart.name = "autostart";
    paramAutoStart.value = autostart;
    control.appendChild(paramAutoStart);

    // OPEN: Should we always set autostart when "controls" are hidden ?

    var paramUrl = document.createElement("param");
    paramUrl.name = "url";
    paramUrl.value = supportedMediaSource;
    control.appendChild(paramUrl);

    return control;
}

var processVideoElements = function()
{
    var videoElements = document.getElementsByTagName("video");

    for (var i=0; i<videoElements.length; i++)
    {
        videoElement = videoElements[i];
        videoElement.removeEventListener("DOMSubtreeModified", processVideoElements, true);

        var objectElement = createControlFromVideo(videoElement);

        if (objectElement)
        {
            videoElement.parentNode.insertBefore(objectElement, videoElement);
            videoElement.setAttribute("class", "__wmpff_replaced");
        }
        else
        {
            videoElement.addEventListener("DOMSubtreeModified", processVideoElements, true);
        }
    }

    while (replacedVideoElement = document.querySelector(".__wmpff_replaced"))
    {
        replacedVideoElement.parentNode.removeChild(replacedVideoElement);
    }
}

var supportsH264BaselineProfile = function() 
{
    return document.createElement("video").canPlayType('video/mp4; codecs="avc1.42E01E"');
}

window.addEventListener("load", function() 
{
    if (!supportsH264BaselineProfile()) 
    {
        processVideoElements();
    }
}, false);

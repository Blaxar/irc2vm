/* Enable mouse interaction on an overlay div to control the remote virtual machine.
   elem: DOM div element.
   server_uri: uri to send mouse events to (WebSocket).
*/
var readyClient = function(elem, server_uri)
{

	var ctx = this;
	
	ctx.uri = server_uri
	ctx.allowed = true;

	ctx.mouse_lbtn_state = 0;
	ctx.mouse_mbtn_state = 0;
	ctx.mouse_rbtn_state = 0;
	ctx.mouse_x = 0;
	ctx.mouse_y = 0;
	ctx.display_width = 0;
	ctx.display_height = 0;
	ctx.sendReady = true;
	ctx.last_move = 0;
	ctx.ws = new WebSocket(ctx.uri);
    ctx.ws.onopen = function() {};
    ctx.ws.onmessage = function (evt) {};
	
	ctx.sendHttpRequest = function()
	{

		value = "arg=" + ctx.mouse_x + "," + ctx.mouse_y + ","
			+ ctx.mouse_lbtn_state + ","
		    + ctx.mouse_mbtn_state + ","
		    + ctx.mouse_rbtn_state;

		ctx.ws.send(value);
	
	}

	ctx.timeoutWrapper = function()
	{
		ctx.sendHttpRequest();
		setTimeout(ctx.timeoutWrapper, 100);
	};
	
	elem.onmousedown = function(event)
	{
		console.log("mousedown " + event.button);
		if(event.button == 0) ctx.mouse_lbtn_state = 1;
		if(event.button == 1) ctx.mouse_mbtn_state = 1;
		if(event.button == 2) ctx.mouse_rbtn_state = 1;

		ctx.sendHttpRequest();
	};

	
	elem.onmouseup = function(event)
	{
		console.log("mouseup " + event.button);
		if(event.button == 0) ctx.mouse_lbtn_state = 0;
		if(event.button == 1) ctx.mouse_mbtn_state = 0;
		if(event.button == 2) ctx.mouse_rbtn_state = 0;

		ctx.sendHttpRequest();
	};

    
	elem.onmouseover = function(event)
	{
		console.log("mouseover");
	};

	
	elem.onmouseout = function(event)
	{
		console.log("mouseout");
	};

	
	elem.onmousemove = function(event)
	{
		
		var rect = elem.getBoundingClientRect();
		ctx.mouse_x = (event.clientX-rect.left)/parseFloat(rect.width);
		ctx.mouse_y = (event.clientY-rect.top)/parseFloat(rect.height);
		console.log("mousemove " + ctx.mouse_x + "," + ctx.mouse_y);

		if((Date.now()-ctx.last_move) > 10) //set a minimum 10ms interval between mousemove requests.
		{
			ctx.sendHttpRequest();
			ctx.last_move = Date.now();
		}
		
	};

	elem.addEventListener("keydown", function (evt) {
		evt.preventDefault();
		evt.stopPropagation();
		if (evt.repeat != undefined) {
			ctx.allowed = !evt.repeat;
		}
		if (!ctx.allowed) return;
		ctx.allowed = false;
		if (evt.key !== undefined) {console.log("down " + evt.key); ctx.ws.send("kd="+evt.key);}
	}, false);

	elem.addEventListener("keyup", function (evt) {
		evt.preventDefault();
		evt.stopPropagation();
		ctx.allowed = true;
		if (evt.key !== undefined) {console.log("up " + evt.key); ctx.ws.send("ku="+evt.key);}
	}, false);

	elem.focus(function(e) { 
		ctx.allowed = true;
	});
	
};

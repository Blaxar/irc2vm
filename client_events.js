/* Enable mouse interaction on an overlay div to control the remote virtual machine.
   elem: DOM div element.
   server_uri: uri to send mouse events to (GET requests).
*/
var readyClient = function(elem, server_uri)
{

	var ctx = this;

	ctx.uri = server_uri

	ctx.mouse_lbtn_state = 0;
	ctx.mouse_mbtn_state = 0;
	ctx.mouse_rbtn_state = 0;
	ctx.mouse_x = 0;
	ctx.mouse_y = 0;
	ctx.display_width = 0;
	ctx.display_height = 0;
	ctx.sendReady = true;
	ctx.last_move = 0;

	ctx.sendHttpRequest = function()
	{

		value = "" + ctx.mouse_x + "," + ctx.mouse_y + ","
			+ ctx.mouse_lbtn_state + ","
		    + ctx.mouse_mbtn_state + ","
		    + ctx.mouse_rbtn_state;

		var script = document.getElementById("jsonp");
		if(script) script.remove();
		
		script = document.createElement('script');
		script.setAttribute('id', "jsonp");
		script.setAttribute('type', "application/javascript");
		script.setAttribute('src', ctx.uri+"?arg="+value);

		document.getElementsByTagName('head')[0].appendChild(script);
	
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

		if((Date.now()-ctx.last_move) > 50) //set a minimum 50ms interval between mousemove requests.
		{
			ctx.sendHttpRequest();
			ctx.last_move = Date.now();
		}
		
	};
	
};

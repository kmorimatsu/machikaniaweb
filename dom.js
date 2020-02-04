/************************************
* MachiKania web written by Katsumi *
*      This script is released      *
*        under the LGPL v2.1.       *
************************************/

window.onkeydown=function(e){
 	keyboard.keydown(e.keyCode);
	if (get.debug) return true;
	else return false;
};
window.onkeyup=function(e){
 	keyboard.keyup(e.keyCode);
	if (get.debug) return true;
	else return false;
};

dom=Object();
dom.getElement=function(id){
	var ret=document.getElementById(id);
	return ret;
};
dom.getContext=function(id){
	var ret=document.getElementById(id).getContext("2d");
	return ret;
};
dom.x=function(obj,e){
	var x;
	if (typeof e.pageX != "undefined") { 
	  x = e.pageX;
	} else {
	  x = e.clientX + document.body.scrollLeft + document.documentElement.scrollLeft; 
	}
	x -= obj.offsetLeft;
	return x;
};
dom.y=function(obj,e){
	var y;
	if (typeof e.pageY != "undefined") { 
	  y = e.pageY;
	} else {
	  y = e.clientY + document.body.scrollTop + document.documentElement.scrollTop; 
	}
	y -= obj.offsetTop;
	return y;
};
dom.logtext="";
dom.log=function(text){
	try{
		console.log(text);
	}catch(e){
		this.logtext+=text+"\n";
	}
};
dom.close=function(){
	try{
		console.log("close");
	}catch(e){
		try{
			alert(this.logtext);
		}catch(e){
			try{
				WScript.Echo(this.logtext);
			}catch(e){
				
			}
		}
	}
};
dom.blankDisplay=function(){
	// Show blank canvas
	document.getElementById("blank").style.display="block";
};
dom.showDisplay=function(){
	// Hide blank canvas.
	document.getElementById("blank").style.display="none";
};
dom.useWorker=false;
dom.showSpeed=function(speed){
	document.getElementById("speed").innerHTML="clock: "+speed
		+(this.useWorker ? " hz (Web Workers)" : " hz");
};
dom.showDebug=function(){
	var classes=document.getElementsByClassName("debug");
	for(i=0;i<classes.length;i++){
		classes[i].style.display="block";
	}
};
dom.debug=function(str){
	document.getElementById("debug").innerHTML=str;
};
dom.displaylog=function(str){
	document.getElementById("displaylog").innerHTML+=str;
};
dom.dump=function(str){
	document.getElementById("dump").innerHTML=str;
};
dom.clickStep=function(){
	clickStep();
};
dom.clickCont=function(){
	clickCont();
};
dom.clickStopAt=function(){
	clickStopAt(prompt('Break at (hex):'));
};
dom.clickLogTo=function(){
	clickLogTo(prompt('Log to (hex):'));
};
dom.clickDump=function(){
	clickDump(prompt('Dump at (hex):'));
};


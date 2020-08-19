/************************************
* MachiKania web written by Katsumi *
*      This script is released      *
*        under the LGPL v2.1.       *
************************************/

/*
	Public methods:
	display.init(FontData);
	display.write(addr,data);
*/
/*
	Buttons assignments:
		D0  SW_DOWN
		D1  SW_LEFT
		D2  SW_UP
		D3  SW_RIGHT
		D4  SW_START
		D5  SW_FIRE
*/

button=new Object();
button.init=function(){
	// All buttons up, first
	SFR.PORTDSET(0x3f);
};
button.kanalock=function(obj){
	if (!obj.style.background) {
		obj.style.background="#A0A0A0";
		keyboard.kanaclick(1);
	} else {
		obj.style.background=null;
		keyboard.kanaclick(0);
	}
};
button.break=function(obj){
	system.write8(system.pPs2keystatus+3,1);
	setTimeout(function(){ system.write8(system.pPs2keystatus+3,0); },100);
	setTimeout(function(){ system.write8(system.pPs2keystatus+3,0); },200);
};
button.start=function(down){
	if (down) SFR.PORTDCLR(0x10);
	else SFR.PORTDSET(0x10);
};
button.fire=function(down){
	if (down) SFR.PORTDCLR(0x20);
	else SFR.PORTDSET(0x20);
};
button.left=function(down){
	if (down) SFR.PORTDCLR(0x02);
	else SFR.PORTDSET(0x02);
};
button.up=function(down){
	if (down) SFR.PORTDCLR(0x04);
	else SFR.PORTDSET(0x04);
};
button.down=function(down){
	if (down) SFR.PORTDCLR(0x01);
	else SFR.PORTDSET(0x01);
};
button.right=function(down){
	if (down) SFR.PORTDCLR(0x08);
	else SFR.PORTDSET(0x08);
};
button.key4button=function(obj){
	this.key4button.checked=obj.checked;
};
button.keyboard=function(code,down){
	if (!this.key4button.checked) return;
	switch(code){
		case 83:
			this.start(down);
			break;
		case 70:
			this.fire(down);
			break;
		case 37:
			this.left(down);
			break;
		case 38:
			this.up(down);
			break;
		case 40:
			this.down(down);
			break;
		case 39:
			this.right(down);
			break;
	}
};

button.init();
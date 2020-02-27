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

button.init();
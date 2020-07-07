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

const VMODE_T30=0; // 標準テキスト30文字互換モード
const VMODE_STDTEXT=1; // 標準テキスト36文字モード
const VMODE_T40=2; // 標準テキスト40文字互換モード（6ドットフォント）
const VMODE_WIDETEXT=3; // ワイドテキスト48文字モード
const VMODE_WIDETEXT6dot=4; // ワイドテキスト64文字モード（6ドットフォント）
const VMODE_MONOTEXT=5; // モノクロテキスト80文字モード
const VMODE_ZOEAGRPH=16; // type Z互換グラフィックモード
const VMODE_STDGRPH=17; // 標準グラフィック＋テキスト36文字モード
const VMODE_WIDEGRPH=18; // ワイドグラフィック＋テキスト48文字モード

display=new Object();
display.fonts=new Image();
display.font=new Array(256);
display.font2=new Array(256);
display.prevview=new Array();
display.palette=new Array(256);
display.bgcolor=[0,0,0];
display.width=36;
display.wide=0;
display.vmode=0;
display.pFontData=0;
display.pFontData2=0;
display.context=null;
display.fontcontext=null;
display.init=function(FontData,FontData2){
	var i;
	// Set the contexts.
	this.fontcontext=dom.getContext("font");
	this.context=dom.getContext("display");
	this.cls();
	// Canvas imageData is created when needed in display.all()
	this.pFontData=FontData;
	this.pFontData2=FontData2;
	// Create default palette
	for(i=0;i<256;i++){
		this.palette[i]=[255,255,255];
	}
};
display.updateFont=function(ascii,pFontData,font,palette,width){
	var context=this.fontcontext;
	var wide=this.wide ? 2:1;
	var fd,x,y;
	var rgb="rgb("+this.palette[palette][0]+","+this.palette[palette][1]+","+this.palette[palette][2]+")";
	var bgrgb="rgb("+this.bgcolor[0]+","+this.bgcolor[1]+","+this.bgcolor[2]+")";
	if (this.width==80) {
		rgb="rgb(255,255,255)";
		bgrgb="rgb(0,0,0)";
	}
	for(y=0;y<8;y++){
		fd=system.read8(pFontData+ascii*8+y);
		for(x=0;x<8;x++){
			context.fillStyle=(fd & (0x80>>x)) ? rgb:bgrgb;
			context.fillRect(x*wide,y*wide,wide,wide);
		}
	}
	if (!font[palette]) font[palette]=Array(256);
	font[palette][ascii]=context.getImageData(0,0,width*wide,8*wide);
};
display.cls=function(){
	var wide=this.wide ? 2:1;
	this.context.fillStyle   = "rgb(64, 64, 64)";
	this.context.fillRect(0,0,480*wide,224*wide);
};
display.zoeagrph=function(){
	var x,y,address,data,palette;
	var wide=this.wide ? 2:1;
	address=this.gvram;
	for(y=0;y<224;y++){
		for(x=0;x<256;x++){
			data=system.RAM[address++];
			palette=this.palette[(data>>12)&15];
			this.context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
			this.context.fillRect(x*wide,y*wide,wide,wide);
			x++;
			palette=this.palette[(data>>8)&15];
			this.context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
			this.context.fillRect(x*wide,y*wide,wide,wide);
			x++;
			palette=this.palette[(data>>4)&15];
			this.context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
			this.context.fillRect(x*wide,y*wide,wide,wide);
			x++;
			palette=this.palette[(data>>0)&15];
			this.context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
			this.context.fillRect(x*wide,y*wide,wide,wide);
			x++;
			palette=this.palette[(data>>28)&15];
			this.context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
			this.context.fillRect(x*wide,y*wide,wide,wide);
			x++;
			palette=this.palette[(data>>24)&15];
			this.context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
			this.context.fillRect(x*wide,y*wide,wide,wide);
			x++;
			palette=this.palette[(data>>20)&15];
			this.context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
			this.context.fillRect(x*wide,y*wide,wide,wide);
			x++;
			palette=this.palette[(data>>16)&15];
			this.context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
			this.context.fillRect(x*wide,y*wide,wide,wide);
		}
	}
};
display.stdgrph=function(){
	alert('graphic '+this.vmode);
};
display.widegrph=function(){
	alert('graphic '+this.vmode);
};
display.all=function(){
	var data,palette,pdata,posy,posx,addr,ascii;
	var wide=this.wide ? 1:0;
	var Fontp=system.read32(system.pFontp);
	if (!Fontp) return;
	if (Fontp!=this.Fontp){
		// Font changed. Discard cached font images.
		this.Fontp=Fontp;
		this.font=Array(256);
		this.font2=Array(256);
	}
	switch(this.vmode){
		case VMODE_ZOEAGRPH:
			this.zoeagrph();
			break;
		case VMODE_STDGRPH:
			this.stdgrph();
			break;
		case VMODE_WIDEGRPH:
			this.widegrph();
			break;
		default: // Not graphic mode
			break;
	}
	switch(this.width){
		case 0: // Disabled (Zoea compatible graphic mode)
			break;
		case 40: case 64: case 80: // 6 dots width
			for (posy=0;posy<27;posy++) {
				for (posx=0;posx<this.width;posx++) {
					addr=posy*this.width+posx;
					ascii=system.readRAM8(system.pTVRAM+addr);
					palette=system.readRAM8(system.pTVRAM+this.width*27+addr);
					if (!this.font2[palette]) {
						this.updateFont(ascii,Fontp,this.font2,palette,6);
					} else if (!this.font2[palette][ascii]) {
						this.updateFont(ascii,Fontp,this.font2,palette,6);
					}
					this.context.putImageData(this.font2[palette][ascii],posx*6<<wide,posy<<(3+wide));
				}
			}
			break;
		default: // 8 dots width
			for (posy=0;posy<27;posy++) {
				for (posx=0;posx<this.width;posx++) {
					addr=posy*this.width+posx;
					ascii=system.readRAM8(system.pTVRAM+addr);
					palette=system.readRAM8(system.pTVRAM+this.width*27+addr);
					if (!this.font[palette]) {
						//this.updateFont(ascii,this.pFontData,this.font,palette);
						this.updateFont(ascii,Fontp,this.font,palette,8);
					} else if (!this.font[palette][ascii]) {
						//this.updateFont(ascii,this.pFontData,this.font,palette);
						this.updateFont(ascii,Fontp,this.font,palette,8);
					}
					this.context.putImageData(this.font[palette][ascii],posx<<(3+wide),posy<<(3+wide));
				}
			}
			break;
	}
};
display.show=function(msec){
	setTimeout(function(){
			display.all();
			setTimeout(arguments.callee,msec);
		},msec);
};
display.set_videomode=function(mode,gvram){
	this.vmode=mode;
	this.gvram=(gvram&0x00ffffff)>>2;
	switch(mode){
		case VMODE_T30: // 標準テキスト30文字互換モード
			this.width=30;
			break;
		case VMODE_STDTEXT: // 標準テキスト36文字モード
			this.width=36;
			break;
		case VMODE_T40: // 標準テキスト40文字互換モード（6ドットフォント）
			this.width=40;
			break;
		case VMODE_WIDETEXT: // ワイドテキスト48文字モード
			this.width=48;
			break;
		case VMODE_WIDETEXT6dot: // ワイドテキスト64文字モード（6ドットフォント）
			this.width=64;
			break;
		case VMODE_MONOTEXT: // モノクロテキスト80文字モード
			this.width=80;
			break;
		case VMODE_ZOEAGRPH: // type Z互換グラフィックモード
			this.width=0;
			break;
		case VMODE_STDGRPH: // 標準グラフィック＋テキスト36文字モード
			this.width=36;
			break;
		case VMODE_WIDEGRPH: // ワイドグラフィック＋テキスト48文字モード
			this.width=48;
			break;
		default:
			system.exception("Wrong video mode: "+m);
			return 0;
	}
	// Clear screen
	this.cls();
	// Clear font image
	this.font=Array(256);
	this.font2=Array(256);
};
display.set_palette=function(n,b,r,g){
	// Update palette array
	this.palette[n]=[r,g,b];
	// Clear font image
	this.font[n]=Array(256);
	this.font2[n]=Array(256);
};
display.set_bgcolor=function(b,r,g){
	// Update bgcolor
	this.bgcolor=[r,g,b];
	// Clear font image
	this.font=Array(256);
	this.font2=Array(256);
};

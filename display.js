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
display.palette=new Array(256);
display.bgcolor=[0,0,0];
display.width=36;
display.wide=0;
display.vmode=0;
display.gcache=new Array();
display.pFontData=0;
display.pFontData2=0;
display.context=null;
display.gcanvas=null;
display.gcontext=null;
display.init=function(FontData,FontData2){
	var i;
	var wide=this.wide ? 2:1;
	// Set the contexts.
	this.context=dom.getContext("display");
	this.gcanvas=document.createElement('canvas');
	this.gcanvas.width=384*wide;
	this.gcanvas.height=216*wide;
	this.gcontext=this.gcanvas.getContext('2d');
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
	if (!font[palette]) {
		font[palette]=Array(256);
	} else if (font[palette][ascii]) {
		return;
	}
	var fd,x,y,rgb,bgrgb;
	var wide=this.wide ? 2:1;
	var canvas=document.createElement('canvas');
	canvas.width=wide*width;
	canvas.height=wide*8;
	var context=canvas.getContext('2d');
	switch(this.vmode){
		case VMODE_STDGRPH:
		case VMODE_WIDEGRPH:
			// Use both text and graphic
			rgb="rgb("+this.palette[palette][0]+","+this.palette[palette][1]+","+this.palette[palette][2]+")";
			bgrgb="rgb(0,0,0,0.0)";
			break;
		case VMODE_MONOTEXT:
			// Black/white 80 width text
			rgb="rgb(255,255,255)";
			bgrgb="rgb(0,0,0)";
			break;
		default:
			// Use only text
			rgb="rgb("+this.palette[palette][0]+","+this.palette[palette][1]+","+this.palette[palette][2]+")";
			bgrgb="rgb("+this.bgcolor[0]+","+this.bgcolor[1]+","+this.bgcolor[2]+")";
			break;
	}
	for(y=0;y<8;y++){
		fd=system.read8(pFontData+ascii*8+y);
		for(x=0;x<width;x++){
			context.fillStyle=(fd & (0x80>>x)) ? rgb:bgrgb;
			context.fillRect(x*wide,y*wide,wide,wide);
		}
	}
	font[palette][ascii]=canvas;
};
display.cls=function(){
	var wide=this.wide ? 2:1;
	this.context.fillStyle   = "rgb(64, 64, 64)";
	this.context.fillRect(0,0,480*wide,224*wide);
	this.gcontext.fillStyle   = "rgb(64, 64, 64)";
	this.gcontext.fillRect(0,0,384*wide,216*wide);
};
display.zoeagrph=function(){
	var x,y,address,data,palette;
	var wide=this.wide ? 2:1;
	var context=this.context;
	address=this.gvram;
	for(y=0;y<224;y++){
		for(x=0;x<256;x++){
			data=system.RAM[address];
			if (this.gcache[address]===data) {
				address++;
				x+=7;
			} else {
				this.gcache[address++]=data;
				palette=this.palette[(data>>12)&15];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>8)&15];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>4)&15];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>0)&15];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>28)&15];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>24)&15];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>20)&15];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>16)&15];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
			}
		}
	}
};
display.stdgrph=function(){
	var x,y,address,data,palette;
	var wide=this.wide ? 2:1;
	var context=this.gcontext;
	address=this.gvram;
	for(y=0;y<216;y++){
		for(x=0;x<288;x++){
			data=system.RAM[address];
			if (this.gcache[address]===data) {
				address++;
				x+=3;
			} else {
				this.gcache[address++]=data;
				palette=this.palette[(data>>0)&255];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>8)&255];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>16)&255];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>24)&255];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
			}
		}
	}
};
display.widegrph=function(){
	var x,y,address,data,palette;
	var wide=this.wide ? 2:1;
	var context=this.gcontext;
	address=this.gvram;
	for(y=0;y<216;y++){
		for(x=0;x<384;x++){
			data=system.RAM[address];
			if (this.gcache[address]===data) {
				address++;
				x+=3;
			} else {
				this.gcache[address++]=data;
				palette=this.palette[(data>>0)&255];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>8)&255];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>16)&255];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
				x++;
				palette=this.palette[(data>>24)&255];
				context.fillStyle='rgb('+palette[0]+','+palette[1]+','+palette[2]+')';
				context.fillRect(x*wide,y*wide,wide,wide);
			}
		}
	}
};
display.all=function(){
	var adata,pdata,aaddr,paddr,ascii,palette,posy,posx;
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
			return; // Only graphic for Zoea compatible mode
		case VMODE_STDGRPH:
			this.stdgrph();
			this.context.drawImage(this.gcanvas,0,0);
			break;
		case VMODE_WIDEGRPH:
			this.widegrph();
			this.context.drawImage(this.gcanvas,0,0);
			break;
		default: // Not graphic mode
			break;
	}
	aaddr=system.pTVRAM & 0x00ffffff;
	paddr=(system.pTVRAM+this.width*27) & 0x00ffffff;
	switch(this.width){
		case 40: case 64: case 80: // 6 dots width
			for (posy=0;posy<27;posy++) {
				for (posx=0;posx<this.width;posx++) {
					if (0==(aaddr&3)) {
						adata=system.RAM[(aaddr++)>>2];
					} else {
						aaddr++;
						adata>>=8;
					}
					if (0==(paddr&3)) {
						pdata=system.RAM[(paddr++)>>2];
					} else {
						paddr++;
						pdata>>=8;
					}
					ascii=adata&255;
					palette=pdata&255;
					this.updateFont(ascii,Fontp,this.font2,palette,6);
					this.context.drawImage(this.font2[palette][ascii],posx*6<<wide,posy<<(3+wide));
 				}
			}
			break;
		case 30: // 8 dots width
			pdata=system.RAM[(paddr-2)>>2]>>8;
		default: // 8 dots width
			for (posy=0;posy<27;posy++) {
				for (posx=0;posx<this.width;posx++) {
					if (0==(aaddr&3)) {
						adata=system.RAM[(aaddr++)>>2];
					} else {
						aaddr++;
						adata>>=8;
					}
					if (0==(paddr&3)) {
						pdata=system.RAM[(paddr++)>>2];
					} else {
						paddr++;
						pdata>>=8;
					}
					ascii=adata&255;
					palette=pdata&255;
					this.updateFont(ascii,Fontp,this.font,palette,8);
					this.context.drawImage(this.font[palette][ascii],posx<<(3+wide),posy<<(3+wide));
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
	// Clear cache
	this.gcache=new Array();
};
display.set_palette=function(n,b,r,g){
	// Update palette array
	this.palette[n]=[r,g,b];
	// Clear font image
	this.font[n]=Array(256);
	this.font2[n]=Array(256);
	// Clear cache
	this.gcache=new Array();
};
display.set_bgcolor=function(b,r,g){
	// Update bgcolor
	this.bgcolor=[r,g,b];
	// Clear font image
	this.font=Array(256);
	this.font2=Array(256);
};

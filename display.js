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
	Support following displays:
		var VMODE_T30=0; // �W���e�L�X�g30�����݊����[�h
		var VMODE_STDTEXT=1; // �W���e�L�X�g36�������[�h
		var VMODE_T40=2; // �W���e�L�X�g40�����݊����[�h�i6�h�b�g�t�H���g�j
		var VMODE_WIDETEXT=3; // ���C�h�e�L�X�g48�������[�h
		var VMODE_WIDETEXT6dot=4; // ���C�h�e�L�X�g64�������[�h�i6�h�b�g�t�H���g�j
		var VMODE_MONOTEXT=5; // ���m�N���e�L�X�g80�������[�h
		
		���m�N��80�������[�h�ł��U�h�b�g�t�H���g���g�����Ƃɂ���ƁA�ő啝��480�h�b�g�B
		
		
	Graphics are not yet supported:
		var VMODE_ZOEAGRPH=16; // type Z�݊��O���t�B�b�N���[�h
		var VMODE_STDGRPH=17; // �W���O���t�B�b�N�{�e�L�X�g36�������[�h
		var VMODE_WIDEGRPH=18; // ���C�h�O���t�B�b�N�{�e�L�X�g48�������[�h
		
*/

display=new Object();
display.fonts=new Image();
display.font=new Array(256);
display.font2=new Array(256);
display.prevview=new Array();
display.palette=new Array(256);
display.bgcolor=[0,0,0];
display.width=36;
display.wide=0;
display.pFontData=0;
display.pFontData2=0;
display.context=null;
display.fontcontext=null;
display.init=function(FontData,FontData2){
	var i;
	var wide=this.wide+1;
	// Set the contexts.
	this.fontcontext=dom.getContext("font");
	this.context=dom.getContext("display");
	this.context.fillStyle   = "rgb(64, 64, 64)";
	this.context.fillRect(0,0,480*wide,216*wide);
	this.context.fillStyle   = "rgb(255, 255, 255)";
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
	var wide=this.wide+1;
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
display.all=function(){
	var data,palette,pdata,posy,posx,addr,ascii;
	var wide=this.wide;
	var Fontp=system.read32(system.pFontp);
	if (!Fontp) return;
	if (Fontp!=this.Fontp){
		// Font changed. Discard cached font images.
		this.Fontp=Fontp;
		this.font=Array(256);
		this.font2=Array(256);
	}
	switch(this.width){
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
//void set_videomode(unsigned char m, unsigned char *gvram); //�r�f�I���[�h�̐؂�ւ�
	var VMODE_T30=0; // �W���e�L�X�g30�����݊����[�h
	var VMODE_STDTEXT=1; // �W���e�L�X�g36�������[�h
	var VMODE_T40=2; // �W���e�L�X�g40�����݊����[�h�i6�h�b�g�t�H���g�j
	var VMODE_WIDETEXT=3; // ���C�h�e�L�X�g48�������[�h
	var VMODE_WIDETEXT6dot=4; // ���C�h�e�L�X�g64�������[�h�i6�h�b�g�t�H���g�j
	var VMODE_MONOTEXT=5; // ���m�N���e�L�X�g80�������[�h
	var VMODE_ZOEAGRPH=16; // type Z�݊��O���t�B�b�N���[�h
	var VMODE_STDGRPH=17; // �W���O���t�B�b�N�{�e�L�X�g36�������[�h
	var VMODE_WIDEGRPH=18; // ���C�h�O���t�B�b�N�{�e�L�X�g48�������[�h
	if (gvram) this.gvram=gvram;
	switch(mode){
		case VMODE_T30: // �W���e�L�X�g30�����݊����[�h
			this.width=30;
			break;
		case VMODE_STDTEXT: // �W���e�L�X�g36�������[�h
			this.width=36;
			break;
		case VMODE_T40: // �W���e�L�X�g40�����݊����[�h�i6�h�b�g�t�H���g�j
			this.width=40;
			break;
		case VMODE_WIDETEXT: // ���C�h�e�L�X�g48�������[�h
			this.width=48;
			break;
		case VMODE_WIDETEXT6dot: // ���C�h�e�L�X�g64�������[�h�i6�h�b�g�t�H���g�j
			this.width=64;
			break;
		case VMODE_MONOTEXT: // ���m�N���e�L�X�g80�������[�h
			this.width=80;
			break;
		case VMODE_ZOEAGRPH: // type Z�݊��O���t�B�b�N���[�h
			alert('VMODE_ZOEAGRPH');
			break;
		case VMODE_STDGRPH: // �W���O���t�B�b�N�{�e�L�X�g36�������[�h
			alert('VMODE_STDGRPH');
			break;
		case VMODE_WIDEGRPH: // ���C�h�O���t�B�b�N�{�e�L�X�g48�������[�h
			alert('VMODE_WIDEGRPH');
			break;
		default:
			system.exception("Wrong video mode: "+m);
			return 0;
	}
	// Clear screen
	this.context.fillStyle   = "rgb(64, 64, 64)";
	this.context.fillRect(0,0,480*(this.wide+1),216*(this.wide+1));
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

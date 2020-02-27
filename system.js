/****************************************
*  PIC32MX emulator written by Katsumi  *
*        This script is released        *
*          under the LGPL v2.1.         *
****************************************/

PROGRAM_FLASH_BASE_ADDRESS  =0x9D000000;
PROGRAM_FLASH_END_ADDRESS   =0x9D07FFFF;
APP_FLASH_BASE_ADDRESS      =0x9D006000;
APP_FLASH_END_ADDRESS       =0x9D07FFFF;
USER_APP_RESET_ADDRESS      =0x9D007000;
RAM_BASE_ADDRESS            =0xA0000000;
RAM_END_ADDRESS             =0xA001FFFF;
SFR_BASE_ADDRESS            =0xBF800000;
SFR_END_ADDRESS             =0xBF8FFFFF;
BOOT_BASE_ADDRESS           =0xBFC00000;
BOOT_END_ADDRESS            =0xBFC00BEF;
CONFIG_BASE_ADDRESS         =0xBFC02FF0;
CONFIG_END_ADDRESS          =0xBFC02FFF;
P_RAM_BASE_ADDRESS          =0x00000000;
P_RAM_END_ADDRESS           =0x0001FFFF;
P_PROGRAM_FLASH_BASE_ADDRESS=0x1D000000;
P_PROGRAM_FLASH_END_ADDRESS =0x1D07FFFF;
P_SFR_BASE_ADDRESS          =0x1F800000;
P_SFR_END_ADDRESS           =0x1F8FFFFF;
P_BOOT_BASE_ADDRESS         =0x1FC00000;
P_BOOT_END_ADDRESS          =0x1FC00BEF;
P_CONFIG_BASE_ADDRESS       =0x1FC02FF0;
P_CONFIG_END_ADDRESS        =0x1FC02FFF;

system=new Object();
// Memory areas
system.RAM=new Array();
system.FLASH=new Array();
system.BOOT=new Array();
system.CONFIG=new Array();
// Pointers to PIC32 variables/functions
system.pTVRAM=0;
system.pFontData=0;
system.reset=function(hexname){
	var i,res;
	mips32.reset(USER_APP_RESET_ADDRESS);
	for(i=0;i<=((RAM_END_ADDRESS-RAM_BASE_ADDRESS)>>2);i++){
		this.RAM[i]=0;
	}
	hexname+="\x00\x00\x00\x00\x00";
	res=new Array();
	for(i=0;i<hexname.length;i+=4){
		res[i>>2]=hexname.charCodeAt(i);
		res[i>>2]+=hexname.charCodeAt(i+1)<<8;
		res[i>>2]+=hexname.charCodeAt(i+2)<<16;
		res[i>>2]+=hexname.charCodeAt(i+3)<<24;
	}
	this.loadFlash(0x1D07EFF0,res);
};
system.loadFlash=function(address,data){
	var i;
	if (P_PROGRAM_FLASH_BASE_ADDRESS<=address && address<=P_PROGRAM_FLASH_END_ADDRESS) {
		// Program flash area
		address-=P_PROGRAM_FLASH_BASE_ADDRESS;
		address>>=2;
		for(i=0;i<data.length;i++){
			this.FLASH[address+i]=data[i];
		}
	} else if (P_BOOT_BASE_ADDRESS<=address && address<=P_BOOT_END_ADDRESS) {
		// Boot flash area
		address-=P_BOOT_BASE_ADDRESS;
		address>>=2;
		for(i=0;i<data.length;i++){
			this.BOOT[address+i]=data[i];
		}
	} else if (P_CONFIG_BASE_ADDRESS<=address && address<=P_CONFIG_END_ADDRESS) {
		// Config area
		address-=P_CONFIG_BASE_ADDRESS;
		address>>=2;
		for(i=0;i<data.length;i++){
			this.CONFIG[address+i]=data[i];
		}
	} else {
		this.exception("Writing to wrong flash address: 0x"+address.toString(16));
	}
};
system.html5functions=function(address,type){
/*
	#define HTML5FUNC_ps2readkey "0"
	#define HTML5FUNC_FindFirst  "1"
	#define HTML5FUNC_FindNext   "2"
	#define HTML5FUNC_FSmkdir    "3"
	#define HTML5FUNC_FSgetcwd   "4"
	#define HTML5FUNC_FSchdir    "5"
	#define HTML5FUNC_FSfwrite   "6"
	#define HTML5FUNC_FSremove   "7"
	#define HTML5FUNC_FSrename   "8"
	#define HTML5FUNC_FSfeof     "9"
	#define HTML5FUNC_FSftell    "10"
	#define HTML5FUNC_FSfseek    "11"
	#define HTML5FUNC_FSfread    "12"
	#define HTML5FUNC_FSrewind   "13"
	#define HTML5FUNC_FSfclose   "14"
	#define HTML5FUNC_FSfopen    "15"
	#define HTML5FUNC_FSInit     "16"
	#define HTML5FUNC_shiftkeys  "17"
	#define HTML5FUNC_set_palette   "256"
	#define HTML5FUNC_set_bgcolor   "257"
	#define HTML5FUNC_set_videomode "258"
*/
	if (address!=mips32.pc) return;
	var v0;
	var v1=mips32.GPR(3);
	var ra=mips32.GPR(31);
	var a0=parseInt(mips32.GPR(4));
	var a1=parseInt(mips32.GPR(5));
	var a2=parseInt(mips32.GPR(6));
	var a3=parseInt(mips32.GPR(7));
	if (a0<0) a0+=0x100000000;
	if (a1<0) a1+=0x100000000;
	if (a2<0) a2+=0x100000000;
	if (a3<0) a3+=0x100000000;
	switch(type){
		case 0: // ps2readkey
			v0=keyboard.ps2readkey(a0,a1,a2,a3);
			break;
		case 1: // FindFirst  
			v0=filesystem.FindFirst(a0,a1,a2,a3);
			break;
		case 2: // FindNext   
			v0=filesystem.FindNext(a0,a1,a2,a3);
			break;
		case 3: // FSmkdir    
			v0=filesystem.FSmkdir(a0,a1,a2,a3);
			break;
		case 4: // FSgetcwd   
			v0=filesystem.FSgetcwd(a0,a1,a2,a3);
			break;
		case 5: // FSchdir    
			v0=filesystem.FSchdir(a0,a1,a2,a3);
			break;
		case 6: // FSfwrite   
			v0=filesystem.FSfwrite(a0,a1,a2,a3);
			break;
		case 7: // FSremove   
			v0=filesystem.FSremove(a0,a1,a2,a3);
			break;
		case 8: // FSrename   
			v0=filesystem.FSrename(a0,a1,a2,a3);
			break;
		case 9: // FSfeof     
			v0=filesystem.FSfeof(a0,a1,a2,a3);
			break;
		case 10: // FSftell    
			v0=filesystem.FSftell(a0,a1,a2,a3);
			break;
		case 11: // FSfseek    
			v0=filesystem.FSfseek(a0,a1,a2,a3);
			break;
		case 12: // FSfread    
			v0=filesystem.FSfread(a0,a1,a2,a3);
			break;
		case 13: // FSrewind   
			v0=filesystem.FSrewind(a0,a1,a2,a3);
			break;
		case 14: // FSfclose   
			v0=filesystem.FSfclose(a0,a1,a2,a3);
			break;
		case 15: // FSfopen    
			v0=filesystem.FSfopen(a0,a1,a2,a3);
			break;
		case 16: // FSInit     
			v0=filesystem.FSInit(a0,a1,a2,a3);
			break;
		case 17: // shiftkeys
			v0=keyboard.shiftkeys(a0,a1,a2,a3);
			break;
		case 256: // set_palette
			// TODO: implement here
			//alert('set_palette');
			break;
		case 257: // set_bgcolor
			// TODO: implement here
			//alert('set_bgcolor');
			break;
		case 258: // set_videomode
			// TODO: implement here
			v0=display.set_videomode(a0,a1);
			break;
		default:
			this.exception("Wrong HTML5 request: "+type);
			break;
	}
	// Set $v0 return value
	v0=parseInt(v0);
	mips32.GPR.set(2,v0);
	// Swap $ra and $v1 to continue to function in C
	mips32.GPR.set(31,v1);
	mips32.GPR.set(3,ra);
};
system.unsigned32=function(num){
	return (num<0) ? (num+0x100000000):num;
};
system.read32=function(address){
	address=parseInt(address);
	if (address<0) address+=0x100000000;
	if ((address&3)!=0) {
		this.exception("Address Error (read32)");
	}
	if (PROGRAM_FLASH_BASE_ADDRESS<=address && address<=PROGRAM_FLASH_END_ADDRESS) {
		if (0x9D006080==address) this.html5functions(address,mips32.GPR(2));
		return this.unsigned32(this.FLASH[(address-PROGRAM_FLASH_BASE_ADDRESS)>>2]);
	}
	if (RAM_BASE_ADDRESS<=address && address<=RAM_END_ADDRESS) {
		return  this.unsigned32(this.RAM[(address-RAM_BASE_ADDRESS)>>2]);
	}
	if (SFR_BASE_ADDRESS<=address && address<=SFR_END_ADDRESS) {
		var sfr=SFR[address];
		return  this.unsigned32(sfr());
	}
	if (BOOT_BASE_ADDRESS<=address && address<=BOOT_END_ADDRESS) {
		return  this.unsigned32(this.BOOT[(address-BOOT_BASE_ADDRESS)>>2]);
	}
	if (CONFIG_BASE_ADDRESS<=address && address<=CONFIG_END_ADDRESS) {
		this.log("Read from config address: 0x"+address.toString(16));
		return 0;
	}
	this.exception("Memory access (reading) error: 0x"+address.toString(16));
};
system.write32=function(address,data){
	address=parseInt(address);
	var i;
	if (address<0) address+=0x100000000;
	if ((address&3)!=0) {
		this.exception("Address Error (write32)");
	}
	if (RAM_BASE_ADDRESS<=address && address<=RAM_END_ADDRESS) {
		this.RAM[(address-RAM_BASE_ADDRESS)>>2]=data;
		return;
	}
	if (SFR_BASE_ADDRESS<=address && address<=SFR_END_ADDRESS) {
		var sfr=SFR[address];
		sfr(data);
		//var re = new RegExp('SFR\.(.*?)\.value','g');
		//re.exec(''+sfr);
		//dom.log(RegExp.$1);
		return;
	}
	if (BOOT_BASE_ADDRESS<=address && address<=BOOT_END_ADDRESS) {
		this.log("Write to boot address: 0x"+address.toString(16));
		return;
	}
	this.exception("Memory access (writing) error: 0x"+address.toString(16));
};
system.read8=function(address){
	var data=this.read32(address&0xFFFFFFFC);
	switch(address&3){
		// Little endian
		case 0:
			return data&0xff
		case 1:
			return (data>>8)&0xff
		case 2:
			return (data>>16)&0xff
		case 3:
		default:
			return (data>>24)&0xff
	}
};
system.write8=function(address,data){
	var read=this.read32(address&0xFFFFFFFC);
	switch(address&3){
		// Little endian
		case 0:
			data=(data&0xff) | (read&0xFFFFFF00);
			break;
		case 1:
			data=((data&0xff)<<8) | (read&0xFFFF00FF);
			break;
		case 2:
			data=((data&0xff)<<16) | (read&0xFF00FFFF);
			break;
		case 3:
		default:
			data=((data&0xff)<<24) | (read&0x00FFFFFF);
			break;
	}
	this.write32(address&0xFFFFFFFC,data);
};
system.read16=function(address){
	if ((address&1)!=0) {
		this.exception("Address Error (read16)");
	}
	var data=this.read32(address&0xFFFFFFFC);
	switch(address&2){
		// Little endian
		case 0:
			return data&0xffff
		case 2:
		default:
			return (data>>16)&0xffff
	}
};
system.write16=function(address,data){
	if ((address&1)!=0) {
		this.exception("Address Error (write16)");
	}
	var read=this.read32(address&0xFFFFFFFC);
	switch(address&2){
		// Little endian
		case 0:
			data=(data&0xffff) | (read&0xFFFF0000);
			break;
		case 2:
		default:
			data=((data&0xffff)<<16) | (read&0x0000FFFF);
			break;
	}
	this.write32(address&0xFFFFFFFC,data);
};
system.log=function(text){
	text+=" at PC:0x"+(mips32.pc-4).toString(16);
	try{
		console.log(text);
	}catch(e){
		try{
			alert(text);
		}catch(e){}
	}
};
system.waitFlag=0;
system.wait=function(){
	this.waitFlag=1;
};
system.exceptionFlag=0;
system.exception=function(text){
	this.log(text);
	this.exceptionFlag=1;
	this.waitFlag=1;
};
// Initialization routine.
system.init=function(){
	// Initialize pointers for used by HTML5
	this.pTVRAM        = this.read32(0x9d006000);
	this.pFontData     = this.read32(0x9d006004);
	this.pHtml5data    = this.read32(0x9d006008);
	this.pPs2keystatus = this.read32(0x9d00600C); // volatile unsigned char ps2keystatus[256];
	this.pVkey         = this.read32(0x9d006010); // volatile unsigned short vkey;
	this.pGFileArray0  = this.read32(0x9d006014); // FSFILE gFileArray[]
	this.pGFileArray1  = this.read32(0x9d006018); // FSFILE gFileArray[]
	// Initialize timers
	this.initTimer(timer1,1,'IFS0<4>');
	this.initTimer(timer2,2,'IFS0<9>');
	this.initTimer(timer3,3);
	this.initTimer(timer4,4);
	this.initTimer(timer5,5);
	coretimer.interrupt=function(){
		SFR.IFS0SET(0x01);
	};
	/* Initialize interrupt
	       vector  IF       IE      Priority    Subprioriy
		CS0  1   IFS0<1>  IEC0<1>  IPC0<12:10> IPC0<9:8>
		CS1  2   IFS0<2>  IEC0<2>  IPC0<20:18> IPC0<17:16>
		T1   4   IFS0<4>  IEC0<4>  IPC1<4:2>   IPC1<1:0>
		T2   8   IFS0<9>  IEC0<9>  IPC2<4:2>   IPC2<1:0>
		T3   12  IFS0<14> IEC0<14> IPC3<4:2>   IPC3<1:0>
		T4   16  IFS0<19> IEC0<19> IPC4<4:2>   IPC4<1:0>
		T5   20  IFS0<24> IEC0<24> IPC5<4:2>   IPC5<1:0>
	*/
	new interrupt(0,'IFS0<0>','IEC0<0>','IPC0<4:2>',  'IPC0<1:0>'  ); // CT
	new interrupt(1,'IFS0<1>','IEC0<1>','IPC0<12:10>','IPC0<9:8>'  ); // CS0
	new interrupt(2,'IFS0<2>','IEC0<2>','IPC0<20:18>','IPC0<17:16>'); // CS1
	new interrupt(4,'IFS0<4>','IEC0<4>','IPC1<4:2>',  'IPC1<1:0>'  ); // T1
	new interrupt(8,'IFS0<9>','IEC0<9>','IPC2<4:2>',  'IPC2<1:0>'  ); // T2
};
// Initialization of timers
system.initTimer=function(timerobj,type,interrupt){
	var con=SFR['T'+type+'CON'];
	SFR['T'+type+'CON']=function(num){
		con(num);
		timerobj.init(num);
	};
	var pr=SFR['PR'+type];
	SFR['PR'+type]=function(num){
		pr(num);
		timerobj.PR=num&0xFFFF;
	};
	var tmr=SFR['TMR'+type];
	SFR['TMR'+type]=function(num){
		tmr(num);
		timerobj.TMR=num&0xFFFF;
	};
	var re=new RegExp("^([A-Z0-9]+)<.*([0-9]+)>$");
	if (interrupt) {
		re.exec(interrupt);
		var set=SFR[RegExp.$1+'SET'];
		var bit=RegExp.$2;
		timerobj.interrupt=function(){
			set(1<<bit);
		};
	}
};

/************************************
* MachiKania web written by Katsumi *
*      This script is released      *
*        under the LGPL v2.1.       *
************************************/

// Initialize system
hexfile.load();
system.init();
mmc.setCard();
display.init(system.pFontData,system.pFontData2);
display.all();
if (get.debug=='hex') {
	system.reset('DUMMY.HEX');
} else {
	system.reset('MACHIKAM.HEX');
}
// Show display every 40 msec (25 frames/sec)
display.show(40);

main=function(maxspeed,breakpoint){
	system.exceptionFlag=0;
	main.active=1;
	// Preare for MIPS32
	maxspeed=parseInt(maxspeed);
	breakpoint=parseInt(breakpoint);
	var speed=1;
	var time=new Date().getTime();
	var lastint=time;
	setTimeout(function(){
			var i;
			var from=time;
			var to=time=new Date().getTime();
			var msec=to-from;
			if (msec<32 && speed<maxspeed) {
				speed<<=1;
				if (maxspeed<speed) speed=maxspeed;
			} else if (50<msec) {
				speed>>=1;
				if (speed<1) speed=1;
			}
			// Show the current speed
			/*if (msec<32) {
				dom.getElement('speed').innerHTML='clock: '+parseInt(speed*1000/16)+' hz';
			} else {
				dom.getElement('speed').innerHTML='clock: '+parseInt(speed*(1000/msec))+' hz';
			}//*/
			// Execute MIPS32 1/64 times
			// This corresponds to something like using NTSC video in MachiKania
			// CPU will work with 1.5 MIPS. This is about 20 times slower than Real MachiKania type M
			// The core timer will work with 95.4 MHz, instead.
			var exectimes=speed>>6;
			for(i=0;i<exectimes;i++){
				mips32.exec();
				if (system.waitFlag || mips32.pc==breakpoint) break;
			}
			system.waitFlag=0;
			// Increment core timer for remaining time
			mips32.incTimer(speed*16/msec-i);
			// Things to do every 16 msec
			// Interrupts: always for T2 (vector 9) and CS0 (vector 1)
			SFR.IFS0SET((1<<9)+(1<<1));
			interrupt.check();
			// Check sound
			audio.play();
			// Check halt state
			if (mips32.checkHalt()||system.exceptionFlag||mips32.pc==breakpoint) {
				mips32.cp0.DI();
				mips32.logreg();
				dom.getElement("debug").style.display="block";
			} else {
				msec=32-msec;
				if (msec<5) msec=5;
				if (main.active) setTimeout(arguments.callee,msec);
			}
		},5);
};
steprun=function(codenum){
	system.exceptionFlag=0;
	system.waitFlag=0;
	var i;
	for(i=0;i<codenum;i++){
		mips32.exec();
		if (system.waitFlag) {
			dom.log('wait');
			break;
		}
		var t='PC: 0x';
		t+=(0>mips32.pc ? mips32.pc+0x100000000 : mips32.pc).toString(16);
		t+=' (0x';
		t+=system.read32(mips32.pc,"nonexec").toString(16);
		t+=')';
		dom.log(t);
		mips32.logreg();
	}
	display.all();
};
breakat=function(breakpoint){
	main(95454.533*16,breakpoint);// 95.4545 MHz
};
setpc=function(address){
	mips32.pc=parseInt(address);
};
dump=function(address){
	var i,j,data,text;
	address=parseInt(address);
	for(i=0;i<16;i++){
		text=address.toString(16)+' : ';
		for(j=0;j<4;j++){
			data=system.read32(address);
			while(data<0x100000000) data+=0x100000000;
			address+=4;
			text+=data.toString(16).substr(-8)+' ';
		}
		dom.log(text);
	}
}

if (get.debug) {
	dom.getElement("debug").style.display="block";
} else {
	main(95454.533*16,get.breakpoint);// 95.4545 MHz
}

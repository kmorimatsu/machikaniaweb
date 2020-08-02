/****************************************
*  PIC32MX emulator written by Katsumi  *
*        This script is released        *
*          under the LGPL v2.1.         *
****************************************/

/*

	Interrupt vectors:
		0x9d006220 vector 1
		0x9d006240 vector 2
		0x9d006280 vector 4
		etc

*/

// Constructor
interrupt=function(vector,flag,enable,priority,subpriority){
	// vector:      integer, 0-42
	// flag:        string, "IFS0<0>" etc
	// enable:      string, "IEC0<0>" etc
	// priority:    string, "IPC0<4:2>" etc
	// subpriority: string, "IPC0<1:0>" etc
	// Example: (1,'IFS0<1>','IEC0<1>','IPC0<12:10>','IPC0<9:8>'  ); // CS0
	var re=new RegExp("^([A-Z0-9]+)<.*?([0-9]+)>$");
	this.vector=vector;
	this.vectorAddress=this.vectorBaseAddress+this.vectorStep*vector;
	re.exec(flag);
	var f=SFR[RegExp.$1];
	var f2=1<<RegExp.$2;
	this.IF=function(){
		return (f()&f2) ? 1:0;
	};
	re.exec(enable);
	var e=SFR[RegExp.$1];
	var e2=1<<RegExp.$2;
	this.IE=function(){
		return (e()&e2) ? 1:0;
	};
	re.exec(priority);
	var p=SFR[RegExp.$1];
	var p2=RegExp.$2;
	re.exec(subpriority);
	var s=SFR[RegExp.$1];
	var s2=RegExp.$2;
	this.priority=function(){
		return (((p()>>p2)&7)<<2) | ((s()>>s2)&3);
	};
	interrupt.handlers.push(this);
};
// Prototype of interrupt
interrupt.prototype=interrupt;
// Parameters of each interrupt
interrupt.vectorBaseAddress=0x9d006200;
interrupt.vectorStep=0x20;
interrupt.vector=0;
interrupt.vectorAddress=0;
interrupt.priority=0;
interrupt.IE=null;
interrupt.IF=null;
interrupt.priority=null;
// Array containing all interrupt settings
interrupt.handlers=new Array();
// Check interrupt routine
interrupt.check=function(){
	var i;
	for (i=0;i<this.handlers.length;i++){
		if (this.handlers[i].IE() && this.handlers[i].IF()) {
			// Execute interrupt here.
			mips32.requestInterrupt(this.handlers[i].vectorAddress,this.handlers[i].priority()>>2);
		}
	}
};

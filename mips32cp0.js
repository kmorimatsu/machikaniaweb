/****************************************
* MIPS32 4k emulator written by Katsumi *
*        This script is released        *
*          under the LGPL v2.1.         *
****************************************/

mips32.cp0=new Object();
mips32.cp0.HWREna=0;           // Not implemented (same as non-commented parameters below).
mips32.cp0.BadVAddr=0;         // Reports the address for the most recent address-related exception.
mips32.cp0.Count=0;            // Processor cycle count.
mips32.cp0.Compare=0;          // Core timer interrupt control.
mips32.cp0.Status=0x00500007;  // Processor status and control.
mips32.cp0.IntCtl=0;           // Interrupt control of vector spacing.
mips32.cp0.SRSCtl=0x00000000;  // Shadow register set control. HSS=0b0000.
                               // One shadow set (only normal GPR set) is present.
mips32.cp0.SRSMap=0;
mips32.cp0.Cause=0;            // Describes the cause of the last exception.
mips32.cp0.EPC=0;              // Program counter at last exception.
mips32.cp0.PRID=0;
mips32.cp0.Ebase=0x80000000;   // Exception base address of exception vectors.
mips32.cp0.Config =0xA4210582; // Configuration register.
mips32.cp0.Config1=0x80000006; // Configuration register 1.
mips32.cp0.Config2=0x80000000; // Configuration register 2.
mips32.cp0.Config3=0x00000160; // Configuration register 3.
mips32.cp0.Debug=0;
mips32.cp0.TraceControl=0;
mips32.cp0.TraceControl2=0;
mips32.cp0.UseTraceData=0;
mips32.cp0.TraceBPC=0;
mips32.cp0.Debug2=0;
mips32.cp0.DEPC=0;
mips32.cp0.ErrorEPC=0;
mips32.cp0.DeSAVE=0;
mips32.cp0.log=function(text){
	text+=" at around PC:0x"+(mips32.pc-4).toString(16);
	try{
		console.log(text);
	}catch(e){
		mips32.log(text);
	}
};
mips32.cp0.MTC0=function(rd,mnl,rtvalue){
	var sel=mnl & 7;
	switch(rd) {
		case 8:  // BadVAddr (read only)
			return;
		case 9:  // Count
			return this.Count=rtvalue;
		case 11: // Compare
			return this.Compare=rtvalue;
		case 12:
			switch(sel){
				case 0: // Status
					rtvalue&=0x1A581C17;
					return this.Status=rtvalue;
				case 1: // IntCtl
					return this.IntCtl=rtvalue;
				case 2: // SRSCtl
					rtvalue&=0xF3C0;
					rtvalue+=this.SRSCtl&0x3C3C000F;
					return this.SRSCtl=rtvalue;
				default:
					break;
			}
			break;
		case 13: // Cause
			rtvalue&=0x08800300;
			rtvalue+=this.Cause&0xF0001C3C;
			return this.Cause=rtvalue;
		case 14: // EPC
			return this.EPC=rtvalue;
		case 15:
			switch(sel){
				case 1: // Ebase
					return this.Ebase=rtvalue;
				default:
					break;
			}
			break;
		case 16:
			switch(sel){
				case 0: // Config
					return this.Config=rtvalue;
				case 1: // Config1
					return this.Config1=rtvalue;
				case 2: // Config2
					return this.Config2=rtvalue;
				case 3: // Config3
					return this.Config3=rtvalue;
				default:
					break;
			}
			break;
		case 24: // DEPC
			return this.DEPC=rtvalue;
		case 31: // DeSAVE
			return this.DeSAVE=rtvalue;
		default:
			break;
	}
	this.log('MTC0: unknown CP0: '+rd+','+sel);
};
mips32.cp0.MFC0=function(rd,mnl){
	var sel=mnl & 7;
	switch(rd) {
		case 8:  // BadVAddr
			return this.BadVAddr;
		case 9:  // Count
			return this.Count;
		case 11: // Compare
			return this.Compare;
		case 12:
			switch(sel){
				case 0: // Status
					return this.Status;
				case 1: // IntCtl
					return this.IntCtl;
				case 2: // SRSCtl
					return this.SRSCtl;
				default:
					break;
			}
			break;
		case 13: // Cause
			return this.Cause;
		case 14: // EPC
			return this.EPC;
		case 15:
			switch(sel){
				case 1: // Ebase
					return this.Ebase;
				default:
					break;
			}
			break;
		case 16:
			switch(sel){
				case 0: // Config
					return this.Config;
				case 1: // Config1
					return this.Config1;
				case 2: // Config2
					return this.Config2;
				case 3: // Config3
					return this.Config3;
				default:
					break;
			}
			break;
		case 24: // DEPC
			return this.DEPC;
		case 31: // DeSAVE
			return this.DeSAVE;
		default:
			break;
	}
	this.log('MTF0: unknown CP0: '+rd+' '+sel);
};
mips32.cp0.DI=function(){
	this.Status &=0x1FFFFFFE;
};
mips32.cp0.EI=function(){
	this.Status &=0x1FFFFFFE;
	this.Status++;
};
mips32.cp0.IE=function(){
	// Interrupts are enabled when all of the following conditions are true:
	//   IE (bit 0) = 1
	//   EXL(bit 1) = 0
	//   ERL(bit 2) = 0
	var res=((this.Status & 7)==1) ? 1:0;
	// Delay rising IE flag
	var previous=this.previous;
	this.previous=res;
	return res && previous
};
mips32.cp0.IE.previous=0;

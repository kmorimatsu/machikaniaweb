/****************************************
* MIPS32 4k emulator written by Katsumi *
*        This script is released        *
*          under the LGPL v2.1.         *
****************************************/


/*
	Public methods/property:
		mips32.reset(address);
			Reset the cpu. PC will be address.
		mips32.exec();
			Execute an instruction.
		mips32.requestInterrupt(address,ipl);
			Request interrupt. Not vector but the address must be set.
			The ipl is interrupt priority level (1-7).
		mips32.checkHalt();
			Check if the CPU is HALT state.
		mips32.pc;
			Program counter.
		mips32.CTIF (read/write)
			Core timer interruption flag.
*/

/*
	This script reauires following objects:
		mips32.cp0;
	This script requires following methods:
		system.read32(address);
		system.write32(address,data);
		system.read16(address);
		system.write16(address,data);
		system.read8(address);
		system.write8(address,data);
		system.wait();

*/


mips32=new Object();
mips32.CTIF=0;
mips32.code=0;
mips32.index=0;
mips32.val16=0;
mips32.mnl=0;
mips32.sa=0;
mips32.rd=0;
mips32.rt=0;
mips32.rs=0;
mips32.mnh=0;
mips32.pc=0;
mips32.interrupts=new Array();
mips32.reset=function(address){
	this.pc=address;
	mips32.interrupt=0; // # of interruption trigered
};
mips32.GPR=function(num){
	return arguments.callee.values[num];
};
mips32.GPR.values=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]; // Including HI and LO
mips32.GPR.set=function(num,value){
	if (num==0) value=0;
	value&=0xFFFFFFFF
	if (value<0) value+=0x100000000;
	this.values[num]=value&0xFFFFFFFF;
};
mips32.GPR.signed=function(num){
	num=this.values[num];
	if (2147483647<num) num-=4294967296;
	return num;
};
mips32.GPR.unsigned=function(num){
	num=this.values[num];
	if (num<0) num+=4294967296;
	return num;
};
mips32.HI=function(){
	if (0==arguments.length) {
		return this.GPR(32);
	} else {
		this.GPR.set(32,arguments[0]);
	}
};
mips32.LO=function(){
	if (0==arguments.length) {
		return this.GPR(33);
	} else {
		this.GPR.set(33,arguments[0]);
	}
};
mips32.logtext="";
mips32.log=function(text){
	text+=" at around PC:0x"+(mips32.pc-4).toString(16);
	try{
		console.log(text);
	}catch(e){
		try{
			dom.log(text);
		}catch(e){}
	}
};
mips32.logreg=function(){
	var hex8=function(val){
		var res;
		if (val<0) val+=0x100000000;
		res=val.toString(16);
		return ('0000000'+res).substr(-8);
	};
	var text="";
	text+="$pc:0x"+this.pc.toString(16)+"\n";
	text+="$zr:"+hex8(this.GPR(0));
	text+=" $at:"+hex8(this.GPR(1));
	text+=" $v0:"+hex8(this.GPR(2));
	text+=" $v1:"+hex8(this.GPR(3));
	text+=" $a0:"+hex8(this.GPR(4));
	text+=" $a1:"+hex8(this.GPR(5));
	text+=" $a2:"+hex8(this.GPR(6));
	text+=" $a3:"+hex8(this.GPR(7))+"\n";
	text+="$t0:"+hex8(this.GPR(8));
	text+=" $t1:"+hex8(this.GPR(9));
	text+=" $t2:"+hex8(this.GPR(10));
	text+=" $t3:"+hex8(this.GPR(11));
	text+=" $t4:"+hex8(this.GPR(12));
	text+=" $t5:"+hex8(this.GPR(13));
	text+=" $t6:"+hex8(this.GPR(14));
	text+=" $t7:"+hex8(this.GPR(15))+"\n";
	text+="$s0:"+hex8(this.GPR(16));
	text+=" $s1:"+hex8(this.GPR(17));
	text+=" $s2:"+hex8(this.GPR(18));
	text+=" $s3:"+hex8(this.GPR(19));
	text+=" $s4:"+hex8(this.GPR(20));
	text+=" $s5:"+hex8(this.GPR(21));
	text+=" $s6:"+hex8(this.GPR(22));
	text+=" $s7:"+hex8(this.GPR(23))+"\n";
	text+="$t8:"+hex8(this.GPR(24));
	text+=" $t9:"+hex8(this.GPR(25));
	text+=" $k0:"+hex8(this.GPR(26));
	text+=" $k1:"+hex8(this.GPR(27));
	text+=" $gp:"+hex8(this.GPR(28));
	text+=" $sp:"+hex8(this.GPR(29));
	text+=" $fp:"+hex8(this.GPR(30));
	text+=" $ra:"+hex8(this.GPR(31))+"\n";
	text+="$hi:"+hex8(this.HI());
	text+=" $lo:"+hex8(this.LO());
	text+="\n";
	this.log(text);
}
mips32.exception=function(text){
	switch(text){
		default:
			this.log(text);
			break;
	}
}
mips32.checkHalt=function(){
	var code;
	code=system.read32(this.pc);
	// The "0x1000FFFF" is infinite loop using B statement.
	return code==0x1000FFFF ? 1:0;
};
mips32.requestInterrupt=function(address,ipl){
	if (typeof this.interrupts[address] === "undefined") {
		this.interrupts[address]=ipl;
	} else {
		if (this.interrupts[address]<ipl) this.interrupts[address]=ipl;
	}
};
mips32.setInterrupt=function(){
	var address;
	// Fetch current IPL status from CP0
	var ipl=(mips32.cp0.Status>>10)&7;
	// Checkout the list for all requested interrupts
	for(address in this.interrupts){
		address=parseInt(address);
		if (ipl<this.interrupts[address]){
			// New interrupt is valid and priority is higher than current one.
			// Delete address from the list.
			delete this.interrupts[address];
			// Set EPC
			mips32.cp0.EPC=this.pc;
			// Disable interrupt until EXL will be set
			mips32.cp0.Status &=0x1FFFFFFD;
			// Set new PC
			this.pc=address;
			break;
		}
	}
};
mips32.incTimer=function(clocks){
	clocks=parseInt(clocks);
	var newCount=mips32.cp0.Count+clocks;
	if (newCount<=0xffffffff) {
		if (mips32.cp0.Count<mips32.cp0.Compare &&
				mips32.cp0.Compare<=newCount) this.CTIF=1;
	} else {
		newCount-=0x100000000;
		if (mips32.cp0.Count<mips32.cp0.Compare ||
				mips32.cp0.Compare<=newCount) this.CTIF=1;
	}
	mips32.cp0.Count=newCount;
};
mips32.exec=function(){
	// Check interrupt first
	if (this.interrupts.length && mips32.cp0.IE()) this.setInterrupt();
	// Increment core timer
	mips32.cp0.Count++;
	mips32.cp0.Count&=0xffffffff;
	if (mips32.cp0.Count==mips32.cp0.Compare) this.CTIF=1;
	// Fetch code from memory
	if (this.pc<0) this.pc+=0x100000000;
	var code;
	code=system.read32(this.pc);
	this.pc+=4;
	// _mnh__ _rs__ _rt__ _rd__ _sa__ _mnl__
	//        _____________index____________
	//                    _____signed16_____
	// xxxxxx xxxxx xxxxx xxxxx xxxxx xxxxxx
	this.code=code;
	this.index=code&0x3ffffff;
	this.signed16=this.unsigned16=code&0xffff;
	if (0x8000<=this.signed16) this.signed16-=0x10000;
	this.mnl=code&0x3f;
	code>>=6;
	this.sa=code&0x1f;
	code>>=5;
	this.rd=code&0x1f;
	code>>=5;
	this.rt=code&0x1f;
	code>>=5;
	this.rs=code&0x1f;
	code=(code>>5)&0x3f;
	this.mnh=code;
	// Execute the function
	return this[this.commands[code]]();
};
mips32.void=function(){
	this.log("Invalid command:0x"+this.code.toString(16));
};
mips32.commands=[
	"execSpecial", //000000
	"execBranch",  //000001
	"J",           //000010
	"JAL",         //000011
	"BEQ",         //000100
	"BNE",         //000101
	"BLEZ",        //000110
	"BGTZ",        //000111
	"ADDI",        //001000
	"ADDIU",       //001001
	"SLTI",        //001010
	"SLTIU",       //001011
	"ANDI",        //001100
	"ORI",         //001101
	"XORI",        //001110
	"LUI",         //001111
	"execCop0",    //010000
	"void",        //010001
	"void",        //010010
	"void",        //010011
	"BEQL",        //010100
	"BNEL",        //010101
	"BLEZL",       //010110
	"BGTZL",       //010111
	"void",        //011000
	"void",        //011001
	"void",        //011010
	"void",        //011011
	"execSpecial2",//011100
	"void",        //011101
	"void",        //011110
	"execSpecial3",//011111
	"LB",          //100000
	"LH",          //100001
	"LWL",         //100010
	"LW",          //100011
	"LBU",         //100100
	"LHU",         //100101
	"LWR",         //100110
	"void",        //100111
	"SB",          //101000
	"SH",          //101001
	"SWL",         //101010
	"SW",          //101011
	"void",        //101100
	"void",        //101101
	"SWR",         //101110
	"CACHE",       //101111
	"LL",          //110000
	"void",        //110001
	"void",        //110010
	"PREF",        //110011
	"void",        //110100
	"void",        //110101
	"void",        //110110
	"void",        //110111
	"SC",          //111000
	"void",        //111001
	"void",        //111010
	"void",        //111011
	"void",        //111100
	"void",        //111101
	"void",        //111110
	"void",        //111111
];
mips32.commandsSp=[
	"SLL",       //000000
	"void",      //000001
	"SRL",       //000010
	"SRA",       //000011
	"SLLV",      //000100
	"void",      //000101
	"SRLV",      //000110
	"SRAV",      //000111
	"JR",        //001000
	"JALR",      //001001
	"MOVZ",      //001010
	"MOVN",      //001011
	"SYSCALL",   //001100
	"BREAK",     //001101
	"void",      //001110
	"SYNC",      //001111
	"MFHI",      //010000
	"MTHI",      //010001
	"MFLO",      //010010
	"MTLO",      //010011
	"void",      //010100
	"void",      //010101
	"void",      //010110
	"void",      //010111
	"MULT",      //011000
	"MULTU",     //011001
	"DIV",       //011010
	"DIVU",      //011011
	"void",      //011100
	"void",      //011101
	"void",      //011110
	"void",      //011111
	"ADD",       //100000
	"ADDU",      //100001
	"SUB",       //100010
	"SUBU",      //100011
	"AND",       //100100
	"OR",        //100101
	"XOR",       //100110
	"NOR",       //100111
	"void",      //101000
	"void",      //101001
	"SLT",       //101010
	"SLTU",      //101011
	"void",      //101100
	"void",      //101101
	"void",      //101110
	"void",      //101111
	"TGE",       //110000
	"TGEU",      //110001
	"TLT",       //110010
	"TLTU",      //110011
	"TEQ",       //110100
	"void",      //110101
	"TNE",       //110110
	"void",      //110111
	"void",      //111000
	"void",      //111001
	"void",      //111010
	"void",      //111011
	"void",      //111100
	"void",      //111101
	"void",      //111110
	"void",      //111111
];
mips32.commandsBr=[
	"BLTZ",      //00000
	"BGEZ",      //00001
	"BLTZL",     //00010
	"BGEZL",     //00011
	"void",      //00100
	"void",      //00101
	"void",      //00110
	"void",      //00111
	"TGEI",      //01000
	"TGEIU",     //01001
	"TLTI",      //01010
	"TLTIU",     //01011
	"TEQI",      //01100
	"void",      //01101
	"TNEI",      //01110
	"void",      //01111
	"BLTZAL",    //10000
	"BGEZAL",    //10001
	"BLTZALL",   //10010
	"BGEZALL",   //10011
	"void",      //10100
	"void",      //10101
	"void",      //10110
	"void",      //10111
	"void",      //11000
	"void",      //11001
	"void",      //11010
	"void",      //11011
	"void",      //11100
	"void",      //11101
	"void",      //11110
	"void",      //11111
];
mips32.commandsSp2=[
	"MADD",      //000000
	"MADDU",     //000001
	"MUL",       //000010
	"void",      //000011
	"MSUB",      //000100
	"MSUBU",     //000101
	"void",      //000110
	"void",      //000111
	"void",      //001000
	"void",      //001001
	"void",      //001010
	"void",      //001011
	"void",      //001100
	"void",      //001101
	"void",      //001110
	"void",      //001111
	"void",      //010000
	"void",      //010001
	"void",      //010010
	"void",      //010011
	"void",      //010100
	"void",      //010101
	"void",      //010110
	"void",      //010111
	"void",      //011000
	"void",      //011001
	"void",      //011010
	"void",      //011011
	"void",      //011100
	"void",      //011101
	"void",      //011110
	"void",      //011111
	"CLZ",       //100000
	"CLO",       //100001
	"void",      //100010
	"void",      //100011
	"void",      //100100
	"void",      //100101
	"void",      //100110
	"void",      //100111
	"void",      //101000
	"void",      //101001
	"void",      //101010
	"void",      //101011
	"void",      //101100
	"void",      //101101
	"void",      //101110
	"void",      //101111
	"void",      //110000
	"void",      //110001
	"void",      //110010
	"void",      //110011
	"void",      //110100
	"void",      //110101
	"void",      //110110
	"void",      //110111
	"void",      //111000
	"void",      //111001
	"void",      //111010
	"void",      //111011
	"void",      //111100
	"void",      //111101
	"void",      //111110
	"SDBBP",     //111111
];
mips32.commandsSp3=[
	"EXT",       //000000
	"void",      //000001
	"void",      //000010
	"void",      //000011
	"INS",       //000100
	"void",      //000101
	"void",      //000110
	"void",      //000111
	"void",      //001000
	"void",      //001001
	"void",      //001010
	"void",      //001011
	"void",      //001100
	"void",      //001101
	"void",      //001110
	"void",      //001111
	"void",      //010000
	"void",      //010001
	"void",      //010010
	"void",      //010011
	"void",      //010100
	"void",      //010101
	"void",      //010110
	"void",      //010111
	"void",      //011000
	"void",      //011001
	"void",      //011010
	"void",      //011011
	"void",      //011100
	"void",      //011101
	"void",      //011110
	"void",      //011111
	"void",      //100000
	"void",      //100001
	"void",      //100010
	"void",      //100011
	"void",      //100100
	"void",      //100101
	"void",      //100110
	"void",      //100111
	"void",      //101000
	"void",      //101001
	"void",      //101010
	"void",      //101011
	"void",      //101100
	"void",      //101101
	"void",      //101110
	"void",      //101111
	"void",      //110000
	"void",      //110001
	"void",      //110010
	"void",      //110011
	"void",      //110100
	"void",      //110101
	"void",      //110110
	"void",      //110111
	"void",      //111000
	"void",      //111001
	"void",      //111010
	"void",      //111011
	"void",      //111100
	"void",      //111101
	"void",      //111110
	"void",      //111111
];
mips32.commandsCp0=[
	"void",      //000000
	"TLBR",      //000001
	"TLBWI",     //000010
	"void",      //000011
	"void",      //000100
	"void",      //000101
	"TLBWR",     //000110
	"void",      //000111
	"TLBP",      //001000
	"void",      //001001
	"void",      //001010
	"void",      //001011
	"void",      //001100
	"void",      //001101
	"void",      //001110
	"void",      //001111
	"void",      //010000
	"void",      //010001
	"void",      //010010
	"void",      //010011
	"void",      //010100
	"void",      //010101
	"void",      //010110
	"void",      //010111
	"ERET",      //011000
	"void",      //011001
	"void",      //011010
	"void",      //011011
	"void",      //011100
	"void",      //011101
	"void",      //011110
	"DERET",     //011111
	"WAIT",      //100000
	"void",      //100001
	"void",      //100010
	"void",      //100011
	"void",      //100100
	"void",      //100101
	"void",      //100110
	"void",      //100111
	"void",      //101000
	"void",      //101001
	"void",      //101010
	"void",      //101011
	"void",      //101100
	"void",      //101101
	"void",      //101110
	"void",      //101111
	"void",      //110000
	"void",      //110001
	"void",      //110010
	"void",      //110011
	"void",      //110100
	"void",      //110101
	"void",      //110110
	"void",      //110111
	"void",      //111000
	"void",      //111001
	"void",      //111010
	"void",      //111011
	"void",      //111100
	"void",      //111101
	"void",      //111110
	"void",      //111111
];
mips32.commandsCp0Rs=[
	"MFC0",      //00000
	"void",      //00001
	"void",      //00010
	"void",      //00011
	"MTC0",      //00100
	"void",      //00101
	"void",      //00110
	"void",      //00111
	"void",      //01000
	"void",      //01001
	"RDPGPR",    //01010
	"void",      //01011
	"void",      //01100
	"void",      //01101
	"WRPGPR",    //01110
	"void",      //01!11
];
mips32.execSpecial=function(){
	return this[this.commandsSp[this.mnl]]();
};
mips32.execBranch=function(){
	return this[this.commandsBr[this.rt]]();
};
mips32.execCop0=function(){
	if (this.rs&0x10) { // CO (25th bit) is 1
		return this[this.commandsCp0[this.mnl]]();
	} else {            // CO (25th bit) is 0
		var mnemonic=this.commandsCp0Rs[this.rs];
		if (mnemonic!='void') {
			return this[mnemonic]();
		}
		if (this.rs==0x0b) {
			if (this.rd==12 && this.sa==0 && this.mnl==0x00) return this.DI();
			if (this.rd==12 && this.sa==0 && this.mnl==0x20) return this.EI();
		}
		this.log("Invalid command:\n COP0 MF 0x"+this.rs.toString(16));
	}
};
mips32.execSpecial2=function(){
	return this[this.commandsSp2[this.mnl]]();
};
mips32.execSpecial3=function(){
	return this[this.commandsSp3[this.mnl]]();
};
mips32.ADD=function(){
	var rs=this.GPR.signed(this.rs);
	var rt=this.GPR.signed(this.rt);
	var rd=rs+rt;
	if ((rd&0x80000000)==(rs&0x80000000)) {
		this.GPR.set(this.rd,rd);
	} else {
		this.exception("IntegerOverflow");
	}
};
mips32.ADDI=function(){
	var rs=this.GPR.signed(this.rs);
	var val16=this.signed16;
	var rt=rs+val16;
	if ((rt&0x80000000)==(rs&0x80000000)) {
		this.GPR.set(this.rt,rt);
	} else {
		this.exception("IntegerOverflow");
	}
};
mips32.ADDIU=function(){
	var rs=this.GPR(this.rs);
	var val16=this.signed16;
	var rt=rs+val16;
	this.GPR.set(this.rt,rt);
};
mips32.ADDU=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	var rd=rs+rt;
	this.GPR.set(this.rd,rd);
};
mips32.AND=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	var rd=rs&rt;
	this.GPR.set(this.rd,rd);
};
mips32.ANDI=function(){
	var rs=this.GPR(this.rs);
	var val16=this.unsigned16;
	var rt=rs&val16;
	this.GPR.set(this.rt,rt);
};
mips32.BEQ=function(){
	var pc=this.pc+this.signed16*4;
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	this.exec();
	if (rs==rt) {
		this.pc=pc;
	}
};
mips32.BEQL=function(){
	var pc=this.pc+this.signed16*4;
	if (this.GPR(this.rs)==this.GPR(this.rt)) {
		this.exec();
		this.pc=pc;
	} else {
		this.pc+=4;
	}
};
mips32.BGEZ=function(){
	var pc=this.pc+this.signed16*4;
	var rs=this.GPR(this.rs);
	this.exec();
	if (!(rs&0x80000000)) {
		this.pc=pc;
	}
};
mips32.BGEZAL=function(){
	var pc=this.pc+this.signed16*4;
	var rs=this.GPR(this.rs);
	this.GPR.set(31,this.pc+4);
	this.exec();
	if (!(this.rs&0x80000000)) {
		this.pc=pc;
	}
};
mips32.BGEZALL=function(){
	var pc=this.pc+this.signed16*4;
	var rs=this.GPR(this.rs);
	this.GPR.set(31,this.pc+4);
	if (!(rs&0x80000000)) {
		this.exec();
		this.pc=pc;
	} else {
		this.pc+=4;
	}
};
mips32.BGEZL=function(){
	var pc=this.pc+this.signed16*4;
	if (!(this.GPR(this.rs)&0x80000000)) {
		this.exec();
		this.pc=pc;
	} else {
		this.pc+=4;
	}
};
mips32.BGTZ=function(){
	var pc=this.pc+this.signed16*4;
	var rs=this.GPR(this.rs);
	this.exec();
	if ((rs!=0)&&(!(rs&0x80000000))) {
		this.pc=pc;
	}
};
mips32.BGTZL=function(){
	var pc=this.pc+this.signed16*4;
	if ((this.GPR(this.rs)!=0)&&(!(this.GPR(this.rs)&0x80000000))) {
		this.exec();
		this.pc=pc;
	} else {
		this.pc+=4;
	}
};
mips32.BLEZ=function(){
	var pc=this.pc+this.signed16*4;
	var rs=this.GPR(this.rs);
	this.exec();
	if ((rs==0)||(rs&0x80000000)) {
		this.pc=pc;
	}
};
mips32.BLEZL=function(){
	var pc=this.pc+this.signed16*4;
	if ((this.GPR(this.rs)==0)||(this.GPR(this.rs)&0x80000000)) {
		this.exec();
		this.pc=pc;
	} else {
		this.pc+=4;
	}
};
mips32.BLTZ=function(){
	var pc=this.pc+this.signed16*4;
	var rs=this.GPR(this.rs);
	this.exec();
	if ((rs!=0)&&(rs&0x80000000)) {
		this.pc=pc;
	}
};
mips32.BLTZAL=function(){
	var pc=this.pc+this.signed16*4;
	this.GPR.set(31,this.pc+4);
	var rs=this.GPR(this.rs);
	this.exec();
	if ((rs!=0)&&(rs&0x80000000)) {
		this.pc=pc;
	}
};
mips32.BLTZALL=function(){
	var pc=this.pc+this.signed16*4;
	this.GPR.set(31,this.pc+4);
	if ((this.GPR(this.rs)!=0)&&(this.GPR(this.rs)&0x80000000)) {
		this.exec();
		this.pc=pc;
	} else {
		this.pc+=4;
	}
};
mips32.BLTZL=function(){
	var pc=this.pc+this.signed16*4;
	if ((this.GPR(this.rs)!=0)&&(this.GPR(this.rs)&0x80000000)) {
		this.exec();
		this.pc=pc;
	} else {
		this.pc+=4;
	}
};
mips32.BNE=function(){
	var pc=this.pc+this.signed16*4;
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	this.exec();
	if (rs!=rt) {
		this.pc=pc;
	}
};
mips32.BNEL=function(){
	var pc=this.pc+this.signed16*4;
	if (this.GPR(this.rs)!=this.GPR(this.rt)) {
		this.exec();
		this.pc=pc;
	} else {
		this.pc+=4;
	}
};
mips32.BREAK=function(){
	this.exception("BREAK");
};
mips32.CACHE=function(){this.log("CACHE");};
mips32.CLO=function(){
	var i;
	var rs=this.GPR(this.rs);
	var rd=0;
	for(i=0;i<32;i++){
		if (rs&1) rd++;
		rs>>=1;
	}
	this.GPR.set(this.rd,rd);
};
mips32.CLZ=function(){
	var i;
	var rs=this.GPR(this.rs);
	var rd=32;
	for(i=0;i<32;i++){
		if (rs&1) rd--;
		rs>>=1;
	}
	this.GPR.set(this.rd,rd);
};
mips32.DERET=function(){
	this.ERET();
	this.log("DERET");
};
mips32.DI=function(){
	this.GPR.set(this.rt,mips32.cp0.Status);
	mips32.cp0.DI();
};
mips32.DIV=function(){
	var rs=this.GPR.signed(this.rs);
	var rt=this.GPR.signed(this.rt);
	var hi=rs%rt;
	var lo=(rs-hi)/rt;
	this.HI(hi);
	this.LO(lo);
};
mips32.DIVU=function(){
	var rs=this.GPR.unsigned(this.rs);
	var rt=this.GPR.unsigned(this.rt);
	var hi=rs%rt;
	var lo=(rs-hi)/rt;
	this.HI(hi);
	this.LO(lo);
};
mips32.EI=function(){
	this.GPR.set(this.rt,mips32.cp0.Status);
	mips32.cp0.EI();
};
mips32.ERET=function(){
	this.pc=mips32.cp0.EPC;
};
mips32.EXT=function(){
	var rs=this.GPR.unsigned(this.rs);
	var size=1+this.rd;
	if (size==32) size=0xffffffff; else size=0x7fffffff>>(31-size);
	var pos=this.sa;
	var rt=(rs>>pos)&size;
	this.GPR.set(this.rt,rt);
};
mips32.INS=function(){
	var pos=this.sa;
	var size=this.rd-pos+1
	var mask=0;
	var res=this.GPR(this.rt);
	var i;
	for(i=0;i<size;i++) mask|=1<<i;
	res&=-1^(mask<<(pos));
	res|=(this.GPR(this.rs)&mask)<<(pos);
	this.GPR.set(this.rt,res);
};
mips32.J=function(){
	var index=this.index;
	this.exec();
	this.pc-=0x0FFFFFFF&this.pc;
	this.pc+=index<<2;

};
mips32.JAL=function(){
	var index=this.index;
	this.exec();
	this.GPR.set(31,this.pc);
	this.pc-=0x0FFFFFFF&this.pc;
	this.pc+=index<<2;
};
mips32.JALR=function(){
	var rs=this.GPR(this.rs);
	this.exec();
	this.GPR.set(31,this.pc);
	this.pc=rs;
};
mips32.JR=function(){
	var rs=this.GPR(this.rs);
	this.exec();
	this.pc=rs;
};
mips32.LB=function(){
	var rs=this.GPR(this.rs);
	var rt=system.read8(rs+this.signed16);
	if (rt&0x80) {
		rt|=0xFFFFFF80;
	} else {
		rt&=0x7f;
	}
	this.GPR.set(this.rt,rt);
};
mips32.LBU=function(){
	var rs=this.GPR(this.rs);
	var rt=system.read8(rs+this.signed16);
	this.GPR.set(this.rt,rt);
};
mips32.LH=function(){
	var rs=this.GPR(this.rs);
	var rt=system.read16(rs+this.signed16);
	if (rt&0x8000) {
		rt|=0xFFFF8000;
	} else {
		rt&=0x7fff;
	}
	this.GPR.set(this.rt,rt);
};
mips32.LHU=function(){
	var rs=this.GPR(this.rs);
	var rt=system.read16(rs+this.signed16);
	this.GPR.set(this.rt,rt);
};
mips32.LL=function(){this.log("LL");};
mips32.LUI=function(){
	var rt=this.unsigned16<<16;
	this.GPR.set(this.rt,rt);
};
mips32.LW=function(){
	var rs=this.GPR(this.rs);
	var rt=system.read32(rs+this.signed16);
	this.GPR.set(this.rt,rt);
};
mips32.LWL=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt)&0x0000FFFF;
	rt|=system.read16(rs+this.signed16)<<16;
	this.GPR.set(this.rt,rt);
};
mips32.LWR=function(){
	var rs=this.GPR(this.rs);
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt)&0xFFFF0000;
	rt|=system.read16(rs+this.signed16);
	this.GPR.set(this.rt,rt);
};
mips32.MADD=function(){this.log("MADD");};
mips32.MADDU=function(){this.log("MADDU");};
mips32.MFC0=function(){
	this.GPR.set(this.rt,mips32.cp0.MFC0(this.rd,this.mnl));
};
mips32.MFHI=function(){
	var rd=this.HI();
	this.GPR.set(this.rd,rd);
};
mips32.MFLO=function(){
	var rd=this.LO();
	this.GPR.set(this.rd,rd);
};
mips32.MOVN=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	var rd=rs;
	if (rt) this.GPR.set(this.rd,rd);
};
mips32.MOVZ=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	var rd=rs;
	if (rt==0) this.GPR.set(this.rd,rd);
};
mips32.MSUB=function(){this.log("MSUB");};
mips32.MSUBU=function(){this.log("MSUBU");};
mips32.MTC0=function(){
	mips32.cp0.MTC0(this.rd,this.mnl,this.GPR(this.rt));
};
mips32.MTHI=function(){
	var rs=this.GPR(this.rs);
	this.HI(rs);
};
mips32.MTLO=function(){
	var rs=this.GPR(this.rs);
	this.LO(rs);
};
mips32.MUL=function(){
	this.MULT();
	var rd=this.LO();
	this.GPR.set(this.rd,rd);
};
mips32.MULT=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	// Calculate with unsigned values
	var sign=1;
	if (rs&0x80000000) {
		rs=0x100000000-rs;
		sign=-sign;
	}
	if (rt&0x80000000) {
		rt=0x100000000-rt;
		sign=-sign;
	}
	// Main calculation routine
	var rsh=rs>>16;
	var rsl=rs&0xffff;
	var rth=rt>>16;
	var rtl=rt&0xffff;
	var lo=rsl*rtl;
	var mid=rsh*rtl+rsl*rth+(lo>>16);
	lo=(lo&0xffff)|((mid&0xffff)<<16);
	var hi=rsh*rth+(mid>>16);
	// Consider sign
	if (sign<0) {
		if (lo==0) {
			hi=0x100000000-hi;
		} else {
			lo=0x100000000-lo;
			hi=0x100000000-hi-1;
		}
	}
	// Save results
	this.HI(hi);
	this.LO(lo);
};
mips32.MULTU=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	var rsh=rs>>16;
	var rsl=rs&0xffff;
	var rth=rt>>16;
	var rtl=rt&0xffff;
	var lo=rsl*rtl;
	var mid=rsh*rtl+rsl*rth+(lo>>16);
	lo=(lo&0xffff)|((mid&0xffff)<<16);
	var hi=rsh*rth+(mid>>16);
	this.HI(hi);
	this.LO(lo);
};
mips32.NOR=function(){
	var rs=this.GPR.unsigned(this.rs);
	var rt=this.GPR.unsigned(this.rt);
	var rd=~(rs|rt);
	this.GPR.set(this.rd,rd);
};
mips32.OR=function(){
	var rs=this.GPR.unsigned(this.rs);
	var rt=this.GPR.unsigned(this.rt);
	var rd=rs|rt;
	this.GPR.set(this.rd,rd);
};
mips32.ORI=function(){
	var rs=this.GPR(this.rs);
	var val16=this.unsigned16;
	var rt=rs|val16;
	this.GPR.set(this.rt,rt);
};
mips32.PREF=function(){this.log("PREF");};
mips32.RDPGPR=function(){
	// Shadow register is not supported
	if (this.rd!=this.rt) this.log("RDPGPR "+this.rd +" "+this.rt);
};
mips32.SB=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	system.write8(rs+this.signed16,rt&0x000000ff);
};
mips32.SC=function(){this.log("SC");};
mips32.SDBBP=function(){this.log("SDBBP");};
mips32.SH=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	system.write16(rs+this.signed16,rt&0x0000ffff);
};
mips32.SLL=function(){
	var rt=this.GPR(this.rt);
	var sa=this.sa;
	var rd=rt<<sa;
	this.GPR.set(this.rd,rd);
};
mips32.SLLV=function(){
	var rt=this.GPR(this.rt);
	var rs=this.GPR(this.rs);
	var rd=rt<<rs;
	this.GPR.set(this.rd,rd);
};
mips32.SLT=function(){
	var rs=this.GPR.signed(this.rs);
	var rt=this.GPR.signed(this.rt);
	var rd=(rs<rt) ? 1:0;
	this.GPR.set(this.rd,rd);
};
mips32.SLTI=function(){
	var rs=this.GPR.signed(this.rs);
	var val16=this.signed16;
	if (0x7fff<val16) val16-=0x10000;
	var rt=(rs<val16) ? 1:0;
	this.GPR.set(this.rt,rt);
};
mips32.SLTIU=function(){
	var rs=this.GPR.unsigned(this.rs);
	var val16=this.signed16;
	if (val16<0) val16+=0x10000;
	var rt=(rs<val16) ? 1:0;
	this.GPR.set(this.rt,rt);
};
mips32.SLTU=function(){
	var rs=this.GPR.unsigned(this.rs);
	var rt=this.GPR.unsigned(this.rt);
	var rd=(rs<rt) ? 1:0;
	this.GPR.set(this.rd,rd);
};
mips32.SRA=function(){
	var rt=this.GPR(this.rt);
	var sa=this.sa;
	var rd=rt;
	var sign=rt&0x80000000;
	while(sa--){
		rd=(rd>>1)|sign;
	}
	this.GPR.set(this.rd,rd);
};
mips32.SRAV=function(){
	var rt=this.GPR(this.rt);
	var rs=this.GPR(this.rs);
	var rd=rt;
	var sign=rt&0x80000000;
	while(ss--){
		rd=(rd>>1)|sign;
	}
	this.GPR.set(this.rd,rd);
};
mips32.SRL=function(){
	var rt=this.GPR.unsigned(this.rt);
	var sa=this.sa;
	var rd=rt>>sa;
	this.GPR.set(this.rd,rd);
};
mips32.SRLV=function(){
	var rt=this.GPR(this.rt);
	var rs=this.GPR(this.sa);
	var rd=rt>>rs;
	this.GPR.set(this.rd,rd);
};
mips32.SUB=function(){
	var rs=this.GPR.signed(this.rs);
	var rt=this.GPR.signed(this.rt);
	var rd=rs-rt;
	if ((rd&0x80000000)==(rs&0x80000000)) {
		this.GPR.set(this.rd,rd);
	} else {
		this.exception("IntegerOverflow");
	}
};
mips32.SUBU=function(){
	var rs=this.GPR.unsigned(this.rs);
	var rt=this.GPR.unsigned(this.rt);
	var rd=rs-rt;
	this.GPR.set(this.rd,rd);
};
mips32.SW=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	system.write32(rs+this.signed16,rt);
};
mips32.SWL=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	system.write16(rs+this.signed16,(rt&0xffff0000)>>16);
};
mips32.SWR=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	system.write16(rs+this.signed16,rt&0x0000ffff);
};
mips32.SYNC=function(){this.log("SYNC");};
mips32.SYSCALL=function(){this.log("SYSCALL");};
mips32.TEQ=function(){
	var rs=this.GPR.signed(this.rs);
	var rt=this.GPR.signed(this.rt);
	if (rs==rt) this.exception("IntegerOverflow");
};
mips32.TEQI=function(){
	var rs=this.GPR.signed(this.rs);
	var val16=this.signed16;
	if (rs==val16) this.exception("IntegerOverflow");
};
mips32.TGE=function(){
	var rs=this.GPR.signed(this.rs);
	var rt=this.GPR.signed(this.rt);
	if (rs>=rt) this.exception("IntegerOverflow");
};
mips32.TGEI=function(){
	var rs=this.GPR.signed(this.rs);
	var val16=this.signed16;
	if (rs>=val16) this.exception("IntegerOverflow");
};
mips32.TGEIU=function(){
	var rs=this.GPR.unsigned(this.rs);
	var val16=this.signed16;
	if (val16<0) val16+=0x10000;
	if (rs>=val16) this.exception("IntegerOverflow");
};
mips32.TGEU=function(){
	var rs=this.GPR.unsigned(this.rs);
	var rt=this.GPR.unsigned(this.rt);
	if (rs>=rt) this.exception("IntegerOverflow");
};
mips32.TLBP=function(){this.log("TLBP");};
mips32.TLBR=function(){this.log("TLBR");};
mips32.TLBWI=function(){this.log("TLBWI");};
mips32.TLBWR=function(){this.log("TLBWR");};
mips32.TLT=function(){
	var rs=this.GPR.signed(this.rs);
	var rt=this.GPR.signed(this.rt);
	if (rs<rt) this.exception("IntegerOverflow");
};
mips32.TLTI=function(){
	var rs=this.GPR.signed(this.rs);
	var val16=this.signed16;
	if (rs<val16) this.exception("IntegerOverflow");
};
mips32.TLTIU=function(){
	var rs=this.GPR.unsigned(this.rs);
	var val16=this.signed16;
	if (val16<0) val16+=0x10000;
	if (rs<val16) this.exception("IntegerOverflow");
};
mips32.TLTU=function(){
	var rs=this.GPR.unsigned(this.rs);
	var rt=this.GPR.unsigned(this.rt);
	if (rs>rt) this.exception("IntegerOverflow");
};
mips32.TNE=function(){
	var rs=this.GPR.signed(this.rs);
	var rt=this.GPR.signed(this.rt);
	if (rs!=rt) this.exception("IntegerOverflow");
};
mips32.TNEI=function(){
	var rs=this.GPR.signed(this.rs);
	var val16=this.signed16;
	if (rs!=val16) this.exception("IntegerOverflow");
};
mips32.WAIT=function(){
	system.wait();
};
mips32.WRPGPR=function(){
	// Shadow register is not supported
	if (this.rd!=this.rt) this.log("WRPGPR "+this.rd +" "+this.rt);
};
mips32.XOR=function(){
	var rs=this.GPR(this.rs);
	var rt=this.GPR(this.rt);
	var rd=rs^rt;
	this.GPR.set(this.rd,rd);
};
mips32.XORI=function(){
	var rs=this.GPR(this.rs);
	var val16=this.unsigned16;
	var rt=rs^val16;
	this.GPR.set(this.rt,rt);
};


/****************************************
*  PIC32MX emulator written by Katsumi  *
*        This script is released        *
*          under the LGPL v2.1.         *
****************************************/

// Prototype of timers
timer=function(){};
timer.prototype=timer;
timer.TMR=0;
timer.PR=0;
timer.ON=0;
timer.PRESCALER=0;
timer.interrupt=function(){}; // Must be set when needed.
timer.initA=function(con){
	/*
		Type A timer
		This function must be called when T1CON is set
		
		bit 15: ON
		bit 14: FRZ,  ignored
		bit 13: SIDL, ignored
		bit 12: TWDIS,ignored
		bit 11: TWIP, ignored
		bit 10:
		bit  9:
		bit  8:
		bit  7: TGATE,ignored
		bit  6:
		bit  5: TCKPS
		bit  4: TCKPS
		bit  3:
		bit  2: TSYNC,ignored
		bit  1: TCS,  ignored
		bit  0:
	*/
	this.ON=(con & 0x8000) ? 1:0;
	switch((con>>4)&3){
		case 3:
			this.PRESCALER=8;
			break;
		case 2:
			this.PRESCALER=6;
			break;
		case 1:
			this.PRESCALER=3;
			break;
		case 0:
		default:
			this.PRESCALER=0;
			break;
	}
};
timer.init=function(con){
	/*
		Type B timer
		This function must be called when TxCON is set
		
		bit 15: ON
		bit 14: FRZ,  ignored
		bit 13: SIDL, ignored
		bit 12:
		bit 11:
		bit 10:
		bit  9:
		bit  8:
		bit  7: TGATE,ignored
		bit  6: TCKPS
		bit  5: TCKPS
		bit  4: TCKPS
		bit  3: T32  ,ignored
		bit  2:
		bit  1: TCS,  ignored
		bit  0:
	*/
	this.ON=(con & 0x8000) ? 1:0;
	switch((con>>4)&7){
		case 7:
			this.PRESCALER=8;
			break;
		case 6:
			this.PRESCALER=6;
			break;
		case 5:
			this.PRESCALER=5;
			break;
		case 4:
			this.PRESCALER=4;
			break;
		case 3:
			this.PRESCALER=3;
			break;
		case 2:
			this.PRESCALER=2;
			break;
		case 1:
			this.PRESCALER=1;
			break;
		case 0:
		default:
			this.PRESCALER=0;
			break;
	}
};
timer.scalerRemaining=0;
timer.inc=function(clocks){
	if (!this.ON) return;
	var num;
	this.TMR&=0xffff;
	this.PR&=0xffff;
	clocks+=this.scalerRemaining;
	num=clocks>>this.PRESCALER;
	this.scalerRemaining=clocks-(num<<this.PRESCALER);
	while(num){
		if (this.PR<=this.TMR) {
			num--;
			this.TMR=0;
			this.interrupt();
		} else if (this.TMR+num<this.PR) {
			this.TMR+=num;
			num=0;
		} else {
			num=num-(this.PR-this.TMR);
			this.TMR=this.PR;
		}
	}
};

// Timer1
timer1=new timer();
timer1.init=timer1.initA;
// Timer2-5
timer2=new timer();
timer3=new timer();
timer4=new timer();
timer5=new timer();

// Core timer
coretimer=new Object();
coretimer.TMR=0;
coretimer.PR=0;
coretimer.interrupt=function(){};
coretimer.inc=function(clocks){
	while(clocks){
		if (this.PR==this.TMR) {
			clocks--;
			this.TMR++;
			this.interrupt();
		} else if (this.TMR<this.PR) {
			if (this.PR<=this.TMR+clocks) {
				this.interrupt();
			}
			tnis.TMR+=clocks;
			clocks=0;
		} else {
			if (this.TMR+clocks<4294967296) {
				this.TMR+=clocks;
				clocks=0;
			} else {
				clocks-=4294967296-this.TMR;
				this.TMR=0;
			}
		}
		if (4294967296<=this.TMR) this.TMR-=4294967296;
	}
};

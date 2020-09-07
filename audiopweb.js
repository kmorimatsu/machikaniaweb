/************************************
* MachiKania web written by Katsumi *
*      This script is released      *
*        under the LGPL v2.1.       *
************************************/

audio=new Object();
audio.rate=48000;
audio.block=-1; // Either 0, 1, or 0 (first half, last half, or invalid)
audio.buffer1=new Array();
audio.buffer2=new Array();
audio.bpos=0;

audio.frequency=0;
audio.count=0;
audio.output=1.0;
audio.reset=function(){
	this.frequency=0;
	this.count=0;
};
audio.play=function(){
	// RPF5, Timer3, OC4 are used
	var RPF5R=SFR.RPF5R.value;
	var T3CON=SFR.T3CON.value;
	var OC4CON=SFR.OC4CON.value;
	var PR3=SFR.PR3.value;
	var ON,TCKPS,OCM;
	// RPF5 must be 11 (for OC4)
	if (RPF5R!=11) return this.reset();
	// Timer3 must be ON
	ON=T3CON&0x8000;
	TCKPS=(T3CON>>4)&7;
	if (!ON) return this.reset();
	// OC4 must be ON and toggle mode
	ON=OC4CON&0x8000;
	OCM=OC4CON&7;
	if (!ON) return this.reset();
	if (OCM!=3) return this.reset();
	// PR3 must be more than 0
	if (0==PR3) return this.reset();
	// Conditions all met for OC4 with Timer 4 in toggle mode
	// Initialize if needed
	if (!this.context) this.init();
	// Lets calculate frequency
	var frequency=95454533; // Toggle mode
	if (7==TCKPS) frequency/=256;
	else frequency/=1<<TCKPS;
	frequency/=PR3+1;
	this.frequency=frequency;
};
audio.init=function(){
	if ((typeof AudioContext)=="undefined") return false;
	this.context=new AudioContext();
	this.rate=audio.context.sampleRate;
	this.buffratio=this.rate/60/262; // 800/262
	this.node=this.context.createScriptProcessor(512, 1, 1);
	this.node.onaudioprocess=function(e){ audio.process(e); };
	this.node.connect(this.context.destination);
	return true;
};
audio.process=function(e){
	var i;
	var data=e.outputBuffer.getChannelData(0);
	if (0==this.frequency) return;
	var every=this.rate/this.frequency;
	for(i=0;i<512;i++){
		this.count++;
		if (every<=this.count) {
			this.count-=every;
			this.output=1.0-this.output;
		}
		data[i]=this.output;
	}
};

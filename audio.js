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
audio.play=function(){
	var i;
	this.WaveTable=system.readRAM32(system.pWaveTable);
	if (!this.WaveTable || !system.readRAM8(system.pMusicActive)) {
		this.block=-1;
		return;
	}
	// WaveTable: alloc_memory(524*2/4,ALLOC_WAVE_BLOCK);
	// Sample rate: 15720 Hz
	if (this.block<0) {
		if (!this.init()) return;
		this.buffer1=new Array();
		this.buffer2=new Array();
		for(i=0;i<262;i++){
			this.buffer1.push(system.readRAM8(this.WaveTable+i*2));
			this.buffer2.push(system.readRAM8(this.WaveTable+i*2+1));
		}
		this.block=0;
	}
	this.block=this.block ? 0:1;
	if (this.block) {
		for(i=262;i<524;i++){
			this.buffer1.push(system.readRAM8(this.WaveTable+i*2));
			this.buffer2.push(system.readRAM8(this.WaveTable+i*2+1));
		}
	} else {
		for(i=0;i<262;i++){
			this.buffer1.push(system.readRAM8(this.WaveTable+i*2));
			this.buffer2.push(system.readRAM8(this.WaveTable+i*2+1));
		}
	}
};
audio.init=function(){
	if ((typeof AudioContext)=="undefined") return false;
	this.context=new AudioContext();
	this.rate=audio.context.sampleRate;
	this.buffratio=this.rate/60/262; // 800/262
	this.node=this.context.createScriptProcessor(512, 2, 2);
	this.node.onaudioprocess=function(e){ audio.process(e); };
	this.node.connect(this.context.destination);
	return true;
};
audio.process=function(e){
	// Delay ~40 msec
	if (this.buffer1.length<2048) return;
	// Stop audio if buffer is empty
	if (this.buffer1.length==0) {
		// Sound has been terminated. Disconnect the node.
		setTimeout(function(){ audio.node.disconnect(); },20);
		return;
	}
	var data1=e.outputBuffer.getChannelData(0);
	var data2=e.outputBuffer.getChannelData(1);
	var i,pos,buff1data,buff2data;
	var prevpos=-1;
	for(i=0;i<512;i++){
		pos=parseInt(i/this.buffratio);
		if (prevpos<pos) {
			prevpos=pos;
			// Read from buffer
			if (this.buffer1.length==0) break; // Terminate if buffer is empty
			buff1data=this.buffer1.shift()/128.0-1.0;
			buff2data=this.buffer2.shift()/128.0-1.0;
		}
		data1[i]=buff1data;
		data2[i]=buff1data;
	}
};

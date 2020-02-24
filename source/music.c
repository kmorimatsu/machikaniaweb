/*
   This file is provided under the LGPL license ver 2.1.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include <xc.h>
#include "compiler.h"
#include "api.h"

/*
	Tone data for 15700 Hz sampling frequency,
	supporting 3.4 Hz - 440 Hz - 31400 Hz,
	are 2147483647 - 16777216 (2^24) - 235095, respectively.
	The counter for DMA is working 470189 times more than the actual clock.
	Therefore, 440 Hz correspond to 16777216 (= 470189 * 15700/440) counts of this timer.

	Example:
	When producing 440 Hz signal, the timer (initially set to 16777216) will be decreased 
	by 470189 at 15700 Hz frequency. When the timer will be equal to or less than half of 
	16777216 (8388608), toggle output. When the timer will be zero or negative, toggle 
	output and add 16777216 to timer.
*/

/*
	c	14107904
	B	14946809
	A#	15835575
	A	16777216
	G#	17774828
	G	18831809
	F#	19951607
	F	21137982
	E	22394866
	D#	23726565
	D	25137402
	C#	26632135
	C	28215755
	Cb	29893558
*/

const static int g_keys[]={
	15835575,14107904,26632135,23726565,21137982,19951607,17774828,//  0 7# C# A#m
	15835575,14946809,26632135,23726565,21137982,19951607,17774828,//  7 6# F# D#m
	15835575,14946809,26632135,23726565,22394866,19951607,17774828,// 14 5# B  G#m
	16777216,14946809,26632135,23726565,22394866,19951607,17774828,// 21 4# E  C#m
	16777216,14946809,26632135,25137402,22394866,19951607,17774828,// 28 3# A  F#m
	16777216,14946809,26632135,25137402,22394866,19951607,18831809,// 35 2# D  Bm
	16777216,14946809,28215755,25137402,22394866,19951607,18831809,// 42 1# G  Em
	16777216,14946809,28215755,25137402,22394866,21137982,18831809,// 49 0  C  Am
	16777216,15835575,28215755,25137402,22394866,21137982,18831809,// 56 1b F  Dm
	16777216,15835575,28215755,25137402,23726565,21137982,18831809,// 63 2b Bb Gm
	17774828,15835575,28215755,25137402,23726565,21137982,18831809,// 70 3b Eb Cm
	17774828,15835575,28215755,26632135,23726565,21137982,18831809,// 77 4b Ab Fm
	17774828,15835575,28215755,26632135,23726565,21137982,19951607,// 84 5b Db Bbm
	17774828,15835575,29893558,26632135,23726565,21137982,19951607,// 91 6b Gb Ebm
	17774828,15835575,29893558,26632135,23726565,22394866,19951607,// 98 7b Cb Abm
};

/*
	2^(1/12)     ~= 1+1/16-1/256
	1/(2^(1/12)) ~= 1-1/16+1/128-1/512
*/

#define toneFlat(x) ((x)+((x)>>4)-((x)>>8))
#define toneSharp(x) ((x)-((x)>>4)+((x)>>7)-((x)>>9))

/* local global vars */
static int* g_tones;
static int g_qvalue;
static int g_lvalue;
static int g_mpoint;
static char* g_mstr;
static int g_mspos;
static int g_musicL[32];
static int g_soundL[32];
static unsigned short g_musiclenL[32];
static unsigned char g_soundlenL[32];
static int g_musicstartL;
static int g_musicendL;
static int g_musicwaitL;
static int g_soundstartL;
static int g_soundendL;
static int g_soundwaitL;
static int g_soundrepeatL;
static int g_musicR[32];
static int g_soundR[32];
static unsigned short g_musiclenR[32];
static unsigned char g_soundlenR[32];
static int g_musicstartR;
static int g_musicendR;
static int g_musicwaitR;
static int g_soundstartR;
static int g_soundendR;
static int g_soundwaitR;
static int g_soundrepeatR;

#define MFLAG_L 2
#define MFLAG_R 1

static char g_sound_mode=0;
static FSFILE* g_fhandle=0;
static unsigned char* wavtable=0;
static int wave_stereo;
#define wave_sector_size (wave_stereo ? 524 : 262)

#define SOUND_MODE_NONE 0
#define SOUND_MODE_MUSIC 1
#define SOUND_MODE_WAVE 2

#define start_wavedma() T4CONSET=0x8000; DCH1CONSET=DCH2CONSET=0x00000080
#define stop_wavedma()  T4CONCLR=0x8000; DCH1CONCLR=DCH2CONCLR=0x00000080; g_sound_mode=SOUND_MODE_NONE

void init_wave_dma();

int musicRemaining(int flagsLR){
	int l, r;
	l=(g_musicendL-g_musicstartL)&31;
	r=(g_musicendL-g_musicstartL)&31;
	if (flagsLR & MFLAG_L) return l;
	if (flagsLR & MFLAG_R) return r;
	return (l>r) ? l:r;
}

int waveRemaining(int mode){
	int ret;
	if (!g_fhandle) return 0;
	switch(mode){
		case 1: // current position (header excluded)
			ret=g_fhandle->seek-0x2c;
			break;
		case 2: // file size (header excluded)
			ret=g_fhandle->size-0x2c;
			break;
		case 0: // remaining
		default:
			ret=g_fhandle->size-g_fhandle->seek;
			break;
	}
	if (wave_stereo) ret=ret>>1;
	return ret;
}

void musicint(){
	static unsigned short wavtable_pos;
	static int music_freq_L=0;
	static int music_timer_L=0;
	static int music_freq_R=0;
	static int music_timer_R=0;
	unsigned int i,j;
	// This function is called every 1/60 sec.
	switch(g_sound_mode){
		case SOUND_MODE_WAVE:
			// Initialize parameters
			if (!T4CONbits.ON){
				wavtable_pos=0;
				music_timer_L=music_timer_R=0;
				start_wavedma();
			}
			wavtable_pos=wave_sector_size-wavtable_pos;
			// Read from file
			if (0 == FSfread((void*)&wavtable[wavtable_pos],1,wave_sector_size,g_fhandle)) {
				// End of file.
				stop_wavedma();
				FSfclose(g_fhandle);
				g_fhandle=0;
				g_sound_mode=SOUND_MODE_NONE;
				stop_music();
				// Raise WAVE interrupt flag
				raise_interrupt_flag(INTERRUPT_WAVE);
				break;
			}
			// Continue to MUSIC sound mode
		case SOUND_MODE_MUSIC:
			// Left sound/music
			if (g_soundstartL!=g_soundendL){
				// Start timer
				music_freq_L=g_soundL[g_soundstartL];
				if ((--g_soundwaitL)<=0) {
					g_soundstartL++;
					if (g_soundstartL==g_soundendL || 31<g_soundstartL) {
						g_soundstartL=0;
						g_soundrepeatL--;
						if (0<g_soundrepeatL) {
							g_soundwaitL=g_soundlenL[g_soundstartL];
						} else {
							g_soundendL=g_soundrepeatL=g_soundwaitL=0;
						}
					} else {
						g_soundwaitL=g_soundlenL[g_soundstartL];
					}
				}
				// Shift music data even though without output.
				if (g_musicstartL!=g_musicendL) {
					if ((--g_musicwaitL)<=0) {
						g_musicstartL++;
						g_musicstartL&=31;
						g_musicwaitL=g_musiclenL[g_musicstartL];
						if (((g_musicstartL+1)&31)==g_musicendL) {
							// Raise MUSIC interrupt flag
							raise_interrupt_flag(INTERRUPT_MUSIC);
						}
					}
				}
			} else if (g_musicstartL!=g_musicendL) {
				// Start timer
				music_freq_L=g_musicL[g_musicstartL];
				if ((--g_musicwaitL)<=0) {
					g_musicstartL++;
					g_musicstartL&=31;
					g_musicwaitL=g_musiclenL[g_musicstartL];
					if (((g_musicstartL+1)&31)==g_musicendL) {
						// Raise MUSIC interrupt flag
						raise_interrupt_flag(INTERRUPT_MUSIC);
					}
				}
			} else {
				music_freq_L=0;
			}
			// Right sound/music
			if (g_soundstartR!=g_soundendR){
				// Start timer
				music_freq_R=g_soundR[g_soundstartR];
				if ((--g_soundwaitR)<=0) {
					g_soundstartR++;
					if (g_soundstartR==g_soundendR || 31<g_soundstartR) {
						g_soundstartR=0;
						g_soundrepeatR--;
						if (0<g_soundrepeatR) {
							g_soundwaitR=g_soundlenR[g_soundstartR];
						} else {
							g_soundendR=g_soundrepeatR=g_soundwaitR=0;
						}
					} else {
						g_soundwaitR=g_soundlenR[g_soundstartR];
					}
				}
				// Shift music data even though without output.
				if (g_musicstartR!=g_musicendR) {
					if ((--g_musicwaitR)<=0) {
						g_musicstartR++;
						g_musicstartR&=31;
						g_musicwaitR=g_musiclenR[g_musicstartR];
						if (((g_musicstartR+1)&31)==g_musicendR) {
							// Raise MUSIC interrupt flag
							raise_interrupt_flag(INTERRUPT_MUSIC);
						}
					}
				}
			} else if (g_musicstartR!=g_musicendR) {
				// Start timer
				music_freq_R=g_musicR[g_musicstartR];
				if ((--g_musicwaitR)<=0) {
					g_musicstartR++;
					g_musicstartR&=31;
					g_musicwaitR=g_musiclenR[g_musicstartR];
					if (((g_musicstartR+1)&31)==g_musicendR) {
						// Raise MUSIC interrupt flag
						raise_interrupt_flag(INTERRUPT_MUSIC);
					}
				}
			} else {
				music_freq_R=0;
			}
			// music_freq_L/R is controlling music/sound.
			// If this value is valid, make sound with this frequency
			if (g_sound_mode!=SOUND_MODE_WAVE) {
				if (!T4CONbits.ON){
					// Initialize parameters
					wavtable_pos=0;
					music_timer_L=music_timer_R=0;
					start_wavedma();
					// Silent in the first sector
					for(i=0;i<wave_sector_size;i++) wavtable[i]=0x80;
				}
				wavtable_pos=wave_sector_size-wavtable_pos;
			}
			// Modify WAVE table (SOUND_MODE_WAVE) or construct WAVE table (SOUND_MODE_MUSIC)
			if (g_sound_mode==SOUND_MODE_WAVE && music_freq_L==0 && music_freq_R==0) {
				// Modification is not needed
				break;
			}
			for(i=0;i<wave_sector_size;i++){
				if (music_freq_L) {
					music_timer_L-=470189;
					if (music_timer_L<=0) {
						music_timer_L+=music_freq_L;
						j=0xc0;
					} else if (music_timer_L<=(music_freq_L>>1)) {
						j=0x40;
					} else {
						j=0xc0;
					}
				} else {
						j=0x80;
				}
				if (g_sound_mode==SOUND_MODE_WAVE) {
					// In WAVE mode, MUSIC/SOUND data is added to WAVE data
					j-=0x80;
					j+=wavtable[wavtable_pos+i];
					if (j<0x00) j=0;
					else if (0xff<j) j=0xff;
					wavtable[wavtable_pos+i]=j;
				} else {
					// In MUSIC mode, MUSIC/SOUND data is newly created
					wavtable[wavtable_pos+i]=j;
				}
				if (music_freq_R) {
					music_timer_R-=470189;
					if (music_timer_R<=0) {
						music_timer_R+=music_freq_R;
						j=0xc0;
					} else if (music_timer_R<=(music_freq_R>>1)) {
						j=0x40;
					} else {
						j=0xc0;
					}
				} else {
						j=0x80;
				}
				if (wave_stereo) {
					if (g_sound_mode==SOUND_MODE_WAVE) {
						// In WAVE mode, MUSIC/SOUND data is added to WAVE data
						j-=0x80;
						j+=wavtable[wavtable_pos+i+1];
						if (j<0x00) j=0;
						else if (0xff<j) j=0xff;
						wavtable[wavtable_pos+i+1]=j;
					} else {
						wavtable[wavtable_pos+i+1]=j;
						// In MUSIC mode, MUSIC/SOUND data is newly created
					}
					// Increment i here and in if statement (total addition 2 for stereo mode)
					i++;
				}	
			}
			break;
		case SOUND_MODE_NONE:
			music_freq_L=0;
			music_timer_L=0;
			music_freq_R=0;
			music_timer_R=0;
			// Move OC3RS and OC4RS to 0x80 if less or more.
			if (OC3RS<0x80) {
				OC3RS++;
			} else if (0x80<OC3RS) {
				OC3RS--;
			}
			if (OC4RS<0x80) {
				OC4RS++;
			} else if (0x80<OC4RS) {
				OC4RS--;
			}
			if (OC3RS==0x80 && OC4RS==0x80) {
				// Stop interrupt
				IEC0bits.CS0IE=0;
			}
			break;
		default:
			break;
	}
}

int musicGetNum(){
	int i, ret;
	char b;
	// Skip non number character
	for(i=0;(b=g_mstr[g_mspos+i])<'0' && '9'<g_mstr[g_mspos+i];i++);
	// Determine the number
	ret=0;
	while('0'<=b && b<='9'){
		ret*=10;
		ret+=b-'0';
		i++;
		b=g_mstr[g_mspos+i];
	}
	g_mspos+=i;
	return ret;
}

static const unsigned int inv_rf4[1]={0x00000010};
static const unsigned int inv_rf5[1]={0x00000020};

void init_normal_music(){
	// Use Timer3 and DMA2 for left (RF5)
	// Use Timer4 and DMA1 for right (RF4)
	stop_music();

	// Alocate 524*2 byte buffer if not assigned
	if (g_var_size[ALLOC_WAVE_BLOCK]==0) {
		wavtable=(char*)alloc_memory(524*2/4,ALLOC_WAVE_BLOCK);
	}

	// Set music mode
	g_sound_mode=SOUND_MODE_MUSIC;
	wave_stereo=1;

	// DMA setting 
	init_wave_dma();

	// Software interrupt every 1/60 sec (triggered by Timer2)
	IFS0bits.CS0IF=0;
	IEC0bits.CS0IE=1;	
}

void init_music(){
	// Currently this function is called in main.c and run.c
	stop_music();

	// Initializations for music/sound.
	g_qvalue=160; // Q: 1/4=90
	g_lvalue=20;   // L: 1/8
	g_tones=(int*)&(g_keys[49]); // C major
	g_musicstartL=g_musicendL=g_musicwaitL=g_soundstartL=g_soundendL=g_soundwaitL=g_soundrepeatL=0;
	g_musicstartR=g_musicendR=g_musicwaitR=g_soundstartR=g_soundendR=g_soundwaitR=g_soundrepeatR=0;

	// Timer3 for PWM
	T3CON=0x0000;
	PR3=0x100;
	TMR3=0;

	// OC4 setting
	RPF5R = 0x0b;    //Use RPF5 for OC4
	if (OC4RS&0xff00) OC4RS=0x00;
	OC4CON=0x000e;
	OC4CONSET=0x8000;
	// OC3 setting
	RPF4R = 0x0b;    //Use RPF4 for OC3
	if (OC3RS&0xff00) OC3RS=0x00;
	OC3CON=0x000e;
	OC3CONSET=0x8000;

	// Start timer3
	T3CON=0x8000;

	// Move OC4RS and OC3 RS from 0x00 to 0x80
	g_sound_mode=SOUND_MODE_NONE;
}

void musicSetL(){
	// Set length of a character.
	// Syntax: L:n/m, where n and m are numbers.
	int n,m;
	n=musicGetNum();
	g_mspos++;
	m=musicGetNum();
	g_lvalue=g_qvalue*n/m;
}

void musicSetQ(){
	int i;
	// Syntax: Q:1/4=n, where n is number.
	// Skip "1/4="
	for(i=0;g_mstr[g_mspos+i]!='=';i++);
	g_mspos+=i+1;
	i=musicGetNum();
	if      (i<48)  { g_qvalue=320; /* 1/4=45  */ }
	else if (i<53)  { g_qvalue=288; /* 1/4=50  */ }
	else if (i<60)  { g_qvalue=256; /* 1/4=56  */ }
	else if (i<70)  { g_qvalue=224; /* 1/4=64  */ }
	else if (i<83)  { g_qvalue=192; /* 1/4=75  */ }
	else if (i<102) { g_qvalue=160; /* 1/4=90  */ }
	else if (i<132) { g_qvalue=128; /* 1/4=113 */ }
	else if (i<188) { g_qvalue=96;  /* 1/4=150 */ }
	else            { g_qvalue=64;  /* 1/4=225 */ }
	g_lvalue=g_qvalue>>3;
}

void musicSetK(){
	// Syntax: K:xxx
	if (!strncmp((char*)&(g_mstr[g_mspos]),"A#m",3)) {
			g_mspos+=3;
			g_tones=(int*)&(g_keys[0]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"D#m",3)) {
			g_mspos+=3;
			g_tones=(int*)&(g_keys[7]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"G#m",3)) {
			g_mspos+=3;
			g_tones=(int*)&(g_keys[14]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"C#m",3)) {
			g_mspos+=3;
			g_tones=(int*)&(g_keys[21]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"F#m",3)) {
			g_mspos+=3;
			g_tones=(int*)&(g_keys[28]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Bbm",3)) {
			g_mspos+=3;
			g_tones=(int*)&(g_keys[84]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Ebm",3)) {
			g_mspos+=3;
			g_tones=(int*)&(g_keys[91]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Abm",3)) {
			g_mspos+=3;
			g_tones=(int*)&(g_keys[98]);
			return;
	}
	if (!strncmp((char*)&(g_mstr[g_mspos]),"C#",2)) {
			g_mspos+=2;
			g_tones=(int*)&(g_keys[0]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"F#",2)) {
			g_mspos+=2;
			g_tones=(int*)&(g_keys[7]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Bm",2)) {
			g_mspos+=2;
			g_tones=(int*)&(g_keys[35]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Em",2)) {
			g_mspos+=2;
			g_tones=(int*)&(g_keys[42]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Am",2)) {
			g_mspos+=2;
			g_tones=(int*)&(g_keys[49]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Dm",2)) {
			g_mspos+=2;
			g_tones=(int*)&(g_keys[56]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Gm",2) || !strncmp((char*)&(g_mstr[g_mspos]),"Bb",2)) {
			g_mspos+=2;
			g_tones=(int*)&(g_keys[63]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Cm",2) || !strncmp((char*)&(g_mstr[g_mspos]),"Eb",2)) {
			g_mspos+=2;
			g_tones=(int*)&(g_keys[70]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Fm",2) || !strncmp((char*)&(g_mstr[g_mspos]),"Ab",2)) {
			g_mspos+=2;
			g_tones=(int*)&(g_keys[77]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Db",2)) {
			g_mspos+=2;
			g_tones=(int*)&(g_keys[84]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Gb",2)) {
			g_mspos+=2;
			g_tones=(int*)&(g_keys[91]);
			return;
	} else if (!strncmp((char*)&(g_mstr[g_mspos]),"Cb",2)) {
			g_mspos+=2;
			g_tones=(int*)&(g_keys[98]);
			return;
	}
	switch(g_mstr[g_mspos]){
		case 'B':
			g_mspos++;
			g_tones=(int*)&(g_keys[14]);
			return;
		case 'E':
			g_mspos++;
			g_tones=(int*)&(g_keys[21]);
			return;
		case 'A':
			g_mspos++;
			g_tones=(int*)&(g_keys[28]);
			return;
		case 'D':
			g_mspos++;
			g_tones=(int*)&(g_keys[35]);
			return;
		case 'G':
			g_mspos++;
			g_tones=(int*)&(g_keys[42]);
			return;
		case 'C':
			g_mspos++;
			g_tones=(int*)&(g_keys[49]);
			return;
		case 'F':
			g_mspos++;
			g_tones=(int*)&(g_keys[56]);
			return;
		default:
			err_music(g_mstr);
			break;
	}
}
void musicSetM(){
	// Currently do nothing
	musicGetNum();
	musicGetNum();
}

void set_sound(unsigned long* data, int flagsLR){
	int sound;
	int len;
	int pos;
	int datalen;
	if (g_sound_mode==SOUND_MODE_NONE) {
		// Start normal music mode
		init_normal_music();
	}
	IEC0bits.CS0IE=0; // Stop interruption, first.
	// Initialize
	if (flagsLR & MFLAG_L) g_soundrepeatL=g_soundstartL=g_soundendL=0;
	if (flagsLR & MFLAG_R) g_soundrepeatR=g_soundstartR=g_soundendR=0;
	pos=0;
	do {
		while(data[1]!=0x00000020) data++; // Seek DATA statement
		datalen=(data[0]&0x00007FFF)-1;    // Use bgezal statement containing data length.
		data+=2;
		while(0<datalen){
			datalen--;
			len=data[0]>>16;
			sound=data[0]&0x0000FFFF;
			data++;
			if (len) {
				// Shift tone for 2048 <--> ~440 Hz.
				// 2048 = 2^11
				// 16777216 = 2^24
				sound=sound<<13;
				if (flagsLR & MFLAG_L) g_soundL[pos]=sound;
				if (flagsLR & MFLAG_R) g_soundR[pos]=sound;
				if (flagsLR & MFLAG_L) g_soundlenL[pos]=len;
				if (flagsLR & MFLAG_R) g_soundlenR[pos]=len;
				pos++;
				if (32<pos) {
					err_music("Sound data too long.");
					return;
				}
			} else {
				if (flagsLR & MFLAG_L) g_soundrepeatL=sound;
				if (flagsLR & MFLAG_R) g_soundrepeatR=sound;
				break;
			}
		}
	} while(len);
	if (flagsLR & MFLAG_L) g_soundendL=pos;
	if (flagsLR & MFLAG_R) g_soundendR=pos;
	if (flagsLR & MFLAG_L) g_soundwaitL=g_soundlenL[0];
	if (flagsLR & MFLAG_R) g_soundwaitR=g_soundlenR[0];
	IEC0bits.CS0IE=1; // Restart interrupt.
	g_music_active=1;// Activate music system
}

void set_music(char* str, int flagsLR){
	char b;
	unsigned long tone,tonenatural;
	int len;
	if (g_sound_mode==SOUND_MODE_NONE) {
		// Start normal music mode
		init_normal_music();
	}
	g_mstr=str;
	g_mspos=0;
	while(0<(b=g_mstr[g_mspos])){
		if (g_mstr[g_mspos+1]==':') {
			// Set property
			g_mspos+=2;
			switch(b){
				case 'L':
					musicSetL();
					break;
				case 'Q':
					musicSetQ();
					break;
				case 'K':
					musicSetK();
					break;
				case 'M':
					musicSetM();
					break;
				default:
					err_music(str);
					break;
			}	
		} else if ('A'<=b && b<='G' || 'a'<=b && b<='g' || b=='z') {
			g_mspos++;
			if (b=='z') {
				tone=0;
			} else if (b<='G') {
				tone=g_tones[b-'A'];
				tonenatural=g_keys[b-'A'+49];
			} else {
				tone=g_tones[b-'a']>>1;
				tonenatural=g_keys[b-'a'+49]>>1;
			}
			// Check "'"s
			while(g_mstr[g_mspos]=='\''){
				g_mspos++;
				tone>>=1;
			}
			// Check ","s
			while(g_mstr[g_mspos]==','){
				g_mspos++;
				tone<<=1;
				tonenatural<<=1;
			}
			// Check "^","=","_"
			switch(g_mstr[g_mspos]){
				case '^':
					g_mspos++;
					tone=toneSharp(tone);
					break;
				case '_':
					g_mspos++;
					tone=toneFlat(tone);
					break;
				case '=':
					g_mspos++;
					tone=tonenatural;
					break;
				default:
					break;
			}
			// Check number for length
			b=g_mstr[g_mspos];
			if ('0'<=b && b<='9') {
				len=g_lvalue*musicGetNum();
			} else {
				len=g_lvalue;
			}
			if (g_mstr[g_mspos]=='/') {
				g_mspos++;
				len=len/musicGetNum();
			}
			// Update music value array
			IEC0bits.CS0IE=0; // Stop interruption, first.
			// Update left music
			if (flagsLR & MFLAG_L) {
				if (g_musicstartL==g_musicendL) {
					g_musicwaitL=len;
				}
				g_musicL[g_musicendL]=tone;
				g_musiclenL[g_musicendL]=len;
				g_musicendL++;
				g_musicendL&=31;
			}
			// Update right music
			if (flagsLR & MFLAG_R) {
				if (g_musicstartR==g_musicendR) {
					g_musicwaitR=len;
				}
				g_musicR[g_musicendR]=tone;
				g_musiclenR[g_musicendR]=len;
				g_musicendR++;
				g_musicendR&=31;
			}
			IEC0bits.CS0IE=1; // Restart interruption.
		} else {
			err_music(str);
		}
		// Go to next character
		while(0<g_mstr[g_mspos] && g_mstr[g_mspos]<=0x20 || g_mstr[g_mspos]=='|') g_mspos++;
	}
	g_music_active=1;// Activate music system
}

/*
	In WAVE mode (for both stereo and monaural):
		DMA2/OC4 is used for left
		DMA1/OC3 is used for right
		Timer 3 is used for PWM for both L/R
		Timer 4 is used for both DMA1/DMA2
*/

int checkChars(char* str1, char* str2, int num){
	int i;
	for(i=0;i<num;i++){
		if (str1[i]!=str2[i]) return 1;
	}
	return 0;
}

FSFILE* openWave(char* file){
	FSFILE *fp;
	int i;

	// Open Wave file
	fp=FSfopen(file,"r");
	if(fp==NULL) err_file();

	// Read the header
	if (0x2c != FSfread((void*)&wavtable[0],1,0x2c,fp)) {
		err_wave();
	}
	i=0;
	i+=checkChars((char*)&wavtable[0],"RIFF",4);                      // Check RIFF
	i+=checkChars((char*)&wavtable[8],"WAVEfmt ",8);                  // Check WAVE and fmt
	i+=checkChars((char*)&wavtable[16],"\x10\x00\x00\x00\x01\x00",6); // Check if liear PCM
	if (!checkChars((char*)&wavtable[22],"\x02\x00\x80\x3e\x00\x00\x00\x7d\x00\x00\x02\x00",12)) {
		// Stereo 16000 Hz
		wave_stereo=1;
	} else if (!checkChars((char*)&wavtable[22],"\x01\x00\x80\x3e\x00\x00\x80\x3e\x00\x00\x01\x00",12)) {
		// Monaural 16000 Hz
		wave_stereo=0;
	} else if (!checkChars((char*)&wavtable[22],"\x02\x00\x54\x3d\x00\x00\xa8\x7a\x00\x00\x02\x00",12)) {
		// Stereo 15700 Hz
		wave_stereo=1;
	} else if (!checkChars((char*)&wavtable[22],"\x01\x00\x54\x3d\x00\x00\x54\x3d\x00\x00\x01\x00",12)) {
		// Monaural 15700 Hz
		wave_stereo=0;
	} else {
		i=1;
	}
	i+=checkChars((char*)&wavtable[34],"\x08\x00\x64\x61\x74\x61",6); // Check bit # and data
	if (i) {
		err_wave();
	}
	return fp;
}

void init_wave_dma(){
	// Timer4 for 15700 Hz
	TMR4=0;
	PR4=6080-1;
	TMR4=PR4-1;
	T4CON=0x0000; // Not start yet

	if (wave_stereo) {
		//DMA2 settings for OC4 for left
		DMACONSET=0x8000;
		DCH2CON=0x00000012;  // CHBUSY=0, CHCHNS=0, CHEN=0, CHAED=0, CHCHN=0, CHAEN=1, CHEDET=0, CHPRI=b10
		DCH2ECON=0x1310;     // CHAIRQ=0, CHSIRQ=19, CFORCE=0, CABRT=0, PATEN=0, SIRQEN=1, AIRQEN=0
		                     // CHSIRQ=19: Timer4 interrupt
		DCH2SSA=((unsigned int)&(wavtable[-1]))&0x1fffffff;
		DCH2DSA=0x1F803620-1; // OC4RS
		DCH2SSIZ=524*2;
		DCH2DSIZ=2;
		DCH2CSIZ=2;
		DCH2INTCLR=0x00FF00FF;
		DCH2CONSET=0x00000080;
	
		//DMA1 settings for OC3 for rifht
		DMACONSET=0x8000;
		DCH1CON=0x00000012;  // CHBUSY=0, CHCHNS=0, CHEN=0, CHAED=0, CHCHN=0, CHAEN=1, CHEDET=0, CHPRI=b10
		DCH1ECON=0x1310;     // CHAIRQ=0, CHSIRQ=19, CFORCE=0, CABRT=0, PATEN=0, SIRQEN=1, AIRQEN=0
		                     // CHSIRQ=19: Timer4 interrupt
		DCH1SSA=((unsigned int)&(wavtable[0]))&0x1fffffff;
		DCH1DSA=0x1F803420-1; // OC3RS
		DCH1SSIZ=524*2;
		DCH1DSIZ=2;
		DCH1CSIZ=2;
		DCH1INTCLR=0x00FF00FF;
		DCH1CONSET=0x00000080;
	} else {
		//DMA2 settings for OC4 for left
		DMACONSET=0x8000;
		DCH2CON=0x00000012;  // CHBUSY=0, CHCHNS=0, CHEN=0, CHAED=0, CHCHN=0, CHAEN=1, CHEDET=0, CHPRI=b10
		DCH2ECON=0x1310;     // CHAIRQ=0, CHSIRQ=19, CFORCE=0, CABRT=0, PATEN=0, SIRQEN=1, AIRQEN=0
		                     // CHSIRQ=19: Timer4 interrupt
		DCH2SSA=((unsigned int)&(wavtable[0]))&0x1fffffff;
		DCH2DSA=0x1F803620; // OC4RS
		DCH2SSIZ=524;
		DCH2DSIZ=1;
		DCH2CSIZ=1;
		DCH2INTCLR=0x00FF00FF;
		DCH2CONSET=0x00000080;
	
		//DMA1 settings for OC3 for rifht
		DMACONSET=0x8000;
		DCH1CON=0x00000012;  // CHBUSY=0, CHCHNS=0, CHEN=0, CHAED=0, CHCHN=0, CHAEN=1, CHEDET=0, CHPRI=b10
		DCH1ECON=0x1310;     // CHAIRQ=0, CHSIRQ=19, CFORCE=0, CABRT=0, PATEN=0, SIRQEN=1, AIRQEN=0
		                     // CHSIRQ=19: Timer4 interrupt
		DCH1SSA=((unsigned int)&(wavtable[0]))&0x1fffffff;
		DCH1DSA=0x1F803420; // OC3RS
		DCH1SSIZ=524;
		DCH1DSIZ=1;
		DCH1CSIZ=1;
		DCH1INTCLR=0x00FF00FF;
		DCH1CONSET=0x00000080;
	}

}

void play_wave(char* filename, int start){
	// First of all, stop music.
	stop_music();
	// Exit function if null filename
	if (filename[0]==0x00) {
		OC3RS=OC4RS=0x80;
		return;
	}
	// Alocate 524*2 byte buffer if not assigned
	if (g_var_size[ALLOC_WAVE_BLOCK]==0) {
		wavtable=(char*)alloc_memory(524*2/4,ALLOC_WAVE_BLOCK);
	}
	// Open file
	if (g_fhandle) FSfclose(g_fhandle);
	g_fhandle=openWave(filename);
	// Support defined start position here to skip file pointer here.
	if (wave_stereo) start=start<<1;
	FSfseek(g_fhandle, start, SEEK_CUR);
	// Read first 262/524 words.
	if (wave_sector_size != FSfread((void*)&wavtable[0],1,wave_sector_size,g_fhandle)) err_file();
	// Initialize DMA
	init_wave_dma();	
	g_sound_mode=SOUND_MODE_WAVE;
	// Enable intterupt
	IFS0bits.CS0IF=0;
	IEC0bits.CS0IE=1;
	g_music_active=1;// Activate music system
}

void stop_music(){
	// Set NONE sound mode
	g_sound_mode=SOUND_MODE_NONE;
	// Stop DMA1 and DMA2
	DCH1CONCLR=0x00000080;
	DCH2CONCLR=0x00000080;
	// Close WAVE file if open
	if (g_fhandle) {
		FSfclose(g_fhandle);
		g_fhandle=0;
	}
	// Inactive music
	g_music_active=0;
}

/*
   This file is provided under the LGPL license ver 2.1.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

#include <xc.h>
#include "main.h"
#include "compiler.h"
#include "api.h"

/*
	void printcomma();
	30, 36, 40, 48, 64, 80 characters per line for Megalopa
*/

void printcomma(void){
	switch(textmode){
		case TMODE_STDTEXT: // 36
			// Every 9 characters
			printstr("         "+rem9_32((unsigned int)(cursor-TVRAM)));
			break;
		case TMODE_WIDETEXT: // 48
		case TMODE_WIDETEXT6dot: // 64
			// Every 8 characters
			printstr("        "+rem8_32((unsigned int)(cursor-TVRAM)));
			break;
		case TMODE_T30: // 30
		case TMODE_T40: // 40
		case TMODE_MONOTEXT: // 80
		default:
			// Every 10 characters
			printstr("          "+rem10_32((unsigned int)(cursor-TVRAM)));
			break;
	}
}

/*
	int readbuttons();
	Read the tact switches.
	For Zoea, disable PS/2 keyboard and enable tact switches, then read.
*/

int readbuttons(){
	return KEYPORT;
}

/*
	void pre_run(void);
	void post_run(void);
	Called before after execution of BASIC code.
*/

void pre_run(void){
	// Reset PWM
	lib_pwm(0,0,0);
}
void post_run(void){
	if (graphmode==GMODE_ZOEAGRPH) {
		// Stop graph mode when using Zoea compatible one.
		usegraphic(0);
		g_use_graphic=0;
	}
	// Stop peripherals
	lib_i2c(0);
	lib_serial(0,0,0);
	lib_spi(0,0,0,0);
}


/*
	void scroll(int x, int y);
	Scroll 
*/

void scroll(int x,int y){
	int i,j;
	int vector=y*twidth+x;
	if (vector<0) {
		// Copy data from upper address to lower address
		for(i=0-vector;i<twidth*WIDTH_Y;i++){
			TVRAM[i+vector]=TVRAM[i];
			TVRAM[twidth*WIDTH_Y+i+vector]=TVRAM[twidth*WIDTH_Y+i];
		}
	} else if (0<vector) {
		// Copy data from lower address to upper address
		for(i=twidth*WIDTH_Y-vector-1;0<=i;i--){
			TVRAM[i+vector]=TVRAM[i];
			TVRAM[twidth*WIDTH_Y+i+vector]=TVRAM[twidth*WIDTH_Y+i];
		}
	} else {
		return;
	}
	if (x<0) {
		// Fill blanc at right
		for(i=x;i<0;i++){
			for(j=twidth+i;j<twidth*WIDTH_Y;j+=twidth){
				TVRAM[j]=0x00;
				TVRAM[twidth*WIDTH_Y+j]=cursorcolor;
			}
		}
	} else if (0<x) {
		// Fill blanc at left
		for(i=0;i<x;i++){
			for(j=i;j<twidth*WIDTH_Y;j+=twidth){
				TVRAM[j]=0x00;
				TVRAM[twidth*WIDTH_Y+j]=cursorcolor;
			}
		}
	}
	if (y<0) {
		// Fill blanc at bottom
		for(i=twidth*(WIDTH_Y+y);i<twidth*WIDTH_Y;i++){
				TVRAM[i]=0x00;
				TVRAM[twidth*WIDTH_Y+i]=cursorcolor;
		}
	} else if (0<y) {
		// Fill blanc at top
		for(i=0;i<twidth*y;i++){
				TVRAM[i]=0x00;
				TVRAM[twidth*WIDTH_Y+i]=cursorcolor;
		}
	}
}

void allocate_graphic_area(int mode){
	static int prevmode=-1;
	if (g_graphic_area) {
		if (mode==prevmode) {
			// Do nothing
			return;
		} else {
			// Clear previous area here
			free_non_temp_str((char*)g_graphic_area);
			g_graphic_area=0;
		}
	}
	switch (mode) {
		case 0:
			g_graphic_area=alloc_memory(X_RESZ*Y_RESZ/2/4,ALLOC_GRAPHIC_BLOCK);
			break;
		case 1:
			g_graphic_area=alloc_memory(X_RES*Y_RES/4,ALLOC_GRAPHIC_BLOCK);
			break;
		case 2:
			g_graphic_area=alloc_memory(X_RESW*Y_RES/4,ALLOC_GRAPHIC_BLOCK);
			break;
		default:
			err_invalid_param();
	}
	// Store current graphic mode
	prevmode=mode;
}

void start_graphic(int mode){
	if (!g_graphic_area) return;
	switch (mode) {
		case 0:
			set_videomode(VMODE_ZOEAGRPH,(unsigned char *)g_graphic_area);
			break;
		case 1:
			set_videomode(VMODE_STDGRPH,(unsigned char *)g_graphic_area);
			break;
		case 2:
			set_videomode(VMODE_WIDEGRPH,(unsigned char *)g_graphic_area);
			break;
		default:
			err_invalid_param();
	}
}

void usegraphic(int mode){
	if (mode<0 || 11<mode) err_invalid_param();
	switch(mode & 3){
		// Modes; 0: stop GRAPHIC, 1: use GRAPHIC, 2: reset GRAPHIC and use it, 3: allocate GRAPHIC area but not use it
		case 0:
			if (g_use_graphic){
				// Stop GRAPHIC if used
				set_videomode(textmode,0);
				g_use_graphic=0;
			} else {
				// Prepare GRAPHIC area if not used and not allcated.
				allocate_graphic_area(mode>>2);
			}
			break;
		case 2:
			// Reset GRAPHIC and use it
			if (g_graphic_area) {
				g_clearscreen();
				init_palette();
			}
			// Continue to case 1:
		case 1:
		case 3:
		default:
			// Use GRAPHIC
			allocate_graphic_area(mode>>2);
			// Start showing GRAPHIC with mode 1, but not with mode 3
			if ((mode & 3) !=3 && !g_use_graphic){
				// Change to graphic mode.
				start_graphic(mode>>2);
				g_use_graphic=1;
			}
			break;
	}
}

int lib_system(int a0, int a1 ,int v0, int a3, int g_gcolor, int g_prev_x, int g_prev_y){
	switch((enum extra)(a3 & EXTRA_MASK)){
		case EXTRA_SYSTEM:
			// SYSTEM statement/function (see below)
			break;
		case EXTRA_OUT:
			lib_out(g_libparams[1],v0);
			return v0;
		case EXTRA_OUT8H:
			lib_out8h(v0);
			return v0;
		case EXTRA_OUT8L:
			lib_out8l(v0);
			return v0;
		case EXTRA_OUT16:
			lib_out16(v0);
			return v0;
		case EXTRA_IN:
			return lib_in(v0);
		case EXTRA_IN8H:
			return lib_in8h();
		case EXTRA_IN8L:
			return lib_in8l();
		case EXTRA_IN16:
			return lib_in16();
		case EXTRA_ANALOG:
			return lib_analog(v0);
		case EXTRA_PWM:
			lib_pwm(g_libparams[1],g_libparams[2],v0);
			return v0;
		case EXTRA_SERIAL:
			lib_serial(g_libparams[1],g_libparams[2],v0);
			return v0;
		case EXTRA_SERIALOUT:
			lib_serialout(v0);
			return v0;
		case EXTRA_SERIALIN:
			return lib_serialin(v0);
		case EXTRA_SPI:
			lib_spi(g_libparams[1],g_libparams[2],g_libparams[3],v0);
			return v0;
		case EXTRA_SPIWRITE:
			lib_spiwrite(v0,g_libparams);
			return v0;
		case EXTRA_SPIREAD:
			return lib_spiread(v0,g_libparams);
		case EXTRA_SPIWRITEDATA:
			lib_spiwritedata(v0,g_libparams+2,g_libparams[2],(unsigned int*)g_libparams[1]);
			return v0;
		case EXTRA_SPIREADDATA:
			lib_spireaddata(v0,g_libparams+2,g_libparams[2],(unsigned int*)g_libparams[1]);
			return v0;
		case EXTRA_SPISWAPDATA:
			lib_spiswapdata(v0,g_libparams+2,g_libparams[2],(unsigned int*)g_libparams[1]);
			return v0;
		case EXTRA_I2C:
			lib_i2c(v0);
			return v0;
		case EXTRA_I2CWRITE:
			lib_i2cwrite(v0,g_libparams);
			return v0;
		case EXTRA_I2CREAD:
			return lib_i2cread(v0,g_libparams);
		case EXTRA_I2CWRITEDATA:
			lib_i2cwritedata(v0,g_libparams+2,g_libparams[2],(unsigned char*)g_libparams[1]);
			return v0;
		case EXTRA_I2CREADDATA:
			lib_i2creaddata(v0,g_libparams+2,g_libparams[2],(unsigned char*)g_libparams[1]);
			return v0;
		case EXTRA_I2CERROR:
			return lib_i2cerror();
		default:
			err_unknown();
			return v0;
	}
	switch(a0){
		// Version info etc
		case 0: return (int)SYSVER1;
		case 1: return (int)SYSVER2;
		case 2: return (int)BASVER;
		case 3: return (int)FILENAME_FLASH_ADDRESS;
		case 4: return (int)CPU_CLOCK_HZ;
		// Display info
		case 20: return twidth;
		case 21: return twidthy;
		case 22: return gwidth;
		case 23: return gwidthy;
		case 24: return cursorcolor;
		case 25: return g_gcolor;
		case 26: return ((int)(cursor-TVRAM))%twidth;
		case 27: return ((int)(cursor-TVRAM))/twidth;
		case 28: return g_prev_x;
		case 29: return g_prev_y;
		// Keyboard info
		case 40: return (int)inPS2MODE();
		case 41: return (int)vkey;
		case 42: return (int)lockkey;
		case 43: return (int)keytype;
		// Pointers to gloval variables
		case 100: return (int)&g_var_mem[0];
		case 101: return (int)&g_rnd_seed;
		case 102: return (int)&TVRAM[0];
		case 103: return (int)&FontData[0];
		case 104: return (int)g_var_mem[ALLOC_PCG_BLOCK];
		case 105: return (int)g_var_mem[ALLOC_GRAPHIC_BLOCK];
		// Change system settings
		case 200:
			// ON/OFF monitor
			if (v0) {
				start_composite();
			} else {
				stop_composite();
			}
			break;
		default:
			break;
	}
	return 0;
}

void videowidth(int width){
	switch(width){
		case 30:
			set_videomode(VMODE_T30,0);
			break;
		case 36:
			set_videomode(VMODE_STDTEXT,0);
			break;
		case 40:
			set_videomode(VMODE_T40,0);
			break;
		case 48:
			set_videomode(VMODE_WIDETEXT,0);
			break;
		case 64:
			set_videomode(VMODE_WIDETEXT6dot,0);
			break;
		case 80:
			set_videomode(VMODE_MONOTEXT,0);
			break;
		default:
			// Do nothing
			return;
	}
	g_use_graphic=0;
}

void set_graphmode(unsigned char m){
	if (m==0) set_videomode(VMODE_T30,0);
}

/*
	Environ specific error handling routines follow
*/

void pre_end_addr(int s6);

#define end_exec() \
	asm volatile("addu $a0,$s6,$zero");\
	asm volatile("j pre_end_addr")


void err_peri_not_init(void){
	printstr("peripheral not initiated.");
	end_exec();
}
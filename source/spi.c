/*
   This file is provided under the LGPL license ver 2.1.
   Written by K.Tanaka & Katsumi
   http://www.ze.em-net.ne.jp/~kenken/index.html
   http://hp.vector.co.jp/authors/VA016157/
*/

#include <xc.h>
#include "compiler.h"

volatile unsigned int* g_csaddress;
unsigned int g_csbit;
unsigned int g_wordmask;

/*
	enable_cs();
	LATxCLR=xxxx
*/
void enable_cs(){
	g_csaddress[1]=g_csbit;
}

/*
	disable_cs();
	LATxSET=xxxx
*/
void disable_cs(){
	g_csaddress[2]=g_csbit;
}

void idle(){
	while (SPI1STATbits.SPIBUSY);
}

#define writeWord(x) do {\
		SPI1BUF = ((x)&g_wordmask);\
		while (SPI1STATbits.SPIBUSY);\
		SPI1BUF;\
	} while(0)	

#define readWord() writeReadWord(g_wordmask)

unsigned int writeReadWord(unsigned int word) {
	SPI1BUF = word&g_wordmask;
	while (SPI1STATbits.SPIBUSY);
	return SPI1BUF&g_wordmask;
}

/*
	Library functions follow
*/

void lib_spi(int baud, int bitmode, int clkmode, int csdata){
	/*
		Fsck=Fpb/(2 x (SPIxBRG+1))
		SPIxBRG=Fpb/Fsck/2 - 1
		Fpb=95454533; Fsck is defined as kHz
		SPIxBRG=47727/Fsck -1
		note that SPIxBRG is a 9 bit value (0-511)

		mode is either 8 (default), 16, or 32

		csdata is 8 bit value to define port for /CS
		Upper 4 bit is between 1-6, corresponding ports B-G.
		Lower 4 bit is between 0-15, corresponding port bit.
		For the case of D9 (default), it is 0x39

			LATB = 0xBF886130
				LATBCLR = 0xBF886134
				LATBSET = 0xBF886138
				TRISBCLR = 0xBF886114
			LATC = 0xBF886230
			LATD = 0xBF886330
			LATE = 0xBF886430
			LATF = 0xBF886530
			LATG = 0xBF886630
	*/
	volatile unsigned int* trisxclr;
	if (baud==0) {
		if (SPI1CONbits.ON) disable_cs();
		SPI1CONbits.ON=0;
		return;
	}
	// Baud rate
	int brg=(47727 + baud/2)/baud-1;
	if (brg<0) brg=0;          // Max 47.7 MHz
	else if (511<brg) brg=511; // Min 93.2 kHz
	// Disable SPI, first
	SPI1CONbits.ON=0;
	// CS setting
	if (0x10<=csdata && csdata<0x70) {
		// Set cs global valiables
		g_csaddress=(unsigned int*)(0xBF886030| ((csdata&0xF0)<<4));
		g_csbit=1<<(csdata&0x0F);
		// Set port for output
		trisxclr=(unsigned int*)(0xBF886014| ((csdata&0xF0)<<4));
		trisxclr[0]=g_csbit;
		// Disable CS
		disable_cs();
	} else {
		err_invalid_param();
	}
	// Other port settings (see above for CS port)
	TRISGbits.TRISG9=0; // SDO1
	TRISFbits.TRISF2=1; // SDI1
	TRISFbits.TRISF6=0; // SCK
	RPG9R=8;            // RPG9:SDO1
	SDI1R=14;           // RPF2:SDI1
	// Do not use frame sync
	SPI1CONbits.FRMEN=0;
	SPI1CONbits.FRMSYNC=0;
	SPI1CONbits.FRMPOL=0;
	SPI1CONbits.MSSEN=0;
	SPI1CONbits.FRMSYPW=0;
	SPI1CONbits.FRMCNT=0;
	SPI1CONbits.SPIFE=0;
	// PBCLK is used
	SPI1CONbits.MCLKSEL=0;
	// Disable enhanced buffer
	SPI1CONbits.ENHBUF=0;
	// Continue in idle mode
	SPI1CONbits.SIDL=0;
	// Do not disable SDO
	SPI1CONbits.DISSDO=0;
	// 8/16/32 bit mode
	switch(bitmode){
		case 32:
			SPI1CONbits.MODE32=1;
			SPI1CONbits.MODE16=0;
			g_wordmask=0xFFFFFFFF;
			break;
		case 16:
			SPI1CONbits.MODE32=0;
			SPI1CONbits.MODE16=1;
			g_wordmask=0x0000FFFF;
			break;
		case 8:
		default:
			SPI1CONbits.MODE32=0;
			SPI1CONbits.MODE16=0;
			g_wordmask=0x000000FF;
			break;
	}
	// Input data sampled at end of data output time
	SPI1CONbits.SMP=1;
	// CKE=0(default): Serial output data changes on transition from Idle clock state to active clock state
	SPI1CONbits.CKE=(clkmode&1) ? 0:1;
	// Not slave mode
	SPI1CONbits.SSEN=0;
	// CKP=1(default): Idle state for clock is a high level; active state is a low level.
	SPI1CONbits.CKP=(clkmode&2) ? 1:0;
	// Master mode
	SPI1CONbits.MSTEN=1;
	// SDI pin is controlled by the SPI module
	SPI1CONbits.DISSDI=0;
	// Interrupt is generated when the last transfer is shifted out of SPISR and transmit operations are complete
	// Interrupt is generated when the last word in the receive buffer is read (i.e., buffer is empty)
	// But interrupt is not used.
	SPI1CONbits.STXISEL=0;
	SPI1CONbits.SRXISEL=0;
	SPI1CON2=0x0300;
	// SPI clock setting (see above)
	SPI1BRG=brg;
	// Enable SPI module
	SPI1CONbits.ON=1;
}

void lib_spiwrite(int num, int* data){
	int i;
	// Check if initiated
	if (!SPI1CONbits.ON) err_peri_not_init();
	// Start signal
	idle();
	enable_cs();
	// Send words
	for(i=1;i<=num;i++){
		writeWord(data[i]);
	}
	// End signal
	idle();
	disable_cs();
}

void lib_spiwritedata(int num1, int* data1, int num2, unsigned int* data2){
	int i;
	// Check if initiated
	if (!SPI1CONbits.ON) err_peri_not_init();
	// Start signal
	idle();
	enable_cs();
	// Send words from parameters
	for(i=1;i<=num1;i++){
		writeWord(data1[i]);
	}
	// Send words from dimension
	if (g_wordmask>>16) {
		// 32 bit mode
		for(i=0;i<num2;i++){
			writeWord(((unsigned int*)data2)[i]);
		}
	} else if (g_wordmask>>8) {
		// 16 bit mode
		for(i=0;i<num2;i++){
			writeWord(((unsigned short*)data2)[i]);
		}
	} else {
		// 8 bit mode
		for(i=0;i<num2;i++){
			writeWord(((unsigned char*)data2)[i]);
		}
	}
	// End signal
	idle();
	disable_cs();
}

unsigned int lib_spiread(int num, int* data){
	unsigned int i;
	// Check if initiated
	if (!SPI1CONbits.ON) err_peri_not_init();
	// Start signal
	idle();
	enable_cs();
	// Send words
	for(i=1;i<=num;i++){
		writeWord(data[i]);
	}
	// Receive word
	i=readWord();
	// End signal
	idle();
	disable_cs();
	return i;
}

void lib_spireaddata(int num1, int* data1, int num2, unsigned int* data2){
	int i;
	// Check if initiated
	if (!SPI1CONbits.ON) err_peri_not_init();
	// Start signal
	idle();
	enable_cs();
	// Send words from parameters
	for(i=1;i<=num1;i++){
		writeWord(data1[i]);
	}
	// Read words and put them into dimension
	if (g_wordmask>>16) {
		// 32 bit mode
		for(i=0;i<num2;i++){
			((unsigned int*)data2)[i]=readWord();
		}
	} else if (g_wordmask>>8) {
		// 16 bit mode
		for(i=0;i<num2;i++){
			((unsigned short*)data2)[i]=readWord();
		}
	} else {
		// 8 bit mode
		for(i=0;i<num2;i++){
			((unsigned char*)data2)[i]=readWord();
		}
	}
	// End signal
	idle();
	disable_cs();
}

void lib_spiswapdata(int num1, int* data1, int num2, unsigned int* data2){
	int i;
	// Check if initiated
	if (!SPI1CONbits.ON) err_peri_not_init();
	// Start signal
	idle();
	enable_cs();
	// Send words from parameters
	for(i=1;i<=num1;i++){
		writeWord(data1[i]);
	}
	// Read words and put them into dimension
	if (g_wordmask>>16) {
		// 32 bit mode
		for(i=0;i<num2;i++){
			((unsigned int*)data2)[i]=writeReadWord(((unsigned int*)data2)[i]);
		}
	} else if (g_wordmask>>8) {
		// 16 bit mode
		for(i=0;i<num2;i++){
			((unsigned short*)data2)[i]=writeReadWord(((unsigned short*)data2)[i]);
		}
	} else {
		// 8 bit mode
		for(i=0;i<num2;i++){
			((unsigned char*)data2)[i]=writeReadWord(((unsigned char*)data2)[i]);
		}
	}
	// End signal
	idle();
	disable_cs();
}


/*
	Statements and function follow
*/

char* spi_statement(){
	char* err;
	// Get 1st parameter
	// SPI frequency
	err=get_value();
	if (err) return err;
	check_obj_space(2);
	g_object[g_objpos++]=0x27BDFFF4; // addiu       sp,sp,-12
	g_object[g_objpos++]=0xAFA20004; // sw          v0,4(sp)
	// Get 2nd parameter
	// Either 8/16/31 bit word length. 8 is the default.
	if (g_source[g_srcpos]==',') {
		g_srcpos++;
		err=get_value();
		if (err) return err;
	} else {
		check_obj_space(1);
		g_object[g_objpos++]=0x34020008; //ori         v0,zero,8
	}
	check_obj_space(1);
	g_object[g_objpos++]=0xAFA20008; // sw          v0,8(sp)
	// Get 3rd parameter
	// Either 0,1,2,3; 0 is the default.
	if (g_source[g_srcpos]==',') {
		g_srcpos++;
		err=get_value();
		if (err) return err;
	} else {
		check_obj_space(1);
		g_object[g_objpos++]=0x34020000; //ori         v0,zero,0
	}
	check_obj_space(1);
	g_object[g_objpos++]=0xAFA2000C; // sw          v0,12(sp)
	// Get 4th parameter
	// Port for CS. PORTD9 (0x39) is the default.
	if (g_source[g_srcpos]==',') {
		g_srcpos++;
		err=get_value();
		if (err) return err;
	} else {
		check_obj_space(1);
		g_object[g_objpos++]=0x34020039; //ori         v0,zero,0x39
	}
	// Insert calling system code
	call_lib_code(LIB_SYSTEM | EXTRA_SPI);
	check_obj_space(1);
	g_object[g_objpos++]=0x27BD000C; // addiu       sp,sp,12
	return 0;
}

// Local prototyping: use I2C routines for SPI statements
char* i2cwrite_read(enum libs lib);

char* spiwrite_statement(){
	return i2cwrite_read(LIB_SYSTEM | EXTRA_SPIWRITE);
}
char* spiread_function(){
	next_position();
	if (g_source[g_srcpos]!=')') {
		// There is/are parameter(s).
		return i2cwrite_read(LIB_SYSTEM | EXTRA_SPIREAD);
	}
	// No parameter. Set $v0=0 and call library
	check_obj_space(1);
	g_object[g_objpos++]=0x34020000; //ori         v0,zero,0
	// Call library
	call_lib_code(LIB_SYSTEM | EXTRA_SPIREAD);
	return 0;	
}

char* spiwritedata_readdata(enum libs lib){
	char* err;
	int opos;
	int stack=8;
	// Prepare stack
	opos=g_objpos;
	check_obj_space(1);
	g_object[g_objpos++]=0x27BD0000;              // addiu       sp,sp,-xxxx
	// Get buffer address
	err=get_value();
	if (err) return err;
	next_position();
	if (g_source[g_srcpos]!=',') return ERR_SYNTAX;
	g_srcpos++;
	check_obj_space(1);
	g_object[g_objpos++]=0xAFA20004;              // sw          v0,4(sp)
	// Get buffer size
	err=get_value();
	if (err) return err;
	check_obj_space(1);
	g_object[g_objpos++]=0xAFA20008;              // sw          v0,8(sp)
	// Additional data to send to device
	while(g_source[g_srcpos]==','){
		g_srcpos++;
		stack+=4;
		err=get_value();
		if (err) return err;
		g_object[g_objpos++]=0xAFA20000|stack;    // sw          v0,xxxx(sp)
	}
	g_object[opos]=0x27BD0000|((0-stack)&0xffff); // addiu       sp,sp,-xxxx (see above)
	// v0 is the number of data to send
	check_obj_space(1);
	g_object[g_objpos++]=0x34020000|(stack/4-2);  // ori         v0,zero,xxxx
	// Call library
	call_lib_code(lib);
	// Remove stack
	check_obj_space(1);
	g_object[g_objpos++]=0x27BD0000|stack;        // addiu       sp,sp,xxxx
	return 0;
}

char* spiwritedata_statement(){
	return spiwritedata_readdata(LIB_SYSTEM | EXTRA_SPIWRITEDATA);
}
char* spireaddata_statement(){
	return spiwritedata_readdata(LIB_SYSTEM | EXTRA_SPIREADDATA);
}
char* spiswapdata_statement(){
	return spiwritedata_readdata(LIB_SYSTEM | EXTRA_SPISWAPDATA);
}

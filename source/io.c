/*
   This file is provided under the LGPL license ver 2.1.
   Written by K.Tanaka & Katsumi
   http://www.ze.em-net.ne.jp/~kenken/index.html
   http://hp.vector.co.jp/authors/VA016157/
*/

#include <xc.h>
#include "compiler.h"

/*
	Library functions follow
*/
static short* g_serial_buff=0;
static int g_serial_buff_read_pos,g_serial_buff_write_pos;
static int g_serial_buff_size;
void lib_serial(int baud, int parity, int bsize){
	// SERIAL x[,y[,z]]
	// where x is baud rate. If zero, stop using it.
	// y (0 is default) is parity setting; 0: 8 bit no parity, 
	// 1: 8 bit even parity, 2: 8 bit odd parity, 3: 9 bit no parity
	// z is input buffer size. If z=0 (default), the size will be calculated automatically.
	// Calculation is: z=BAUD/10/60/2*3 (z=BAUD/400), which is 1.5 times more size required for 1/60 sec.
	/*
		RC13 U1TX RC14 U1RX 
		U1MODE=0x0000 | (parity<<1)
		U1MODEbits.ON=0;
		U1MODEbits.FRZ=0;     Continue operation when CPU is in Debug Exception mode;
		U1MODEbits.SIDL=0;    Continue operation in Idle mode
		U1MODEbits.IREN=0;    IrDA is disabled;
		U1MODEbits.RTSMD=0;   UxRTS pin is in Flow Control mode
		U1MODEbits.UEN=0;     Only UxTX and UxRX pins are enabled and used;
		U1MODEbits.WAKE=0;    Wake-up disabled
		U1MODEbits.LPBACK=0;  Loopback mode is disabled
		U1MODEbits.ABAUD=0;   Baud rate measurement disabled or completed
		U1MODEbits.RXINV=0;   UxRX Idle state is e1f
		U1MODEbits.BRGH=0;    Standard Speed mode ? 16x baud clock enabled
		U1MODEbits.PDSEL=0-3;
		U1MODEbits.STSEL=0;   1 Stop bit
		U1STA=0x00001400;
		U1STAbits.ADM_EN=0;   Automatic Address Detect mode is disabled
		U1STAbits.ADDR=0;     Don't care
		U1STAbits.UTXISEL=0;  Don't care. Don't use TX interrupt
		U1STAbits.UTXINV=0;   UxTX Idle state is e1f
		U1STAbits.URXEN=1;    UARTx receiver is enabled. UxRX pin is controlled by UARTx (if ON = 1)
		U1STAbits.UTXBRK=0;   Break transmission is disabled or completed
		U1STAbits.UTXEN=1;    UARTx transmitter is enabled. UxTX pin is controlled by UARTx (if ON = 1)
		U1STAbits.URXISEL=0;  Interrupt flag bit is set when a character is received
		U1STAbits.ADDEN=0;    Address Detect mode is disabled
		U1STAbits.OERR=0;     Receive buffer has not overflowed
		U1BRG=FPB/16/baud - 1
	*/
	if (baud==0) {
		// Disable UART
		U1MODE=0x0000;
		U1STA=0x00000000;
		IEC1bits.U1TXIE=0;
		IEC1bits.U1RXIE=0;
		// Free buffer area
		if (g_serial_buff) free_non_temp_str((char*)g_serial_buff);
		g_serial_buff=0;
	} else {
		// Prepare buffer for SERIALIN
		if (bsize==0) bsize=baud/400; // Area corresponds to ~1/40 sec (> 1/60 sec)
		g_serial_buff_size=bsize;
		if (g_serial_buff) free_non_temp_str((char*)g_serial_buff);
		g_serial_buff=(short*)alloc_memory((bsize+1)/2,get_permanent_var_num());
		g_serial_buff_read_pos=g_serial_buff_write_pos=0;
		// Initialize I/O ports
		TRISCSET=1<<14;  // Input from RC14
		ANSELCCLR=1<<14; // Digital for RC14
		U1RXR=7;         // Use RC14 for U1RX
		TRISCCLR=1<<13;  // Output to RC13
		RPC13R=3;        // Use RC13 for U1TX
		// Initialize UART
		U1MODE=0x0000 | ((parity&3)<<1);
		U1STA=0x00001400;
		U1BRG=95454533/16/baud -1;
		// Interrupt settings. Use only RX interrupt.
		IEC1bits.U1TXIE=0;
		IFS1bits.U1RXIF=0;
		IEC1bits.U1RXIE=1;
		IPC7bits.U1IP=3;
		IPC7bits.U1IS=0;
		// Start UART
		U1MODEbits.ON=1;
	}
}
#pragma interrupt uartint IPL3SOFT vector 31
void uartint(){
	int err;
	IFS1bits.U1RXIF=0;
	while(U1STAbits.URXDA){
		// Fill into the buffer while RX data is available
		err=0;
		if (U1STAbits.PERR) {
			if (U1MODEbits.PDSEL==1 || U1MODEbits.PDSEL==2) {
				// Parity error
				err=0x100;
			}
		}
		g_serial_buff[g_serial_buff_write_pos]=err|U1RXREG;
		g_serial_buff_write_pos++;
		if (g_serial_buff_size<=g_serial_buff_write_pos) g_serial_buff_write_pos=0;
	}
}

void lib_serialout(int data){
	// SERIALOUT x
	// where x is 8 bit data. If FIFO buffer is full, wait until ready.
	if (U1MODEbits.PDSEL==3) {
		// 9 bit
		data&=0x1ff;
	} else {
		data&=0xff;
	}
	// Wait while buffer is full
	while(U1STAbits.UTXBF){
		asm("nop");
	}
	// Send data.
	U1TXREG=data;
}
int lib_serialin(int mode){
	int i;
	// SERIALIN([x])
	// x=0 (default): return data. If no data remaining, return -1.
	// x=1: If data exist(s) return 1, if not, return 0.
	switch(mode){
		case 1:
			// Return # of data in buffer
			i=g_serial_buff_write_pos-g_serial_buff_read_pos;
			if (i<0) i+=g_serial_buff_size;
			return i;
		case 0:
		default:
			if (g_serial_buff_write_pos==g_serial_buff_read_pos) return -1;
			i=g_serial_buff[g_serial_buff_read_pos];
			g_serial_buff_read_pos++;
			if (g_serial_buff_size<=g_serial_buff_read_pos) g_serial_buff_read_pos=0;
			return i;
	}
}

void lib_pwm(int duty, int freq, int num){
	/*
		OCxCON=0x000D;
		OCxCONbits.ON=0;
		OCxCONbits.SIDL=0;   // Continue operation in Idle mode
		OCxCONbits.OC32=0;   // OCxR<15:0> and OCxRS<15:0> are used for comparisons to the 16-bit timer source
		OCxCONbits.OCFLT;    // Not used
		OCxCONbits.OCTSEL=1; // Timer3 is the clock source for this Output Compare module
		OCxCONbits.OCM=5;    // Initialize OCx pin low; generate continuous output pulses on OCx pin

		Signal will be L when TMR3 reaches OCxRS, and H when TMR3 reaches OCxR

		Timer3 prescaler: 1, 2, 4, 8, 16, 32, 64, 256
		For slowest Timer3 (PR3=65535), in Hz: 1456.5, 728.3, 364.1, 182.1, 91.0, 45.5, 22.8, 5.69
		For fastest Timer3 (PR3=1000), in Hz: 95455, 47727, 23864, 11932, 5966, 2983, 1492, 373
		Therefore, for freq value,
			6-22:       1/256 prescaler
			23-45:      1/64
			46-91:      1/32
			92-182:     1/16
			183-364:    1/8
			365-728:    1/4
			729-1456:   1/2
			1457-95455: 1/1

	*/
	static int prevfreq=0, prevPR3=0;
	if (num==0) {
		// Reset PWM
		prevfreq=prevPR3=0;
		return;
	}
	if (duty==0) {
		// Continuous output of L signal
		switch(num){
			case 1:
				// Use RD10 for digital output
				LATDCLR=1<<10;
				RPD10R=0;
				TRISDCLR=1<<10;
				return;
			case 2:
				// Use RD11 for digital output
				LATDCLR=1<<11;
				RPD11R=0;
				TRISDCLR=1<<11;
				return;
			default:
				err_invalid_param();
		}
	} else if (duty==1000) {
		// Continuous output of H signal
		switch(num){
			case 1:
				// Use RD10 for digital output
				LATDSET=1<<10;
				RPD10R=0;
				TRISDCLR=1<<10;
				return;
			case 2:
				// Use RD11 for digital output
				LATDSET=1<<11;
				RPD11R=0;
				TRISDCLR=1<<11;
				return;
			default:
				err_invalid_param();
		}
	}
	// PWM mode
	if (freq!=prevfreq || PR3!=prevPR3) {
		// Initialize PWM system
		OC3CON=0x0000;
		OC4CON=0x0000;
		IEC0bits.OC3IE=0;
		IEC0bits.OC4IE=0;
		// Stop music
		stop_music();
		// Initialize timer 3
		IEC0bits.T3IE=0;
		if (freq<6) {
			err_invalid_param();
		} else if (freq<23) {
			// 1/256 prescaler
			T3CON=0x0070;
			prevPR3=(int)((95454533/256)/freq)-1;
		} else if (freq<46) {
			// 1/64 prescaler
			T3CON=0x0060;
			prevPR3=(int)((95454533/64)/freq)-1;
		} else if (freq<92) {
			// 1/32 prescaler
			T3CON=0x0050;
			prevPR3=(int)((95454533/32)/freq)-1;
		} else if (freq<183) {
			// 1/16 prescaler
			T3CON=0x0040;
			prevPR3=(int)((95454533/16)/freq)-1;
		} else if (freq<365) {
			// 1/8 prescaler
			T3CON=0x0030;
			prevPR3=(int)((95454533/8)/freq)-1;
		} else if (freq<729) {
			// 1/4 prescaler
			T3CON=0x0020;
			prevPR3=(int)((95454533/4)/freq)-1;
		} else if (freq<1457) {
			// 1/2 prescaler
			T3CON=0x0010;
			prevPR3=(int)((95454533/2)/freq)-1;
		} else if (freq<95455) {
			// 1/1 prescaler
			T3CON=0x0000;
			prevPR3=(int)(95454533/freq)-1;
		} else {
			err_invalid_param();
		}
		TMR3=0;
		PR3=prevPR3;
		prevfreq=freq;
	}
	// Wait until Timer3 will reset
	if (T3CONbits.ON) {
		IFS0bits.T3IF=0;
		while (IFS0bits.T3IF==0); 
	};
	// Stop and reset timer
	T3CONCLR=0x8000;
	TMR3=PR3;
	// New PWM setting follows
	switch(num){
		case 1:
			// Use OC3/RD10
			RPD10R=11;
			TRISDCLR=1<<10;
			// Disable OC3 first
			OC3CON=0x0000;
			// OC3 settings
			OC3CON=0x000D;
			OC3R=0;
			OC3RS=(((int)PR3+1)*duty) / 1000;
			// Start OC3 and timer
			OC3CONSET=0x8000;
			T3CONSET=0x8000;
			break;
		case 2:
			// Use OC4/RD11
			RPD11R=11;
			TRISDCLR=1<<11;
			// Disable OC4 first
			OC4CON=0x0000;
			// OC4 settings
			OC4CON=0x000D;
			OC4R=0;
			OC4RS=(((int)PR3+1)*duty) / 1000;
			// Wait until Timer3 will reset
			if (T3CONbits.ON) {
				IFS0bits.T3IF=0;
				while (IFS0bits.T3IF==0); 
			};
			// Start OC4 and timer
			OC4CONSET=0x8000;
			T3CONSET=0x8000;
			break;
		default:
			err_invalid_param();
	}
}

void lib_out(int pos, int val){
	// pos must be between 0 and 15 or 16 and 18
	if (0<=pos && pos<=15) {
		// PORTB0-15
		// Set output vale
		if (val) {
			LATBSET=1<<pos;
		} else {
			LATBCLR=1<<pos;
		}
		// Enable output
		TRISBCLR=1<<pos;
		// Disable pullup
		CNPUBCLR=1<<pos;
	} else if (16<=pos && pos<=18) {
		// PORTE5-7, open drain
		pos=pos-16+5;
		// Set output vale (L)
		LATECLR=1<<pos;
		// Output for L and input for H
		if (val) {
			// Disable output
			TRISESET=1<<pos;
		} else {
			// Enable output
			TRISECLR=1<<pos;
		}
	} else {
		return;
	}
}

void lib_out8h(int val){
	// Set output vale
	LATB=(LATB&0x00FF)|((val&0xff)<<8);
	// Enable output
	TRISBCLR=0xFF00;
	// Disable pullup
	CNPUBCLR=0xFF00;
}

void lib_out8l(int val){
	// Set output vale
	LATB=(LATB&0xFF00)|(val&0xff);
	// Enable output
	TRISBCLR=0x00FF;
	// Disable pullup
	CNPUBCLR=0x00FF;
}

int lib_out16(int val){
	// Set output vale
	LATB=(val&0xFFFF);
	// Enable output
	TRISBCLR=0xFFFF;
	// Disable pullup
	CNPUBCLR=0xFFFF;
}

int lib_in(int pos){
	// pos must be between 0 and 15 or 16 and 18
	if (0<=pos && pos<=15) {
		// PORTB0-15
		// Enable pullup
		CNPUBSET=1<<pos;
		// Enable input
		TRISBSET=1<<pos;
		ANSELBCLR=1<<pos;
		// Read value and return
		return (PORTB&(1<<pos)) ? 1:0;
	} else if (16<=pos && pos<=18) {
		// PORTE5-7
		pos=pos-16+5;
		// Enable pullup
		CNPUESET=1<<pos;
		// Enable input
		TRISESET=1<<pos;
		ANSELECLR=1<<pos;
		// Read value and return
		return (PORTE&(1<<pos)) ? 1:0;
	} else {
		return 0;
	}
}

int lib_in8h(){
	// Enable pullup
	CNPUBSET=0xFF00;
	// Enable input
	TRISBSET=0xFF00;
	ANSELBCLR=0xFF00;
	// Read value and return
	return (PORTB&0xFF00)>>8;
}

int lib_in8l(){
	// Enable pullup
	CNPUBSET=0x00FF;
	// Enable input
	TRISBSET=0x00FF;
	ANSELBCLR=0x00FF;
	// Read value and return
	return PORTB&0x00FF;
}

int lib_in16(){
	// Enable pullup
	CNPUBSET=0xFFFF;
	// Enable input
	TRISBSET=0xFFFF;
	ANSELBCLR=0xFFFF;
	// Read value and return
	return PORTB&0xFFFF;
}

int lib_analog(int pos){
	/*
		Analog to 12 bit digital converter function.
		AD1CON1=0x00E0;
		AD1CON1bits.ON=0;
		AD1CON1bits.SIDL=0;    // Continue module operation in Idle mode
		AD1CON1bits.FORM=0;    // Integer 16-bit
		AD1CON1bits.SSRC=8;    // Internal counter ends sampling and starts conversion (auto convert)
		AD1CON1bits.CLRASAM=0; // Normal operation, buffer contents will be overwritten by the next conversion sequence
		AD1CON1bits.ASAM=0;    // Sampling begins when SAMP bit is set.
		AD1CON1bits.SAMP=0;    // The ADC sample/hold amplifier is holding
		AD1CON1bits.DONE=0;    // Analog-to-digital conversion is not done or has not started
		AD1CON2=0;
		AD1CON2bits.VCFG=0;   // Voltage reference:AVdd-AVss
		AD1CON2bits.OFFCAL=0; // Disable Offset Calibration mode
		AD1CON2bits.CSCNA=0;  // Do not scan inputs
		AD1CON2bits.BUFS=0;   // Do not care, only valid when BUFM=1
		AD1CON2bits.SMPI=0;   // Do not care. Do not use interrupt.
		AD1CON2bits.BUFM=0;   // Buffer configured as one 16-word buffer ADC1BUFF-ADC1BUF0
		AD1CON2bits.ALTS=0;   // Always use Sample A input multiplexer settings
		AD1CON3=0x0607;
		AD1CON3bits.ADRC=0; // Clock derived from Peripheral Bus Clock (PBCLK)
		AD1CON3bits.SAMC=6; // Auto-Sample Time: 6 TAD = 1005.72 ns (> 1000 ns)
		AD1CON3bits.ADCS=7; // TAD=TPB*2*8=167.62 ns (> 154 ns)
		AD1CHS=(0-27)<<16;
		AD1CHSbits.CN0NB=0;    // Do not care, only valid when using channel B
		AD1CHSbits.CH0SB=0;    // Do not care, only valid when using channel B
		AD1CHSbits.CH0NA=0;    // Channel 0 negative input is VREFL
		AD1CHSbits.CH0SA=0-27; // Set input channel here
		AD1CSSL; // Do not care, only valid when CSCNA=1;
	*/
	AD1CON1=0x00E0;
	AD1CON2=0x0000;
	AD1CON3=0x0607;
	// pos must be between 0 and 15 or 16 and 18
	if (0<=pos && pos<=15) {
		// RB0-RB15: AN0-AN15
		// Disable pullup
		CNPUBCLR=1<<pos;
		// Enable input
		TRISBSET=1<<pos;
		// Enable analog
		ANSELBSET=1<<pos;
		// Select input pin
		AD1CHS=pos<<16;
	} else if (16<=pos && pos<=18) {
		// RE5,6,7:AN22,23,27
		pos=pos-16+5;
		// Disable pullup
		CNPUECLR=1<<pos;
		// Enable input
		TRISESET=1<<pos;
		// Enable analog
		ANSELESET=1<<pos;
		// Select input pin
		if (pos<=6) {
			pos=pos-5+22;
		} else {
			pos=pos-7+27;
		}
		AD1CHS=pos<<16;
	} else {
			err_invalid_param();
	}
	// Enable ADC
	AD1CON1bits.ON=1;
	// Start 
	AD1CON1bits.SAMP=1;
	// Wait until done
	while(!AD1CON1bits.DONE){
		asm("nop");
	}
	// Disable ADC
	AD1CON1bits.ON=0;
	// Return value
	return ADC1BUF0;
}

/*
	Statements and functions implementations follow
*/

// Local prototyping
char* param2_statement(enum libs lib);

char* out_statement(){
	return param2_statement(LIB_SYSTEM | EXTRA_OUT);
}
char* out8h_statement(){
	char* err;
	err=get_value();
	if (err) return err;
	call_lib_code(LIB_SYSTEM | EXTRA_OUT8H);
	return 0;
}
char* out8l_statement(){
	char* err;
	err=get_value();
	if (err) return err;
	call_lib_code(LIB_SYSTEM | EXTRA_OUT8L);
	return 0;
}
char* out16_statement(){
	char* err;
	err=get_value();
	if (err) return err;
	call_lib_code(LIB_SYSTEM | EXTRA_OUT16);
	return 0;
}
char* pwm_statement(){
	char* err;
	// Get 1st parameter
	err=get_value();
	if (err) return err;
	check_obj_space(2);
	g_object[g_objpos++]=0x27BDFFF8; // addiu       sp,sp,-8
	g_object[g_objpos++]=0xAFA20004; // sw          v0,4(sp)
	// Get 2nd parameter
	if (g_source[g_srcpos]==',') {
		g_srcpos++;
		err=get_value();
		if (err) return err;
	} else {
		check_obj_space(1);
		g_object[g_objpos++]=0x340203E8; //ori         v0,zero,1000
	}
	check_obj_space(1);
	g_object[g_objpos++]=0xAFA20008; // sw          v0,8(sp)
	// Get 3rd parameter
	if (g_source[g_srcpos]==',') {
		g_srcpos++;
		err=get_value();
		if (err) return err;
	} else {
		check_obj_space(1);
		g_object[g_objpos++]=0x34020001; //ori         v0,zero,1
	}
	// Insert calling system code
	call_lib_code(LIB_SYSTEM | EXTRA_PWM);
	check_obj_space(1);
	g_object[g_objpos++]=0x27BD0008; // addiu       sp,sp,8
	return 0;
}
char* in_function(){
	char* err;
	err=get_value();
	if (err) return err;
	call_lib_code(LIB_SYSTEM | EXTRA_IN);
	return 0;
}
char* in8h_function(){
	call_lib_code(LIB_SYSTEM | EXTRA_IN8H);
	return 0;
}
char* in8l_function(){
	call_lib_code(LIB_SYSTEM | EXTRA_IN8L);
	return 0;
}
char* in16_function(){
	call_lib_code(LIB_SYSTEM | EXTRA_IN16);
	return 0;
}
char* analog_function(){
	char* err;
	err=get_value();
	if (err) return err;
	call_lib_code(LIB_SYSTEM | EXTRA_ANALOG);
	return 0;
}
char* serial_statement(){
	char* err;
	// Get 1st parameter
	err=get_value();
	if (err) return err;
	check_obj_space(2);
	g_object[g_objpos++]=0x27BDFFF8; // addiu       sp,sp,-8
	g_object[g_objpos++]=0xAFA20004; // sw          v0,4(sp)
	// Get 2nd parameter
	if (g_source[g_srcpos]==',') {
		g_srcpos++;
		err=get_value();
		if (err) return err;
	} else {
		check_obj_space(1);
		g_object[g_objpos++]=0x34020000; //ori         v0,zero,0000
	}
	check_obj_space(1);
	g_object[g_objpos++]=0xAFA20008; // sw          v0,8(sp)
	// Get 3rd parameter
	if (g_source[g_srcpos]==',') {
		g_srcpos++;
		err=get_value();
		if (err) return err;
	} else {
		check_obj_space(1);
		g_object[g_objpos++]=0x34020000; //ori         v0,zero,0
	}
	// Insert calling system code
	call_lib_code(LIB_SYSTEM | EXTRA_SERIAL);
	check_obj_space(1);
	g_object[g_objpos++]=0x27BD0008; // addiu       sp,sp,8
	return 0;
}
char* serialout_statement(){
	char* err;
	err=get_value();
	if (err) return err;
	call_lib_code(LIB_SYSTEM | EXTRA_SERIALOUT);
	return 0;
}
char* serialin_function(){
	char* err;
	next_position();
	if (g_source[g_srcpos]==')') {
		check_obj_space(1);
		g_object[g_objpos++]=0x34020000; //ori         v0,zero,0000
	} else {
		err=get_value();
		if (err) return err;
	}
	call_lib_code(LIB_SYSTEM | EXTRA_SERIALIN);
	return 0;
}

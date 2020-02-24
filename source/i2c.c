/*
   This file is provided under the LGPL license ver 2.1.
   Written by K.Tanaka & Katsumi
   http://www.ze.em-net.ne.jp/~kenken/index.html
   http://hp.vector.co.jp/authors/VA016157/
*/

#include <xc.h>
#include "compiler.h"

char g_ack;

/*
	Local macros and functions
*/

#define idle() while (I2C1STATbits.TRSTAT)

#define start() do {\
		I2C1CONbits.SEN = 1;\
		while (I2C1CONbits.SEN);\
	} while (0)

#define restart() do {\
		I2C1CONbits.RSEN = 1;\
		while (I2C1CONbits.RSEN);\
	} while (0)

#define stop() do {\
		I2C1CONbits.PEN = 1;\
		while (I2C1CONbits.PEN);\
	} while (0)

#define writeByte(x) do{\
		I2C1TRN = (x);\
		while (I2C1STATbits.TBF);\
	} while (0)

#define ackbit() (I2C1STATbits.ACKSTAT)

unsigned char readByte(char ack) {
	unsigned char ret;
	// Enable Master receive
	I2C1CONbits.RCEN = 1;
	asm volatile("nop");
	// Wait for receive bufer to be full and read data
	while(!I2C1STATbits.RBF);
	ret=I2C1RCV;
	// ACK or NACK
	if (ack) {
		I2C1CONbits.ACKDT = 0;
		I2C1CONbits.ACKEN = 1;
		while(I2C1CONbits.ACKEN);
	} else {
		I2C1CONbits.ACKDT = 1;
		I2C1CONbits.ACKEN = 1;
		while(I2C1CONbits.ACKEN);
		I2C1CONbits.ACKDT = 0;
	}
	// Return value
	return ret;
}

/*
	Library functions follow
*/

void lib_i2c(int freq){
	if (freq==0) {
		I2C1CON = 0;
		return;
	}
	/*
		Tpgd=104e-9, PBclk=95454533
		I2CxBRG=((1/(2 x Fsck) - Tpgd) x PBclk) -2
			=PBclk / (2  x Fsck) - Tpgd x PBclk -2
			=(PBclk / 2) / Fsck - Tpgd x PBclk -2
			=47727267 / Fsck - 0.000000104 x 95454533 - 2
			=47727267 / Fsck - 9.93 -2
	*/
	int brg = (47727 + freq/2)/freq - 10 - 2;
	if (brg<2) brg=2;       // Max 3409 kHz
	if (4095<brg) brg=4905; // Min 11.6 kHz

	// Note that there are no RPG2R and RPG3R registors.
	// The pins G2 and G3 will be activated when I2C1CONbits.ON=1
	I2C1CON = 0;
	I2C1BRG = brg;
	I2C1CON = 0x1200;
	I2C1RCV = 0x0000;
	I2C1TRN = 0x0000;
	I2C1CON = 0x9200;
}

int lib_i2cerror(){
	// Check if initiated
	if (!I2C1CONbits.ON) err_peri_not_init();
	return (int)g_ack;
}

void lib_i2cwrite(int num, int* data){
	int ack;
	int i;
	// Check if initiated
	if (!I2C1CONbits.ON) err_peri_not_init();
	// Start signal
	idle();
	start();
	for(i=1;i<=num;i++){
		// Write byte. First byte is address, and bit 0 must be zero.
		if (i==1) writeByte((unsigned char)data[1]<<1);
		else writeByte((unsigned char)data[i]);
		idle();
		// Check ACK bit. If 1, quit the job
		ack=ackbit();
		if (ack) break;
	}
	stop();
	g_ack=ack;
}

void lib_i2cwritedata(int num1, int* data1, int num2, unsigned char* data2){
	int ack;
	int i;
	// Check if initiated
	if (!I2C1CONbits.ON) err_peri_not_init();
	// Start signal
	idle();
	start();
	for(i=1;i<=num1;i++){
		// Write byte. First byte is address, and bit 0 must be zero.
		if (i==1) writeByte((unsigned char)data1[1]<<1);
		else writeByte((unsigned char)data1[i]);
		// Check ACK bit. If 1, quit the job
		idle();
		ack=ackbit();
		if (ack) break;
	}
	if (!ack) {
		for(i=0;i<num2;i++){
			// Write bytes from data array
			writeByte((unsigned char)data2[i]);
			idle();
			ack=ackbit();
			if (ack) break;
		}
	}
	stop();
	g_ack=ack;
}

unsigned int lib_i2cread(int num, int* data){
	int ack;
	int i;
	unsigned int ret=0xffffffff; // Retuen -1 as error
	// Check if initiated
	if (!I2C1CONbits.ON) err_peri_not_init();
	// Start signal
	idle();
	start();
	for(i=1;i<=num;i++){
		// Write byte. First byte is address, and bit 0 must be zero.
		if (i==1) writeByte((unsigned char)data[1]<<1);
		else writeByte((unsigned char)data[i]);
		// Check ACK bit. If 1, quit the job
		idle();
		ack=ackbit();
		if (ack) break;
	}
	if (!ack) {
		idle();
		restart();
		// Write first byte as address, and bit 0 must be 1.
		writeByte(((unsigned char)data[1]<<1)|0x01);
		idle();
		ack=ackbit();
		// Read byte and send NACK
		if (!ack) ret=(unsigned int)readByte(0);
	}
	stop();
	g_ack=ack;
	return (unsigned int) ret;
}

void lib_i2creaddata(int num1, int* data1, int num2, unsigned char* data2){
	int ack;
	int i;
	// Check if initiated
	if (!I2C1CONbits.ON) err_peri_not_init();
	// Start signal
	idle();
	start();
	for(i=1;i<=num1;i++){
		// Write byte. First byte is address, and bit 0 must be zero.
		if (i==1) writeByte((unsigned char)data1[1]<<1);
		else writeByte((unsigned char)data1[i]);
		// Check ACK bit. If 1, quit the job
		idle();
		ack=ackbit();
		if (ack) break;
	}
	if (!ack) {
		idle();
		restart();
		// Write first byte as address, and bit 0 must be 1.
		writeByte(((unsigned char)data1[1]<<1)|0x01);
		idle();
		ack=ackbit();
		if (!ack) {
			for(i=0;i<num2;i++){
				// Read byte and send ACK/NACK. NACK for the last rading.
				if (i<num2-1) data2[i]=(unsigned int)readByte(1);
				else data2[i]=(unsigned int)readByte(0);
				idle();
			}
		}
	}
	stop();
	g_ack=ack;
}


/*
	Statements and functions implementations follow
*/

char* i2c_statement(){
	char* err;
	int spos,opos;
	spos=g_srcpos;
	opos=g_objpos;
	err=get_value();
	if (err) {
		g_srcpos=spos;
		g_objpos=opos;
		// Use 100 kHz if parameter is omitted
		check_obj_space(1);
		g_object[g_objpos++]=0x34020064; // ori   v0,zero,100
	}
	call_lib_code(LIB_SYSTEM | EXTRA_I2C);
	return 0;
}

char* i2cerror_function(){
	call_lib_code(LIB_SYSTEM | EXTRA_I2CERROR);
	return 0;
}

char* i2cwrite_read(enum libs lib){
	char* err;
	int opos;
	int stack=4;
	// Get address
	err=get_value();
	if (err) return err;
	// Prepare stack and store $v0
	opos=g_objpos;
	check_obj_space(2);
	g_object[g_objpos++]=0x27BD0000;              // addiu       sp,sp,-xxxx
	g_object[g_objpos++]=0xAFA20004;              // sw          v0,4(sp)
	// Additional data to send to device
	while(g_source[g_srcpos]==','){
		g_srcpos++;
		stack+=4;
		err=get_value();
		if (err) return err;
		g_object[g_objpos++]=0xAFA20000|stack;    // sw          v0,xxxx(sp)
	}
	g_object[opos]=0x27BD0000|((0-stack)&0xffff); // addiu       sp,sp,-xxxx (see above)
	// v0 is the number of data to send (including 7 bit address)
	check_obj_space(1);
	g_object[g_objpos++]=0x34020000|(stack/4);    // ori         v0,zero,xxxx
	// Call library
	call_lib_code(lib);
	// Remove stack
	check_obj_space(1);
	g_object[g_objpos++]=0x27BD0000|stack;        // addiu       sp,sp,xxxx
	return 0;
}

char* i2cwrite_statement(){
	return i2cwrite_read(LIB_SYSTEM | EXTRA_I2CWRITE);
}

char* i2cread_function(){
	return i2cwrite_read(LIB_SYSTEM | EXTRA_I2CREAD);
}

char* i2cwritedata_readdata(enum libs lib){
	char* err;
	int opos;
	int stack=12;
	// Get address
	err=get_value();
	if (err) return err;
	next_position();
	if (g_source[g_srcpos]!=',') return ERR_SYNTAX;
	g_srcpos++;
	// Prepare stack and store $v0
	opos=g_objpos;
	check_obj_space(2);
	g_object[g_objpos++]=0x27BD0000;              // addiu       sp,sp,-xxxx
	g_object[g_objpos++]=0xAFA2000C;              // sw          v0,12(sp)
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
	// v0 is the number of data to send (including 7 bit address)
	check_obj_space(1);
	g_object[g_objpos++]=0x34020000|(stack/4-2);  // ori         v0,zero,xxxx
	// Call library
	call_lib_code(lib);
	// Remove stack
	check_obj_space(1);
	g_object[g_objpos++]=0x27BD0000|stack;        // addiu       sp,sp,xxxx
	return 0;
}

char* i2cwritedata_statement(){
	return i2cwritedata_readdata(LIB_SYSTEM | EXTRA_I2CWRITEDATA);
}

char* i2creaddata_statement(){
	return i2cwritedata_readdata(LIB_SYSTEM | EXTRA_I2CREADDATA);
}


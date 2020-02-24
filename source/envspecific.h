/*
   This file is provided under the LGPL license ver 2.1.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

// Megalopa uses I/O statements/functions
#include "io.h"

#define CPU_CLOCK_HZ 95454533
#define PERSISTENT_RAM_SIZE (1024*100)

int readbuttons();
void scroll(int x, int y);
void usegraphic(int mode);
void videowidth(int width);
int lib_system(int a0, int a1 ,int v0, int a3, int g_gcolor, int g_prev_x, int g_prev_y);
void pre_run(void);
void post_run(void);
void err_peri_not_init(void);

// 30, 36, 40, 48, 64, 80 characters per line for Megalopa
void printcomma(void);

// Check break key or buttons when executing BASIC code.
// In PS/2 mode, detect ctrl-break.
// In button mode, detect pushing four buttons are pushed simultaneously.
#define check_break() \
	if (g_disable_break==0) {\
		if (ps2keystatus[0x03]) err_break();\
	}

// Megalopa specific lists of statements and functions
#define ADDITIONAL_STATEMENTS \
	"OUT "         ,out_statement,\
	"OUT8H "       ,out8h_statement,\
	"OUT8L "       ,out8l_statement,\
	"OUT16 "       ,out16_statement,\
	"PWM "         ,pwm_statement,\
	"SERIAL "      ,serial_statement,\
	"SERIALOUT "   ,serialout_statement,\
	"I2CWRITE "    ,i2cwrite_statement,\
	"I2CWRITEDATA ",i2cwritedata_statement,\
	"I2CREADDATA " ,i2creaddata_statement,\
	"I2C"          ,i2c_statement,\
	"SPI "         ,spi_statement,\
	"SPIWRITE "    ,spiwrite_statement,\
	"SPIWRITEDATA ",spiwritedata_statement,\
	"SPIREADDATA " ,spireaddata_statement,\
	"SPISWAPDATA " ,spiswapdata_statement,

#define ADDITIONAL_INT_FUNCTIONS \
	"IN("  ,    in_function,\
	"IN8H("  ,  in8h_function,\
	"IN8L("  ,  in8l_function,\
	"IN16("  ,  in16_function,\
	"ANALOG("  ,analog_function,\
	"SERIALIN(",serialin_function,\
	"I2CREAD(" ,i2cread_function,\
	"I2CERROR(",i2cerror_function,\
	"SPIREAD(" ,spiread_function,

#define ADDITIONAL_STR_FUNCTIONS
#define ADDITIONAL_RESERVED_VAR_NAMES \
	0x000154aa, /*OUT*/ \
	0x01c5c253, /*OUT8H*/ \
	0x01c5c257, /*OUT8L*/ \
	0x01c5c145, /*OUT16*/ \
	0x00010164, /*IN*/ \
	0x000870fd, /*IN8H*/ \
	0x00087101, /*IN8L*/ \
	0x00086fef, /*IN16*/ \
	0x06bd06f3, /*ANALOG*/ \
	0x00015a46, /*PWM*/ \
	0x502e9b15, /*SERIAL*/ \
	0x0001694a, /*SPI*/ \
	0x00013077, /*I2C*/ \

#define EXTRA_MASK 0x003F
#define EXTRA_STEP 0x0001
enum extra{
	EXTRA_SYSTEM       =EXTRA_STEP*0,
	EXTRA_OUT          =EXTRA_STEP*1,
	EXTRA_OUT8H        =EXTRA_STEP*2,
	EXTRA_OUT8L        =EXTRA_STEP*3,
	EXTRA_OUT16        =EXTRA_STEP*4,
	EXTRA_IN           =EXTRA_STEP*5,
	EXTRA_IN8H         =EXTRA_STEP*6,
	EXTRA_IN8L         =EXTRA_STEP*7,
	EXTRA_IN16         =EXTRA_STEP*8,
	EXTRA_ANALOG       =EXTRA_STEP*9,
	EXTRA_PWM          =EXTRA_STEP*10,
	EXTRA_SERIALOUT    =EXTRA_STEP*11,
	EXTRA_SERIALIN     =EXTRA_STEP*12,
	EXTRA_SERIAL       =EXTRA_STEP*13,
	EXTRA_I2C          =EXTRA_STEP*14,
	EXTRA_I2CWRITE     =EXTRA_STEP*15,
	EXTRA_I2CREAD      =EXTRA_STEP*16,
	EXTRA_I2CWRITEDATA =EXTRA_STEP*17,
	EXTRA_I2CREADDATA  =EXTRA_STEP*18,
	EXTRA_I2CERROR     =EXTRA_STEP*19,
	EXTRA_SPI          =EXTRA_STEP*20,
	EXTRA_SPIWRITE     =EXTRA_STEP*21,
	EXTRA_SPIREAD      =EXTRA_STEP*22,
	EXTRA_SPIWRITEDATA =EXTRA_STEP*23,
	EXTRA_SPIREADDATA  =EXTRA_STEP*24,
	EXTRA_SPISWAPDATA  =EXTRA_STEP*25,
	// MAX 63
};

#define ADDITIONAL_INTERRUPT_FUNCTIONS

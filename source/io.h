/*
   This file is provided under the LGPL license ver 2.1.
   Written by K.Tanaka & Katsumi
   http://www.ze.em-net.ne.jp/~kenken/index.html
   http://hp.vector.co.jp/authors/VA016157/
*/

/*
	Library and statements/functions in io.c
*/

void lib_out(int pos, int val);
void lib_out8h(int val);
void lib_out8l(int val);
int lib_out16(int val);
int lib_in(int pos);
int lib_in8h();
int lib_in8l();
int lib_in16();
int lib_analog(int pos);
void lib_pwm(int duty, int freq, int num);
void lib_serial(int baud, int parity, int bsize);
void lib_serialout(int data);
int lib_serialin(int mode);

char* out_statement();
char* out8h_statement();
char* out8l_statement();
char* out16_statement();
char* pwm_statement();
char* serial_statement();
char* serialout_statement();
char* in_function();
char* in8h_function();
char* in8l_function();
char* in16_function();
char* analog_function();
char* serialin_function();

/*
	Library and statements/functions in i2c.c
*/

void lib_i2c(int freq);
int lib_i2cerror();
void lib_i2cwrite(int num, int* data);
void lib_i2cwritedata(int num1, int* data1, int num2, unsigned char* data2);
unsigned int lib_i2cread(int num, int* data);
void lib_i2creaddata(int num1, int* data1, int num2, unsigned char* data2);

char* i2c_statement();
char* i2cerror_function();
char* i2cwrite_statement();
char* i2cread_function();
char* i2cwritedata_statement();
char* i2creaddata_statement();

/*
	Library and statements/functions in spi.c
*/

void lib_spi(int baud, int bitmode, int clkmode, int csdata);
void lib_spiwrite(int num, int* data);
void lib_spiwritedata(int num1, int* data1, int num2, unsigned int* data2);
unsigned int lib_spiread(int num, int* data);
void lib_spireaddata(int num1, int* data1, int num2, unsigned int* data2);

char* spi_statement();
char* spiwrite_statement();
char* spiread_function();
char* spiwritedata_statement();
char* spireaddata_statement();
char* spiswapdata_statement();

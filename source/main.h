/*
   This file is provided under the LGPL license ver 2.1.
   Written by K.Tanaka & Katsumi
   http://www.ze.em-net.ne.jp/~kenken/index.html
   http://hp.vector.co.jp/authors/VA016157/
*/

#define MEGALOPA
#define SYSVER1 "Megalopa"
#define SYSVER2 "1.3"
#define BASVER "KM-1303"

#define INIFILE "MACHIKAM.INI" // 初期設定ファイル
#define HEXFILE "MACHIKAM.HEX" // 実行中HEXファイル名がこれと一致した場合はエディタ起動

#define FILENAME_FLASH_ADDRESS 0x9D07EFF0
#define PIC32MX_RAMSIZE 0x20000
#define PIC32MX_FLASHSIZE 0x80000

void printhex8(unsigned char d);
void printhex16(unsigned short d);
void printhex32(unsigned int d);

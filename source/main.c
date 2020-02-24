/*
   This file is provided under the LGPL license ver 2.1.
   Written by K.Tanaka & Katsumi
   http://www.ze.em-net.ne.jp/~kenken/index.html
   http://hp.vector.co.jp/authors/VA016157/
*/

// main.c
// MachiKania BASIC System Ver Megalopa
// KM-BASIC 統合開発実行環境 for PIC32MX370F512H by K.Tanaka

// 利用システム
// ps2keyboard370f.X.a : PS/2キーボード入力システムライブラリ
// lib_videoout_megalopa.X.a : カラービデオ信号出力システムライブラリ
// sdfsio370fLib.a ： SDカードアクセス用ライブラリ


/*
	PIC32MX ペリフェラル使用状況
	
	割り込み
		NTSC,   Timer2, vector  8, priority 5
		NTSC,   OC5,    vector 22, priority 5
		NTSC,   OC2,    vector 10, priority 5
		NTSC,   OC1,    vector  6, priority 5
		PS/2,   CNF,    vector 33, priority 6
		PS/2,   Timer5, vector 20, priority 4
		MUSIC,  CS0,    vector  1, priority 2
		SERIAL, UART,   vector 31, priority 3
	
	タイマー
		Timer1 未使用
		Timer2 NTSC
		Timer3 MUSIC/PWM
		Timer4 MUSIC
		Timer5 PS/2
	
	DMA
		DMA0 未使用
		DMA1 MUSIC
		DMA2 MUSIC
		DMA3 PS/2
	
	Output compair
		OC1 NTSC
		OC2 NTSC
		OC3 MUSIC/PWM
		OC4 MUSIC/PWM
		OC5 NTSC
	
	UART
		UART1 シリアル通信
		UART2 未使用
	
	SPI
		SPI1 SPI通信（予定）
		SPI2 マルチメディアカード
	
	I2C
		I2C1 I2C通信（予定）
		I2C2 未使用
	
	ポート使用
		B0  I/O, AN0
		B1  I/O, AN1
		B2  I/O, AN2
		B3  I/O, AN3
		B4  I/O, AN4
		B5  I/O, AN5
		B6  I/O, AN6
		B7  I/O, AN7
		B8  I/O, AN8
		B9  I/O, AN9
		B10 I/O, AN10
		B11 I/O, AN11
		B12 I/O, AN12
		B13 I/O, AN13
		B14 I/O, AN14
		B15 I/O, AN15
		C12 OSC1 (Crystal)
		C13 U1TX (UART)
		C14 U1RX (UART)
		C15 OSC2 (Crystal)
		D0  SW_DOWN
		D1  SW_LEFT
		D2  SW_UP
		D3  SW_RIGHT
		D4  SW_START
		D5  SW_FIRE
		D6  
		D7  
		D8  
		D9  SPI1_CS (SPI)
		D10 PWM1
		D11 PWM2
		E0  NTSC
		E1  NTSC
		E2  NTSC
		E3  NTSC
		E4  NTSC
		E5  I/O, AN22
		E6  I/O, AN23
		E7  I/O, AN27
		F0  PS/2 DAT
		F1  PS/2 CLK
		F2  SDI1 (SPI)
		F3  SPI2_CS (MMC)
		F4  AUDIO_R
		F5  AUDIO_L
		F6  SCK1 (SPI)
		G2  SCL1 (I2C)
		G3  SDA1 (I2C)
		G6  SCK2 (MMC)
		G7  SDI2 (MMC)
		G8  SDO2 (MMC)
		G9  SDO1 (SPI)
*/

#include <xc.h>
#include "api.h"
#include "compiler.h"
#include "editor.h"
#include "interface/keyinput.h"
#include "main.h"

//外付けクリスタル with PLL (20/3倍)
//クリスタルは3.579545×4＝14.31818MHz
#pragma config FSRSSEL = PRIORITY_7
#pragma config PMDL1WAY = OFF
#pragma config IOL1WAY = OFF
//#pragma config FUSBIDIO = OFF
//#pragma config FVBUSONIO = OFF
#pragma config FPLLIDIV = DIV_3
#pragma config FPLLMUL = MUL_20
//#pragma config UPLLIDIV = DIV_1
//#pragma config UPLLEN = OFF
#pragma config FPLLODIV = DIV_1
#pragma config FNOSC = PRIPLL
#pragma config FSOSCEN = OFF
#pragma config IESO = OFF
#pragma config POSCMOD = XT
#pragma config OSCIOFNC = OFF
#pragma config FPBDIV = DIV_1
#pragma config FCKSM = CSDCMD
#pragma config FWDTEN = OFF
#pragma config DEBUG = OFF
#pragma config PWP = OFF
#pragma config BWP = OFF
#pragma config CP = OFF

#define mBMXSetRAMKernProgOffset(offset)	(BMXDKPBA = (offset))
#define mBMXSetRAMUserDataOffset(offset)	(BMXDUDBA = (offset))
#define mBMXSetRAMUserProgOffset(offset)	(BMXDUPBA = (offset))

// INIファイル指定キーワード（8文字以内）
const char InitKeywords[][9]={
	"106KEY","101KEY","NUMLOCK","CAPSLOCK","SCRLLOCK","WIDTH36","WIDTH48","WIDTH80"
};
unsigned char initialvmode;

void freadline(char *s,FSFILE *fp){
// ファイルから1行読み込み、配列sに返す
// 最大8文字まで。9文字以上の場合無効
// #または0x20以下のコードを見つけた場合、以降は無視
// s:9バイト以上の配列
// fp:ファイルポインタ
	int n;
	char c,*p;
	n=0;
	p=s;
	*p=0;
	while(n<=8){
		if(FSfread(p,1,1,fp)==0 || *p=='\n'){
			*p=0;
			return;
		}
		if(*p=='#'){
			*p=0;
			break;
		}
		if(*p<=' '){
			if(n>0){
				*p=0;
				break;
			}
			continue;
		}
		p++;
		n++;
	}
	if(n>8) *s=0; //9文字以上の文字列の場合は無効
	//以降の文字は無視
	while(FSfread(&c,1,1,fp) && c!='\n') ;
}
int searchinittext(char *s){
// InitKeywords配列の中から文字列sを探し、位置した場合何番目かを返す
// 見つからなかった場合-1を返す
	int i;
	char *p1;
	const char *p2;
	for(i=0;i<sizeof(InitKeywords)/sizeof(InitKeywords[0]);i++){
		p1=s;
		p2=InitKeywords[i];
		while(*p1==*p2){
			if(*p1==0) return i;
			p1++;
			p2++;
		}
	}
	return -1;
}
void readinifile(void){
	FSFILE *fp;
	char inittext[9];

	fp=FSfopen(INIFILE,"r");
	if(fp==NULL) return;
	printstr("Initialization File Found\n");
	lockkey=0; //INIファイルが存在する場合、Lock関連キーはINIファイルに従う
	while(1){
		if(FSfeof(fp)) break;
		freadline(inittext,fp);
		switch(searchinittext(inittext)){
			case 0:
				keytype=0;//日本語キーボード
				break;
			case 1:
				keytype=1;//英語キーボード
				break;
			case 2:
				lockkey|=2;//Num Lock
				break;
			case 3:
				lockkey|=4;//CAPS Lock
				break;
			case 4:
				lockkey|=1;//Scroll Lock
				break;
			case 5:
				initialvmode=VMODE_STDTEXT;
				break;
			case 6:
				initialvmode=VMODE_WIDETEXT;
				break;
			case 7:
				initialvmode=VMODE_MONOTEXT;
				break;
		}
	}
	FSfclose(fp);
}

void printhex8(unsigned char d){
	printchar("0123456789ABCDEF"[d>>4]);
	printchar("0123456789ABCDEF"[d&0x0f]);	
}

void printhex16(unsigned short d){
	printhex8(d>>8);
	printhex8(d&0x00ff);
}

void printhex32(unsigned int d){
	printhex16(d>>16);
	printhex16(d&0x0000ffff);
}

int main(void){
	char *appname,*s;

	/* ポートの初期設定 */
	CNPUB = 0xFFFF; // PORTB全てプルアップ(I/O)
	TRISB = 0xFFFF; // PORTB全て入力
	CNPUC = 0x4000; // PORTC14プルアップ(U1RX)
	TRISC = 0x4000; // PORTC14以外は出力
	TRISD = KEYSTART | KEYFIRE | KEYUP | KEYDOWN | KEYLEFT | KEYRIGHT;// ボタン接続ポート入力設定
	CNPUE = 0x00E0; // PORTE5-7プルアップ(I/O)
	TRISE = 0x00E0; // PORTE0-4出力5-7入力
	CNPUF = 0x0004; // PORTF2プルアップ(SDI1)
	TRISF = 0x0004; // PORTF2以外は出力
	TRISG = 0x0080; // PORTG7以外は出力

	ANSELB = 0x0000; // 全てデジタル
	ANSELD = 0x0000; // 全てデジタル
	ANSELE = 0x0000; // 全てデジタル
	ANSELG = 0x0000; // 全てデジタル
	CNPUDSET=KEYSTART | KEYFIRE | KEYUP | KEYDOWN | KEYLEFT | KEYRIGHT;// プルアップ設定
	CNPUFSET=0x0004; // PORTF2 (SDI1) プルアップ
	ODCF = 0x0003;	//RF0,RF1はオープンドレイン
	ODCG = 0x000c;//RG2,RG3はオープンドレイン
	LATGSET = 0x000c;

	// 周辺機能ピン割り当て
	SDI2R = 1; //RPG7にSDI2を割り当て
	RPG8R = 6; //RPG8にSDO2を割り当て

	// Make RAM executable. See also "char RAM[RAMSIZE]" in globalvars.c
	mBMXSetRAMKernProgOffset(PIC32MX_RAMSIZE-RAMSIZE);
	mBMXSetRAMUserDataOffset(PIC32MX_RAMSIZE);
	mBMXSetRAMUserProgOffset(PIC32MX_RAMSIZE);

	init_composite(); // ビデオメモリクリア、割り込み初期化、カラービデオ出力開始
	setcursor(0,0,COLOR_NORMALTEXT);

	// Show blue screen if exception before soft reset.
	blue_screen();

	printstr("MachiKania BASIC System\n");
	printstr(" Ver "SYSVER1" "SYSVER2" by KENKEN\n");
	printstr("BASIC Compiler "BASVER"\n");
	printstr(" by Katsumi\n\n");
	//SDカードファイルシステム初期化
	setcursorcolor(COLOR_NORMALTEXT);
	printstr("Init File System...");
	// Initialize the File System
	if(FSInit()==FALSE){ //ファイルシステム初期化
		//エラーの場合停止
		setcursorcolor(COLOR_ERRORTEXT);
		printstr("\nFile System Error\n");
		printstr("Insert Correct Card\n");
		printstr("And Reset\n");
		while(1) asm("wait");
	}
	printstr("OK\n");

	// 音源初期化
	OC4RS=LATFbits.LATF5 ? 0xff:0x00;
	OC3RS=LATFbits.LATF4 ? 0xff:0x00;
	init_music();

	initialvmode=VMODE_STDTEXT; // 標準テキストモード（36文字）
	lockkey=2; // NumLockキーオン
	keytype=0; // 日本語キーボード
	readinifile(); //INIファイル読み込み
	printstr("Init PS/2...");
	wait60thsec(30); //0.5秒待ち
	if(ps2init()){ //PS/2初期化
		//キーボードが見つからない場合
		printstr("Keyboard Not Found\n");
	}
	else printstr("OK\n");

	wait60thsec(60); //1秒待ち

	set_videomode(initialvmode,0); //ビデオモード切替

	// 実行中HEXファイル名がHEXFILEと一致した場合はエディタ起動
	appname=(char*)FILENAME_FLASH_ADDRESS;
	s=HEXFILE;
	while(*s++==*appname++) if(*s==0) texteditor(); //テキストエディター呼び出し

	// 実行中HEXファイル名の「.HEX」を「.BAS」に置き換えてBASファイルを実行
	appname=(char*)FILENAME_FLASH_ADDRESS;
	s=tempfile;
	while(*appname!='.') *s++=*appname++;
	appname=".BAS";
	while(*appname!=0) *s++=*appname++;
	*s=0;
	// buttonmode(); //ボタン有効化
	g_disable_break=1; // Breakキー無効化
	runbasic(tempfile,0);
	while(1) asm(WAIT);
}

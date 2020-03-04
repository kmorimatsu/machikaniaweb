#include <xc.h>
#include "../api.h"
#include "./mweb.h"

/*
	TODO:
		set_videomode()の際の、ビデオモード切替。
			フォント切換え：lib_video_megalopa内で成される。fontdataを表示に使うこと。
			表示モード：videomodeを参照。videomode>=16でグラフィック、micdemode<16でキャラクター
		タイマー２の割り込み処理
			TMR=0とし、割り込み用の関数を呼び出す。表示は、OC2, OC5の割り込みで成されるが、こちらは無視する。
		ソフトウェア割り込み処理
			音楽再生など
		

0x34020000|(stack/4-4); // ori         v0,zero,xx

*/

/*
	Macros for HTML5 functions
*/

int html5data[4];
void html5returnValue();
FSFILE gFileArray[2];

// Construct jump assembly in boot area.
#ifndef __DEBUG
	const unsigned int _debug_boot[] __attribute__((address(0xBFC00000))) ={
		0x0B401C00,//   j           0x9d007000
		0x00000000,//   nop         
};
#endif

// Data area for html5
const unsigned int _html5_data[] __attribute__((address(0x9D006000))) ={
	(unsigned int)(&TVRAM[0]),
	(unsigned int)(&FontData[0]),
	(unsigned int)(&FontData2[0]),
	(unsigned int)(&html5data[0]),
	(unsigned int)(&ps2keystatus[0]),
	(unsigned int)(&vkey),
	(unsigned int)(&gFileArray[0]),
	(unsigned int)(&gFileArray[1]),
};

const unsigned int _html5_func[] __attribute__((address(0x9D006080))) ={
	0x00600008, // jr          v1
	0x00000000, // nop
};

/*
	Implementation of file library
*/

#ifndef __DEBUG
	int FindFirst (const char * fileName, unsigned int attr, SearchRec * rec){ html5func(HTML5FUNC_FindFirst); }
	int FindNext (SearchRec * rec){ html5func(HTML5FUNC_FindNext); }
	int FSmkdir (char * path){ html5func(HTML5FUNC_FSmkdir); }
	char * FSgetcwd (char * path, int numbchars){ html5func(HTML5FUNC_FSgetcwd); }
	int FSchdir (char * path){ html5func(HTML5FUNC_FSchdir); }
	size_t FSfwrite(const void *data_to_write, size_t size, size_t n, FSFILE *stream){ html5func(HTML5FUNC_FSfwrite); }
	int FSremove (const char * fileName){ html5func(HTML5FUNC_FSremove); }
	int FSrename (const char * fileName, FSFILE * fo){ html5func(HTML5FUNC_FSrename); }
	int FSfeof( FSFILE * stream ){ html5func(HTML5FUNC_FSfeof); }
	long FSftell(FSFILE *fo){ html5func(HTML5FUNC_FSftell); }
	int FSfseek(FSFILE *stream, long offset, int whence){ html5func(HTML5FUNC_FSfseek); }
	size_t FSfread(void *ptr, size_t size, size_t n, FSFILE *stream){ html5func(HTML5FUNC_FSfread); }
	void FSrewind (FSFILE *fo){ html5func(HTML5FUNC_FSrewind); }
	int FSfclose(FSFILE *fo){ html5func(HTML5FUNC_FSfclose); }
	FSFILE * FSfopen(const char * fileName, const char *mode){ html5func(HTML5FUNC_FSfopen); }
	int FSInit(void){ html5func(HTML5FUNC_FSInit); }
#endif

/*
	Implementation of PS/2 keyboard library
*/

volatile unsigned char ps2keystatus[256]; // 仮想コードに相当するキーの状態（Onの時1）
volatile unsigned short vkey; //仮想キーコード
unsigned char lockkey; // 初期化時にLockキーの状態指定。下位3ビットが<CAPSLK><NUMLK><SCRLK>
unsigned char keytype; // キーボードの種類。0：日本語109キー、1：英語104キー

#ifndef __DEBUG
	int ps2init(){ return 0; } // PS/2ライブラリ関連初期化。正常終了0、エラーで-1を返す
#endif
unsigned char shiftkeys(){ html5func(HTML5FUNC_shiftkeys); } // SHIFT関連キーの押下状態を返す
unsigned char ps2readkey(){ html5func(HTML5FUNC_ps2readkey); }
// 入力された1つのキーのキーコードをグローバル変数vkeyに格納（押されていなければ0を返す）
// 下位8ビット：キーコード
// 上位8ビット：シフト状態（押下：1）、上位から<0><CAPSLK><NUMLK><SCRLK><Win><ALT><CTRL><SHIFT>
// 英数・記号文字の場合、戻り値としてASCIIコード（それ以外は0を返す）

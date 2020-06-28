// liv_video_megalopa.c
// テキスト＋グラフィックビデオ出力プログラム　PIC32MX370F512H用　by K.Tanaka
// 出力 PORTE（RE0-4）
// グラフィック解像度288×216ドット（標準時）、384×216ドット（ワイド時）
// 256色同時表示、1バイトで1ドットを表す
// テキスト解像度 36×27（標準時）、48×27（ワイド時）
// カラーパレット対応
// クロック周波数3.579545MHz×4×20/3倍
//
// 標準時
// 1ドットがカラーサブキャリアの5分の3周期（16クロック）
// 1ドットあたり4回信号出力（カラーサブキャリア1周期あたり3分の20回出力）
//
// ワイド時
// 1ドットがカラーサブキャリアの20分の9周期（12クロック）
// 1ドットあたり3回信号出力（カラーサブキャリア1周期あたり3分の20回出力）

#define _SUPPRESS_PLIB_WARNING
#include "lib_video_megalopa.h"
#include <plib.h>
/* Begin insertion for MachiKania web */
#include "mweb.h"
// End insertion for MachiKania web */


//カラー信号出力データ
//
#define C_SYN	0
#define C_BLK	7
#define C_WHT	25

#define C_BST1	7
#define C_BST2	5
#define C_BST3	4
#define C_BST4	6
#define C_BST5	10
#define C_BST6	11
#define C_BST7	10
#define C_BST8	6
#define C_BST9	4
#define C_BST10	5
#define C_BST11	7
#define C_BST12	10
#define C_BST13	11
#define C_BST14	9
#define C_BST15	5
#define C_BST16	4
#define C_BST17	5
#define C_BST18	9
#define C_BST19	11
#define C_BST20	10

// パルス幅定数
#define V_NTSC		262				// 262本/画面
#define V_SYNC		10				// 垂直同期本数
#define V_PREEQ		26				// ブランキング区間上側
#define V_LINE		Y_RES				// 画像描画区間
#define H_NTSC		6080				// 1ラインのクロック数（約63.5→63.7μsec）（色副搬送波の228周期）
#define H_SYNC		449				// 水平同期幅、約4.7μsec
#define H_BWVIDEO	1147			// 白黒80文字モードの映像信号開始位置

#define nop()	asm volatile("nop")
#define nop5()	nop();nop();nop();nop();nop();
#define nop10()	nop5();nop5();

// グローバル変数定義
unsigned char *GVRAM; //グラフィックVRAM開始位置のポインタ
unsigned char *GVRAMp; //処理中VRAMアドレス
unsigned char TVRAM[WIDTH_XMAX*WIDTH_Y*2] __attribute__ ((aligned (4))); //TextVRAM
unsigned char *TVRAMp; //処理中TVRAMアドレス
unsigned char fontdata[8*256]; //固定フォント領域、初期化時にFontData[]からコピー
unsigned char *Fontp; //現在のフォントパターンの先頭アドレス
unsigned char *fontp; //処理中の行のフォントパターン先頭アドレス

volatile unsigned short LineCount;	// 処理中の行
volatile unsigned short drawcount;	//　1画面表示終了ごとに1足す。アプリ側で0にする。
					// 最低1回は画面表示したことのチェックと、アプリの処理が何画面期間必要かの確認に利用。
volatile char drawing;		//　映像区間処理中は-1、その他は0
unsigned char videostopflag; // ビデオ出力停止フラグ

unsigned char videomode,textmode,graphmode; //画面モード
int twidth,twidthy; //テキスト文字数（横）および（縦）
int attroffset; // TVRAMのカラー情報エリア位置
int gwidth,gwidthy; // グラフィックX方向解像度

//カラー信号波形テーブル
//256色分のカラーパレット
//20分の3周期単位で3周期分、ただし計算の都合上1色32バイトとする
unsigned char ClTable[32*256] __attribute__ ((aligned (4)));

//バックグランドカラーテーブル
unsigned char BGClTable[20];

/**********************
*  Timer2 割り込み処理関数
***********************/
void __attribute__ ((aligned (16))) __ISR(8, ipl5) T2Handler(void)
{
/* Begin skip for MachiKania web
	asm volatile("#":::"a0");
	asm volatile("#":::"v0");

	//TMR2の値でタイミングのずれを補正
	asm volatile("la	$v0,%0"::"i"(&TMR2));
	asm volatile("lhu	$a0,0($v0)");
	asm volatile("addiu	$a0,$a0,-23");
	asm volatile("bltz	$a0,label1_2");
	asm volatile("addiu	$v0,$a0,-18");
	asm volatile("bgtz	$v0,label1_2");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label1");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");
asm volatile("label1:");
	nop10();nop10();nop();nop();

asm volatile("label1_2:");
	//LATE=C_SYN;
	asm volatile("addiu	$a0,$zero,%0"::"n"(C_SYN));
	asm volatile("la	$v0,%0"::"i"(&LATE));
	asm volatile("sb	$a0,0($v0)");// 同期信号立ち下がり。ここを基準に全ての信号出力のタイミングを調整する

	if(videostopflag){
		if(LineCount==V_SYNC+V_PREEQ+V_LINE)

// End skip for MachiKania web */

	drawcount++;

/* Begin skip for MachiKania web
	}
	else if(LineCount<V_SYNC){
		// 垂直同期期間
		OC5R = H_NTSC-H_SYNC-1;	// 切り込みパルス幅設定
		OC5CON = 0x8001;
	}
	else if(videomode==VMODE_MONOTEXT){
		//モノクロテキストモード(WIDTH 72)
		OC5R = H_SYNC+20;		// 同期パルス幅4.7usec
		OC5CON = 0x8001;
		if(LineCount>=V_SYNC+V_PREEQ && LineCount<V_SYNC+V_PREEQ+WIDTH_Y*8){
			OC2R = H_BWVIDEO-2;		// 映像信号開始位置
			OC2CON = 0x8001;		// タイマ2選択ワンショット
			if(LineCount==V_SYNC+V_PREEQ){
				TVRAMp=TVRAM;
				fontp=Fontp;
				drawing=-1; // 画像描画区間
			}
			else{
				fontp++;// 次の行へ（フォント）
				if(fontp==Fontp+8){
					//次の行へ
					TVRAMp+=twidth;
					fontp=Fontp;
				}
			}
		}
		else if(LineCount==V_SYNC+V_PREEQ+WIDTH_Y*8){
			drawing=0;
			drawcount++;
		}
	}
	else if(videomode<16){
	//テキストモード(WIDTH 30/36/40/48/64)
		OC1R = H_SYNC-1-9;		// 同期パルス幅4.7usec
		OC1CON = 0x8001;		// タイマ2選択ワンショット
		if(LineCount>=V_SYNC+V_PREEQ && LineCount<V_SYNC+V_PREEQ+WIDTH_Y*8){
			if(LineCount==V_SYNC+V_PREEQ){
				TVRAMp=TVRAM;
				fontp=Fontp;
				drawing=-1; // 画像描画区間
			}
			else{
				fontp++;// 次の行へ（フォント）
				if(fontp==Fontp+8){
					//次の行へ
					TVRAMp+=twidth;
					fontp=Fontp;
				}
			}
		}
		else if(LineCount==V_SYNC+V_PREEQ+WIDTH_Y*8){
			drawing=0;
			drawcount++;
		}
	}
	else if(videomode==VMODE_ZOEAGRPH){
	//type Z互換グラフィックモード
		OC1R = H_SYNC-1-9;		// 同期パルス幅4.7usec
		OC1CON = 0x8001;		// タイマ2選択ワンショット
		if(LineCount>=V_SYNC+V_PREEQ-8 && LineCount<V_SYNC+V_PREEQ+V_LINE){
			if(LineCount==V_SYNC+V_PREEQ-8){
				GVRAMp=GVRAM;
				drawing=-1; // 画像描画区間
			}
			else{
				GVRAMp+=X_RESZ/2;// 次の行へ（グラフィック）
			}
		}
		else if(LineCount==V_SYNC+V_PREEQ+V_LINE){
			drawing=0;
			drawcount++;
		}
	}
	else{
	//テキスト＋グラフィックモード
		OC1R = H_SYNC-1-9;		// 同期パルス幅4.7usec
		OC1CON = 0x8001;		// タイマ2選択ワンショット
		if(LineCount>=V_SYNC+V_PREEQ && LineCount<V_SYNC+V_PREEQ+V_LINE){
			if(LineCount==V_SYNC+V_PREEQ){
				GVRAMp=GVRAM;
				TVRAMp=TVRAM;
				fontp=Fontp;
				drawing=-1; // 画像描画区間
			}
			else{
				GVRAMp+=gwidth;// 次の行へ（グラフィック）
				fontp++;// 次の行へ（フォント）
				if(fontp==Fontp+8){
					TVRAMp+=twidth;// 次の行へ（テキスト）
					fontp=Fontp;
				}
			}
		}
		else if(LineCount==V_SYNC+V_PREEQ+V_LINE){
			drawing=0;
			drawcount++;
		}
	}

	LineCount++;
	if(LineCount>=V_NTSC) LineCount=0;

// End skip for MachiKania web */

	mT2ClearIntFlag();			// T2割り込みフラグクリア
	if(LineCount==1) IFS0bits.CS0IF=1;//ソフトウェア割り込み1発生
}

/*********************
*  OC5割り込み処理関数 垂直同期切り込みパルス
*********************/
/* Begin skip for MachiKania web
void __attribute__ ((aligned (16))) __ISR(22, ipl5) OC5Handler(void)
{
	asm volatile("#":::"v0");
	asm volatile("#":::"v1");
	asm volatile("#":::"a0");

	//TMR2の値でタイミングのずれを補正
	asm volatile("la	$v0,%0"::"i"(&TMR2));
	asm volatile("lhu	$a0,0($v0)");
	asm volatile("addiu	$a0,$a0,%0"::"n"(-(H_NTSC-H_SYNC+23)));
	asm volatile("bltz	$a0,label4_2");
	asm volatile("addiu	$v0,$a0,-18");
	asm volatile("bgtz	$v0,label4_2");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label4");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");

asm volatile("label4:");
	nop10();nop10();nop();nop();

asm volatile("label4_2:");
	// 同期信号のリセット
	//	LATE=C_BLK;
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("la	$v0,%0"::"i"(&LATE));
	asm volatile("sb	$v1,0($v0)");	// 同期信号リセット。同期信号立ち下がりから5631サイクル

	mOC5ClearIntFlag();			// OC5割り込みフラグクリア
}
// End skip for MachiKania web */

/*********************
*  OC2割り込み処理関数 モノクロモード映像出力
*********************/
/* Begin skip for MachiKania web
void __attribute__ ((aligned (16))) __ISR(10, ipl5) OC2Handler(void)
{
	nop();nop();nop();

	asm volatile("#":::"v0");
	asm volatile("#":::"v1");
	asm volatile("#":::"a0");
	asm volatile("#":::"a1");
	asm volatile("#":::"a2");
	asm volatile("#":::"a3");
	asm volatile("#":::"t0");
	asm volatile("#":::"t1");
	asm volatile("#":::"t2");
	asm volatile("#":::"t4");
	asm volatile("#":::"t5");

	//	a2=&LATE;
	asm volatile("la	$a2,%0"::"i"(&LATE));

	//	a0=TVRAMp;
	asm volatile("la	$t0,%0"::"i"(&TVRAMp));
	asm volatile("lw	$a0,0($t0)");

	//	t4=fontp;
	asm volatile("la	$t0,%0"::"i"(&fontp));
	asm volatile("lw	$t4,0($t0)");

	//	a3=C_WHT; a1=C_BLK;
	asm volatile("addiu	$a3,$zero,%0"::"n"(C_WHT));
	asm volatile("addiu	$a1,$zero,%0"::"n"(C_BLK));

	asm volatile("addiu	$v0,$zero,80"); //loop counter

	//	t2=*(fontp+(*TVRAMp*8));
	//	if(*(TVRAMp+ATTROFFSETW3) & 0x80) t2~=t2;
	asm volatile("lbu	$t5,0($a0)");
	asm volatile("sll	$t5,$t5,3");
	asm volatile("addu	$t5,$t5,$t4");
	asm volatile("lbu	$t1,0($t5)");
	asm volatile("lbu	$t5,%0($a0)"::"n"(ATTROFFSETBW));
	asm volatile("ext	$t5,$t5,7,1");
	asm volatile("subu	$t5,$zero,$t5");
	asm volatile("xor	$t2,$t1,$t5");

bwtextmodeloop:
asm volatile("bwtextmodeloop:");
asm volatile("nop");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movz	$v1,$a1,$t0");
	asm volatile("sb	$v1,0($a2)"); //最初の出力、水平同期立下りから1147クロック
			asm volatile("addiu	$v0,$v0,-1"); //loop counter
asm volatile("nop");
asm volatile("nop");
	asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movz	$v1,$a1,$t0");
	asm volatile("sb	$v1,0($a2)");
asm volatile("nop");
asm volatile("nop");
asm volatile("nop");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movz	$v1,$a1,$t0");
	asm volatile("sb	$v1,0($a2)");
asm volatile("nop");
asm volatile("nop");
asm volatile("nop");
	asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movz	$v1,$a1,$t0");
	asm volatile("sb	$v1,0($a2)");
asm volatile("nop");
asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movz	$v1,$a1,$t0");
	asm volatile("sb	$v1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movz	$v1,$a1,$t0");
	asm volatile("sb	$v1,0($a2)");
			asm volatile("lbu	$t1,0($t5)");
			asm volatile("lbu	$t5,%0($a0)"::"n"(ATTROFFSETBW));
			asm volatile("ext	$t5,$t5,7,1");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movz	$v1,$a1,$t0");
	asm volatile("sb	$v1,0($a2)");
	asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movz	$v1,$a1,$t0");
			asm volatile("subu	$t5,$zero,$t5");
			asm volatile("xor	$t2,$t1,$t5");
	asm volatile("sb	$v1,0($a2)");
			asm volatile("bnez	$v0,bwtextmodeloop");

asm volatile("nop");
asm volatile("nop");
asm volatile("nop");
asm volatile("nop");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$a1,0($a2)");

	mOC2ClearIntFlag();			// OC2割り込みフラグクリア
}
// End skip for MachiKania web */

/*********************
*  OC1割り込み処理関数 水平同期立ち上がり--カラーバースト--映像信号
*********************/
/* Begin skip for MachiKania web
void __attribute__ ((aligned (16))) __ISR(6, ipl5) OC1Handler(void)
{
	//ビデオモードジャンプテーブル
	static void *vmodejtable[]={
		&&l_width30textmode,
		&&l_stdtextmode,
		&&l_width40textmode,
		&&l_widetextmode,
		&&l_wide6dottextmode,
		0,0,0,0,0,0,0,0,0,0,0,
		&&l_zoeagmode,
		&&l_stdgmode,
		&&l_widegmode
	};

	asm volatile("#":::"v0");
	asm volatile("#":::"v1");
	asm volatile("#":::"a0");
	asm volatile("#":::"a1");
	asm volatile("#":::"a2");
	asm volatile("#":::"a3");
	asm volatile("#":::"t0");
	asm volatile("#":::"t1");
	asm volatile("#":::"t2");
	asm volatile("#":::"t3");
	asm volatile("#":::"t4");
	asm volatile("#":::"t5");

	//TMR2の値でタイミングのずれを補正
	asm volatile("la	$v0,%0"::"i"(&TMR2));
	asm volatile("lhu	$a0,0($v0)");
	asm volatile("addiu	$a0,$a0,%0"::"n"(-(H_SYNC+23)));
	asm volatile("bltz	$a0,label2_2");
	asm volatile("addiu	$v0,$a0,-18");
	asm volatile("bgtz	$v0,label2_2");
	asm volatile("sll	$a0,$a0,2");
	asm volatile("la	$v0,label2");
	asm volatile("addu	$a0,$v0");
	asm volatile("jr	$a0");

asm volatile("label2:");
	nop10();nop10();nop();nop();

asm volatile("label2_2:");
	// 同期信号のリセット
	//	LATE=C_BLK;
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("la	$v0,%0"::"i"(&LATE));
	asm volatile("sb	$v1,0($v0)");	// 同期信号リセット。水平同期立ち下がりから449サイクル

	// 54クロックウェイト
	nop10();nop10();nop10();nop10();nop10();nop();nop();nop();nop();

	// カラーバースト信号 9周期出力
	asm volatile("la	$v0,%0"::"i"(&LATE));
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BST1));

	asm volatile("sb	$v1,0($v0)");	// カラーバースト開始。水平同期立ち下がりから507サイクル
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BST2));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST3));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST4));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST5));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST6));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST7));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST8));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST9));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST10));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST11));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST12));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST13));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST14));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST15));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST16));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST17));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST18));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST19));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST20));nop();nop();

	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST1));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST2));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST3));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST4));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST5));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST6));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST7));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST8));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST9));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST10));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST11));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST12));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST13));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST14));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST15));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST16));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST17));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST18));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST19));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST20));nop();nop();

	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST1));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST2));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST3));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST4));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST5));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST6));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST7));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST8));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST9));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST10));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST11));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST12));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST13));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST14));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST15));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST16));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST17));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST18));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST19));nop();nop();
	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST20));nop();nop();

	asm volatile("sb $v1,0($v0)");asm volatile("addiu $v1,$zero,%0"::"n"(C_BST1));nop();nop();
	asm volatile("sb	$v1,0($v0)");	// カラーバースト終了。水平同期立ち下がりから747サイクル

	//	if(drawing==0) goto label3;  //映像期間でなければ終了
	asm volatile("la	$v0,%0"::"i"(&drawing));
	asm volatile("lb	$t1,0($v0)");
	asm volatile("beqz	$t1,label3");
	asm volatile("nop");

nop();nop();nop();
	// ウェイト
	asm volatile("addiu	$a1,$zero,117");
asm volatile("waitloop1:");
	asm volatile("addiu	$a1,$a1,-1");
	asm volatile("nop");
	asm volatile("bnez	$a1,waitloop1");

	// goto *vmodejtable[videomode];
	asm volatile("la	$t0,%0"::"i"(&videomode));
	asm volatile("la	$t1,%0"::"i"(vmodejtable));
	asm volatile("lbu	$v0,0($t0)");
	asm volatile("sll	$v0,$v0,2");
	asm volatile("addu	$t0,$t1,$v0");
	asm volatile("lw	$v0,0($t0)");
	asm volatile("nop");
	asm volatile("j		$v0");

	nop();nop();nop();nop();//プリフェッチの影響排除用
	nop();nop();nop();nop();
	nop();nop();nop();// 16 align 調整用
//----------------------------------------------------------------------
//　288x216標準グラフィック+テキストモード
//----------------------------------------------------------------------
l_stdgmode:
	//	a1=ClTable;
	asm volatile("la	$a1,%0"::"i"(ClTable));
	//	a2=&LATE;
	asm volatile("la	$a2,%0"::"i"(&LATE));

	//	v0=GVRAMp;
	asm volatile("la	$t0,%0"::"i"(&GVRAMp));
	asm volatile("lw	$v0,0($t0)");

	//	a3=TVRAMp;
	asm volatile("la	$t0,%0"::"i"(&TVRAMp));
	asm volatile("lw	$a3,0($t0)");

	//	t4=fontp;
	asm volatile("la	$t0,%0"::"i"(&fontp));
	asm volatile("lw	$t4,0($t0)");

	asm volatile("lbu	$t5,0($a3)");
	asm volatile("addiu	$a3,$a3,1");
	asm volatile("sll	$t5,$t5,3");
	asm volatile("addu	$t5,$t5,$t4");
	asm volatile("lbu	$t2,0($t5)");
	asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSET-1));

	asm volatile("lbu	$v1,0($v0)");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("addiu	$v0,$v0,1");

	asm volatile("addiu	$a0,$zero,7"); //loop counter

stdgmodeloop:
asm volatile("stdgmodeloop:");

//----------------------------------------------------------------------
//ここが16バイト境界となるようにする
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//最初の映像信号出力。カラーバースト開始から640サイクル
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("addiu	$a0,$a0,-1");	// ループカウンタ
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("nop");
		asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("nop");
		asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lbu	$t5,0($a3)");
			asm volatile("addiu	$a3,$a3,1");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lbu	$t2,0($t5)");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSET-1));
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("nop");
			asm volatile("nop");
//-------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("nop");
		asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("nop");
		asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lbu	$t5,0($a3)");
			asm volatile("addiu	$a3,$a3,1");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lbu	$t2,0($t5)");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSET-1));
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("nop");
			asm volatile("nop");
//-------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("nop");
		asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("nop");
		asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lbu	$t5,0($a3)");
			asm volatile("addiu	$a3,$a3,1");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lbu	$t2,0($t5)");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSET-1));
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("nop");
			asm volatile("nop");
//-------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("nop");
		asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("nop");
		asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lbu	$t5,0($a3)");
			asm volatile("addiu	$a3,$a3,1");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lbu	$t2,0($t5)");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSET-1));
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("nop");
			asm volatile("nop");
//-------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("nop");
		asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t0,$t1,8");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lbu	$t5,0($a3)");
	asm volatile("sb	$t0,0($a2)");
			asm volatile("addiu	$a3,$a3,1");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t0,$t1,8");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
		asm volatile("sb	$t0,0($a2)");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t0,$t1,8");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t0,0($a2)");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSET-1));
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t0,$t1,8");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("sb	$t0,0($a2)");
			asm volatile("bnez	$a0,stdgmodeloop");

	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
//-------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("nop");
		asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("nop");
		asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("nop");
		asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$v1,0($v0)");
		asm volatile("addiu	$v0,$v0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$v1,$v1,5");
		asm volatile("addu	$v1,$v1,$a1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
//----------------------------------------------------------------------

	nop();nop();

	//	LATE=C_BLK;
	asm volatile("addiu	$t1,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
	asm volatile("j		label3");

//nop(); // 16 align 調整用

//----------------------------------------------------------------------
// 36x27標準テキストモード
//----------------------------------------------------------------------
l_stdtextmode:
	nop();
	//	a1=ClTable;
	asm volatile("la	$a1,%0"::"i"(ClTable));

	//	a2=&LATE;
	asm volatile("la	$a2,%0"::"i"(&LATE));

	//	a0=TVRAMp;
	asm volatile("la	$t0,%0"::"i"(&TVRAMp));
	asm volatile("lw	$a0,0($t0)");

	//	t4=fontp;
	asm volatile("la	$t0,%0"::"i"(&fontp));
	asm volatile("lw	$t4,0($t0)");

	//	a3=BGClTable
	asm volatile("la	$a3,%0"::"i"(BGClTable));

	//	t2=*(fontp+(*TVRAMp*8));
	//	t3=&ClTable[*(TVRAMp+ATTROFFSET)*32];
	asm volatile("lbu	$t5,0($a0)");
	asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET));
	asm volatile("sll	$t5,$t5,3");
	asm volatile("addu	$t5,$t5,$t4");
	asm volatile("lbu	$t2,0($t5)");
	asm volatile("sll	$t3,$t3,5");
	asm volatile("addu	$t3,$t3,$a1");

	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("addiu	$a0,$a0,1");

	asm volatile("addiu	$v0,$zero,7"); //loop counter

stdtextmodeloop:
asm volatile("stdtextmodeloop:");
//--------------------------------------------------------------------------
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//最初の映像信号出力。カラーバースト開始から640サイクル
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t2,0($t5)");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t2,0($t5)");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t2,0($t5)");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t2,0($t5)");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t2,0($t5)");
			asm volatile("addiu	$v0,$v0,-1");//loop counter
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t0,$t1,8");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("sb	$t0,0($a2)");
			asm volatile("bnez	$v0,stdtextmodeloop");


//-----------------------------------------------------------------------
asm volatile("nop");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
asm volatile("nop");
asm volatile("nop");

	//	LATE=C_BLK;
	asm volatile("addiu	$t1,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
	asm volatile("j		label3");

//nop();nop();nop();//16 align 調整用
//----------------------------------------------------------------------
// 384x216ワイドテキスト＆グラフィックモード
//----------------------------------------------------------------------
l_widegmode:
	asm volatile("addiu	$sp,$sp,-4");
	asm volatile("sw	$t6,0($sp)");
	asm volatile("addiu	$t6,$zero,9"); //loop counter

	//	a1=ClTable;
	asm volatile("la	$a1,%0"::"i"(ClTable));
	//	a2=&LATE;
	asm volatile("la	$a2,%0"::"i"(&LATE));

	//	v0=GVRAMp;
	asm volatile("la	$t0,%0"::"i"(&GVRAMp));
	asm volatile("lw	$v0,0($t0)");

	//	a3=TVRAMp;
	asm volatile("la	$t0,%0"::"i"(&TVRAMp));
	asm volatile("lw	$a3,0($t0)");

	//	t4=fontp;
	asm volatile("la	$t0,%0"::"i"(&fontp));
	asm volatile("lw	$t4,0($t0)");

	//	t2=*(fontp+*TVRAMp*8);
	//	t3=*(TVRAMp+ATTROFFSETW);
	asm volatile("lbu	$t5,0($a3)");
	asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSETW));
	asm volatile("sll	$t5,$t5,3");
	asm volatile("addu	$t5,$t5,$t4");
	asm volatile("lbu	$t2,0($t5)");

	asm volatile("lbu	$a0,0($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
	asm volatile("lw	$t5,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...

widegmodeloop:
asm volatile("widegmodeloop:");
//--------------------------------------------------------------------------
		asm volatile("lbu	$a0,1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t5,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t5,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$a3,$a3,1");
		asm volatile("lbu	$t1,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a3)");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("sll	$t5,$t5,3");
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,4($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$v0,$v0,8");
	asm volatile("lw	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("lbu	$t1,15($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,-2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("movn	$a0,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,1($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,0($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$t6,$t6,-1"); // loop counter
	asm volatile("lw	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$a3,$a3,1");
		asm volatile("lbu	$t1,7($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a3)");
	asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("sll	$t5,$t5,3");
		asm volatile("lbu	$t1,13($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,4($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$v0,$v0,8");
	asm volatile("lw	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("lbu	$t1,19($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,-2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("movn	$a0,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,5($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,0($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
asm volatile("nop");
	asm volatile("lw	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$a3,$a3,1");
		asm volatile("lbu	$t1,11($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a3)");
	asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("sll	$t5,$t5,3");
		asm volatile("lbu	$t1,17($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,4($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$v0,$v0,8");
	asm volatile("lw	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("lbu	$t1,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,-2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("movn	$a0,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,0($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
asm volatile("nop");
	asm volatile("lw	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$a3,$a3,1");
		asm volatile("lbu	$t1,15($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a3)");
	asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("sll	$t5,$t5,3");
		asm volatile("lbu	$t1,1($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,4($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$v0,$v0,8");
	asm volatile("lw	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("lbu	$t1,7($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,-2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("movn	$a0,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,13($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,0($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
asm volatile("nop");
	asm volatile("lw	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$a3,$a3,1");
		asm volatile("lbu	$t1,19($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a3)");
	asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("sll	$t5,$t5,3");
		asm volatile("lbu	$t1,5($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,4($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$v0,$v0,8");
	asm volatile("lw	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("lbu	$t1,11($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,-2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("swl	$t1,1($a2)");	//ドット最初の出力
		asm volatile("movn	$a0,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("sll	$a0,$a0,5");
	asm volatile("swl	$t1,0($a2)");
	asm volatile("lbu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$a0,$a1");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSETW));
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lbu	$t1,17($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,0($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lw	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sll	$a0,$a0,5");
		asm volatile("swl	$t1,1($a2)");
	asm volatile("addu	$v1,$a0,$a1");
	asm volatile("lw	$t5,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("swl	$t1,0($a2)");
			asm volatile("bnez	$t6,widegmodeloop");

//----------------------------------------------------------------------------40ドット境界

asm volatile("nop");
asm volatile("nop");
		asm volatile("lbu	$a0,1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t5,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t5,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$a3,$a3,1");
		asm volatile("lbu	$t1,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a3)");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("sll	$t5,$t5,3");
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,4($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$v0,$v0,8");
	asm volatile("lw	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("lbu	$t1,15($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,-2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("movn	$a0,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,1($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,0($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
asm volatile("nop");
	asm volatile("lw	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$a3,$a3,1");
		asm volatile("lbu	$t1,7($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a3)");
	asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("sll	$t5,$t5,3");
		asm volatile("lbu	$t1,13($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,4($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$v0,$v0,8");
	asm volatile("lw	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("lbu	$t1,19($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,-2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("movn	$a0,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,5($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,0($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
asm volatile("nop");
	asm volatile("lw	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$a3,$a3,1");
		asm volatile("lbu	$t1,11($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a3)");
	asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("sll	$t5,$t5,3");
		asm volatile("lbu	$t1,17($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,4($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("addiu	$v0,$v0,8");
	asm volatile("lw	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-3($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("lbu	$t1,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,-2($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("lbu	$a0,-1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("movn	$a0,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("sll	$a0,$a0,5");
		asm volatile("addu	$v1,$a0,$a1");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a3)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("lbu	$a0,0($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$a0,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$a0,$a0,5");
	asm volatile("addu	$v1,$a0,$a1");
		asm volatile("sb	$t1,0($a2)");

	asm volatile("lw	$t6,($sp)");
	asm volatile("addiu	$sp,$sp,4");

	//	LATE=C_BLK;
	asm volatile("addiu	$t1,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
	asm volatile("j		label3");

//----------------------------------------------------------------------
// 48x27ワイドテキストモード
//----------------------------------------------------------------------
l_widetextmode:
nop();nop();
	//	a1=ClTable;
	asm volatile("la	$a1,%0"::"i"(ClTable));

	//	a2=&LATE;
	asm volatile("la	$a2,%0"::"i"(&LATE));

	//	a0=TVRAMp;
	asm volatile("la	$t0,%0"::"i"(&TVRAMp));
	asm volatile("lw	$a0,0($t0)");

	//	t4=fontp;
	asm volatile("la	$t0,%0"::"i"(&fontp));
	asm volatile("lw	$t4,0($t0)");

	//	a3=BGClTable
	asm volatile("la	$a3,%0"::"i"(BGClTable));

	//	t2=*(fontp+(*TVRAMp*8));
	//	t3=&ClTable[*(TVRAMp+ATTROFFSET)*32];
	asm volatile("lbu	$t5,0($a0)");
	asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW));
	asm volatile("sll	$t5,$t5,3");
	asm volatile("addu	$t5,$t5,$t4");
	asm volatile("lbu	$t2,0($t5)");
	asm volatile("sll	$t3,$t3,5");
	asm volatile("addu	$t3,$t3,$a1");

	asm volatile("addiu	$v0,$zero,9"); //loop counter

	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
//----------------------------------------------------------------------
	asm volatile("sb	$t1,0($a2)");	//最初の映像信号出力。カラーバースト開始から640サイクル
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t5,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
//----------------------------------------------------------------------
//ここが16バイト境界となるようにする
widetextmodeloop:
asm volatile("widetextmodeloop:");

	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t5,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,15($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("addiu	$a0,$a0,1");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,1($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,7($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,13($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,19($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("addiu	$a0,$a0,1");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,5($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,11($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,17($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("addiu	$a0,$a0,1");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,15($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,1($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,7($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("addiu	$a0,$a0,1");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,13($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,19($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,5($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,11($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("addiu	$a0,$a0,1");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("addiu	$v0,$v0,-1");//loop counter
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,17($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$t5,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("sb	$t1,0($a2)");
			asm volatile("bnez	$v0,widetextmodeloop");

//-----------------------------------------------------------------------

	asm volatile("nop");
	asm volatile("nop");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t5,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,15($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("addiu	$a0,$a0,1");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,1($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,7($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,13($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,19($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("addiu	$a0,$a0,1");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,5($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
//------------------------------------------------------------------8ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,11($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,17($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("addiu	$a0,$a0,1");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("nop");
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW));
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");

	asm volatile("nop");
	asm volatile("nop");

	//	LATE=C_BLK;
	asm volatile("addiu	$t1,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
	asm volatile("j		label3");


//----------------------------------------------------------------------
// 64x27ワイドテキストモード(6ドットフォント)
//----------------------------------------------------------------------
l_wide6dottextmode:
nop();nop();
	//	a1=ClTable;
	asm volatile("la	$a1,%0"::"i"(ClTable));

	//	a2=&LATE;
	asm volatile("la	$a2,%0"::"i"(&LATE));

	//	a0=TVRAMp;
	asm volatile("la	$t0,%0"::"i"(&TVRAMp));
	asm volatile("lw	$a0,0($t0)");

	//	t4=fontp;
	asm volatile("la	$t0,%0"::"i"(&fontp));
	asm volatile("lw	$t4,0($t0)");

	//	a3=BGClTable
	asm volatile("la	$a3,%0"::"i"(BGClTable));

	//	t2=*(fontp+(*TVRAMp*8));
	//	t3=&ClTable[*(TVRAMp+ATTROFFSETW2)*32];
	asm volatile("lbu	$t5,0($a0)");
	asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
	asm volatile("sll	$t5,$t5,3");
	asm volatile("addu	$t5,$t5,$t4");
	asm volatile("lbu	$t2,0($t5)");
	asm volatile("sll	$t3,$t3,5");
	asm volatile("addu	$t3,$t3,$a1");

	asm volatile("addiu	$v0,$zero,6"); //loop counter

	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...

		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//最初の映像信号出力。カラーバースト開始から640サイクル
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t5,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
//----------------------------------------------------------------------
//ここが16バイト境界となるようにする
wide6dottextmodeloop:
asm volatile("wide6dottextmodeloop:");

	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t5,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("addu	$v1,$zero,$a3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t3,$t3,5");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("lbu	$t1,15($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,1($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lw	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,7($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
	asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("lbu	$t1,13($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,19($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("lbu	$t1,5($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("addu	$v1,$zero,$a3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t3,$t3,5");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("lbu	$t1,11($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,17($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lw	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,5($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("lbu	$t1,1($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("addu	$v1,$zero,$a3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t3,$t3,5");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("lbu	$t1,7($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,13($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lw	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,19($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
	asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("lbu	$t1,5($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,11($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("lbu	$t1,17($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("addu	$v1,$zero,$a3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t3,$t3,5");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("lbu	$t1,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lw	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,15($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
	asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("lbu	$t1,1($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,7($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("lbu	$t1,13($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("addu	$v1,$zero,$a3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t3,$t3,5");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("lbu	$t1,19($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,5($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lw	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,11($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("addiu	$v0,$v0,-1");//loop counter
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
	asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("lbu	$t1,17($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("lbu	$t5,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("sb	$t1,0($a2)");
			asm volatile("bnez	$v0,wide6dottextmodeloop");

	asm volatile("nop");
	asm volatile("nop");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t5,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("addu	$v1,$zero,$a3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t3,$t3,5");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("lbu	$t1,15($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,1($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lw	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,7($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
	asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("lbu	$t1,13($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,19($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,2($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("lbu	$t1,5($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("addu	$v1,$zero,$a3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("sll	$t3,$t3,5");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("lbu	$t1,11($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,12($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lhu	$t1,14($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,17($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,18($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("nop");
	asm volatile("lw	$t1,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
		asm volatile("lbu	$t1,3($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,4($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("movn	$v1,$t3,$t0");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t5,0($a0)");
	asm volatile("lhu	$t1,6($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t5,$t5,3");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("addu	$t5,$t5,$t4");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("lbu	$t1,8($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("movn	$v1,$t3,$t0");
	asm volatile("sb	$t1,0($a2)");
			asm volatile("lbu	$t2,0($t5)");
		asm volatile("lbu	$t1,9($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSETW2));
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("lhu	$t1,10($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("addu	$v1,$zero,$a3");
		asm volatile("sb	$t1,0($a2)");

	asm volatile("nop");
	asm volatile("nop");

	//	LATE=C_BLK;
	asm volatile("addiu	$t1,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
	asm volatile("j		label3");

//----------------------------------------------------------------------
// type Z互換グラフィックモード（256x224, 4bit color）
//----------------------------------------------------------------------
l_zoeagmode:
nop();nop();nop();nop();
	asm volatile("addiu	$a1,$zero,82");
asm volatile("zoeagmodewaitloop1:");
	asm volatile("addiu	$a1,$a1,-1");
	asm volatile("nop");
	asm volatile("bnez	$a1,zoeagmodewaitloop1");

	//	a1=ClTable;
	asm volatile("la	$a1,%0"::"i"(ClTable));

	//	a2=&LATE;
	asm volatile("la	$a2,%0"::"i"(&LATE));

	//	v0=GVRAMp;
	asm volatile("la	$t0,%0"::"i"(&GVRAMp));
	asm volatile("lw	$v0,0($t0)");

	asm volatile("addiu	$t4,$zero,12");	//loop counter
	asm volatile("lhu	$a0,0($v0)");	//lhu a0,n(v0) n:0,2,4,6,8,0,2...
	asm volatile("ext	$v1,$a0,12,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t5,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...

zoeagmodeloop:
asm volatile("zoeagmodeloop:");
//--------------------------------------------------------------------------
//ここが16バイト境界となるようにする
asm volatile("nop");
	asm volatile("sb	$t5,0($a2)");	//最初の映像信号出力。カラーバースト開始から880サイクル
	asm volatile("rotr	$t1,$t5,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,8,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,4,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,0,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lhu	$a0,2($v0)");	//lhu a0,n(v0) n:0,2,4,6,8,0,2...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,12,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,8,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,4,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,0,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lhu	$a0,4($v0)");	//lhu a0,n(v0) n:0,2,4,6,8,0,2...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,12,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,8,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,4,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,0,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lhu	$a0,6($v0)");	//lhu a0,n(v0) n:0,2,4,6,8,0,2...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,12,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,8,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,4,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,0,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lhu	$a0,8($v0)");	//lhu a0,n(v0) n:0,2,4,6,8,0,2...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,12,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,8,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,4,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("addiu	$v0,$v0,10");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,0,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lhu	$a0,0($v0)");	//lhu a0,n(v0) n:0,2,4,6,8,0,2...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
			asm volatile("addiu	$t4,$t4,-1");	//loop counter
	asm volatile("ext	$v1,$a0,12,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("sll	$v1,$v1,5");
	asm volatile("addu	$v1,$v1,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("lw	$t5,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("sb	$t1,0($a2)");
			asm volatile("bnez	$t4,zoeagmodeloop");

//--------------------------------------------------------------------------
asm volatile("nop");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t5,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t5,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,8,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,4,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,0,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lhu	$a0,2($v0)");	//lhu a0,n(v0) n:0,2,4,6,8,0,2...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,12,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,8,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,4,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,0,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lhu	$a0,4($v0)");	//lhu a0,n(v0) n:0,2,4,6,8,0,2...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,12,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,8,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,4,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,0,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lhu	$a0,6($v0)");	//lhu a0,n(v0) n:0,2,4,6,8,0,2...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,12,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,8,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,4,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("addu	$v1,$v1,$a1");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("ext	$v1,$a0,0,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
		asm volatile("sll	$v1,$v1,5");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("addu	$v1,$v1,$a1");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("lhu	$a0,8($v0)");	//lhu a0,n(v0) n:0,2,4,6,8,0,2...
		asm volatile("sb	$t1,0($a2)");	//ドット最初の出力
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("ext	$v1,$a0,12,4");	//ext t0,t2,n,4 n:12,8,4,0,12,8...
	asm volatile("sll	$v1,$v1,5");
		asm volatile("sb	$t1,0($a2)");

	asm volatile("nop");
	asm volatile("nop");

	//	LATE=C_BLK;
	asm volatile("addiu	$t1,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
	asm volatile("j		label3");

//nop(); // 16 align 調整用
//----------------------------------------------------------------------
// 30x27標準（互換）テキストモード
//----------------------------------------------------------------------
l_width30textmode:
// 320クロックウェイト
	nop();nop();nop();
	asm volatile("addiu	$a1,$zero,105");
asm volatile("width30waitloop1:");
	asm volatile("addiu	$a1,$a1,-1");
	asm volatile("nop");
	asm volatile("bnez	$a1,width30waitloop1");

	nop();nop();

	//	a1=ClTable;
	asm volatile("la	$a1,%0"::"i"(ClTable));

	//	a2=&LATE;
	asm volatile("la	$a2,%0"::"i"(&LATE));

	//	a0=TVRAMp;
	asm volatile("la	$t0,%0"::"i"(&TVRAMp));
	asm volatile("lw	$a0,0($t0)");

	//	t4=fontp;
	asm volatile("la	$t0,%0"::"i"(&fontp));
	asm volatile("lw	$t4,0($t0)");

	//	a3=BGClTable
	asm volatile("la	$a3,%0"::"i"(BGClTable));

	//	t2=*(fontp+(*TVRAMp*8));
	//	t3=&ClTable[*(TVRAMp+ATTROFFSET30)*32];
	asm volatile("lbu	$t5,0($a0)");
	asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET30));
	asm volatile("sll	$t5,$t5,3");
	asm volatile("addu	$t5,$t5,$t4");
	asm volatile("lbu	$t2,0($t5)");
	asm volatile("sll	$t3,$t3,5");
	asm volatile("addu	$t3,$t3,$a1");

	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("addiu	$a0,$a0,1");

	asm volatile("addiu	$v0,$zero,6"); //loop counter

width30textmodeloop:
asm volatile("width30textmodeloop:");
//--------------------------------------------------------------------------
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//最初の映像信号出力。カラーバースト開始から960サイクル
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET30));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t2,0($t5)");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET30));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t2,0($t5)");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET30));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t2,0($t5)");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET30));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t2,0($t5)");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,1,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,0,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET30));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t2,0($t5)");
			asm volatile("addiu	$v0,$v0,-1");//loop counter
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
			asm volatile("addiu	$a0,$a0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t0,$t1,8");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("sb	$t0,0($a2)");
			asm volatile("bnez	$v0,width30textmodeloop");

asm volatile("nop");
asm volatile("nop");

	//	LATE=C_BLK;
	asm volatile("addiu	$t1,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
	asm volatile("j		label3");

//nop(); // 16 align 調整用
//----------------------------------------------------------------------
// 40x27標準（互換）テキストモード(6dotフォント)
//----------------------------------------------------------------------
l_width40textmode:
// 320クロックウェイト
	nop();nop();nop();
	asm volatile("addiu	$a1,$zero,105");
asm volatile("width40waitloop1:");
	asm volatile("addiu	$a1,$a1,-1");
	asm volatile("nop");
	asm volatile("bnez	$a1,width40waitloop1");

	nop();nop();

	//	a1=ClTable;
	asm volatile("la	$a1,%0"::"i"(ClTable));

	//	a2=&LATE;
	asm volatile("la	$a2,%0"::"i"(&LATE));

	//	a0=TVRAMp;
	asm volatile("la	$t0,%0"::"i"(&TVRAMp));
	asm volatile("lw	$a0,0($t0)");

	//	t4=fontp;
	asm volatile("la	$t0,%0"::"i"(&fontp));
	asm volatile("lw	$t4,0($t0)");

	//	a3=BGClTable
	asm volatile("la	$a3,%0"::"i"(BGClTable));

	//	t2=*(fontp+(*TVRAMp*8));
	//	t3=&ClTable[*(TVRAMp+ATTROFFSET40)*32];
	asm volatile("lbu	$t5,0($a0)");
	asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET40));
	asm volatile("sll	$t5,$t5,3");
	asm volatile("addu	$t5,$t5,$t4");
	asm volatile("lbu	$t2,0($t5)");
	asm volatile("sll	$t3,$t3,5");
	asm volatile("addu	$t3,$t3,$a1");

	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("movn	$v1,$t3,$t0");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
	asm volatile("addiu	$a0,$a0,1");

	asm volatile("addiu	$v0,$zero,4"); //loop counter

//--------------------------------------------------------------------------
//ここが16バイト境界となるようにする
width40textmodeloop:
asm volatile("width40textmodeloop:");

	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET40));
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET40));
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET40));
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET40));
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET40));
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET40));
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET40));
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET40));
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET40));
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
//------------------------------------------------------------------6ドット境界
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,6,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,4($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,4,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
asm volatile("nop");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,3,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
asm volatile("nop");
			asm volatile("lbu	$t5,0($a0)");
		asm volatile("sb	$t1,0($a2)");
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t5,$t5,3");
			asm volatile("addu	$t5,$t5,$t4");
	asm volatile("sb	$t1,0($a2)");	//dot最初の出力
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("addu	$v1,$zero,$a3");
		asm volatile("ext	$t0,$t2,2,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
		asm volatile("movn	$v1,$t3,$t0");
			asm volatile("lbu	$t2,0($t5)");
	asm volatile("sb	$t1,0($a2)");
	asm volatile("rotr	$t1,$t1,8");
			asm volatile("lbu	$t3,%0($a0)"::"n"(ATTROFFSET40));
			asm volatile("addiu	$a0,$a0,1");
	asm volatile("sb	$t1,0($a2)");
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
			asm volatile("sll	$t3,$t3,5");
			asm volatile("addu	$t3,$t3,$a1");
		asm volatile("sb	$t1,0($a2)");	//dot最初の出力
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("addu	$v1,$zero,$a3");
	asm volatile("ext	$t0,$t2,7,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("movn	$v1,$t3,$t0");
			asm volatile("addiu	$v0,$v0,-1"); //loop counter
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t0,$t1,8");
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
		asm volatile("sb	$t0,0($a2)");
			asm volatile("bnez	$v0,width40textmodeloop");
//------------------------------------------------------------------6ドット境界
asm volatile("nop");
asm volatile("nop");

	//	LATE=C_BLK;
	asm volatile("addiu	$t1,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
	asm volatile("j		label3");


asm volatile("label3:");

	mOC1ClearIntFlag();			// OC1割り込みフラグクリア
}
// End skip for MachiKania web */

// グラフィック画面クリア
void g_clearscreen(void)
{
	unsigned int *vp;
	int i;
	vp=(unsigned int *)GVRAM;
	if(graphmode==GMODE_STDGRPH){
		for(i=0;i<X_RES*Y_RES/4;i++) *vp++=0;
	}
	else if(graphmode==GMODE_WIDEGRPH){
		for(i=0;i<X_RESW*Y_RES/4;i++) *vp++=0;
	}
	else if(graphmode==GMODE_ZOEAGRPH){
		for(i=0;i<X_RESZ*Y_RESZ/8;i++) *vp++=0;
	}
}
//テキスト画面クリア
void clearscreen(void)
{
	unsigned int *vp;
	int i;
	vp=(unsigned int *)TVRAM;
	for(i=0;i<WIDTH_XMAX*WIDTH_Y*2/4;i++) *vp++=0;
	cursor=TVRAM;
}

void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g)
{
	// カラーパレット設定（5ビットDA、電源3.3V、1周期を5分割）
	// n:パレット番号0-255、r,g,b:0-255
	// 輝度Y=0.587*G+0.114*B+0.299*R
	// 信号N=Y+0.4921*(B-Y)*sinθ+0.8773*(R-Y)*cosθ
	// 出力データS=(N*0.71[v]+0.29[v])/3.3[v]*64*1.3
/* Begin insertion for MachiKania web */
	html5func(HTML5FUNC_set_palette);
// End insertion for MachiKania web */
/* Begin skip for MachiKania web

	int y;
	y=(150*g+29*b+77*r+128)/256;

	ClTable[n*32+ 0]=(4582*y+   0*((int)b-y)+4020*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*0/20
	ClTable[n*32+ 1]=(4582*y+1824*((int)b-y)+2363*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*1/20
	ClTable[n*32+ 2]=(4582*y+2145*((int)b-y)-1242*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*2/20
	ClTable[n*32+ 3]=(4582*y+ 697*((int)b-y)-3824*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*3/20
	ClTable[n*32+ 4]=(4582*y-1325*((int)b-y)-3252*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*4/20
	ClTable[n*32+ 5]=(4582*y-2255*((int)b-y)+   0*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*5/20
	ClTable[n*32+ 6]=(4582*y-1326*((int)b-y)+3252*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*6/20
	ClTable[n*32+ 7]=(4582*y+ 697*((int)b-y)+3824*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*7/20
	ClTable[n*32+ 8]=(4582*y+2145*((int)b-y)+1242*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*8/20
	ClTable[n*32+ 9]=(4582*y+1824*((int)b-y)-2363*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*9/20
	ClTable[n*32+10]=(4582*y+   0*((int)b-y)-4020*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*10/20
	ClTable[n*32+11]=(4582*y-1824*((int)b-y)-2363*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*11/20
	ClTable[n*32+12]=(4582*y-2145*((int)b-y)+1242*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*12/20
	ClTable[n*32+13]=(4582*y- 697*((int)b-y)+3823*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*13/20
	ClTable[n*32+14]=(4582*y+1325*((int)b-y)+3252*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*14/20
	ClTable[n*32+15]=(4582*y+2255*((int)b-y)+   0*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*15/20
	ClTable[n*32+16]=(4582*y+1326*((int)b-y)-3252*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*16/20
	ClTable[n*32+17]=(4582*y- 697*((int)b-y)-3824*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*17/20
	ClTable[n*32+18]=(4582*y-2145*((int)b-y)-1242*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*18/20
	ClTable[n*32+19]=(4582*y-1824*((int)b-y)+2363*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*19/20
// End skip for MachiKania web */
}
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g)
{
	// バックグラウンドカラー設定
/* Begin insertion for MachiKania web */
	html5func(HTML5FUNC_set_bgcolor);
// End insertion for MachiKania web */
/* Begin skip for MachiKania web

	int y;
	y=(150*g+29*b+77*r+128)/256;

	BGClTable[ 0]=(4582*y+   0*((int)b-y)+4020*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*0/20
	BGClTable[ 1]=(4582*y+1824*((int)b-y)+2363*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*1/20
	BGClTable[ 2]=(4582*y+2145*((int)b-y)-1242*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*2/20
	BGClTable[ 3]=(4582*y+ 697*((int)b-y)-3824*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*3/20
	BGClTable[ 4]=(4582*y-1325*((int)b-y)-3252*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*4/20
	BGClTable[ 5]=(4582*y-2255*((int)b-y)+   0*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*5/20
	BGClTable[ 6]=(4582*y-1326*((int)b-y)+3252*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*6/20
	BGClTable[ 7]=(4582*y+ 697*((int)b-y)+3824*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*7/20
	BGClTable[ 8]=(4582*y+2145*((int)b-y)+1242*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*8/20
	BGClTable[ 9]=(4582*y+1824*((int)b-y)-2363*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*9/20
	BGClTable[10]=(4582*y+   0*((int)b-y)-4020*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*10/20
	BGClTable[11]=(4582*y-1824*((int)b-y)-2363*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*11/20
	BGClTable[12]=(4582*y-2145*((int)b-y)+1242*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*12/20
	BGClTable[13]=(4582*y- 697*((int)b-y)+3823*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*13/20
	BGClTable[14]=(4582*y+1325*((int)b-y)+3252*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*14/20
	BGClTable[15]=(4582*y+2255*((int)b-y)+   0*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*15/20
	BGClTable[16]=(4582*y+1326*((int)b-y)-3252*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*16/20
	BGClTable[17]=(4582*y- 697*((int)b-y)-3824*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*17/20
	BGClTable[18]=(4582*y-2145*((int)b-y)-1242*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*18/20
	BGClTable[19]=(4582*y-1824*((int)b-y)+2363*((int)r-y)+1872*256+32768)/65536;//θ=2Π*3*19/20
// End skip for MachiKania web */
}

void init_palette(void){
	//カラーパレット初期化
	int i;
	for(i=0;i<8;i++){
		set_palette(i,255*(i&1),255*((i>>1)&1),255*(i>>2));
	}
	for(i=0;i<8;i++){
		set_palette(i+8,128*(i&1),128*((i>>1)&1),128*(i>>2));
	}
	for(i=16;i<256;i++){
		set_palette(i,255,255,255);
	}
}
void start_composite(void)
{
	// 変数初期設定
//	LineCount=0;				// 処理中ラインカウンター
	drawing=0;
	TVRAMp=TVRAM;
	fontp=Fontp;
	GVRAMp=GVRAM;

	videostopflag=0;			// ビデオ出力停止解除
//	PR2 = H_NTSC -1; 			// 約63.5usecに設定
//	T2CONSET=0x8000;			// タイマ2スタート
}
void stop_composite(void)
{
//	T2CONCLR = 0x8000;			// タイマ2停止
	videostopflag=1;			// ビデオ出力停止
}

// カラーコンポジット出力初期化
void init_composite(void)
{
	unsigned int *fontROMp,*fontRAMp;
	unsigned int i;

	videomode=VMODE_STDTEXT;
	textmode=TMODE_STDTEXT;
	graphmode=GMODE_NOGRPH;
	twidth=WIDTH_X;
	twidthy=WIDTH_Y;
	attroffset=ATTROFFSET;
	clearscreen();

	//カラーパレット初期化
	init_palette();
	set_bgcolor(0,0,0); //バックグランドカラーは黒
	setcursorcolor(7);

	//フォント初期化　FontData[]からfontdata[]にコピー
	fontROMp=(unsigned int *)FontData;
	fontRAMp=(unsigned int *)fontdata;
	for(i=0;i<256*8/4;i++) *fontRAMp++=*fontROMp++;
	Fontp=fontdata;

	// タイマ2の初期設定,内部クロックで63.5usec周期、1:1
	T2CON = 0x0000;				// タイマ2停止状態
	mT2SetIntPriority(5);			// 割り込みレベル5
	mT2ClearIntFlag();
	mT2IntEnable(1);			// タイマ2割り込み有効化

	// OC1の割り込み有効化
	mOC1SetIntPriority(5);			// 割り込みレベル5
	mOC1ClearIntFlag();
	mOC1IntEnable(1);			// OC1割り込み有効化

	// OC2の割り込み有効化
	mOC2SetIntPriority(5);			// 割り込みレベル5
	mOC2ClearIntFlag();
	mOC2IntEnable(1);			// OC2割り込み有効化

	// OC5の割り込み有効化
	mOC5SetIntPriority(5);			// 割り込みレベル5
	mOC5ClearIntFlag();
	mOC5IntEnable(1);			// OC5割り込み有効化

	OSCCONCLR=0x10; // WAIT命令はアイドルモード

	// Data Memory SRAM wait states: Default Setting = 1; set it to 0
	BMXCONbits.BMXWSDRM = 0; // SRAMのウェイトステートを0にする

	// Flash PM Wait States: MX Flash runs at 3 wait states @ 100 MHz
	CHECONbits.PFMWS = 2; // フラッシュのウェイトステートを2にする（100MHz動作時）

	// Prefetch-cache: Enable prefetch for cacheable PFM instructions
	CHECONbits.PREFEN = 1; //プリフェッチ有効化

	__builtin_mtc0(16, 0, (__builtin_mfc0(16, 0) & 0xfffffff8 | 3)); // キャッシュ有効化

	// Set the interrupt controller for multi-vector mode
	INTCONSET = _INTCON_MVEC_MASK; //割り込みをマルチベクタモードに設定

	// Set the CP0 Status IE bit to turn on interrupts globally
	__builtin_enable_interrupts(); //割り込み有効化

	LineCount=0;				// 処理中ラインカウンター
	PR2 = H_NTSC -1; 			// 約63.5usecに設定
	T2CONSET=0x8000;			// タイマ2スタート
	start_composite();
}

//ビデオモードの切り替え
void set_videomode(unsigned char m, unsigned char *gvram){
// m:ビデオモード
// gvram:グラフィック用メモリ先頭アドレス

	unsigned int *fontROMp,*fontRAMp;
	unsigned int i;
/* Begin insertion for MachiKania web */
	html5func(HTML5FUNC_set_videomode);
// End insertion for MachiKania web */

	if(videomode==m) return;
	stop_composite();
/* Begin modification for MachiKania web */
//	if((videomode!=VMODE_T40 && videomode!=VMODE_WIDETEXT6dot) && (m==VMODE_T40 || m==VMODE_WIDETEXT6dot)){
	if((videomode!=VMODE_T40 && videomode!=VMODE_WIDETEXT6dot && videomode!=VMODE_MONOTEXT) 
			&& (m==VMODE_T40 || m==VMODE_WIDETEXT6dot || m==VMODE_MONOTEXT)){
// End insertion for MachiKania web */
		//6ドットフォントに切り替え
		fontROMp=(unsigned int *)FontData2;
		fontRAMp=(unsigned int *)fontdata;
		for(i=0;i<256*8/4;i++) *fontRAMp++=*fontROMp++;
	}
/* Begin modification for MachiKania web */
//	else if((videomode==VMODE_T40 || videomode==VMODE_WIDETEXT6dot) && (m!=VMODE_T40 && m!=VMODE_WIDETEXT6dot)){
	else if((videomode==VMODE_T40 || videomode==VMODE_WIDETEXT6dot || videomode==VMODE_MONOTEXT) 
			&& (m!=VMODE_T40 && m!=VMODE_WIDETEXT6dot && m!=VMODE_MONOTEXT)){
// End insertion for MachiKania web */
		//8ドットフォントに切り替え
		fontROMp=(unsigned int *)FontData;
		fontRAMp=(unsigned int *)fontdata;
		for(i=0;i<256*8/4;i++) *fontRAMp++=*fontROMp++;
	}
	switch(m){
		case VMODE_T30: // 標準テキスト30文字互換モード
			if(textmode!=TMODE_T30){
				textmode=TMODE_T30;
				twidth=WIDTH_30;
				attroffset=ATTROFFSET30;
				clearscreen();
			}
			break;
		case VMODE_STDTEXT: // 標準テキスト36文字モード
			if(textmode!=TMODE_STDTEXT){
				textmode=TMODE_STDTEXT;
				twidth=WIDTH_X;
				attroffset=ATTROFFSET;
				clearscreen();
			}
			break;
		case VMODE_T40: // 標準テキスト40文字互換モード（6ドットフォント）
			if(textmode!=TMODE_T40){
				textmode=TMODE_T40;
				twidth=WIDTH_40;
				attroffset=ATTROFFSET40;
				clearscreen();
			}
			break;
		case VMODE_WIDETEXT: // ワイドテキスト48文字モード
			if(textmode!=TMODE_WIDETEXT){
				textmode=TMODE_WIDETEXT;
				twidth=WIDTH_XW;
				attroffset=ATTROFFSETW;
				clearscreen();
			}
			break;
		case VMODE_WIDETEXT6dot: // ワイドテキスト64文字モード（6ドットフォント）
			if(textmode!=TMODE_WIDETEXT6dot){
				textmode=TMODE_WIDETEXT6dot;
				twidth=WIDTH_XW2;
				attroffset=ATTROFFSETW2;
				clearscreen();
			}
			break;
		case VMODE_MONOTEXT: // モノクロテキスト80文字モード
			if(textmode!=TMODE_MONOTEXT){
				textmode=TMODE_MONOTEXT;
				twidth=WIDTH_XBW;
				attroffset=ATTROFFSETBW;
				clearscreen();
			}
			break;
		case VMODE_ZOEAGRPH: // type Z互換グラフィックモード
			graphmode=GMODE_ZOEAGRPH;
			gwidth=X_RESZ;
			gwidthy=Y_RESZ;
			break;
		case VMODE_STDGRPH: // 標準グラフィック＋テキスト36文字モード
			graphmode=GMODE_STDGRPH;
			gwidth=X_RES;
			gwidthy=Y_RES;
			if(textmode!=TMODE_STDTEXT){
				textmode=TMODE_STDTEXT;
				twidth=WIDTH_X;
				attroffset=ATTROFFSET;
				clearscreen();
			}
			break;
		case VMODE_WIDEGRPH: // ワイドグラフィック＋テキスト48文字モード
			graphmode=GMODE_WIDEGRPH;
			gwidth=X_RESW;
			gwidthy=Y_RES;
			if(textmode!=TMODE_WIDETEXT){
				textmode=TMODE_WIDETEXT;
				twidth=WIDTH_XW;
				attroffset=ATTROFFSETW;
				clearscreen();
			}
			break;
	}
	videomode=m;
	if(m>=16){
		// グラフィック使用モード
		GVRAM=gvram;
		g_clearscreen();
	}
	else{
		// グラフィック不使用モード
		graphmode=0;
	}
	start_composite();
}

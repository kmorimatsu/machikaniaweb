// TGvideo370f.c
// テキスト＋グラフィックビデオ出力プログラム　PIC32MX370F512H用　by K.Tanaka
// 出力 PORTE（RE0-4）
// グラフィック解像度300×225ドット
// 256色同時表示、1バイトで1ドットを表す
// テキスト解像度 36×28
// カラーパレット対応
// クロック周波数3.579545MHz×4×20/3倍
// 1ドットがカラーサブキャリアの5分の3周期（16クロック）
// 1ドットあたり4回信号出力（カラーサブキャリア1周期あたり3分の20回出力）


#define _SUPPRESS_PLIB_WARNING 1
#include "../TGvideo370f.h"
#include <plib.h>
/* Begin insertion for MachiKania web */
#include "./mweb.h"
// End insertion for MachiKania web */

//カラー信号出力データ
//
#define C_SYN	0
#define C_BLK	7

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
#define V_PREEQ		18				// ブランキング区間上側
#define V_LINE		Y_RES				// 画像描画区間
#define H_NTSC		6080				// 1ラインのクロック数（約63.5→63.7μsec）（色副搬送波の228周期）
#define H_SYNC		449				// 水平同期幅、約4.7μsec

#define nop()	asm volatile("nop")
#define nop5()	nop();nop();nop();nop();nop();
#define nop10()	nop5();nop5();

// グローバル変数定義
unsigned char VRAM[X_RES*Y_RES] __attribute__ ((aligned (4))); //VRAM
unsigned char *VRAMp; //処理中VRAMアドレス
unsigned char TVRAM[WIDTH_X*(WIDTH_Y+1)*2] __attribute__ ((aligned (4))); //TextVRAM
unsigned char *TVRAMp; //処理中TVRAMアドレス
unsigned char PCG[8*256]; //FontDataから初期化時にコピー
unsigned char *Fontp; //処理中行のPCGの先頭アドレス

volatile unsigned short LineCount;	// 処理中の行
volatile unsigned short drawcount;	//　1画面表示終了ごとに1足す。アプリ側で0にする。
					// 最低1回は画面表示したことのチェックと、アプリの処理が何画面期間必要かの確認に利用。
volatile char drawing;		//　映像区間処理中は-1、その他は0


//カラー信号波形テーブル
//256色分のカラーパレット
//20分の3周期単位で3周期分、ただし計算の都合上1色32バイトとする
unsigned char ClTable[32*256] __attribute__ ((aligned (4)));


/**********************
*  Timer2 割り込み処理関数
***********************/
void __ISR(8, ipl5) T2Handler(void)
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

	if(LineCount<V_SYNC){
		// 垂直同期期間
		OC3R = H_NTSC-H_SYNC-1;	// 切り込みパルス幅設定
		OC3CON = 0x8001;
	}
	else{
		OC1R = H_SYNC-1-9;		// 同期パルス幅4.7usec
		OC1CON = 0x8001;		// タイマ2選択ワンショット
		if(LineCount>=V_SYNC+V_PREEQ && LineCount<V_SYNC+V_PREEQ+V_LINE){
			drawing=-1; // 画像描画区間
		}
	}
	LineCount++;
	if(LineCount>=V_NTSC) LineCount=0;
// End skip for MachiKania web */
/* Begin insertion for MachiKania web */
	drawcount++;
// End insertion for MachiKania web */
	mT2ClearIntFlag();			// T2割り込みフラグクリア
}

/*********************
*  OC3割り込み処理関数 垂直同期切り込みパルス
*********************/
/* Begin skip for MachiKania web
void __ISR(14, ipl5) OC3Handler(void)
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

	mOC3ClearIntFlag();			// OC3割り込みフラグクリア
}
// End skip for MachiKania web */

/*********************
*  OC1割り込み処理関数 水平同期立ち上がり--カラーバースト--映像信号
*********************/
/* Begin skip for MachiKania web
void __ISR(6, ipl5) OC1Handler(void)
{
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
	nop();

	// 208クロックウェイト
	nop10();nop10();nop10();nop10();nop10();nop10();nop10();nop10();nop10();nop10();
	nop10();nop10();nop10();nop10();nop10();nop10();nop10();nop10();nop10();nop10();
	nop5();nop();nop();nop();


// 映像信号出力

	//	a0=VRAM;
	asm volatile("la	$a0,%0"::"i"(VRAM));
	//	a1=ClTable;
	asm volatile("la	$a1,%0"::"i"(ClTable));
	//	a2=&LATE;
	asm volatile("la	$a2,%0"::"i"(&LATE));

	//	v0=VRAMp;
	asm volatile("la	$t0,%0"::"i"(&VRAMp));
	asm volatile("lw	$v0,0($t0)");

	//	a3=TVRAMp;
	asm volatile("la	$t0,%0"::"i"(&TVRAMp));
	asm volatile("lw	$a3,0($t0)");

	//	t4=Fontp;
	asm volatile("la	$t0,%0"::"i"(&Fontp));
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

//----------------------------------------------------------------------
	asm volatile("sb	$t1,0($a2)");	//最初の映像信号出力。水平同期立下りから987サイクル
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
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,8($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,12($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
		asm volatile("lw	$t1,16($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lw	$t1,0($v1)");	//lw t1,n(v1) n:0,4,8,12,16,0,4...
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
	asm volatile("lbu	$v1,0($v0)");
	asm volatile("addiu	$v0,$v0,1");
		asm volatile("sb	$t1,0($a2)");
		asm volatile("rotr	$t1,$t1,8");
	asm volatile("nop");
	asm volatile("nop");
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
		asm volatile("nop");
		asm volatile("nop");
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
	asm volatile("nop");
	asm volatile("nop");
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
		asm volatile("nop");
		asm volatile("nop");
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
	asm volatile("nop");
	asm volatile("nop");
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
		asm volatile("nop");
		asm volatile("nop");
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
	asm volatile("nop");
	asm volatile("nop");
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
		asm volatile("nop");
		asm volatile("nop");
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
	asm volatile("nop");
	asm volatile("nop");
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
		asm volatile("nop");
		asm volatile("nop");
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
	asm volatile("nop");
	asm volatile("nop");
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
		asm volatile("nop");
		asm volatile("nop");
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

	VRAMp+=X_RES;// 次の行へ（グラフィック）
	Fontp++;// 次の行へ（フォント）
	if(Fontp==PCG+8){
		TVRAMp+=WIDTH_X;// 次の行へ（テキスト）
		Fontp=PCG;
	}
	if(LineCount==V_SYNC+V_PREEQ+V_LINE){ // 1画面最後の描画終了
			drawing=0;
			drawcount++;
			VRAMp=VRAM;
			TVRAMp=TVRAM;
			Fontp=PCG;
	}

asm volatile("label3:");
	mOC1ClearIntFlag();			// OC1割り込みフラグクリア
}
// End skip for MachiKania web */

// グラフィック画面クリア
void clearscreen(void)
{
	unsigned int *vp;
	int i;
	vp=(unsigned int *)VRAM;
	for(i=0;i<X_RES*Y_RES/4;i++) *vp++=0;
}
//テキスト画面クリア
void clearTVRAM(void)
{
	unsigned int *vp;
	int i;
	vp=(unsigned int *)TVRAM;
	for(i=0;i<WIDTH_X*(WIDTH_Y+1)*2/4;i++) *vp++=0;
}

void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g)
{
	// カラーパレット設定（5ビットDA、電源3.3V、1周期を5分割）
	// n:パレット番号0?255、r,g,b:0?255
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

void start_composite(void)
{
	// 変数初期設定
	LineCount=0;				// 処理中ラインカウンター
	drawing=0;
	VRAMp=VRAM;
	TVRAMp=TVRAM;
	Fontp=PCG;

	PR2 = H_NTSC -1; 			// 約63.5usecに設定
	T2CONSET=0x8000;			// タイマ2スタート
}
void stop_composite(void)
{
	T2CONCLR = 0x8000;			// タイマ2停止
}

// カラーコンポジット出力初期化
void init_composite(void)
{
	unsigned int i;
	unsigned int *fontp,*pcgp;

	clearscreen();
	clearTVRAM();

	//フォント初期化　FontData[]からRAM上のPCG[]にコピー
	fontp=(unsigned int *)FontData;
	pcgp=(unsigned int *)PCG;
	for(i=0;i<256*8/4;i++) *pcgp++=*fontp++;

	//カラーパレット初期化
	for(i=0;i<8;i++){
		set_palette(i,255*(i&1),255*((i>>1)&1),255*(i>>2));
	}
	for(i=0;i<8;i++){
		set_palette(i+8,128*(i&1),128*((i>>1)&1),128*(i>>2));
	}
	for(i=16;i<256;i++){
		set_palette(i,255,255,255);
	}

	// タイマ2の初期設定,内部クロックで63.5usec周期、1:1
	T2CON = 0x0000;				// タイマ2停止状態
	mT2SetIntPriority(5);			// 割り込みレベル5
	mT2ClearIntFlag();
	mT2IntEnable(1);			// タイマ2割り込み有効化

	// OC1の割り込み有効化
	mOC1SetIntPriority(5);			// 割り込みレベル5
	mOC1ClearIntFlag();
	mOC1IntEnable(1);			// OC1割り込み有効化

	// OC3の割り込み有効化
	mOC3SetIntPriority(5);			// 割り込みレベル5
	mOC3ClearIntFlag();
	mOC3IntEnable(1);			// OC3割り込み有効化

	OSCCONCLR=0x10; // WAIT命令はアイドルモード
	INTEnableSystemMultiVectoredInt();
	SYSTEMConfig(95000000,SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE); //キャッシュ有効化（周辺クロックには適用しない）
	BMXCONCLR=0x40;	// RAMアクセスウェイト0
	start_composite();
}

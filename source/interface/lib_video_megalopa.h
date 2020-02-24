// lib_video_megalopa.h
// テキスト＋グラフィックビデオ出力　PIC32MX370F512H用ヘッダーファイル　by K.Tanaka

#define X_RES	288 // Graphic横方向解像度
#define X_RESW	384 // Graphic横方向解像度（ワイド時）
#define X_RESZ	256 // Graphic横方向解像度（type Z互換時）
#define Y_RES	216 // Graphic縦方向解像度
#define Y_RESZ	224 // Graphic縦方向解像度（type Z互換時）
#define WIDTH_X	36 // 横方向文字数
#define WIDTH_Y	27 // 縦方向文字数
#define WIDTH_XW	48 // 横方向文字数（ワイド時）
#define WIDTH_XW2	64 // 横方向文字数（ワイド、6ドットフォント時）
#define WIDTH_XBW	80 // 横方向文字数（モノクロ時）
#define WIDTH_30	30 // 横方向文字数（互換モード8ドットフォント時）
#define WIDTH_40	40 // 横方向文字数（互換モード6ドットフォント時）
#define ATTROFFSET	(WIDTH_X*WIDTH_Y) // TVRAM上のカラーパレット格納位置（標準時）
#define ATTROFFSETW	(WIDTH_XW*WIDTH_Y) // TVRAM上のカラーパレット格納位置（ワイド時）
#define ATTROFFSETW2	(WIDTH_XW2*WIDTH_Y) // TVRAM上のカラーパレット格納位置（ワイド、6ドットフォント時）
#define ATTROFFSETBW	(WIDTH_XBW*WIDTH_Y) // TVRAM上のカラーパレット格納位置（モノクロ時）
#define ATTROFFSET30	(WIDTH_30*WIDTH_Y) // TVRAM上のカラーパレット格納位置（互換モード8ドットフォント時）
#define ATTROFFSET40	(WIDTH_40*WIDTH_Y) // TVRAM上のカラーパレット格納位置（互換モード6ドットフォント時）
#define WIDTH_XMAX WIDTH_XBW // 横方向最大文字数

// videomode値
#define VMODE_T30 0 // 標準テキスト30文字互換モード
#define VMODE_STDTEXT 1 // 標準テキスト36文字モード
#define VMODE_T40 2 // 標準テキスト40文字互換モード（6ドットフォント）
#define VMODE_WIDETEXT 3 // ワイドテキスト48文字モード
#define VMODE_WIDETEXT6dot 4 // ワイドテキスト64文字モード（6ドットフォント）
#define VMODE_MONOTEXT 5 // モノクロテキスト80文字モード
#define VMODE_ZOEAGRPH 16 // type Z互換グラフィックモード
#define VMODE_STDGRPH 17 // 標準グラフィック＋テキスト36文字モード
#define VMODE_WIDEGRPH 18 // ワイドグラフィック＋テキスト48文字モード

// textomode値
#define TMODE_T30 0 // 標準テキスト30文字互換モード
#define TMODE_STDTEXT 1 // 標準テキスト36文字モード
#define TMODE_T40 2 // 標準テキスト40文字互換モード（6ドットフォント）
#define TMODE_WIDETEXT 3 // ワイドテキスト48文字モード
#define TMODE_WIDETEXT6dot 4 // ワイドテキスト64文字モード（6ドットフォント）
#define TMODE_MONOTEXT 5 // モノクロテキスト80文字モード

// graphmode値
#define GMODE_NOGRPH 0 // グラフィック不使用
#define GMODE_ZOEAGRPH 1 // type Z互換グラフィックモード
#define GMODE_STDGRPH 2 // 標準グラフィック＋テキスト36文字モード
#define GMODE_WIDEGRPH 3 // ワイドグラフィック＋テキスト48文字モード

#define KEYPORT PORTD
#define KEYUP 0x0004
#define KEYDOWN 0x0001
#define KEYLEFT 0x0002
#define KEYRIGHT 0x0008
#define KEYSTART 0x0010
#define KEYFIRE 0x0020

extern volatile char drawing;		//　表示期間中は-1
extern volatile unsigned short drawcount;		//　1画面表示終了ごとに1足す。アプリ側で0にする。
							// 最低1回は画面表示したことのチェックと、アプリの処理が何画面期間必要かの確認に利用。
extern unsigned char *GVRAM; // Graphicビデオメモリ
extern unsigned char TVRAM[]; // Characterビデオメモリ
extern const unsigned char FontData[]; //フォントパターン(初期化時にここからfontdata[]にコピー)
extern const unsigned char FontData2[]; //フォントパターン6ドットフォント
extern unsigned char videomode,textmode,graphmode; //画面モード
extern int twidth,twidthy; //テキスト文字数（横）および（縦
extern int attroffset; // TVRAMのカラー情報エリア位置
extern int gwidth,gwidthy; // グラフィックX方向解像度
extern unsigned char fontdata[]; //固定フォント領域、初期化時にFontData[]からコピー
extern unsigned char *Fontp; //現在のフォントパターンの先頭アドレス

void start_composite(void); //カラーコンポジット出力開始
void stop_composite(void); //カラーコンポジット出力停止
void init_composite(void); //カラーコンポジット出力初期化
void init_palette(void); //カラーパレット初期化
void g_clearscreen(void); //Graphic画面クリア
void clearscreen(void); //Character画面クリア
void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g); //カラーパレット設定
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g); // バックグラウンドカラー設定
void set_videomode(unsigned char m, unsigned char *gvram); //ビデオモードの切り替え

// 以下は text_graph_library.c ライブラリを使用するための宣言

// グラフィック画面関連
void g_pset(int x,int y,unsigned int c);
	// (x,y)の位置にカラーcで点を描画
void g_putbmpmn(int x,int y,char m,char n,const unsigned char bmp[]);
	// 横m*縦nドットのキャラクターを座標x,yに表示
	// unsigned char bmp[m*n]配列に、単純にカラー番号を並べる
	// カラー番号が0の部分は透明色として扱う
void g_clrbmpmn(int x,int y,char m,char n);
	// 縦m*横nドットのキャラクター消去
	// カラー0で塗りつぶし
void g_gline(int x1,int y1,int x2,int y2,unsigned int c);
	// (x1,y1)-(x2,y2)にカラーcで線分を描画
void g_hline(int x1,int x2,int y,unsigned int c);
	// (x1,y)-(x2,y)の水平ラインをカラーcで高速描画
void g_boxfill(int x1,int y1,int x2,int y2,unsigned int c);
	//座標(x1,y1),(x2,y2)を対角線とするカラーcで塗られた長方形を描画
	// (x1,y1),(x2,y2)を対角線とするカラーcで塗られた長方形を描画
void g_circle(int x0,int y0,unsigned int r,unsigned int c);
	// (x0,y0)を中心に、半径r、カラーcの円を描画
void g_circlefill(int x0,int y0,unsigned int r,unsigned int c);
	// (x0,y0)を中心に、半径r、カラーcで塗られた円を描画
void g_putfont(int x,int y,unsigned int c,int bc,unsigned char n);
	//8*8ドットのアルファベットフォント表示
	//座標（x,y)、カラー番号c
	//bc:バックグランドカラー、負数の場合無視
	//n:文字番号
void g_printstr(int x,int y,unsigned int c,int bc,unsigned char *s);
	//座標(x,y)からカラー番号cで文字列sを表示、bc:バックグランドカラー
void g_printnum(int x,int y,unsigned char c,int bc,unsigned int n);
	//座標(x,y)にカラー番号cで数値nを表示、bc:バックグランドカラー
void g_printnum2(int x,int y,unsigned char c,int bc,unsigned int n,unsigned char e);
	//座標(x,y)にカラー番号cで数値nを表示、bc:バックグランドカラー、e桁で表示
unsigned int g_color(int x,int y);
//座標(x,y)のVRAM上の現在のパレット番号を返す、画面外は0を返す

//テキスト画面関連
extern unsigned char *cursor;
extern unsigned char cursorcolor;
void vramscroll(void);
	//1行スクロール
void setcursor(unsigned char x,unsigned char y,unsigned char c);
	//カーソル位置とカラーを設定
void setcursorcolor(unsigned char c);
	//カーソル位置そのままでカラー番号をcに設定
void printchar(unsigned char n);
	//カーソル位置にテキストコードnを1文字表示し、カーソルを1文字進める
void printstr(unsigned char *s);
	//カーソル位置に文字列sを表示
void printnum(unsigned int n);
	//カーソル位置に符号なし整数nを10進数表示
void printnum2(unsigned int n,unsigned char e);
	//カーソル位置に符号なし整数nをe桁の10進数表示（前の空き桁部分はスペースで埋める）
void cls(void);
	//テキスト画面を0でクリアし、カーソルを画面先頭に移動
void startPCG(unsigned char *p,int a);
	// RAMフォント（PCG）の利用開始、pがフォント格納場所、aが0以外でシステムフォントをコピー
void stopPCG(void);
	// RAMフォント（PCG）の利用停止

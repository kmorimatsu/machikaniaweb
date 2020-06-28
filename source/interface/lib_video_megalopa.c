// liv_video_megalopa.c
// �e�L�X�g�{�O���t�B�b�N�r�f�I�o�̓v���O�����@PIC32MX370F512H�p�@by K.Tanaka
// �o�� PORTE�iRE0-4�j
// �O���t�B�b�N�𑜓x288�~216�h�b�g�i�W�����j�A384�~216�h�b�g�i���C�h���j
// 256�F�����\���A1�o�C�g��1�h�b�g��\��
// �e�L�X�g�𑜓x 36�~27�i�W�����j�A48�~27�i���C�h���j
// �J���[�p���b�g�Ή�
// �N���b�N���g��3.579545MHz�~4�~20/3�{
//
// �W����
// 1�h�b�g���J���[�T�u�L�����A��5����3�����i16�N���b�N�j
// 1�h�b�g������4��M���o�́i�J���[�T�u�L�����A1����������3����20��o�́j
//
// ���C�h��
// 1�h�b�g���J���[�T�u�L�����A��20����9�����i12�N���b�N�j
// 1�h�b�g������3��M���o�́i�J���[�T�u�L�����A1����������3����20��o�́j

#define _SUPPRESS_PLIB_WARNING
#include "lib_video_megalopa.h"
#include <plib.h>
/* Begin insertion for MachiKania web */
#include "mweb.h"
// End insertion for MachiKania web */


//�J���[�M���o�̓f�[�^
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

// �p���X���萔
#define V_NTSC		262				// 262�{/���
#define V_SYNC		10				// ���������{��
#define V_PREEQ		26				// �u�����L���O��ԏ㑤
#define V_LINE		Y_RES				// �摜�`����
#define H_NTSC		6080				// 1���C���̃N���b�N���i��63.5��63.7��sec�j�i�F�������g��228�����j
#define H_SYNC		449				// �����������A��4.7��sec
#define H_BWVIDEO	1147			// ����80�������[�h�̉f���M���J�n�ʒu

#define nop()	asm volatile("nop")
#define nop5()	nop();nop();nop();nop();nop();
#define nop10()	nop5();nop5();

// �O���[�o���ϐ���`
unsigned char *GVRAM; //�O���t�B�b�NVRAM�J�n�ʒu�̃|�C���^
unsigned char *GVRAMp; //������VRAM�A�h���X
unsigned char TVRAM[WIDTH_XMAX*WIDTH_Y*2] __attribute__ ((aligned (4))); //TextVRAM
unsigned char *TVRAMp; //������TVRAM�A�h���X
unsigned char fontdata[8*256]; //�Œ�t�H���g�̈�A����������FontData[]����R�s�[
unsigned char *Fontp; //���݂̃t�H���g�p�^�[���̐擪�A�h���X
unsigned char *fontp; //�������̍s�̃t�H���g�p�^�[���擪�A�h���X

volatile unsigned short LineCount;	// �������̍s
volatile unsigned short drawcount;	//�@1��ʕ\���I�����Ƃ�1�����B�A�v������0�ɂ���B
					// �Œ�1��͉�ʕ\���������Ƃ̃`�F�b�N�ƁA�A�v���̏���������ʊ��ԕK�v���̊m�F�ɗ��p�B
volatile char drawing;		//�@�f����ԏ�������-1�A���̑���0
unsigned char videostopflag; // �r�f�I�o�͒�~�t���O

unsigned char videomode,textmode,graphmode; //��ʃ��[�h
int twidth,twidthy; //�e�L�X�g�������i���j����сi�c�j
int attroffset; // TVRAM�̃J���[���G���A�ʒu
int gwidth,gwidthy; // �O���t�B�b�NX�����𑜓x

//�J���[�M���g�`�e�[�u��
//256�F���̃J���[�p���b�g
//20����3�����P�ʂ�3�������A�������v�Z�̓s����1�F32�o�C�g�Ƃ���
unsigned char ClTable[32*256] __attribute__ ((aligned (4)));

//�o�b�N�O�����h�J���[�e�[�u��
unsigned char BGClTable[20];

/**********************
*  Timer2 ���荞�ݏ����֐�
***********************/
void __attribute__ ((aligned (16))) __ISR(8, ipl5) T2Handler(void)
{
/* Begin skip for MachiKania web
	asm volatile("#":::"a0");
	asm volatile("#":::"v0");

	//TMR2�̒l�Ń^�C�~���O�̂����␳
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
	asm volatile("sb	$a0,0($v0)");// �����M������������B��������ɑS�Ă̐M���o�͂̃^�C�~���O�𒲐�����

	if(videostopflag){
		if(LineCount==V_SYNC+V_PREEQ+V_LINE)

// End skip for MachiKania web */

	drawcount++;

/* Begin skip for MachiKania web
	}
	else if(LineCount<V_SYNC){
		// ������������
		OC5R = H_NTSC-H_SYNC-1;	// �؂荞�݃p���X���ݒ�
		OC5CON = 0x8001;
	}
	else if(videomode==VMODE_MONOTEXT){
		//���m�N���e�L�X�g���[�h(WIDTH 72)
		OC5R = H_SYNC+20;		// �����p���X��4.7usec
		OC5CON = 0x8001;
		if(LineCount>=V_SYNC+V_PREEQ && LineCount<V_SYNC+V_PREEQ+WIDTH_Y*8){
			OC2R = H_BWVIDEO-2;		// �f���M���J�n�ʒu
			OC2CON = 0x8001;		// �^�C�}2�I�������V���b�g
			if(LineCount==V_SYNC+V_PREEQ){
				TVRAMp=TVRAM;
				fontp=Fontp;
				drawing=-1; // �摜�`����
			}
			else{
				fontp++;// ���̍s�ցi�t�H���g�j
				if(fontp==Fontp+8){
					//���̍s��
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
	//�e�L�X�g���[�h(WIDTH 30/36/40/48/64)
		OC1R = H_SYNC-1-9;		// �����p���X��4.7usec
		OC1CON = 0x8001;		// �^�C�}2�I�������V���b�g
		if(LineCount>=V_SYNC+V_PREEQ && LineCount<V_SYNC+V_PREEQ+WIDTH_Y*8){
			if(LineCount==V_SYNC+V_PREEQ){
				TVRAMp=TVRAM;
				fontp=Fontp;
				drawing=-1; // �摜�`����
			}
			else{
				fontp++;// ���̍s�ցi�t�H���g�j
				if(fontp==Fontp+8){
					//���̍s��
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
	//type Z�݊��O���t�B�b�N���[�h
		OC1R = H_SYNC-1-9;		// �����p���X��4.7usec
		OC1CON = 0x8001;		// �^�C�}2�I�������V���b�g
		if(LineCount>=V_SYNC+V_PREEQ-8 && LineCount<V_SYNC+V_PREEQ+V_LINE){
			if(LineCount==V_SYNC+V_PREEQ-8){
				GVRAMp=GVRAM;
				drawing=-1; // �摜�`����
			}
			else{
				GVRAMp+=X_RESZ/2;// ���̍s�ցi�O���t�B�b�N�j
			}
		}
		else if(LineCount==V_SYNC+V_PREEQ+V_LINE){
			drawing=0;
			drawcount++;
		}
	}
	else{
	//�e�L�X�g�{�O���t�B�b�N���[�h
		OC1R = H_SYNC-1-9;		// �����p���X��4.7usec
		OC1CON = 0x8001;		// �^�C�}2�I�������V���b�g
		if(LineCount>=V_SYNC+V_PREEQ && LineCount<V_SYNC+V_PREEQ+V_LINE){
			if(LineCount==V_SYNC+V_PREEQ){
				GVRAMp=GVRAM;
				TVRAMp=TVRAM;
				fontp=Fontp;
				drawing=-1; // �摜�`����
			}
			else{
				GVRAMp+=gwidth;// ���̍s�ցi�O���t�B�b�N�j
				fontp++;// ���̍s�ցi�t�H���g�j
				if(fontp==Fontp+8){
					TVRAMp+=twidth;// ���̍s�ցi�e�L�X�g�j
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

	mT2ClearIntFlag();			// T2���荞�݃t���O�N���A
	if(LineCount==1) IFS0bits.CS0IF=1;//�\�t�g�E�F�A���荞��1����
}

/*********************
*  OC5���荞�ݏ����֐� ���������؂荞�݃p���X
*********************/
/* Begin skip for MachiKania web
void __attribute__ ((aligned (16))) __ISR(22, ipl5) OC5Handler(void)
{
	asm volatile("#":::"v0");
	asm volatile("#":::"v1");
	asm volatile("#":::"a0");

	//TMR2�̒l�Ń^�C�~���O�̂����␳
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
	// �����M���̃��Z�b�g
	//	LATE=C_BLK;
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("la	$v0,%0"::"i"(&LATE));
	asm volatile("sb	$v1,0($v0)");	// �����M�����Z�b�g�B�����M�����������肩��5631�T�C�N��

	mOC5ClearIntFlag();			// OC5���荞�݃t���O�N���A
}
// End skip for MachiKania web */

/*********************
*  OC2���荞�ݏ����֐� ���m�N�����[�h�f���o��
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
	asm volatile("sb	$v1,0($a2)"); //�ŏ��̏o�́A�������������肩��1147�N���b�N
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

	mOC2ClearIntFlag();			// OC2���荞�݃t���O�N���A
}
// End skip for MachiKania web */

/*********************
*  OC1���荞�ݏ����֐� �������������オ��--�J���[�o�[�X�g--�f���M��
*********************/
/* Begin skip for MachiKania web
void __attribute__ ((aligned (16))) __ISR(6, ipl5) OC1Handler(void)
{
	//�r�f�I���[�h�W�����v�e�[�u��
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

	//TMR2�̒l�Ń^�C�~���O�̂����␳
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
	// �����M���̃��Z�b�g
	//	LATE=C_BLK;
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BLK));
	asm volatile("la	$v0,%0"::"i"(&LATE));
	asm volatile("sb	$v1,0($v0)");	// �����M�����Z�b�g�B�����������������肩��449�T�C�N��

	// 54�N���b�N�E�F�C�g
	nop10();nop10();nop10();nop10();nop10();nop();nop();nop();nop();

	// �J���[�o�[�X�g�M�� 9�����o��
	asm volatile("la	$v0,%0"::"i"(&LATE));
	asm volatile("addiu	$v1,$zero,%0"::"n"(C_BST1));

	asm volatile("sb	$v1,0($v0)");	// �J���[�o�[�X�g�J�n�B�����������������肩��507�T�C�N��
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
	asm volatile("sb	$v1,0($v0)");	// �J���[�o�[�X�g�I���B�����������������肩��747�T�C�N��

	//	if(drawing==0) goto label3;  //�f�����ԂłȂ���ΏI��
	asm volatile("la	$v0,%0"::"i"(&drawing));
	asm volatile("lb	$t1,0($v0)");
	asm volatile("beqz	$t1,label3");
	asm volatile("nop");

nop();nop();nop();
	// �E�F�C�g
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

	nop();nop();nop();nop();//�v���t�F�b�`�̉e���r���p
	nop();nop();nop();nop();
	nop();nop();nop();// 16 align �����p
//----------------------------------------------------------------------
//�@288x216�W���O���t�B�b�N+�e�L�X�g���[�h
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
//������16�o�C�g���E�ƂȂ�悤�ɂ���
	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//�ŏ��̉f���M���o�́B�J���[�o�[�X�g�J�n����640�T�C�N��
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
			asm volatile("addiu	$a0,$a0,-1");	// ���[�v�J�E���^
	asm volatile("nop");
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//-------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//-------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//-------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//-------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//-------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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

//nop(); // 16 align �����p

//----------------------------------------------------------------------
// 36x27�W���e�L�X�g���[�h
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
	asm volatile("sb	$t1,0($a2)");	//�ŏ��̉f���M���o�́B�J���[�o�[�X�g�J�n����640�T�C�N��
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

//nop();nop();nop();//16 align �����p
//----------------------------------------------------------------------
// 384x216���C�h�e�L�X�g���O���t�B�b�N���[�h
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
	asm volatile("sb	$t5,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("swl	$t1,1($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
		asm volatile("lw	$t1,16($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
	asm volatile("movn	$a0,$t3,$t0");
	asm volatile("sll	$a0,$a0,5");
		asm volatile("swl	$t1,1($a2)");
	asm volatile("addu	$v1,$a0,$a1");
	asm volatile("lw	$t5,0($v1)");	//lbu t1,n(v1) n:0,1,2...,19,0,1...
		asm volatile("swl	$t1,0($a2)");
			asm volatile("bnez	$t6,widegmodeloop");

//----------------------------------------------------------------------------40�h�b�g���E

asm volatile("nop");
asm volatile("nop");
		asm volatile("lbu	$a0,1($v0)");	//lbu a0,n(v0) n:0,1,2,3...7,0,1,...
	asm volatile("sb	$t5,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
// 48x27���C�h�e�L�X�g���[�h
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
	asm volatile("sb	$t1,0($a2)");	//�ŏ��̉f���M���o�́B�J���[�o�[�X�g�J�n����640�T�C�N��
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
//������16�o�C�g���E�ƂȂ�悤�ɂ���
widetextmodeloop:
asm volatile("widetextmodeloop:");

	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,1,0,7,6...
		asm volatile("sb	$t5,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t5,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------8�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
// 64x27���C�h�e�L�X�g���[�h(6�h�b�g�t�H���g)
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
	asm volatile("sb	$t1,0($a2)");	//�ŏ��̉f���M���o�́B�J���[�o�[�X�g�J�n����640�T�C�N��
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
//������16�o�C�g���E�ƂȂ�悤�ɂ���
wide6dottextmodeloop:
asm volatile("wide6dottextmodeloop:");

	asm volatile("ext	$t0,$t2,5,1");	//ext t0,t2,n,1 n:7,6,5,4,3,2,7,6...
		asm volatile("sb	$t5,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t5,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
// type Z�݊��O���t�B�b�N���[�h�i256x224, 4bit color�j
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
//������16�o�C�g���E�ƂȂ�悤�ɂ���
asm volatile("nop");
	asm volatile("sb	$t5,0($a2)");	//�ŏ��̉f���M���o�́B�J���[�o�[�X�g�J�n����880�T�C�N��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t5,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//�h�b�g�ŏ��̏o��
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

//nop(); // 16 align �����p
//----------------------------------------------------------------------
// 30x27�W���i�݊��j�e�L�X�g���[�h
//----------------------------------------------------------------------
l_width30textmode:
// 320�N���b�N�E�F�C�g
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
	asm volatile("sb	$t1,0($a2)");	//�ŏ��̉f���M���o�́B�J���[�o�[�X�g�J�n����960�T�C�N��
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

//nop(); // 16 align �����p
//----------------------------------------------------------------------
// 40x27�W���i�݊��j�e�L�X�g���[�h(6dot�t�H���g)
//----------------------------------------------------------------------
l_width40textmode:
// 320�N���b�N�E�F�C�g
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
//������16�o�C�g���E�ƂȂ�悤�ɂ���
width40textmodeloop:
asm volatile("width40textmodeloop:");

	asm volatile("nop");
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
	asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
		asm volatile("sb	$t1,0($a2)");	//dot�ŏ��̏o��
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
//------------------------------------------------------------------6�h�b�g���E
asm volatile("nop");
asm volatile("nop");

	//	LATE=C_BLK;
	asm volatile("addiu	$t1,$zero,%0"::"n"(C_BLK));
	asm volatile("sb	$t1,0($a2)");
	asm volatile("nop");
	asm volatile("j		label3");


asm volatile("label3:");

	mOC1ClearIntFlag();			// OC1���荞�݃t���O�N���A
}
// End skip for MachiKania web */

// �O���t�B�b�N��ʃN���A
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
//�e�L�X�g��ʃN���A
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
	// �J���[�p���b�g�ݒ�i5�r�b�gDA�A�d��3.3V�A1������5�����j
	// n:�p���b�g�ԍ�0-255�Ar,g,b:0-255
	// �P�xY=0.587*G+0.114*B+0.299*R
	// �M��N=Y+0.4921*(B-Y)*sin��+0.8773*(R-Y)*cos��
	// �o�̓f�[�^S=(N*0.71[v]+0.29[v])/3.3[v]*64*1.3
/* Begin insertion for MachiKania web */
	html5func(HTML5FUNC_set_palette);
// End insertion for MachiKania web */
/* Begin skip for MachiKania web

	int y;
	y=(150*g+29*b+77*r+128)/256;

	ClTable[n*32+ 0]=(4582*y+   0*((int)b-y)+4020*((int)r-y)+1872*256+32768)/65536;//��=2��*3*0/20
	ClTable[n*32+ 1]=(4582*y+1824*((int)b-y)+2363*((int)r-y)+1872*256+32768)/65536;//��=2��*3*1/20
	ClTable[n*32+ 2]=(4582*y+2145*((int)b-y)-1242*((int)r-y)+1872*256+32768)/65536;//��=2��*3*2/20
	ClTable[n*32+ 3]=(4582*y+ 697*((int)b-y)-3824*((int)r-y)+1872*256+32768)/65536;//��=2��*3*3/20
	ClTable[n*32+ 4]=(4582*y-1325*((int)b-y)-3252*((int)r-y)+1872*256+32768)/65536;//��=2��*3*4/20
	ClTable[n*32+ 5]=(4582*y-2255*((int)b-y)+   0*((int)r-y)+1872*256+32768)/65536;//��=2��*3*5/20
	ClTable[n*32+ 6]=(4582*y-1326*((int)b-y)+3252*((int)r-y)+1872*256+32768)/65536;//��=2��*3*6/20
	ClTable[n*32+ 7]=(4582*y+ 697*((int)b-y)+3824*((int)r-y)+1872*256+32768)/65536;//��=2��*3*7/20
	ClTable[n*32+ 8]=(4582*y+2145*((int)b-y)+1242*((int)r-y)+1872*256+32768)/65536;//��=2��*3*8/20
	ClTable[n*32+ 9]=(4582*y+1824*((int)b-y)-2363*((int)r-y)+1872*256+32768)/65536;//��=2��*3*9/20
	ClTable[n*32+10]=(4582*y+   0*((int)b-y)-4020*((int)r-y)+1872*256+32768)/65536;//��=2��*3*10/20
	ClTable[n*32+11]=(4582*y-1824*((int)b-y)-2363*((int)r-y)+1872*256+32768)/65536;//��=2��*3*11/20
	ClTable[n*32+12]=(4582*y-2145*((int)b-y)+1242*((int)r-y)+1872*256+32768)/65536;//��=2��*3*12/20
	ClTable[n*32+13]=(4582*y- 697*((int)b-y)+3823*((int)r-y)+1872*256+32768)/65536;//��=2��*3*13/20
	ClTable[n*32+14]=(4582*y+1325*((int)b-y)+3252*((int)r-y)+1872*256+32768)/65536;//��=2��*3*14/20
	ClTable[n*32+15]=(4582*y+2255*((int)b-y)+   0*((int)r-y)+1872*256+32768)/65536;//��=2��*3*15/20
	ClTable[n*32+16]=(4582*y+1326*((int)b-y)-3252*((int)r-y)+1872*256+32768)/65536;//��=2��*3*16/20
	ClTable[n*32+17]=(4582*y- 697*((int)b-y)-3824*((int)r-y)+1872*256+32768)/65536;//��=2��*3*17/20
	ClTable[n*32+18]=(4582*y-2145*((int)b-y)-1242*((int)r-y)+1872*256+32768)/65536;//��=2��*3*18/20
	ClTable[n*32+19]=(4582*y-1824*((int)b-y)+2363*((int)r-y)+1872*256+32768)/65536;//��=2��*3*19/20
// End skip for MachiKania web */
}
void set_bgcolor(unsigned char b,unsigned char r,unsigned char g)
{
	// �o�b�N�O���E���h�J���[�ݒ�
/* Begin insertion for MachiKania web */
	html5func(HTML5FUNC_set_bgcolor);
// End insertion for MachiKania web */
/* Begin skip for MachiKania web

	int y;
	y=(150*g+29*b+77*r+128)/256;

	BGClTable[ 0]=(4582*y+   0*((int)b-y)+4020*((int)r-y)+1872*256+32768)/65536;//��=2��*3*0/20
	BGClTable[ 1]=(4582*y+1824*((int)b-y)+2363*((int)r-y)+1872*256+32768)/65536;//��=2��*3*1/20
	BGClTable[ 2]=(4582*y+2145*((int)b-y)-1242*((int)r-y)+1872*256+32768)/65536;//��=2��*3*2/20
	BGClTable[ 3]=(4582*y+ 697*((int)b-y)-3824*((int)r-y)+1872*256+32768)/65536;//��=2��*3*3/20
	BGClTable[ 4]=(4582*y-1325*((int)b-y)-3252*((int)r-y)+1872*256+32768)/65536;//��=2��*3*4/20
	BGClTable[ 5]=(4582*y-2255*((int)b-y)+   0*((int)r-y)+1872*256+32768)/65536;//��=2��*3*5/20
	BGClTable[ 6]=(4582*y-1326*((int)b-y)+3252*((int)r-y)+1872*256+32768)/65536;//��=2��*3*6/20
	BGClTable[ 7]=(4582*y+ 697*((int)b-y)+3824*((int)r-y)+1872*256+32768)/65536;//��=2��*3*7/20
	BGClTable[ 8]=(4582*y+2145*((int)b-y)+1242*((int)r-y)+1872*256+32768)/65536;//��=2��*3*8/20
	BGClTable[ 9]=(4582*y+1824*((int)b-y)-2363*((int)r-y)+1872*256+32768)/65536;//��=2��*3*9/20
	BGClTable[10]=(4582*y+   0*((int)b-y)-4020*((int)r-y)+1872*256+32768)/65536;//��=2��*3*10/20
	BGClTable[11]=(4582*y-1824*((int)b-y)-2363*((int)r-y)+1872*256+32768)/65536;//��=2��*3*11/20
	BGClTable[12]=(4582*y-2145*((int)b-y)+1242*((int)r-y)+1872*256+32768)/65536;//��=2��*3*12/20
	BGClTable[13]=(4582*y- 697*((int)b-y)+3823*((int)r-y)+1872*256+32768)/65536;//��=2��*3*13/20
	BGClTable[14]=(4582*y+1325*((int)b-y)+3252*((int)r-y)+1872*256+32768)/65536;//��=2��*3*14/20
	BGClTable[15]=(4582*y+2255*((int)b-y)+   0*((int)r-y)+1872*256+32768)/65536;//��=2��*3*15/20
	BGClTable[16]=(4582*y+1326*((int)b-y)-3252*((int)r-y)+1872*256+32768)/65536;//��=2��*3*16/20
	BGClTable[17]=(4582*y- 697*((int)b-y)-3824*((int)r-y)+1872*256+32768)/65536;//��=2��*3*17/20
	BGClTable[18]=(4582*y-2145*((int)b-y)-1242*((int)r-y)+1872*256+32768)/65536;//��=2��*3*18/20
	BGClTable[19]=(4582*y-1824*((int)b-y)+2363*((int)r-y)+1872*256+32768)/65536;//��=2��*3*19/20
// End skip for MachiKania web */
}

void init_palette(void){
	//�J���[�p���b�g������
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
	// �ϐ������ݒ�
//	LineCount=0;				// ���������C���J�E���^�[
	drawing=0;
	TVRAMp=TVRAM;
	fontp=Fontp;
	GVRAMp=GVRAM;

	videostopflag=0;			// �r�f�I�o�͒�~����
//	PR2 = H_NTSC -1; 			// ��63.5usec�ɐݒ�
//	T2CONSET=0x8000;			// �^�C�}2�X�^�[�g
}
void stop_composite(void)
{
//	T2CONCLR = 0x8000;			// �^�C�}2��~
	videostopflag=1;			// �r�f�I�o�͒�~
}

// �J���[�R���|�W�b�g�o�͏�����
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

	//�J���[�p���b�g������
	init_palette();
	set_bgcolor(0,0,0); //�o�b�N�O�����h�J���[�͍�
	setcursorcolor(7);

	//�t�H���g�������@FontData[]����fontdata[]�ɃR�s�[
	fontROMp=(unsigned int *)FontData;
	fontRAMp=(unsigned int *)fontdata;
	for(i=0;i<256*8/4;i++) *fontRAMp++=*fontROMp++;
	Fontp=fontdata;

	// �^�C�}2�̏����ݒ�,�����N���b�N��63.5usec�����A1:1
	T2CON = 0x0000;				// �^�C�}2��~���
	mT2SetIntPriority(5);			// ���荞�݃��x��5
	mT2ClearIntFlag();
	mT2IntEnable(1);			// �^�C�}2���荞�ݗL����

	// OC1�̊��荞�ݗL����
	mOC1SetIntPriority(5);			// ���荞�݃��x��5
	mOC1ClearIntFlag();
	mOC1IntEnable(1);			// OC1���荞�ݗL����

	// OC2�̊��荞�ݗL����
	mOC2SetIntPriority(5);			// ���荞�݃��x��5
	mOC2ClearIntFlag();
	mOC2IntEnable(1);			// OC2���荞�ݗL����

	// OC5�̊��荞�ݗL����
	mOC5SetIntPriority(5);			// ���荞�݃��x��5
	mOC5ClearIntFlag();
	mOC5IntEnable(1);			// OC5���荞�ݗL����

	OSCCONCLR=0x10; // WAIT���߂̓A�C�h�����[�h

	// Data Memory SRAM wait states: Default Setting = 1; set it to 0
	BMXCONbits.BMXWSDRM = 0; // SRAM�̃E�F�C�g�X�e�[�g��0�ɂ���

	// Flash PM Wait States: MX Flash runs at 3 wait states @ 100 MHz
	CHECONbits.PFMWS = 2; // �t���b�V���̃E�F�C�g�X�e�[�g��2�ɂ���i100MHz���쎞�j

	// Prefetch-cache: Enable prefetch for cacheable PFM instructions
	CHECONbits.PREFEN = 1; //�v���t�F�b�`�L����

	__builtin_mtc0(16, 0, (__builtin_mfc0(16, 0) & 0xfffffff8 | 3)); // �L���b�V���L����

	// Set the interrupt controller for multi-vector mode
	INTCONSET = _INTCON_MVEC_MASK; //���荞�݂��}���`�x�N�^���[�h�ɐݒ�

	// Set the CP0 Status IE bit to turn on interrupts globally
	__builtin_enable_interrupts(); //���荞�ݗL����

	LineCount=0;				// ���������C���J�E���^�[
	PR2 = H_NTSC -1; 			// ��63.5usec�ɐݒ�
	T2CONSET=0x8000;			// �^�C�}2�X�^�[�g
	start_composite();
}

//�r�f�I���[�h�̐؂�ւ�
void set_videomode(unsigned char m, unsigned char *gvram){
// m:�r�f�I���[�h
// gvram:�O���t�B�b�N�p�������擪�A�h���X

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
		//6�h�b�g�t�H���g�ɐ؂�ւ�
		fontROMp=(unsigned int *)FontData2;
		fontRAMp=(unsigned int *)fontdata;
		for(i=0;i<256*8/4;i++) *fontRAMp++=*fontROMp++;
	}
/* Begin modification for MachiKania web */
//	else if((videomode==VMODE_T40 || videomode==VMODE_WIDETEXT6dot) && (m!=VMODE_T40 && m!=VMODE_WIDETEXT6dot)){
	else if((videomode==VMODE_T40 || videomode==VMODE_WIDETEXT6dot || videomode==VMODE_MONOTEXT) 
			&& (m!=VMODE_T40 && m!=VMODE_WIDETEXT6dot && m!=VMODE_MONOTEXT)){
// End insertion for MachiKania web */
		//8�h�b�g�t�H���g�ɐ؂�ւ�
		fontROMp=(unsigned int *)FontData;
		fontRAMp=(unsigned int *)fontdata;
		for(i=0;i<256*8/4;i++) *fontRAMp++=*fontROMp++;
	}
	switch(m){
		case VMODE_T30: // �W���e�L�X�g30�����݊����[�h
			if(textmode!=TMODE_T30){
				textmode=TMODE_T30;
				twidth=WIDTH_30;
				attroffset=ATTROFFSET30;
				clearscreen();
			}
			break;
		case VMODE_STDTEXT: // �W���e�L�X�g36�������[�h
			if(textmode!=TMODE_STDTEXT){
				textmode=TMODE_STDTEXT;
				twidth=WIDTH_X;
				attroffset=ATTROFFSET;
				clearscreen();
			}
			break;
		case VMODE_T40: // �W���e�L�X�g40�����݊����[�h�i6�h�b�g�t�H���g�j
			if(textmode!=TMODE_T40){
				textmode=TMODE_T40;
				twidth=WIDTH_40;
				attroffset=ATTROFFSET40;
				clearscreen();
			}
			break;
		case VMODE_WIDETEXT: // ���C�h�e�L�X�g48�������[�h
			if(textmode!=TMODE_WIDETEXT){
				textmode=TMODE_WIDETEXT;
				twidth=WIDTH_XW;
				attroffset=ATTROFFSETW;
				clearscreen();
			}
			break;
		case VMODE_WIDETEXT6dot: // ���C�h�e�L�X�g64�������[�h�i6�h�b�g�t�H���g�j
			if(textmode!=TMODE_WIDETEXT6dot){
				textmode=TMODE_WIDETEXT6dot;
				twidth=WIDTH_XW2;
				attroffset=ATTROFFSETW2;
				clearscreen();
			}
			break;
		case VMODE_MONOTEXT: // ���m�N���e�L�X�g80�������[�h
			if(textmode!=TMODE_MONOTEXT){
				textmode=TMODE_MONOTEXT;
				twidth=WIDTH_XBW;
				attroffset=ATTROFFSETBW;
				clearscreen();
			}
			break;
		case VMODE_ZOEAGRPH: // type Z�݊��O���t�B�b�N���[�h
			graphmode=GMODE_ZOEAGRPH;
			gwidth=X_RESZ;
			gwidthy=Y_RESZ;
			break;
		case VMODE_STDGRPH: // �W���O���t�B�b�N�{�e�L�X�g36�������[�h
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
		case VMODE_WIDEGRPH: // ���C�h�O���t�B�b�N�{�e�L�X�g48�������[�h
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
		// �O���t�B�b�N�g�p���[�h
		GVRAM=gvram;
		g_clearscreen();
	}
	else{
		// �O���t�B�b�N�s�g�p���[�h
		graphmode=0;
	}
	start_composite();
}

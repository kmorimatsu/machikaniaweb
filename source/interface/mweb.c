#include <xc.h>
#include "../api.h"
#include "./mweb.h"

/*
	TODO:
		PCG�̍ۂ̕\���ύX
			PCG�ŃL�����N�^�[��\���������̂���PCG�X�e�[�g�����g�ŃL�����N�^�[��ύX���Ă��A�\���X�V����Ȃ��B������C������K�v�A���B
		�^�C�}�[�Q�̊��荞�ݏ���
			TMR=0�Ƃ��A���荞�ݗp�̊֐����Ăяo���B�\���́AOC2, OC5�̊��荞�݂Ő�����邪�A������͖�������B
		�\�t�g�E�F�A���荞�ݏ���
			���y�Đ��Ȃ�
		

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
	(unsigned int)(&Fontp),
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

volatile unsigned char ps2keystatus[256]; // ���z�R�[�h�ɑ�������L�[�̏�ԁiOn�̎�1�j
volatile unsigned short vkey; //���z�L�[�R�[�h
unsigned char lockkey; // ����������Lock�L�[�̏�Ԏw��B����3�r�b�g��<CAPSLK><NUMLK><SCRLK>
unsigned char keytype; // �L�[�{�[�h�̎�ށB0�F���{��109�L�[�A1�F�p��104�L�[

#ifndef __DEBUG
	int ps2init(){ return 0; } // PS/2���C�u�����֘A�������B����I��0�A�G���[��-1��Ԃ�
#endif
unsigned char shiftkeys(){ html5func(HTML5FUNC_shiftkeys); } // SHIFT�֘A�L�[�̉�����Ԃ�Ԃ�
unsigned char ps2readkey(){ html5func(HTML5FUNC_ps2readkey); }
// ���͂��ꂽ1�̃L�[�̃L�[�R�[�h���O���[�o���ϐ�vkey�Ɋi�[�i������Ă��Ȃ����0��Ԃ��j
// ����8�r�b�g�F�L�[�R�[�h
// ���8�r�b�g�F�V�t�g��ԁi�����F1�j�A��ʂ���<0><CAPSLK><NUMLK><SCRLK><Win><ALT><CTRL><SHIFT>
// �p���E�L�������̏ꍇ�A�߂�l�Ƃ���ASCII�R�[�h�i����ȊO��0��Ԃ��j

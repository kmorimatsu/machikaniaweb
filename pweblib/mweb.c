#include <xc.h>
#include "./mweb.h"
#include "../TGvideo370f.h"

/*
	TGvideo370f.c functions:
		void __ISR(8, ipl5) T2Handler(void);
		void __ISR(14, ipl5) OC3Handler(void);
		void __ISR(6, ipl5) OC1Handler(void);
		void clearscreen(void);
		void clearTVRAM(void);
		void set_palette(unsigned char n,unsigned char b,unsigned char r,unsigned char g);
		void start_composite(void);
		void stop_composite(void);
		void init_composite(void);
	Only set_palette() is used for HTML5 function 

*/

int html5data[4];
void html5returnValue();
char* pFontp=&PCG[0];

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
	(unsigned int)(&VRAM[0]),//(&FontData2[0]),
	(unsigned int)(&pFontp),
	(unsigned int)(&html5data[0]),
	(unsigned int)0,//(&ps2keystatus[0]),
	(unsigned int)0,//(&vkey),
	(unsigned int)0,//(&gFileArray[0]),
	(unsigned int)0,//(&gFileArray[1]),
	(unsigned int)0,//(&g_var_mem[ALLOC_WAVE_BLOCK]),
	(unsigned int)0,//(&g_music_active),
};

const unsigned int _html5_func[] __attribute__((address(0x9D006080))) ={
	0x00600008, // jr          v1
	0x00000000, // nop
};


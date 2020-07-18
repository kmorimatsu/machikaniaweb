/*
   This file is provided under the LGPL license ver 2.1.
   Written by Katsumi.
   http://hp.vector.co.jp/authors/VA016157/
   kmorimatsu@users.sourceforge.jp
*/

/*
	This file is shared by Megalopa and Zoea
*/

#include "compiler.h"

/*
Operators: (upper ones have higher priority
()
* / %
+ -
<< >>
< <= > >=
= !=
XOR
AND
OR
*/


const unsigned char g_priority[]={
	0, // OP_VOID
	1, // OP_OR
	2, // OP_AND
	3, // OP_XOR
	4,4, // OP_EQ, OP_NEQ
	5,5,5,5, // OP_LT, OP_LTE, OP_MT, OP_MTE
	6,6, // OP_SHL, OP_SHR
	7,7, // OP_ADD, OP_SUB
	8,8,8 // OP_MUL, OP_DIV, OP_REM
};

enum operator g_last_op;

char* get_operator(void){
	char b1,b2,b3;
	next_position();
	b1=g_source[g_srcpos];
	b2=g_source[g_srcpos+1];
	b3=g_source[g_srcpos+2];
	switch(b1){
		case '%': g_last_op=OP_REM; break;
		case '/': g_last_op=OP_DIV; break;
		case '*': g_last_op=OP_MUL; break;
		case '-': g_last_op=OP_SUB; break;
		case '+': g_last_op=OP_ADD; break;
		case '>':
			if (b2=='>') {
				g_srcpos++;
				g_last_op=OP_SHR;
			} else if (b2=='=') {
				g_srcpos++;
				g_last_op=OP_MTE;
			} else {
				g_last_op=OP_MT;
			}
			break;
		case '<':
			if (b2=='<') {
				g_srcpos++;
				g_last_op=OP_SHL;
			} else if (b2=='=') {
				g_srcpos++;
				g_last_op=OP_LTE;
			} else {
				g_last_op=OP_LT;
			}
			break;
		case '!':
			if (b2!='=') return ERR_SYNTAX;
			g_srcpos++;
			g_last_op=OP_NEQ;
			break;
		case '=':
			if (b2=='=') g_srcpos++;
			g_last_op=OP_EQ;
			break;
		case 'X':
			if (b2!='O') return ERR_SYNTAX;
			if (b3!='R') return ERR_SYNTAX;
			g_srcpos++;
			g_srcpos++;
			g_last_op=OP_XOR;
			break;
		case 'O':
			if (b2!='R') return ERR_SYNTAX;
			g_srcpos++;
			g_last_op=OP_OR;
			break;
		case 'A':
			if (b2!='N') return ERR_SYNTAX;
			if (b3!='D') return ERR_SYNTAX;
			g_srcpos++;
			g_srcpos++;
			g_last_op=OP_AND;
			break;
		default:
			return ERR_SYNTAX;
	}
	g_srcpos++;
	return 0;
}

char* get_floatOperator(void){
	char* err;
	int spos;
	next_position();
	spos=g_srcpos;
	err=get_operator();
	if (err) return err;
	switch(g_last_op){
		// Following operators cannot be used for float values.
		case OP_XOR:
		case OP_REM:
		case OP_SHR:
		case OP_SHL:
			g_srcpos=spos;
			return ERR_SYNTAX;
		default:
			return 0;
	}
}

char* calculation(enum operator op){
	// $v0 = $v1 <op> $v0;
	switch(op){
		case OP_OR:
			check_obj_space(1);
			g_object[g_objpos++]=0x00621025; // or          v0,v1,v0
			break;
		case OP_AND:
			check_obj_space(1);
			g_object[g_objpos++]=0x00621024; // and         v0,v1,v0
			break;
		case OP_XOR:
			check_obj_space(1);
			g_object[g_objpos++]=0x00621026; // xor         v0,v1,v0
			break;
		case OP_EQ:
			check_obj_space(2);
			g_object[g_objpos++]=0x00621026; // xor         v0,v1,v0
			g_object[g_objpos++]=0x2C420001; // sltiu       v0,v0,1
			break;
		case OP_NEQ:
			check_obj_space(2);
			g_object[g_objpos++]=0x00621026; // xor         v0,v1,v0
			g_object[g_objpos++]=0x0002102B; // sltu        v0,zero,v0
			break;
		case OP_LT:
			check_obj_space(1);
			g_object[g_objpos++]=0x0062102A; // slt         v0,v1,v0
			break;
		case OP_LTE:
			check_obj_space(2);
			g_object[g_objpos++]=0x0043102A; // slt         v0,v0,v1
			g_object[g_objpos++]=0x38420001; // xori        v0,v0,0x1
			break;
		case OP_MT:
			check_obj_space(1);
			g_object[g_objpos++]=0x0043102A; // slt         v0,v0,v1
			break;
		case OP_MTE:
			check_obj_space(2);
			g_object[g_objpos++]=0x0062102A; // slt         v0,v1,v0
			g_object[g_objpos++]=0x38420001; // xori        v0,v0,0x1
			break;
		case OP_SHR:
			check_obj_space(1);
			g_object[g_objpos++]=0x00431006; // srlv        v0,v1,v0
			break;
		case OP_SHL:
			check_obj_space(1);
			g_object[g_objpos++]=0x00431004; // sllv        v0,v1,v0
			break;
		case OP_ADD:
			check_obj_space(1);
			g_object[g_objpos++]=0x00621021; // addu        v0,v1,v0
			break;
		case OP_SUB:
			check_obj_space(1);
			g_object[g_objpos++]=0x00621023; // subu        v0,v1,v0
			break;
		case OP_MUL:
			check_obj_space(1);
			g_object[g_objpos++]=0x70621002; // mul         v0,v1,v0
			break;
		case OP_DIV:
			// Note that intterupt functions do not use mflo and mfhi.
			// Probably using div does not cause delay of interrupt.
			check_obj_space(5);
			g_object[g_objpos++]=0x14400003;                                        // bne         v0,zero,label
			g_object[g_objpos++]=0x0062001A;                                        // div         v1,v0
			call_lib_code(LIB_DIV0); // 2 words
 			                                                                        // label:
			g_object[g_objpos++]=0x00001012;                                        // mflo        v0
			break;
		case OP_REM:
			check_obj_space(5);
			g_object[g_objpos++]=0x14400003;                                        // bne         v0,zero,label
			g_object[g_objpos++]=0x0062001A;                                        // div         v1,v0
			call_lib_code(LIB_DIV0); // 2 words
 			                                                                        // label:
			g_object[g_objpos++]=0x00001010;                                        // mfhi        v0
			break;
		default:
			return ERR_SYNTAX;
	}
	return 0;
}

#ifdef MIPS_ENABLE_FP

char* calculation_float(enum operator op){
	// $v0 = $a0 <op> $v0;
	// All the calculations will be here.

	// First, move values to FPR
	/*
		MTC1 Move Word to Floating Point: MTC1 rt,fs
		 COP1   MF    rt    fs       0
		010001 00100 00010 00001 00000000000
	*/
	check_obj_space(2);
	g_object[g_objpos++]=0x44820800; // MTC1 v0,f1
	g_object[g_objpos++]=0x44840000; // MTC1 a0,f0

	// Main calculation routines follow
	switch(op){
		case OP_EQ:
			/*
				C.EQ.S f0,f1
				 COP1   fmt   ft     fs   cc 0 A FC cond
				010001 10000 00001 00000 000 0 0 11 0010
				BC1TL
				  COP1    BC   cc nd tf     offset
				 010001 01000 000 1  1 0000000000000001
			*/
			check_obj_space(4);
			g_object[g_objpos++]=0x46010032; // C.EQ.S f0,f1
			g_object[g_objpos++]=0x34020000; // ori v0,zero,0000
			g_object[g_objpos++]=0x45030001; // BC1TL label
			g_object[g_objpos++]=0x3C023f80; // lui   v0,3f80
			                                 // label:
			return 0;
		case OP_NEQ:
			/*
				C.EQ.S f0,f1
				 COP1   fmt   ft     fs   cc 0 A FC cond
				010001 10000 00000 00001 000 0 0 11 0010
				BC1FL
				  COP1    BC   cc nd tf     offset
				 010001 01000 000 1  0 0000000000000001
			*/
			check_obj_space(4);
			g_object[g_objpos++]=0x46010032; // C.EQ.S f0,f1
			g_object[g_objpos++]=0x34020000; // ori v0,zero,0000
			g_object[g_objpos++]=0x45020001; // BC1FL label
			g_object[g_objpos++]=0x3C023f80; // lui   v0,3f80
			                                 // label:
			return 0;
		case OP_LT:
			/*
				C.LT.S
				BC1TL
			*/
			check_obj_space(4);
			g_object[g_objpos++]=0x4601003C; // C.LT.S f0,f1
			g_object[g_objpos++]=0x34020000; // ori v0,zero,0000
			g_object[g_objpos++]=0x45030001; // BC1TL label
			g_object[g_objpos++]=0x3C023f80; // lui   v0,3f80
			                                 // label:
			return 0;
		case OP_LTE:
			/*
				C.LE.S
				BC1TL
			*/
			check_obj_space(4);
			g_object[g_objpos++]=0x4601003E; // C.LE.S f0,f1
			g_object[g_objpos++]=0x34020000; // ori v0,zero,0000
			g_object[g_objpos++]=0x45030001; // BC1TL label
			g_object[g_objpos++]=0x3C023f80; // lui   v0,3f80
			                                 // label:
			return 0;
		case OP_MT:
			/*
				C.LE.S
				BC1FL
			*/
			check_obj_space(4);
			g_object[g_objpos++]=0x4601003E; // C.LE.S f0,f1
			g_object[g_objpos++]=0x34020000; // ori v0,zero,0000
			g_object[g_objpos++]=0x45020001; // BC1FL label
			g_object[g_objpos++]=0x3C023f80; // lui   v0,3f80
			                                 // label:
			return 0;
		case OP_MTE:
			/*
				C.LT.S
				BC1FL
			*/
			check_obj_space(4);
			g_object[g_objpos++]=0x4601003C; // C.LT.S f0,f1
			g_object[g_objpos++]=0x34020000; // ori v0,zero,0000
			g_object[g_objpos++]=0x45020001; // BC1FL label
			g_object[g_objpos++]=0x3C023f80; // lui   v0,3f80
			                                 // label:
			return 0;
		case OP_ADD:
			/*
				ADD.S fd,fs,ft, where fd=$f0, fs=$f0, ft=$f1
				
				 COP1   fmt   ft    fs    fd    ADD
				010001 10000 00001 00000 00000 000000
				
				0x46010000
			*/
			check_obj_space(1);
			g_object[g_objpos++]=0x46010000;    // add.s       f0,f0,f1
			break;
		case OP_SUB:
			check_obj_space(1);
			g_object[g_objpos++]=0x46010001;    // sub.s       f0,f0,f1
			break;
		case OP_MUL:
			check_obj_space(1);
			g_object[g_objpos++]=0x46010002;    // mul.s       f0,f0,f1
			break;
		case OP_DIV:
			check_obj_space(4);
			g_object[g_objpos++]=0x14400003;    // bne         v0,zero,label
			g_object[g_objpos++]=0x46010003;    // div.s       f0,f0,f1
			call_lib_code(LIB_DIV0); // 2 words
 			                                    // label:
			break;
		case OP_OR:
			g_objpos--;
			g_objpos--;
			g_object[g_objpos++]=0x00821025; // or          v0,a0,v0
			break;
		case OP_AND:
			g_objpos--;
			g_objpos--;
			g_object[g_objpos++]=0x00821024; // and         v0,a0,v0
			break;
		default:
			return ERR_SYNTAX;
	}
	// In the last, move value from FPR
	/*
		MFC1 Move Word From Floating Point: MFC1 rt,fs
		 COP1   MF    rt    fs       0
		010001 00000 00010 00000 00000000000
	*/
	check_obj_space(1);
	g_object[g_objpos++]=0x44020000; // MFC1 $v0,$f0
	return 0;
}

#else // MIPS_ENABLE_FP

char* calculation_float(enum operator op){
	// $v0 = $a0 <op> $v0;
	// All the calculations will be done in library code, lib_float function (see below).
	call_lib_code(LIB_FLOAT | op);
	return 0;
}

#endif // MIPS_ENABLE_FP

int lib_float(int ia0,int iv0, enum operator a1){
	// This function was called from _call_library().
	// Variable types must be all int.
	// Casting cannot be used.
	// Instead, by using pointer, put as int value, get as float value, 
	// calculate, put as float value, then get as int value for returning.
	volatile float a0,v0;
	((int*)(&a0))[0]=ia0;
	((int*)(&v0))[0]=iv0;
	switch(a1){
		case OP_EQ:
			v0= a0==v0?1:0;
			break;
		case OP_NEQ:
			v0= a0!=v0?1:0;
			break;
		case OP_LT:
			v0= a0<v0?1:0;
			break;
		case OP_LTE:
			v0= a0<=v0?1:0;
			break;
		case OP_MT:
			v0= a0>v0?1:0;
			break;
		case OP_MTE:
			v0= a0>=v0?1:0;
			break;
		case OP_ADD:
			v0= a0+v0;
			break;
		case OP_SUB:
			v0= a0-v0;
			break;
		case OP_MUL:
			v0= a0*v0;
			break;
		case OP_DIV:
			if (v0==0) err_div_zero();
			v0= a0/v0;
			break;
		case OP_OR:
			v0= a0||v0?1:0;
			break;
		case OP_AND:
			v0= a0&&v0?1:0;
			break;
		default:
			err_unknown();
			return 0;
	}
	return ((int*)(&v0))[0];
}; 
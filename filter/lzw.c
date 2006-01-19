/************************************************************************/
/*									*/
/*  Project:	DrawView - Decompression filter				*/
/*  SCCS:	<%Z% %M% %I%>					*/
/*  Edit:	17-Jan-06						*/
/*									*/
/************************************************************************/
/*									*/
/*  LZW decompression for 4Mation compressed Draw file format.		*/
/*									*/
/*  The compression format is intended to be TIFF-compatible LZW.	*/
/*  Unfortunately I can't conveniently find any suitable decoder	*/
/*  code; the standard Unix compress(1) utility doesn't appear to be	*/
/*  compatible even allowing for the absence of a header.  So this	*/
/*  code is a straight translation from Paul's original ARM version	*/
/*  back into C.							*/
/*									*/
/*  It could probably be cleaned up and optimised, of course.  But	*/
/*  its performance is acceptable at the moment (it normally operates	*/
/*  on fairly small, by today's standards, blocks of data).  It is	*/
/*  left in this state as a demonstration that it is indeed possible	*/
/*  to write a machine code program in C :-)				*/
/*									*/
/************************************************************************/
/*									*/
/*  Copyright (c) 2004 Jonathan Marten <jjm@keelhaul.demon.co.uk>	*/
/*  Home & download:  http://www.keelhaul.demon.co.uk/acorn/drawview/	*/
/*									*/
/*  This program is free software; you can redistribute it and/or	*/
/*  modify it under the terms of the GNU General Public License as	*/
/*  published by the Free Software Foundation; either version 2 of the	*/
/*  License, or (at your option) any later version.			*/
/*									*/
/*  It is distributed in the hope that it will be useful, but		*/
/*  WITHOUT ANY WARRANTY; without even the implied warranty of		*/
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	*/
/*  GNU General Public License for more details.			*/
/*									*/
/*  You should have received a copy of the GNU General Public		*/
/*  License along with this program; see the file COPYING for further	*/
/*  details.  If not, write to the Free Software Foundation, Inc.,	*/
/*  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.		*/
/*									*/
/************************************************************************/

#undef DEBUG_DECOMP					/* Generate debug/progress messages */

/************************************************************************/
/*  Include files							*/
/************************************************************************/

#ifdef DEBUG_DECOMP
#include <stdio.h>					/* Standard I/O routines */
#endif
#include "draw/drawLZW.h"				/* TIFF-compatible compression */

/************************************************************************/
/*  Constants and macros						*/
/************************************************************************/

/*	SET_BITS	*	8		; the 256 predefined codes  */
#define SET_BITS	8
/*	MAX_BITS	*	12		; 4096 codes at most  */
#define MAX_BITS	12
/*	CLEAR_CODE	*	(1<<SET_BITS)	; the code to clear the code table  */
#define CLEAR_CODE	(1<<SET_BITS)
/*	MAX_CODE	*	(1<<MAX_BITS)	; highest allowed code  */
#define MAX_CODE	(1<<MAX_BITS)

/************************************************************************/
/*  Internal variables							*/
/************************************************************************/

/*		AREA	|lzw$$data|,DATA	*/
/*		EXPORT	lzw_codetable		*/
/*	lzw_codetable				*/
/*		%	4<<MAX_BITS		; 4096 words, low  2 bytes = suffix char  */
/*						;             high 2 bytes = prefix code  */
static unsigned int lzw_codetable[4<<MAX_BITS];

static unsigned int r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr;

/************************************************************************/
/*  lzw_init_bit_buffer -- Initialises input bit-buffer with the bytes	*/
/*  up to the first word boundary.  This so that the 'get_code'		*/
/*  routine can be faster by reading in a word at a time.		*/
/************************************************************************/

/*	lzw_init_bit_buffer			*/
static void lzw_init_bit_buffer()
{
/*		AND	r9, r2,#3		; r9 = from MOD 4  */
	r9 = r2 & 3;
/*		MOV	r9, r9,LSL #3		; convert to bits  */
	r9 <<= 3;
/*		LDR	r10, [r2]		; read a (possibly-non-aligned) word from input	*/
/*		MOV	r10, r10,LSL r9		; clear unwanted bits by the old...  */
/*		MOV	r10, r10,LSR r9		; ...shift up and down technique  */
/*		EOR	r12, r10,r10,ROR #16	; reverse the bytes in r10  */
/*		BIC	r12, r12,#&FF0000	; using the RWST method  */
/*		MOV	r10, r10,ROR #8		; (Roger Wilson Sneaky Technique)  */
/*		EOR	r10, r10,r12,LSR #8	*/
	r10 = 0;
	do
	{
		r10 = (r10<<8)+(*((unsigned char *) r2));
		++r2;
	}
	while ((r2 & 3)!=0);
	r10 <<= r9;

/*		RSB	r9, r9,#32		; convert from 'excess bits' to 'bits read'  */
	r9 = 32-r9;
/*		ADD	r2, r2,#4		; align input ptr to next word boundary  */
/*		BIC	r2, r2,#3		*/
/*		MOVS	pc, lr			*/
	return;
}

/************************************************************************/
/*  lzw_clear_table -- Clears the code table and initialises related	*/
/*  variables.								*/
/************************************************************************/

/*	lzw_clear_table				*/
static void lzw_clear_table()
{
/*						; initialise the 'preset' codes	*/
/*		;LDR	r7, addr_of_codetable  */
/*		MOV	r8, #0			; counter  */
	r8 = 0;
/*		MOV	r6, #&100		; value to stick in table ( = &100 (length) + counter)  */
	r6 = 0x100;
/*	lzwc_clear_loop1			; initialise the preset codes  */
lzwc_clear_loop1:
/*		STR	r6, [r7, r8,LSL #2]	*/
	*((int *) (r7+(r8<<2))) = r6;
/*		ADD	r8, r8,#1		*/
	++r8;
/*		ADD	r6, r6,#1		*/
	++r6;
/*		CMP	r8, #CLEAR_CODE		*/
	if (r8<CLEAR_CODE)
/*		BLT	lzwc_clear_loop1	*/
	goto lzwc_clear_loop1;
/*						; the rest of the entries are set to zero	*/
/*		MOV	r6, #0			*/
	r6 = 0;
/*	lzwc_clear_loop2			*/
lzwc_clear_loop2:
/*		STR	r6, [r7, r8,LSL #2]	*/
	*((int *) (r7+(r8<<2))) = r6;
/*		ADD	r8, r8,#1		*/
	++r8;
/*		CMP	r8, #MAX_CODE		*/
	if (r8<MAX_CODE)
/*		BLT	lzwc_clear_loop2	*/
	goto lzwc_clear_loop2;

/*						; set up the vars that get reset at a clear-table  */
/*		MOV	r8, #SET_BITS		*/
	r8 = SET_BITS;
/*		ADD	r8, r8,#1		; 'code_bits'  */
	++r8;
/*		MOV	r6, #CLEAR_CODE		*/
	r6 = CLEAR_CODE;
/*		ADD	r6, r6,#2		; 'next_code'  */
	r6 += 2;
/*		MOVS	pc, lr			*/
}

/************************************************************************/
/*  lzw_get_code2 -- Gets a code into r12 from the bit buffer (filling	*/
/*  it first from the input as required).				*/
/************************************************************************/

/*	lzw_get_code2				*/
static void lzw_get_code2()
{
	unsigned int b0,b1,b2,b3;

/*		CMP	r8, r9			; test if we have enough bits in buffer  */
	if (r8>r9)
/*		BGT	lzwd_read_more2 	*/
	goto lzwd_read_more2;
/*						; enough bits in buffer - so get some out  */
/*		RSB	r12, r8,#32		*/
	r12 = 32-r8;
/*		MOV	r12, r10,LSR r12	; r12 = code to return  */
	r12 = r10>>r12;
/*		MOV	r10, r10,LSL r8		; move the rest of the bits up  */
	r10 <<= r8;
/*		SUB	r9, r9,r8		; update bit pointer  */
	r9 -= r8;
/*		MOVS	pc, lr			*/
	return;

/*	lzwd_read_more2				*/
lzwd_read_more2:
/*		LDR	r1, [r2],#4		; read a (aligned) word from input  */
	r1 = *((unsigned int *) r2); r2 += 4;
/*		EOR	r12, r1,r1,ROR #16	; reverse the bytes in r10  */
/*		BIC	r12, r12,#&FF0000	; using the RWST method  */
/*		MOV	r1, r1,ROR #8		; (Roger Wilson Sneaky Technique)  */
/*		EOR	r1, r1,r12,LSR #8	*/
	b0 = (r1 & 0x000000FF);
	b1 = (r1 & 0x0000FF00);
	b2 = (r1 & 0x00FF0000);
	b3 = (r1 & 0xFF000000);
	r1 = (b0<<24)+(b1<<8)+(b2>>8)+(b3>>24);

/*		ORR	r12, r10,r1,LSR r9	; shift read bits down to current bit pointer  */
	r12 = r10|(r1>>r9);
/*		SUB	r9, r8,r9		; no. of bits used from newly-read word  */
	r9 = r8-r9;
/*		MOV	r10, r1,LSL r9		; move unread bits up to start of bit buffer  */
	r10 = r1<<r9;
/*		RSB	r1, r8,#32		*/
	r1 = 32-r8;
/*		MOV	r12, r12,LSR r1		; r12 = code to return  */
	r12 >>= r1;
/*		RSB	r9, r9,#32		; update bit pointer  */
	r9 = 32-r9;
/*		MOVS	pc, lr			; return  */
	return;
}

/************************************************************************/
/*  lzw_decompress -- Main routine.					*/
/* 									*/
/*  C form: new_to = lzw_decompress((void *)to,max_out,(void *)from);	*/
/* 									*/
/*  ;  in: r0 (a1) - pointer to output buffer	(to)			*/
/*  ;		r1 (a2) - size of output buffer (0 = dunno, don't care)	*/
/*  ;		r2 (a3) - pointer to data to compress (from_size)	*/
/* 									*/
/*  ; out: r0 (a1) - updated 'to' pointer				*/
/*  ;		Others preserved.					*/
/* 									*/
/*  ; If r0 = 0 on entry, then no compressed data will be written.	*/
/*  ; However, the pointer WILL be updated, thus giving the size of	*/
/*  ; the compressed data.  (Make sure r1 = 0 though)			*/
/* 									*/
/*  ; routine register allocation:					*/
/* 									*/
/*  ; r0  = 'to' pointer						*/
/*  ; r1  = 'to' size limit						*/
/*  ; r2  = 'from' pointer						*/
/*  ; r4  = 'old code'							*/
/*  ; r5  = 'first char of string old-code'				*/
/*  ; r6  = next_code (next code that will be allocated)		*/
/*  ; r7  = pointer to codetable					*/
/*  ; r8  = code_bits (current number of bits used for code)		*/
/*  ; r9  = cur_bit (bit pointer into buffer word)			*/
/*  ; r10 = 'bit buffer' (buffer word)					*/
/*  ; r11 = 'we are writing' flag					*/
/*  ; r12 = ) work regs							*/
/*  ; r14 = )								*/
/* 									*/
/*  When called from the 'drawcomp' decompression, the 'max_out'	*/
/*  parameter is always set to 0 (no output size limit).  Therefore	*/
/*  the slightly more involved code which checks and respects this	*/
/*  limit, in that case, has been removed.  See the original source	*/
/*  'lzw_decomp.s' for the full story.					*/
/************************************************************************/

/*	EXPORT	lzw_decompress			*/
/*	lzw_decompress				*/
int lzw_decompress(void *to,int max_out,const void *from)
{
	r0 = (int) to;
	r1 = max_out;
	r2 = (int) from;
#ifdef DEBUG_DECOMP
	fprintf(stderr,"\nlzw_decompress: %p -> %p max %d\n",from,to,max_out);
#endif

/*		STMFD	sp!, {r1-r12, lr} 	*/
/*						; set up regs in special case conditions  */
/*						; preserve 'to' AND use as a 'are we writing'  */
/*						; flag ( <>0 = yes)  */
/*		MOV	r11, r0			; if r0 = 0  */
	r11 = r0;
/*						; set up appropriate 'end of output' pointer  */
/*		CMP	r1, #0			*/
	if (r1==0)
/*		BEQ	lzw_decompress2		; branch to faster no-overflow-checks version of code  */
	goto lzw_decompress2;
	return (-1);

/*						; slightly different (and faster) version  */
/*						; used when overflow checking is not required  */
/*	lzw_decompress2				*/
lzw_decompress2:
/*						; setup/initialise all the other vars  */
/*		LDR	r7, addr_of_codetable	*/
	r7 = (int) &lzw_codetable;
/*						; initialise the bit buffer and pointer	*/
/*		BL	lzw_init_bit_buffer	*/
	lzw_init_bit_buffer();
/*						; set up the code table etc  */
/*						; clearing not strictly necessary,  */
/*						; but routine does set up some vars  */
/*		BL	lzw_clear_table		*/
	lzw_clear_table();
/*						; start the decompression process  */
/*		B	lzwd_while_test2	*/
	goto lzwd_while_test2;

/*	lzwd_got_clearcode2			*/
lzwd_got_clearcode2:
/*		BL	lzw_clear_table		*/
	lzw_clear_table();
/*		BL	lzw_get_code2		*/
	lzw_get_code2();
/*		SUB	lr, r12,#CLEAR_CODE	; is the code an 'END_CODE'?  */
	lr = r12-CLEAR_CODE;
/*		CMP	lr, #1			*/
	if (lr==1)
/*		BEQ	lzwd_exit2		*/
	goto lzwd_exit2;

/*						; algorithm dictates 'write_string(string_from_code(code))',  */
/*						; but as we have just had a clearcode,  */
/*						; 'code' will only be one of the preset codes.  */
/*		CMP	r11, #0			*/
	if (r11!=0)
/*		STRNEB	r12, [r0]		; store code (if allowed)  */
	*((unsigned char *) r0) = (r12 & 0xFF);
/*		ADD	r0, r0,#1		; increment output pointer  */
	++r0;
/*		MOV	r4, r12			; 'oldcode = code'  */
	r4 = r12;
/*		MOV	r5, r12			; r5 = 'first-char-of-string(oldcode)'  */
	r5 = r12;
/*		B	lzwd_while_test2	*/
	goto lzwd_while_test2;

/*	lzwd_while_loop2			*/
lzwd_while_loop2:
/*		CMP	r12, #CLEAR_CODE	*/
	if (r12==CLEAR_CODE)
/*		BEQ	lzwd_got_clearcode2	*/
	goto lzwd_got_clearcode2;
/*						; therefore... code other than 'clearcode' received  */
/*		MOV	lr, r12			; preserve 'code' for later  */
	lr = r12;
/*		CMP	r12, r6			; is read code in code_table?  */
	if (r12<r6)
/*		BLT	lzwd_was_in_table2	; yes - branch  */
	goto lzwd_was_in_table2;

/*						; if read code is not in table  */
/*		LDR	r3, [r7, r4,LSL #2]	; get entry from table  */
	r3 = *((int *) (r7+(r4<<2)));
/*		MOV	r3, r3,LSL #12		; get 'size-of-string-to-this-point'  */
/*						; out of bits 19..8  */
	r3 <<= 12;
/*		MOV	r3, r3,LSR #20		*/
	r3 >>= 20;
/*		ADD	r1, r3,#1		; (keep in r1)  */
	r1 = r3+1;
/*		CMP	r11, #0			; allowed to write?  */
	if (r11!=0)
/*		STRNEB	r5, [r0, r3]		; yes - store code  */
	*((unsigned char *) (r0+r3)) = (r5 & 0xFF);
/*		MOV	r12, r4			; set r12 ready to output 'string(oldcode)'  */
/*						; (otherwise we want to output string(code))  */
	r12 = r4;
/*		CMP	r12, #CLEAR_CODE	; if code is one of the preset ones...  */
	if (r12>=CLEAR_CODE)
/*		BGE	lzwd_backwd_copy_loop2	*/
	goto lzwd_backwd_copy_loop2;
/*		MOV	r5, r12			; don't bother launching into write loop,  */
	r5 = r12;
/*		B	lzwd_write_last2	; just branch to push-last-char instruction.  */
	goto lzwd_write_last2;

/*	lzwd_was_in_table2			*/
lzwd_was_in_table2:
/*						; send string (backwards) to output buffer  */
/*		CMP	r12, #CLEAR_CODE	; if code is one of the preset ones...  */
	if (r12<CLEAR_CODE)
	{
/*		MOVLT	r5, r12			; don't bother launching into push loop,  */
		r5 = r12;
/*		MOVLT	r1, #1			; (need to store a 'length-of-string on stack for later use)  */
		r1 = 1;
/*		BLT	lzwd_write_last2	; just branch to push-last-char instruction.  */
		goto lzwd_write_last2;
	}
/*		LDR	r5, [r7, r12,LSL #2]	; get entry from table  */
	r5 = *((unsigned int *) (r7+(r12<<2)));
/*		MOV	r3, r5,LSL #12		; get 'size-of-string-to-this-point'  */
/*						; out of bits 19..8  */
	r3 = r5<<12;
/*		MOV	r3, r3,LSR #20		*/
	r3 >>= 20;
/*		MOV	r1, r3			; preserve in r1 for later  */
	r1 = r3;

/*	lzwd_backwd_copy_loop2			*/
lzwd_backwd_copy_loop2:
/*		LDR	r5, [r7, r12,LSL #2]	*/
	r5 = *((unsigned int *) (r7+(r12<<2)));
/*		SUB	r3, r3,#1		*/
	--r3;
/*		CMP	r11, #0			; allowed to write?  */
	if (r11!=0)
/*		STRNEB	r5, [r0, r3]		; yes - store code  */
	*((unsigned char *) (r0+r3)) = (r5 & 0xFF);
/*		MOV	r12, r5,LSR #20		; code = code part of table entry (bits 31-20)	*/
	r12 = r5>>20;
/*		CMP	r12, #CLEAR_CODE	*/
	if (r12>=CLEAR_CODE)
/*		BGE	lzwd_backwd_copy_loop2	*/
	goto lzwd_backwd_copy_loop2;
/*						; fallen out of loop here with one  */
/*						; last one to write - also, if we have  */
/*						; done things right, r3 should be 1 at  */
/*						; this point (meaning one char left)  */
/*		LDR	r5, [r7, r12,LSL #2]	; get last char of string  */
	r5 = *((unsigned int *) (r7+(r12<<2)));
/*		AND	r5, r5,#&ff		*/
	r5 &= 0xFF;

/*	lzwd_write_last2			*/
lzwd_write_last2:
/*						; at this point, r5 contains the last char  */
/*						; to be written out (+bits 19-8 contain 'count')  */
/*						; add new table entry  */
/*		LDR	r3, [r7, r4,LSL #2]	; get entry for 'prefix code'  */
	r3 = *((unsigned int *) (r7+(r4<<2)));
/*		MOV	r3, r3,LSL #12		; clear out top 12 bits...  */
	r3 <<= 12;
/*		MOV	r3, r3,LSR #20		; ... and bottom eight  */
	r3 >>= 20;
/*		ADD	r3, r3,#1		; add one (for the current suffix char)  */
	++r3;
/*		ORR	r5, r5,r3,LSL #8	; and merge with suffix char  */
	r5 |= (r3<<8);
/*		ORR	r12, r5,r4,LSL #20	; "new entry = 'string(oldcode)+firstchar({old}code)'"  */
	r12 = r5 | (r4<<20);
/*		STR	r12, [r7, r6,LSL #2]	*/
	*((unsigned int *) (r7+(r6<<2))) = r12;
/*		ADD	r6, r6,#1		*/
	++r6;
/*						; check whether we need to increment 'code_bits'  */
/*		MOVS	r12, r6,LSR r8		; has 'next_code' reached 'change_code'?  */
	r12 = r6>>r8;
	if (r12!=0)
/*		ADDNE	r8, r8,#1		; yes - code_bits += 1  */
	++r8;
/*		MOV	r4, lr			; oldcode = code  */
	r4 = lr;

/*						; now is the time to write out last char  */
/*		CMP	r11, #0			; allowed to write?  */
	if (r11!=0)
/*		STRNEB	r5, [r0]		; yes - store code  */
	*((unsigned char *) r0) = (r5 & 0xFF);
/*		ADD	r0, r0,r1		; and update output pointer with length of written string  */
	r0 += r1;

/*	lzwd_while_test2			*/
lzwd_while_test2:
/*		BL	lzw_get_code2		; r12 = next code from compressed data  */
	lzw_get_code2();
/*		SUB	lr, r12,#CLEAR_CODE	; is the code an 'END_CODE'?  */
	lr = r12-CLEAR_CODE;
/*		CMP	lr, #1			*/
	if (lr!=1)
/*		BNE	lzwd_while_loop2	; no - go round again  */
	goto lzwd_while_loop2;

/*	lzwd_exit2				*/
lzwd_exit2:
/*						; tidy up and exit  */
/*		LDMFD	sp!, {r1-r12, pc}^	*/
#ifdef DEBUG_DECOMP
	fprintf(stderr,"                output %d bytes, maxbits %d\n",(r0-r11),r8);
#endif
	return (r0);
}

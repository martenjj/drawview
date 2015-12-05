/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		sprite.h		Edit:  19-Oct-05	*/
/*	SCCS:		<@(#) sprite.h 1.6>				*/
/*	Language:	ANSI C						*/
/*	Project:	Rewritten RiscOS Library (system)		*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	This is the header file for access to the sprite facilities.	*/
/*									*/
/************************************************************************/
/*									*/
/*	Library headers originally released by Acorn under the		*/
/*	following notice:						*/
/*									*/
/*	   This source file was written by Acorn Computers Limited.	*/
/*	   It is part of the RiscOS library for writing applications	*/
/*	   in C for RiscOS.  It may be used freely in the creation of	*/
/*	   programs for Archimedes.  It should be used with Acorn's	*/
/*	   C Compiler Release 3 or later.				*/
/*									*/
/*	Extensively rewritten and updated by Jonathan Marten as part	*/
/*	of the development of Draw 1½, DrawPlus, Vector, GridPro	*/
/*	and other products.						*/
/*									*/
/*	Copyright © Acorn Computers Limited 1992			*/
/*	Copyright © Jonathan Marten 1992-1998				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/

#ifndef __sprite_h
#define __sprite_h

/************************************************************************/
/*									*/
/*	Include files							*/
/*									*/
/************************************************************************/

#ifndef __ostypes_h
#include "sys/ostypes.h"				/* Operating system data types */
#endif

/************************************************************************/
/*									*/
/*	Constants, structures and types					*/
/*									*/
/************************************************************************/

typedef enum						/* Palette presence flag */
{
	sprite_paletteNONE	= 0,
	sprite_palettePRESENT	= 1,
	sprite_palette256ENTRY	= 2
} sprite_palflag;

typedef enum						/* Mask bit state */
{
	sprite_maskTRANSPARENT	= 0,
	sprite_maskSOLID	= 1
} sprite_maskstate;

#define sprite_areaSYSTEM	((sprite_area *) 0)	/* System sprite area */
#define sprite_areaWIMP		((sprite_area *) 1)	/* Wimp sprite area */
#define sprite_areaDUMMY	((sprite_area *) 256)	/* Sprite area irrelevant */

typedef struct						/* Scaling factors */
{
	int xmag,ymag;					/* Multipliers */
	int xdiv,ydiv;					/* Divisors */
} sprite_factors;

typedef char sprite_pixtrans;				/* Entry in translation table */

typedef struct						/* Sprite area control block */
{
	int size;					/* Total size of area */
	int number;					/* Number of sprites */
	int sproff;					/* Offset to first sprite */
	int freeoff;					/* Offset to first free */
} sprite_area;

typedef union						/* Sprite mode */
{
	int m;						/* Old-style mode number */
	struct						/* New-style mode word */
	{
		BOOL sprmode : 1;			/* Always TRUE for mode word */
		int hdpi : 13;				/* Horizontal resolution */
		int vdpi : 13;				/* Vertical resolution */
		int type : 5;				/* Sprite type */
	} w;
} sprite_mode;

typedef struct						/* Sprite header block */
{
	int next;					/* Offset to next sprite */
	char name[12];					/* Sprite name */
	int width;					/* Width (words-1) */
	int height;					/* Height (pixels-1) */
	int lbit;					/* First bit used (left end of row) */
	int rbit;					/* Last bit used (right end of row) */
	int image;					/* Offset to sprite image */
	int mask;					/* Offset to transparency mask */
	sprite_mode mode;				/* Mode sprite was defined in */
} sprite_header;					/* Palette, mask & image follow */

typedef sprite_header *sprite_ptr;			/* Pointer to sprite */

typedef enum						/* Sprite identifier tag */
{
	sprite_idNAME		= 0,			/* Name supplied */
	sprite_idADDR		= 0x74527053		/* Pointer supplied ("SpRt") */
} sprite_idtag;

typedef struct						/* Sprite identifier */
{
	union						/* Name or pointer */
	{
		const char *name;			/* Sprite name */
		sprite_ptr addr;			/* Sprite pointer */
	} s;
	sprite_idtag tag;				/* Identifier tag */
} sprite_id;

typedef struct						/* Sprite information */
{
	int width,height;				/* Size (pixels) */
	int mask;					/* Mask present */
	int mode;					/* Screen mode */
} sprite_info;

typedef struct						/* Pixel colour */
{
	int colour;					/* Base colour */
	int tint;					/* Supplementary tint */
} sprite_colour;

typedef struct						/* Redirection state */
{
	int r[4];					/* Saved registers */
} sprite_state;

typedef struct						/* Destination parallelogram */
{
	int p0[2];
	int p1[2];
	int p2[2];
	int p3[2];
} sprite_pgm;

typedef int sprite_transmat[6];				/* Transform matrix */

typedef struct						/* Rectangle box */
{
	int x0,y0;
	int x1,y1;
} sprite_box;

/************************************************************************/
/*									*/
/*	External functions						*/
/*									*/
/************************************************************************/

extern os_error *sprite_screensave(const char *filename,sprite_palflag palette);
extern os_error *sprite_screenload(const char *filename);

extern void sprite_area_initialise(sprite_area *area,int size);
extern os_error *sprite_area_readinfo(sprite_area *area,sprite_area *result);
extern os_error *sprite_area_reinit(sprite_area *area);
extern os_error *sprite_area_load(sprite_area *area,const char *filename);
extern os_error *sprite_area_merge(sprite_area *area,const char *filename);
extern os_error *sprite_area_save(sprite_area *area,const char *filename);
extern os_error *sprite_getname(sprite_area *area,char *buffer,int *length,int idx);

extern os_error *sprite_get(sprite_area *area,const char *name,sprite_palflag palette);
extern os_error *sprite_get_rp(sprite_area *area,const char *name,sprite_palflag palette,sprite_ptr *result);
extern os_error *sprite_get_given(sprite_area *area,const char *name,sprite_palflag palette,
                                  int x0,int y0,int x1,int y1);
extern os_error *sprite_get_given_rp(sprite_area *area,const char *name,sprite_palflag palette,
                                     int x0,int y0,int x1,int y1,sprite_ptr *result);
extern os_error *sprite_create(sprite_area *area,const char *name,sprite_palflag palette,
                               int width,int height,int mode);
extern os_error *sprite_create_rp(sprite_area *area,const char *name,sprite_palflag palette,
                                  int width,int height,int mode,sprite_ptr *result);

extern os_error *sprite_select(sprite_area *area,const sprite_id *id);
extern os_error *sprite_select_rp(sprite_area *area,const sprite_id *id,sprite_ptr *result);

extern os_error *sprite_delete(sprite_area *area,const sprite_id *id);
extern os_error *sprite_rename(sprite_area *area,const sprite_id *id,const char *newname);
extern os_error *sprite_copy(sprite_area *area,const sprite_id *id,char *newname);
extern os_error *sprite_removewastage(sprite_area *area,const sprite_id *id);

extern os_error *sprite_put(sprite_area *area,const sprite_id *id,int gcol);
extern os_error *sprite_put_given(sprite_area *area,const sprite_id *id,int gcol,int x,int y);
extern os_error *sprite_put_scaled(sprite_area *,const sprite_id *id, int gcol,int x,int y,
                                   const sprite_factors *factors,const sprite_pixtrans pixtrans[]);
extern os_error *sprite_put_greyscaled(sprite_area *area,const sprite_id *id,int x,int y,
                                       const sprite_factors *factors,const sprite_pixtrans pixtrans[]);
extern os_error *sprite_put_mask(sprite_area *area,const sprite_id *id);
extern os_error *sprite_put_mask_given(sprite_area *area,const sprite_id *id,int x,int y);
extern os_error *sprite_put_mask_scaled(sprite_area *,const sprite_id *id,int x,int y,
                                        const sprite_factors *factors);
extern os_error *sprite_put_char_scaled(char ch,int x,int y,const sprite_factors *factors);

extern os_error *sprite_create_mask(sprite_area *area,const sprite_id *id);
extern os_error *sprite_remove_mask(sprite_area *area,const sprite_id *id);
extern os_error *sprite_insert_row(sprite_area *area,const sprite_id *id,int row);
extern os_error *sprite_delete_row(sprite_area *area,const sprite_id *id,int row);
extern os_error *sprite_insert_column(sprite_area *area,const sprite_id *id,int column);
extern os_error *sprite_delete_column(sprite_area *area,const sprite_id *id,int column);
extern os_error *sprite_flip_x(sprite_area *area,const sprite_id *id);
extern os_error *sprite_flip_y(sprite_area *area,const sprite_id *id);
extern os_error *sprite_createremove_palette(sprite_area *area,const sprite_id *id,sprite_palflag flag);

extern os_error *sprite_readsize(sprite_area *area,const sprite_id *id,sprite_info *result);
extern os_error *sprite_readinfo(sprite_area *area,const sprite_id *id,sprite_info *result);

extern os_error *sprite_readpixel(sprite_area *area,const sprite_id *id,int x,int y,sprite_colour *result);
extern os_error *sprite_writepixel(sprite_area *area,const sprite_id *id,int x,int y,sprite_colour *colour);
extern os_error *sprite_readmask(sprite_area *area,const sprite_id *id,int x,int y,sprite_maskstate *result);
extern os_error *sprite_writemask(sprite_area *area,const sprite_id *id,int x,int y,sprite_maskstate *state);

extern os_error *sprite_restorestate(sprite_state *state);
extern os_error *sprite_outputtosprite(sprite_area *area,const sprite_id *id,
                                       int *save_area,sprite_state *state);
extern os_error *sprite_outputtomask(sprite_area *area,const sprite_id *id,
                                       int *save_area,sprite_state *state);
extern os_error *sprite_outputtoscreen(int *save_area,sprite_state *state);
extern os_error *sprite_sizeof_spritecontext(sprite_area *area,const sprite_id *id,int *size);
extern os_error *sprite_sizeof_screencontext(int *size);

extern os_error *sprite_change_size(sprite_area *area,const sprite_id *id,BOOL rows,int at,int number);

extern os_error *sprite_put_mask_trans(sprite_area *area,const sprite_id *id,
                                       const sprite_box *box,const sprite_transmat *mat);
extern os_error *sprite_put_mask_pgm(sprite_area *area,const sprite_id *id,
                                     const sprite_box *box,const sprite_pgm *pgm);
extern os_error *sprite_put_trans(sprite_area *area,const sprite_id *id,int gcol,const sprite_box *box,
                                  const sprite_transmat *mat,const sprite_pixtrans pixtrans[]);
extern os_error *sprite_put_pgm(sprite_area *area,const sprite_id *id,int gcol,const sprite_box *box,
                                const sprite_pgm *pgm,const sprite_pixtrans pixtrans[]);

extern os_error *sprite_merge_areas(sprite_area *area1,sprite_area *area2);
extern os_error *sprite_merge_sprite(sprite_area *area2,sprite_area *area1,const sprite_id *spr);

extern os_error *sprite_get_pointer(sprite_area *area,sprite_id *id);

#endif

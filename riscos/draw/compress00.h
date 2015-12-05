/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		compress00.h		Edit:  23-Mar-05	*/
/*	SCCS:		<@(#) compress00.h 1.4>				*/
/*	Language:	ANSI C						*/
/*	Project:	Draw file compression library			*/
/*	Developed on:	Vector						*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	This is the internal header file for the Draw file compression	*/
/*	and decompression routines.  It defines symbolic codes for the	*/
/*	various object compression types, along with some other		*/
/*	miscellaneous definitions.					*/
/*									*/
/************************************************************************/
/*									*/
/*	Written by Paul Lebeau and possibly others as part of the	*/
/*	development of Poster, smArt and other products for		*/
/*	4Mation Educational Resources.  Updated and extended by		*/
/*	Jonathan Marten in development of DrawPlus, Vector and		*/
/*	associated products.						*/
/*									*/
/*	Copyright © Paul Lebeau 1991-1992				*/
/*	Copyright © Jonathan Marten 1992-1994				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/


/************************************************************************/
/*									*/
/*	Macros and constants						*/
/*									*/
/************************************************************************/

#define drwptr(diag,o)		((int *) (((char *) (diag)->data)+((int) o)))

#define DRAW_FILE_ID		"Draw"			/* Identifier for first word of file */
#define DRWC_FILE_ID		"DrwC"

#define INITBUF			2000			/* Allocation for object buffer */

/************************************************************************/
/*									*/
/*	Compression codes for general objects				*/
/*									*/
/************************************************************************/

#define CC_NO_COMPRESSION	0			/* No compression other than 2
							   bytes in the object header */

#define CC_LZW_ALL		1			/* TIFF-compatible LZW compression
							   on all of the object from the
							   byte +8 onwards (includes bbox
							   if present).  Used for unknown
							   objects (if possible) etc. */

#define CC_LZW_BBOX		2			/* Keep bbox and do TIFF-compatible
							   LZW compression on all of the
							   rest of the object */

#define CC_LZW_NO_BBOX		3			/* Skip bbox and do TIFF-compatible
							   LZW compression on all of the
							   rest of the object */

/************************************************************************/
/*									*/
/*	Special types of compression for individual objects		*/
/*									*/
/************************************************************************/

#define CC_SPECIALS_START	128

#define CC_PATH_PACK_COORDS	128			/* Minimum Byte Coordinate
							   Difference packing */

#define CC_GROUP_SIMPLE		128			/* Removal of 'group name' string
							   and compression of sub-objects */

#define CC_TAGGED_SIMPLE	128			/* Copy tag id, compression of
							   'draw-able' part and straight
							   copy of 'extra data' */

#define CC_TAGGED_EXTRALZW	129			/* Copy tag id, compression of
							   'draw-able' part and LZW'ing
							   of extra data */

#define CC_TEXTAREA_SIMPLE	128			/* Compression of column object
							   headers, straight copy of data
							   words and text */

#define CC_TEXTAREA_TEXTLZW	129			/* Compression of column object
							   headers, straight copy of data
							   words and LZW'ing of text */

/************************************************************************/
/*									*/
/*	Mask bits definitions						*/
/*									*/
/************************************************************************/

#define CFLAGS_OBJBYTES		0x00000003		/* Bytes to use for storing object type */
#define CFLAGS_RESERVED		0xfffffffc		/* Bits that are reserved and should be zero */

/************************************************************************/
/*									*/
/*	External functions - from 'computil.s'				*/
/*									*/
/************************************************************************/

#define get4		drawcomp_get4
#define fetch4		drawcomp_fetch4
#define store4		drawcomp_store4
#define put4		drawcomp_put4

extern int get4(char *from,int *o_in);
extern int fetch4(char *from,int o_in);
extern void store4(char *to,int o_out,int word);
extern void put4(char *to,int *o_out,int word);

/************************************************************************/
/*									*/
/*	File types							*/
/*									*/
/************************************************************************/

#define file_Draw	0xAFF				/* 'DrawFile' */

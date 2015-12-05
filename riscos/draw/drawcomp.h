/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		drawcomp.h		Edit:  23-Mar-05	*/
/*	SCCS:		<@(#) drawcomp.h 1.4>				*/
/*	Language:	ANSI C						*/
/*	Project:	Draw file compression library			*/
/*	Developed on:	Vector						*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	This is the header file for the Draw file compression		*/
/*	and decompression functions.					*/
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
/*	Copyright © Jonathan Marten 1991-1996				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/

#ifndef __drawcomp_h
#define __drawcomp_h

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
/*	Compression options.  The tag size options must always keep	*/
/*	these values, since they are saved with the compressed file.	*/
/*									*/
/************************************************************************/

#define drawcomp_compTAG1	0x0000			/* 1-byte object tag (normal file) */
#define drawcomp_compTAG3	0x0002			/* 3-byte object tag (Vector file) */

#define drawcomp_compNOTDRAW	0x000E			/* Not a Draw file */
#define drawcomp_compNOCOMP	0x000F			/* No compression */

/************************************************************************/
/*									*/
/*	Decompression status						*/
/*									*/
/************************************************************************/

#define drawcomp_typeERROR	(-1)			/* Error while decompressing */
#define drawcomp_typeRAMXFER	(-2)			/* RAM transfer refused */

/************************************************************************/
/*									*/
/*	External functions						*/
/*									*/
/************************************************************************/

extern int drawcomp_readDrawFile(draw_diag *diag,const char *name,BOOL any,int *cf,unsigned int *addr);

/************************************************************************/
/*									*/
/*	DRAWCOMP_COMPRESSDIAG - Compress a diagram either into another	*/
/*	diagram, or in place.						*/
/*									*/
/************************************************************************/

extern BOOL drawcomp_compressDiag(draw_diag *diag_in,draw_diag *diag_out,int cflags);

#endif

/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		drawLZW.h		Edit:  23-Mar-05	*/
/*	SCCS:		<@(#) drawLZW.h 1.4>				*/
/*	Language:	ANSI C						*/
/*	Project:	Draw file compression library			*/
/*	Developed on:	DrawCompress					*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	This is the header file for the TIFF-compatible LZW		*/
/*	compression and decompression functions.			*/
/*									*/
/************************************************************************/
/*									*/
/*	Written by Paul Lebeau and possibly others as part of the	*/
/*	development of Poster, smArt and other products for		*/
/*	4Mation Educational Resources.					*/
/*									*/
/*	Copyright © Paul Lebeau 1991-1992				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/

extern void *lzw_compress(void *to,int max_out,const void *from,int from_size);
extern int lzw_decompress(void *to,int max_out,const void *from);

/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		ostypes.h		Edit:  14-Mar-05	*/
/*	SCCS:		<@(#) ostypes.h 1.4>				*/
/*	Language:	ANSI C						*/
/*	Project:	Rewritten RiscOS Library (system)		*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	This is the header file for the low-level OS routine access.	*/
/*	This file defines fundamental OS data types only;  system	*/
/*	calls (use of them is inefficient & deprecated) are in		*/
/*	"oscalls.h".							*/
/*									*/
/************************************************************************/
/*									*/
/*	Library sources originally released by Acorn under the		*/
/*	following notice:						*/
/*									*/
/*	   This file forms part of an unsupported source release	*/
/*	   of RISC_OSLib.						*/
/*									*/
/*	   It may be freely used to create executable images for	*/
/*	   saleable products but cannot be sold in source form or	*/
/*	   as an object library without the prior written consent	*/
/*	   of Acorn Computers Ltd.					*/
/*									*/
/*	   If this file is re-distributed (even if modified) it		*/
/*	   should retain this copyright notice.				*/
/*									*/
/*	Extensively rewritten and updated by Jonathan Marten as part	*/
/*	of the development of Draw 1½, DrawPlus, Vector, GridPro	*/
/*	and other products.						*/
/*									*/
/*	Copyright © Acorn Computers Limited 1992			*/
/*	Copyright © Jonathan Marten 1992-1996				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/

#ifndef __ostypes_h
#define __ostypes_h

/************************************************************************/
/*									*/
/*	Boolean data type and values					*/
/*									*/
/************************************************************************/

typedef int		BOOL;
#define TRUE		(1)
#define FALSE		(0)

/************************************************************************/
/*									*/
/*	Fundamental data types and values				*/
/*									*/
/************************************************************************/

typedef unsigned int	os_colour;			/* BBGGRRXX true colour */

#define TRANSPARENT	(0xFFFFFFFF)			/* Transparent (no) colour */

typedef void **mem_ptr;					/* Memory block anchor */

/************************************************************************/
/*									*/
/*	Structures and types ('os_regset' needed for "sys/wimp.h") 	*/
/*									*/
/************************************************************************/

typedef struct						/* Error data */
{
	int errnum;					/* Error number */
	char errmess[252];				/* Error message (zero terminated) */
} os_error;

typedef struct						/* Registers for system call */
{
	int r[10];					/* Only R0-R9 needed for SWIs */
} os_regset;

/************************************************************************/
/*									*/
/*	Macros								*/
/*									*/
/************************************************************************/

#define roundup(n)	(((n)+3) & ~3)			/* For memory allocation */

#endif

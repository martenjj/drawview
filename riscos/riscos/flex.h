/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		flex.h			Edit:  20-Mar-05	*/
/*	SCCS:		<@(#) flex.h 1.4>				*/
/*	Language:	ANSI C						*/
/*	Project:	Rewritten RiscOS Library			*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	This is the header file for the flexible memory allocation.	*/
/*									*/
/*	The memory is allocated from the Wimp's free pool, if		*/
/*	available.  The allocated store must then be accessed		*/
/*	indirectly, through the anchor pointer, i.e.			*/
/*									*/
/*		(*anchor)[0] ..(*anchor)[n]				*/
/*									*/
/*	This is important since the allocated store may later be	*/
/*	moved (it's a shifting heap!) to extend or free lower blocks.	*/
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
/*	Copyright © Jonathan Marten 1992-1996				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/

#ifndef __flex_h
#define __flex_h

/************************************************************************/
/*									*/
/*	Include files							*/
/*									*/
/************************************************************************/

#ifndef __stddef_h
#include <stddef.h>					/* Standard definitions */
#endif
#ifndef __ostypes_h
#include "sys/ostypes.h"				/* Operating system data types */
#endif

/************************************************************************/
/*									*/
/*	FLEX_ALLOC - Allocates flex store, obtained from the Wimp.	*/
/*									*/
/*	Parameters:	anchor		Anchor pointer for new block	*/
/*			n		Size of block to be allocated	*/
/*									*/
/*	Returns:	TRUE if memory successfully allocated.		*/
/*									*/
/************************************************************************/

extern BOOL flex_alloc(mem_ptr anchor,size_t n);

/************************************************************************/
/*									*/
/*	FLEX_FREE - Frees the previously-allocated store.		*/
/*									*/
/*	Parameters:	anchor		Anchor pointer of block		*/
/*									*/
/*	Other Info:	The anchor will be set to NULL to indicate	*/
/*			that the block is no longer allocated.		*/
/*									*/
/************************************************************************/

extern void flex_free(mem_ptr anchor);

/************************************************************************/
/*									*/
/*	FLEX_SIZE - Read the allocated size of a flex block.		*/
/*									*/
/*	Parameters:	anchor		Anchor pointer of block		*/
/*									*/
/*	Returns:	Current size of allocated block.		*/
/*									*/
/************************************************************************/

extern size_t flex_size(mem_ptr anchor);

/************************************************************************/
/*									*/
/*	FLEX_EXTEND - Extend or truncate the block at the end.		*/
/*									*/
/*	Parameters:	anchor		Anchor pointer of block		*/
/*			newsize		New size requested		*/
/*									*/
/*	Returns:	TRUE if size successfully changed.		*/
/*									*/
/************************************************************************/

extern BOOL flex_extend(mem_ptr anchor,size_t newsize);

/************************************************************************/
/*									*/
/*	FLEX_MIDEXTEND - Extend or truncate the block, at any point.	*/
/*									*/
/*	Parameters:	anchor		Anchor pointer of block		*/
/*			at		Offset within the block		*/
/*			by		Amount to change size by	*/
/*									*/
/*	Returns:	TRUE if size successfully changed.		*/
/*									*/
/*	Other Info:	If "by" is +ve, then the store is extended,	*/
/*			and locations above "at" are copied up by	*/
/*			"by".  If "by" is -ve, then the store is	*/
/*			shrunk, and any bytes beyond "at" are copied	*/
/*			down to "at+by".				*/
/*									*/
/************************************************************************/

extern BOOL flex_midextend(mem_ptr anchor,size_t at,int by);

/************************************************************************/
/*									*/
/*	FLEX_LOCK - Stop stack extension from causing flex memory	*/
/*	to move.							*/
/*									*/
/*	FLEX_UNLOCK - Allow flex memory to move if required for		*/
/*	stack extension.						*/
/*									*/
/*	FLEX_PERMLOCK - Permanently stop stack extension from causing	*/
/*	flex memory to move.						*/
/*									*/
/************************************************************************/

extern void flex_lock(void);
extern void flex_unlock(void);
extern void flex_permlock(void);

/************************************************************************/
/*									*/
/*	FLEX_BUDGE - Move flex store when the C library needs to	*/
/*	extend the heap.  This function is registered with the		*/
/*	kernel's slot extend handler if flex blocks can be allowed	*/
/*	to move to extend the heap.  This can happen by default		*/
/*	before flex initialisation.					*/
/*									*/
/*	Parameters:	n		Size needed by C library	*/
/*			a		Address of acquired store	*/
/*									*/
/*	Returns:        Amount of store acquired.			*/
/*									*/
/*	Other Info:	If budging is enabled, the location of flex	*/
/*			blocks may change over any function call	*/
/*			(which may extend the stack) or any use of	*/
/*			the "malloc" system.				*/
/*									*/
/************************************************************************/

extern int flex_budge(int n,void **a);

/************************************************************************/
/*									*/
/*	FLEX_DONT_BUDGE - Refuse to move flex store when the C library	*/
/*	needs to extend the heap.  This is the default state after	*/
/*	flex initialisation.						*/
/*									*/
/*	Parameters:	n		Size needed by C library	*/
/*			a		Address of acquired store	*/
/*									*/
/*	Returns:        Amount of store acquired (always 0).		*/
/*									*/
/************************************************************************/

extern int flex_dont_budge(int n,void **a);

/************************************************************************/
/*									*/
/*	FLEX_DYNAMICINIT - Initialise the flex allocation system to	*/
/*	use a dynamic area if possible.					*/
/*									*/
/*	Parameters:	areatitle	Title for dynamic area		*/
/*									*/
/************************************************************************/

extern void flex_dynamicinit(const char *areatitle);

/************************************************************************/
/*									*/
/*	FLEX_INIT - Initialise the flex allocation system to use	*/
/*	application memory only.					*/
/*									*/
/************************************************************************/

extern void flex_init(void);

/************************************************************************/
/*									*/
/*	FLEX_REPOINT - Copy a flex pointer and register it as the new	*/
/*	block anchor, and set the old one to NULL to indicate that it	*/
/*	is no longer valid.						*/
/*									*/
/*	Parameters:	old		Old block anchor		*/
/*			new		New block anchor		*/
/*									*/
/*	Note:		This is the only safe way of copying a flex	*/
/*			anchor pointer.					*/
/*									*/
/************************************************************************/

extern void flex_repoint(mem_ptr old,mem_ptr new);

/************************************************************************/
/*									*/
/*	FLEX_VALID - See if that anchor addresses a valid flex block.	*/
/*									*/
/*	Parameters:	anchor		Anchor pointer of block		*/
/*									*/
/*	Returns:	TRUE if a valid flex block.			*/
/*									*/
/************************************************************************/

extern BOOL flex_valid(mem_ptr anchor);

#endif

/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		visdelay.h		Edit:  14-Mar-05	*/
/*	SCCS:		<@(#) visdelay.h 1.4>				*/
/*	Language:	ANSI C						*/
/*	Project:	Rewritten RiscOS Library (system)		*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	This is the header file for the visible delay indication.	*/
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
/*	Copyright © Acorn Computers Limited 1990			*/
/*	Copyright © Codemist Limited 1990				*/
/*	Copyright © Jonathan Marten 1992				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/

#ifndef __visdelay_h
#define __visdelay_h

extern void visdelay_begin(void);
extern void visdelay_percent(int pct);
extern void visdelay_end(void);
extern void visdelay_init(void);

extern void visdelay_start(int delay);
extern void visdelay_leds(int eor,int mask);
extern void visdelay_smash(void);

#endif

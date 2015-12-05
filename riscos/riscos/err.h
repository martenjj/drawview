/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		err.h			Edit:  17-Mar-05	*/
/*	SCCS:		<@(#) err.h 1.4>				*/
/*	Language:	ANSI C						*/
/*	Project:	Rewritten RiscOS Library			*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	This is the header file for the error handling facilities.	*/
/*	Some of the equivalent facilities were originally in 'wimpt'.	*/
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
/*	Copyright © Jonathan Marten 1992-1997				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/

#ifndef __err_h
#define __err_h

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
/*	External variables						*/
/*									*/
/************************************************************************/

extern BOOL err_debug;						/* Backtrace debug enable */

/************************************************************************/
/*									*/
/*	External functions						*/
/*									*/
/************************************************************************/

extern os_error *err_check(os_error *e);			/* wimpt_complain */
extern void err_checkFatal(os_error *e);			/* wimpt_noerr */
extern void err_reportError(os_error *e,int flags);		/* wimpt_reporterror */

extern os_error *err_lastError(void);				/* synonym for _kernel_last_oserror */

extern os_error *err_openError(void);				/* wimpt_openerror */
extern void err_openPrepare(void);

#pragma -v1							/* check f/s/printf format */
extern void err_report(BOOL fatal,const char *format,...);	/* werr */
extern os_error *err_generate(const char *format,...);		/* wimpt_errorblock */
#pragma -v0							/* back to default */

extern void err_reportLookup(BOOL fatal,const char *key,...);	/* werr_lookup */
extern void err_infoLookup(const char *key,...);		/* same but info message */
extern os_error *err_generateLookup(const char *key,...);	/* wimpt_errorblocklookup */

extern void err_setsprite(const char *name);			/* set sprite to be used */

#endif

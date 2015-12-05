/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		program.h		Edit:  23-Mar-05	*/
/*	SCCS:		<@(#) program.h 1.4>				*/
/*	Language:	ANSI C						*/
/*	Project:	Everything					*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	Definitions and macros for programming convenience.		*/
/*									*/
/************************************************************************/
/*									*/
/*	Written by Jonathan Marten as part of the development of	*/
/*	DrawPlus, Vector, GridPro and various other applications.	*/
/*									*/
/*	Copyright © Jonathan Marten 1990-1996				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/

#define swi(code)	((os_error *) _kernel_swi(code,&regs,&regs))

#ifdef SIMPLEKEYWORDS
#define forward
#define global
#define local		static
#define endcase		break
#define forever		for (;;)
#endif

#ifndef NULL
#define NULL		(0)
#endif

#define complain(p)	err_check((os_error *) p)
#define noerr(p)	err_checkFatal((os_error *) p)

#define min(a,b)	((a)<(b) ? (a) : (b))
#define max(a,b)	((a)>(b) ? (a) : (b))
#define sgn(a)		((a)==0 ? 0 : ((a)>0 ? +1 : -1))

#define ptrdiff(p,q)	(((char *) (p))-((char *) (q)))
#define ptroff(p,i)	((void *) (((char *) (p))+((int) (i))))

#define unspec(x)	(*((int *) &(x)))

#define idmul(i,f)	((int) (((double) (i))*(f)))	/* int := int*double */
#define iddiv(i,f)	((int) (((double) (i))/(f)))	/* int := int/double */
#define iidiv(a,b)	(((double) (a))/((double) (b)))	/* double := int/int */

#define PI		3.14159265358979

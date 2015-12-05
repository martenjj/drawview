/************************************************************************/
/*  File:	cmdutils.h				Edit: 19-Oct-05	*/
/*  Author:	jonathan.marten@uk					*/
/*  SCCS:	<@(#) cmdutils.h 1.6>					*/
/************************************************************************/

#ifndef __CMDUTILS_H
#define __CMDUTILS_H

/************************************************************************/
/*  Message keys for cmderr() - keep ERROR and NONE as these values	*/
/*  backwards compatibility, also sync with 'errstrings'.		*/
/************************************************************************/

enum cemsg
{
	CEERROR		= 0,				/* compatibility - exit */
	CENONE		= 1,				/* compatibility - benign */
	CEWARNING	= 2,				/* continue */
	CENOTICE	= 3,				/* continue */
	CEINFO		= 4,				/* continue */
	CESUCCESS	= 5,				/* continue */
	CEFATAL		= 6,				/* exit */
	CEUNKNOWN	= 7				/* must be last */
};

/************************************************************************/
/*  System error keys for cmderr() - also keep NONE and ERRNO for	*/
/*  backwards compatibility.						*/
/************************************************************************/

enum cesys
{
	CSNONE		= 0,				/* compatibility - no message */
	CSERRNO		= 1,				/* compatibility - decode 'errno' */
	CSHOST		= 2				/* decode 'h_errno' */
};

/************************************************************************/
/*  MYNAME - The name by which this program was invoked.		*/
/*  MYDIR  - The true directory location of this program file.		*/
/*									*/
/*  These are set up by calling cmdinit() with the value of argv[0]	*/
/*  passed to the program.						*/
/************************************************************************/

extern char myname[];
extern char mydir[];

/************************************************************************/
/*  CMDINIT - Read argv[0] and set the above variables appropriately.	*/
/************************************************************************/

extern void cmdinit(const char *argv0);

/************************************************************************/
/*  CMDRESOURCE - Locate a resource file using the true location of	*/
/*  the program.							*/
/************************************************************************/

extern char *cmdresource(const char *resfile,char *buf);

/************************************************************************/
/*  CMDERR - Display an error message with parameters, and optionally	*/
/*  with the system error as well.					*/
/************************************************************************/

extern void cmderr(enum cesys src,enum cemsg severity,const char *fmt,...)
#if defined(__GNUC__)
__attribute__((format (printf,3,4)))
#endif
;

#endif							/* __CMDUTILS_H */

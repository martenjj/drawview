/*  It's nested a bit deep, so...              -*- c-basic-offset:4 -*-	*/
/************************************************************************/
/*  File:	cmdutils.c				Edit: 18-Mar-03	*/
/*  Author:	jonathan.marten@uk					*/
/************************************************************************/

static const char sccsid[] = "@(#) cmdutils.c 1.10";

/************************************************************************/
/*  Include files							*/
/************************************************************************/

#ifdef LINUX
#define _GNU_SOURCE					/* needed for RTLD_DEFAULT */
#endif

#include <stdlib.h>					/* standard library */
#include <stdio.h>					/* filename length */
#include <string.h>					/* string handling */
#include <stdarg.h>					/* variable argument lists */
#include <errno.h>					/* system call errors */
#include <netdb.h>					/* network database lookup */
#include <dlfcn.h>					/* dymanic linking */

#include <unistd.h>					/* system routines */
#include <sys/stat.h>					/* file status data */

#include "cmdutils.h"					/* our header file */

/************************************************************************/
/*  Severity strings for cmderr() reports				*/
/************************************************************************/

static const char *errstrings[] =
{
    "ERROR",						/* 0 */
    NULL,						/* 1 */
    "WARNING",						/* 2 */
    "NOTICE",						/* 3 */
    "INFO",						/* 4 */
    "SUCCESS",						/* 5 */
    "FATAL",						/* 6 */
    NULL						/* 7 */
};

/************************************************************************/
/*  MYNAME - The name by which this program was invoked.  Obvious.	*/
/*									*/
/*  MYDIR  - The true directory location of this program file.  This	*/
/*  is not so obvious due to the need to handle two possible directory	*/
/*  structures - one organised (with bin, etc, &c. subdirectories)	*/
/*  and the other not (everything together).				*/
/*									*/
/*  If the last directory component of the containing directory is	*/
/*  "bin" or "sbin", then this must be the organised structure and	*/
/*  the directory is recorded as the one containing that.  If not, the	*/
/*  directory containing the executable is used.			*/
/************************************************************************/

char myname[FILENAME_MAX] = "unknown";
char mydir[FILENAME_MAX] = ".";

/************************************************************************/
/*  CMDINIT - Read argv[0] and set the above variables appropriately.	*/
/************************************************************************/

void cmdinit(const char *argv0)
{
    register char *p;					/* filename parsing */
    static const char *pathvar;				/* to search for executables */
    static char *pathbuf = NULL;			/* copy of PATH variable */
    char *pc;						/* each component of that */
    char execpath[FILENAME_MAX];			/* full name of executable */
    struct stat st;					/* file information */

    p = strrchr(argv0,'/');				/* locate the leaf name */
    if (p==NULL) p = (char *) argv0;			/* just a leaf name */
    else ++p;						/* past the last '/' */
    strcpy(myname,p);					/* save as our name */

    if (strchr(argv0,'/')==NULL)			/* it's a leaf name only */
    {
	if (pathbuf==NULL)				/* see if read PATH already */
	{
	    pathvar = getenv("PATH");			/* if not then find in env */
	    if (pathvar==NULL) return;			/* unlikely to ever happen */

	    pathbuf = malloc(strlen(pathvar)+2);	/* get memory to hold it */
	    if (pathbuf==NULL) return;			/* very unlikely to happen */
	}

	strcpy(pathbuf,pathvar);			/* working copy of PATH */
	pc = strtok(pathbuf,":"); while (pc!=NULL)	/* look at each component */
	{
	    strcpy(execpath,pc);			/* directory to search */
	    if (pc[strlen(pc)-1]!='/') strcat(execpath,"/");
	    strcat(execpath,argv0);			/* file name to find */

	    if (stat(execpath,&st)==0 &&		/* see if file exists */
		S_ISREG(st.st_mode) &&			/* and is a regular file */
		access(execpath,X_OK)==0)		/* and execute is allowed */
	    {
		argv0 = execpath;			/* if so then found */
		break;					/* can end search now */
	    }

	    pc = strtok(NULL,":");			/* else try next component */
	}
    }

    realpath(argv0,mydir);				/* now get the real path */
    p = strrchr(mydir,'/');				/* locate the last component */
    if (p!=NULL && p>mydir) *p = '\0';			/* and trim it off */

    p = strrchr(mydir,'/');				/* locate new last component */
    if (p!=NULL && p>mydir)				/* if there is one */
    {							/* trim off if subdirectory */
	if (strcmp(p,"/bin")==0 || strcmp(p,"/sbin")==0) *p = '\0';
    }
}

/************************************************************************/
/*  CMDRESOURCE - Locate a resource file relative to the true location	*/
/*  of the program.							*/
/************************************************************************/

char *cmdresource(const char *resfile,char *buf)
{
    buf[0] = '\0';					/* start with empty buffer */
    if (resfile[0]!='/')				/* not a full pathname */
    {
	strcpy(buf,mydir);				/* directory location */
	if (buf[strlen(buf)-1]!='/') strcat(buf,"/");	/* separator if necessary */
    }
    strcat(buf,resfile);				/* the file name */

    return (buf);					/* return full file name */
}

/************************************************************************/
/*  CMDERR - Display an error message with parameters, optionally	*/
/*  adding the system error as well.					*/
/************************************************************************/

void cmderr(enum cesys src,enum cemsg sev,const char *fmt,...)
{
    va_list args;					/* parameters for format */
    const char *ss;					/* severity string */
    const char *mm = NULL;				/* textual error message*/

    if (src==CSHOST)					/* network host lookup error */
    {
	int *herrno;

	herrno = dlsym(RTLD_DEFAULT,"h_errno");		/* get lookup error symbol */
	if (herrno==NULL) mm = "Error unavailable";	/* symbol not found */
	else
	{
	    switch (*herrno)				/* get code from symbol */
	    {
case HOST_NOT_FOUND:	mm = "No such host";		break;
case TRY_AGAIN:		mm = "Try again";		break;
case NO_RECOVERY:	mm = "Name server failure";	break;
case NO_DATA:		mm = "Address not available";	break;
default:		mm = "Unknown error";		break;
	    }
	}
    }
    else if (src==CSERRNO) mm = strerror(errno);	/* system error message */

    fflush(stdout); fflush(stderr);			/* flush current output */

    if (sev<0 || sev>CEUNKNOWN) sev = CEUNKNOWN;	/* catch bad parameter */
    ss = errstrings[sev];

    fprintf(stderr,"%s",myname);			/* report our program name */
    if (ss!=NULL) fprintf(stderr," (%s)",ss);		/* report severity if there is one */
    fprintf(stderr,": ");

    va_start(args,fmt);					/* locate variable parameters */
    vfprintf(stderr,fmt,args);				/* user message and parameters */
    if (mm!=NULL) fprintf(stderr,", %s",mm);		/* system error message */
    va_end(args);					/* finished with parameters */
    fprintf(stderr,"\n");				/* end of message */
							/* exit from application */
    if (sev==CEERROR || sev==CEFATAL) exit(EXIT_FAILURE);
}

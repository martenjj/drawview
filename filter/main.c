/************************************************************************/
/*									*/
/*  Project:	DrawView - Decompression filter				*/
/*  SCCS:	<%Z% %M% %I%>					*/
/*  Edit:	20-Oct-05						*/
/*									*/
/************************************************************************/
/*									*/
/*  Main program and file output for the Draw file decompression	*/
/*  filter.  File input is handled by the 'drawcomp' library.		*/
/*									*/
/************************************************************************/
/*									*/
/*  Copyright (c) 2004 Jonathan Marten <jjm@keelhaul.demon.co.uk>	*/
/*  Home & download:  http://www.keelhaul.demon.co.uk/acorn/drawview/	*/
/*									*/
/*  This program is free software; you can redistribute it and/or	*/
/*  modify it under the terms of the GNU General Public License as	*/
/*  published by the Free Software Foundation; either version 2 of the	*/
/*  License, or (at your option) any later version.			*/
/*									*/
/*  It is distributed in the hope that it will be useful, but		*/
/*  WITHOUT ANY WARRANTY; without even the implied warranty of		*/
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	*/
/*  GNU General Public License for more details.			*/
/*									*/
/*  You should have received a copy of the GNU General Public		*/
/*  License along with this program; see the file COPYING for further	*/
/*  details.  If not, write to the Free Software Foundation, Inc.,	*/
/*  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.		*/
/*									*/
/************************************************************************/

#undef DEBUG						/* Generate debug/progress messages */

/************************************************************************/
/*  Include files							*/
/************************************************************************/

#include <stdio.h>					/* Standard I/O routines */
#include <stdlib.h>					/* Standard library routines */
#include <string.h>					/* String handling routines */
#include <unistd.h>					/* Unix(TM) system routines */

#include <sys/types.h>					/* System data types */
#include <sys/stat.h>					/* File information lookup */
#include <fcntl.h>					/* File descriptor and open modes */

#include "cmdutils.h"					/* Command line utilities */

#include "draw/drawtypes.h"				/* Extended Draw file data types */
#include "draw/drawcomp.h"				/* Draw file compression/depression */

/************************************************************************/
/*  hexdump -- Simple memory dump, for debugging.			*/
/************************************************************************/

void hexdump(const void *ptr,int offset,int length)
{
	int a;
	const unsigned char *p = ptr;
	p = &p[offset];

	for (a = offset-(offset % 16); a<(offset+length); ++a)
	{
		if ((a % 16)==0) fprintf(stderr,"\n%08X: ",a);
		else if ((a % 4)==0) fprintf(stderr," ");
		if (a<offset) fprintf(stderr," --");
		else fprintf(stderr," %02X",((unsigned int) *p));
		++p;
	}

	fprintf(stderr,"\n");
}

/************************************************************************/
/*  usage -- Display a help/usage message.				*/
/************************************************************************/

static void usage()
{
	fprintf(stderr,"Usage:    %s [-o outfile] infile\n",myname);
	fprintf(stderr,"for help: %s -h\n",myname);
}

/************************************************************************/
/*  Main -- Main program.						*/
/************************************************************************/

int main(int argc,char *argv[])
{
	int ch;						/* Option character */
	const char *infile = NULL;			/* That not specified yet */
	int outfd;					/* Output (drawing) file */
	const char *outfile = NULL;			/* That not specified yet */
	struct stat st;					/* File information */
	int ftype;					/* RiscOS file type */
	int cf;						/* Compression flags */
	int outsize;					/* Size of output data */
	const char *addr;				/* Output data address */
	int nwrite;					/* How much data written */

	cmdinit(argv[0]);				/* Find my program name */
	opterr = 0;					/* We'll report the problem */
	while ((ch = getopt(argc,argv,":ho:"))!=-1)
	{
		switch (ch)
		{
case 'o':		outfile = optarg;
			break;

case 'h':		usage();
			exit(EXIT_SUCCESS);
			break;

case ':':		cmderr(CSNONE,CEERROR,"Option '%c' requires a value",optopt);
			break;

case '?':
default:		cmderr(CSNONE,CEERROR,"Unknown option '%c'",optopt);
			break;
		}
	}

	infile = argv[optind++];
	if (infile==NULL) cmderr(CSNONE,CEERROR,"No file specified (use \"-\" for standard input)");
	if (argv[optind]!=NULL) cmderr(CSNONE,CEWARNING,"Additional arguments from \"%s\" ignored",argv[optind]);

	if (strcmp(infile,"-")==0) infile = "/dev/stdin";

	if (outfile==NULL || strcmp(outfile,"-")==0) outfd = 1;
	else
	{
		outfd = open(outfile,O_WRONLY|O_CREAT|O_TRUNC,0644);
		if (outfd<0) cmderr(CSERRNO,CEERROR,"Cannot write '%s'",outfile);
	}

	if (stat(infile,&st)<0) cmderr(CSERRNO,CEFATAL,"Cannot get '%s' file information",infile);
#ifdef DEBUG
	fprintf(stderr,"input size = %d\n",((int) st.st_size));
#endif
	if (st.st_size<=0) cmderr(CSERRNO,CEFATAL,"Cannot get file size of '%s' (or empty file)",infile);

	draw_diag dg;
	dg.data = NULL; dg.length = 0;
	ftype = drawcomp_readDrawFile(&dg,infile,FALSE,&cf,NULL);
#ifdef DEBUG
	fprintf(stderr,"decompressed ftype=%d cflags=%d at %p length %d\n",ftype,cf,dg.data,dg.length);
#endif
	addr = dg.data;
	outsize = dg.length;
	while (outsize>0)
	{
		nwrite = write(outfd,addr,outsize);
		if (nwrite<0) cmderr(CSERRNO,CEFATAL,"Cannot write %d to '%s'",outsize,outfile);

		outsize -= nwrite;
		addr += nwrite;
	}

	close(outfd);
	return (EXIT_SUCCESS);
}

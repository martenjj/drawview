/************************************************************************/
/*									*/
/*  Project:	DrawView - Decompression filter				*/
/*  Edit:	05-Dec-15						*/
/*									*/
/************************************************************************/
/*									*/
/*  Emulation routines for Draw file decompression.			*/
/*									*/
/*  These mostly do the same thing, or equivalent, as the same named	*/
/*  RiscOS library routines.  The useful ones do their equivalent	*/
/*  task, or at least the necessary subset of it, using standard Unix	*/
/*  system facilities.  Some others have no effect or should never be	*/
/*  called, and are just here to placate the linker.			*/
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

#undef DEBUG_COMPAT					/* Generate debug/progress messages */

/************************************************************************/
/*  Include files							*/
/************************************************************************/

#include <stdio.h>					/* Standard I/O routines */
#include <stdarg.h>					/* Variable parameter handling */
#include <string.h>					/* String handling routines */
#include <stdlib.h>					/* Standard library routines */
#include <errno.h>					/* Library error numbers */

#include <fcntl.h>					/* File descriptor and open modes */
#include <unistd.h>					/* Unix(TM) system routines */
#include <sys/stat.h>					/* File information lookup */

#include "jjm/program.h"				/* Useful macros and definitions */

#include "draw/drawtypes.h"				/* Extended Draw file data types */
#include "draw/drawobject.h"				/* Draw file object-level routines */
#include "draw/compress00.h"				/* Internal definitions */

#include "riscos/flex.h"				/* Flexible memory allocation */
#include "riscos/err.h"					/* Desktop error reporting */
#include "riscos/xferrecv.h"				/* Data transfer receiving */

#include "sys/visdelay.h"				/* Hourglass system calls */
#include "sys/io.h"					/* Low level I/O utilities */

/************************************************************************/
/*  Constants and macros						*/
/************************************************************************/

#define NFLEX		3				/* Allocate flex blocks */

/************************************************************************/
/*  Internal variables							*/
/************************************************************************/

static os_error errblk = { 0, "" };			/* Generated OS/file error */

static void *flexuser[NFLEX] = { NULL,NULL,NULL };	/* Flex user base pointer */
static int flexsize[NFLEX] = { 0,0,0 };			/* Flex area allocated size */

/************************************************************************/
/*  draw_setBBox -- Set the bounding box of a Draw object.		*/
/************************************************************************/

BOOL draw_setBBox(draw_diag *diag,draw_object object,BOOL recurse)
{
#ifdef DEBUG_COMPAT
	fprintf(stderr,"draw_setBBox: object%s at %p+%d\n",
		(recurse ? " and subobjects" : ""),diag->data,object);
#endif

	char *ptr = ((char *) diag->data)+object+(4+4);
	memset(ptr,0,4*4);
	return (TRUE);
}

/************************************************************************/
/*  drawcomp_fetch4 -- Fetch a 4-byte word (may not be aligned) from	*/
/*  the input buffer.						 	*/
/************************************************************************/

int drawcomp_fetch4(unsigned char *from,int o_in)
{
	int o_temp = o_in;
	int got = drawcomp_get4(from,&o_temp);
	return (got);
}

/************************************************************************/
/*  drawcomp_get4 -- Fetch a 4-byte word (may not be aligned) from	*/
/*  the input buffer, and update the offset accordingly.	 	*/
/************************************************************************/

int drawcomp_get4(unsigned char *from,int *o_in)
{
	int got = 0;
	int i;
	const unsigned char *ptr = ptroff(from,*o_in);

	for (i = 0; i<32; i += 8)
	{
		got += ((int) (*ptr))<<i;
		++ptr;
	}

	*o_in += 4;
	return (got);
}

/************************************************************************/
/*  err_generateLookup -- Generate an OS error block using the		*/
/*  specified message key and optional arguments.  In this application	*/
/*  the message always has an expanded text provided, so use that if	*/
/*  it is plausible.							*/
/************************************************************************/

os_error *err_generateLookup(const char *key,...)
{
	va_list ap;
	const char *msg = key;
	const char *p = strchr(msg,':');
	if (p!=NULL && p[1]!='\0' && (p-msg)<10) msg = p+1;

	va_start(ap,key);
	vsprintf(errblk.errmess,msg,ap);
	va_end(ap);
	return (&errblk);
}

/************************************************************************/
/*  err_lastError -- Return the last generated error.			*/
/************************************************************************/

os_error *err_lastError()
{
	return (&errblk);
}

/************************************************************************/
/*  flex_alloc -- Allocate a memory block.  Flex memory is emulated	*/
/*  for our purposes by simply using the malloc(3) heap.		*/
/************************************************************************/

BOOL flex_alloc(mem_ptr anchor,size_t n)
{
	int i;

#ifdef DEBUG_COMPAT
	fprintf(stderr,"flex_alloc: at %p size %d->%d\n",anchor,n,roundup(n));
#endif
	n = roundup(n);
	for (i = 0; i<NFLEX; ++i)
	{
		if (flexuser[i]==NULL)
		{
			*anchor = malloc(n);
			if (*anchor==NULL)
			{
#ifdef DEBUG_COMPAT
				fprintf(stderr,"flex_alloc: cannot allocate %d\n",n);
#endif
				return (FALSE);
			}

			flexsize[i] = n;
			flexuser[i] = anchor;
#ifdef DEBUG_COMPAT
			fprintf(stderr,"            slot %d allocated %p\n",i,*anchor);
#endif
			return (TRUE);
		}
	}

#ifdef DEBUG_COMPAT
	fprintf(stderr,"flex_alloc: no free records\n");
#endif
	*anchor = NULL;
	return (FALSE);
}

/************************************************************************/
/*  flex_extend -- Grow or shrink an allocated memory block.		*/
/************************************************************************/

BOOL flex_extend(mem_ptr anchor,size_t newsize)
{
	int i;

	if (*anchor==NULL) return (flex_alloc(anchor,newsize));

#ifdef DEBUG_COMPAT
	fprintf(stderr,"flex_extend: at %p add %d->%d\n",anchor,newsize,roundup(newsize));
#endif
	newsize = roundup(newsize);
	for (i = 0; i<NFLEX; ++i)
	{
		if (flexuser[i]==anchor)
		{
			*anchor = realloc(*anchor,newsize);
			if (*anchor==NULL)
			{
#ifdef DEBUG_COMPAT
				fprintf(stderr,"flex_realloc: cannot reallocate %d\n",newsize);
#endif
				return (FALSE);
			}
#ifdef DEBUG_COMPAT
			fprintf(stderr,"             slot %d was %d now allocated %p\n",i,flexsize[i],*anchor);
#endif
			flexsize[i] = newsize;
			return (TRUE);
		}
	}

#ifdef DEBUG_COMPAT
	fprintf(stderr,"flex_free: at %p not allocated\n",anchor);
#endif
	return (FALSE);
}

/************************************************************************/
/*  flex_free -- Free an allocated memory block.			*/
/************************************************************************/

void flex_free(mem_ptr anchor)
{
	int i;

	for (i = 0; i<NFLEX; ++i)
	{
		if (flexuser[i]==anchor)
		{
#ifdef DEBUG_COMPAT
			fprintf(stderr,"flex_free: at %p slot %d was allocated %p size %d\n",*anchor,i,*anchor,flexsize[i]);
#endif
			free(*anchor);
			*anchor = NULL;
			flexuser[i] = NULL;
			return;
		}
	}

#ifdef DEBUG_COMPAT
	fprintf(stderr,"flex_free: at %p not allocated\n",anchor);
#endif
}

/************************************************************************/
/*  flex_midextend -- Extend an allocated memory block by opening up	*/
/*  a gap in the middle, moving data appropriately.			*/
/************************************************************************/

BOOL flex_midextend(mem_ptr anchor,size_t at,int by)
{
	int i;

#ifdef DEBUG_COMPAT
	fprintf(stderr,"flex_midextend: at %p by %d add %d\n",anchor,at,by);
#endif
	for (i = 0; i<NFLEX; ++i)
	{
		if (flexuser[i]==anchor)
		{
			int oldsize = flexsize[i];
			flex_extend(anchor,flexsize[i]+roundup(by));
			memmove(ptroff(*anchor,at+by),ptroff(*anchor,at),oldsize-at);
			return (TRUE);
		}
	}

#ifdef DEBUG_COMPAT
	fprintf(stderr,"flex_midextend: at %p not allocated\n",anchor);
#endif
	return (FALSE);
}

/************************************************************************/
/*  flex_repoint -- Move the user block pointer.			*/
/************************************************************************/

void flex_repoint(mem_ptr old,mem_ptr new)
{
	int i;

#ifdef DEBUG_COMPAT
	fprintf(stderr,"flex_repoint: at %p to %p\n",old,new);
#endif
	for (i = 0; i<NFLEX; ++i)
	{
		if (flexuser[i]==old)
		{
			flexuser[i] = new;
			*new = *old;
			*old = NULL;
#ifdef DEBUG_COMPAT
			fprintf(stderr,"              slot %d\n",i);
#endif
			return;
		}
	}

#ifdef DEBUG_COMPAT
	fprintf(stderr,"flex_repoint: at %p not allocated\n",old);
#endif
}

/************************************************************************/
/*  io_generr -- Generate an error where a filesystem object found	*/
/*  is not of the expected type.					*/
/************************************************************************/

os_error *io_generr(const char *path,int expect,int got)
{
	sprintf(errblk.errmess,"Wrong file type for '%s': expected %d is %d",path,expect,got);
	return (&errblk);
}

/************************************************************************/
/*  io_load -- Read the contents of a file into a previously allocated	*/
/*  memory buffer.							*/
/************************************************************************/

os_error *io_load(const char *path,void *addr)
{
	struct stat st;
	int fd;
	int nread;
	int size;

	fd = open(path,O_RDONLY);
	if (fd<0)
	{
		sprintf(errblk.errmess,"Cannot read '%s', %s",path,strerror(errno));
		return (&errblk);
	}

	if (fstat(fd,&st)<0)
	{
		sprintf(errblk.errmess,"Cannot get information for '%s', %s",path,strerror(errno));
		return (&errblk);
	}

	size = st.st_size;
	while (size>0)
	{
		nread = read(fd,addr,size);
		if (nread<0)
		{
			sprintf(errblk.errmess,"Cannot read %d from '%s', %s",size,path,strerror(errno));
			close(fd);
			return (&errblk);
		}

		size -= nread;
		addr = ptroff(addr,nread);
	}

	close(fd);
	return (NULL);
}

/************************************************************************/
/*  io_stat -- Look up size and other information for a file.		*/
/************************************************************************/

os_error *io_stat(const char *path,struct iostat *iost)
{
	struct stat st;

	if (stat(path,&st)<0)
	{
		sprintf(errblk.errmess,"Cannot get information for '%s', %s",path,strerror(errno));
		return (&errblk);
	}

	switch (st.st_mode & S_IFMT)
	{
case S_IFREG:	iost->obj = 1; break;			/* file */
case S_IFDIR:	iost->obj = 2; break;			/* directory */
default:	iost->obj = 4; break;			/* other */
	}

	iost->name = (char *) path;
	iost->load = iost->exec = 0;
	iost->size = st.st_size;
	iost->attr = 0;
	iost->type = 0;
	return (NULL);
}

/************************************************************************/
/*  visdelay_percent -- Show progress in busy indicator.		*/
/************************************************************************/

void visdelay_percent(int pct)
{
#ifdef DEBUG_COMPAT
	fprintf(stderr,"visdelay_percent: %d%%\n",pct);
#endif
}

/************************************************************************/
/*  xferrecv_checkimport -- Set up acknowledgement to DATASAVE message	*/
/*  as ready to initiate RAM transfer.					*/
/************************************************************************/

int xferrecv_checkimport(int *estsize)
{
#ifdef DEBUG_COMPAT
	fprintf(stderr,"xferrecv_checkimport: should never be called\n");
#endif
	return (-1);
}

/************************************************************************/
/*  xferrecv_fleximport -- Do RAM transfer into a previously allocated	*/
/*  flex memory block, extending it as required.			*/
/************************************************************************/

int xferrecv_fleximport(mem_ptr anchor,int size)
{
#ifdef DEBUG_COMPAT
	fprintf(stderr,"xferrecv_fleximport: should never be called\n");
#endif
	return (-1);
}

/************************************************************************/
/*  xferrecv_xfersize -- Read total size transferred during the	last	*/
/*  RAM transfer exchange.						*/
/************************************************************************/

int xferrecv_xfersize(void)
{
#ifdef DEBUG_COMPAT
	fprintf(stderr,"xferrecv_xfersize: should never be called\n");
#endif
	return (0);
}

/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		io.h			Edit:  19-Oct-05	*/
/*	SCCS:		<@(#) io.h 1.5>					*/
/*	Language:	ANSI C						*/
/*	Project:	Additions to RiscOS Library			*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	This is the header file for the low level I/O routines.		*/
/*									*/
/************************************************************************/
/*									*/
/*	Written by Jonathan Marten initially as part of Draw 1½ and	*/
/*	then DrawPlus, later evolving into Vector and related		*/
/*	products for 4Mation Educational Resources.			*/
/*									*/
/*	Copyright © Jonathan Marten 1992-1999				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/

#ifndef __io_h
#define __io_h

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
/*	Structures and types						*/
/*									*/
/************************************************************************/

#ifdef RISCOS
struct stat						/* File information */
#else
struct iostat						/* File information */
#endif
{
	int obj;					/* Object type */
	char *name;					/* Object pathname */
	unsigned int load,exec;				/* Load, execute addresses */
	unsigned int size;				/* Object size */
	unsigned int attr;				/* Access attributes */
	unsigned int type;				/* File type */
};

/************************************************************************/
/*									*/
/*	File open modes							*/
/*									*/
/************************************************************************/

#define io_READ		0x40
#define io_WRITE	0x80
#define io_UPDATE	0xC0

/************************************************************************/
/*									*/
/*	External functions						*/
/*									*/
/************************************************************************/

extern int io_ARGSextent(int file);
extern os_error *io_GBPBread(const void *data,int len,int file);
extern os_error *io_GBPBwrite(void *data,int len,int file);

extern os_error *io_checksum(const char *name,unsigned int *sum);

extern int io_decodeType(int);
extern char *io_formatType(int);
extern char *io_formatInfo(int);
extern void io_decodeTime(unsigned int,unsigned int,unsigned char *);
extern int io_encodeType(const char *);

extern void io_flushArrows(void);

extern char *io_canonicalise(const char *path);
extern char *io_readCurrent(void);
extern os_error *io_setCurrent(const char *dir);

#ifdef RISCOS
extern os_error *io_stat(const char *path,struct stat *st);
#else
extern os_error *io_stat(const char *path,struct iostat *st);
#endif

extern os_error *io_save1(const char *path,const void *start,unsigned int length,unsigned int type);
extern os_error *io_save2(const char *path,const void *start,const void *end,unsigned int type);
extern os_error *io_load(const char *path,void *addr);
extern os_error *io_settype(const char *path,unsigned int type);
extern os_error *io_mkdir(const char *path);

extern int io_open(int mode,const char *path);
extern void io_close(int);

extern os_error *io_generr(const char *path,int expect,int got);

#endif

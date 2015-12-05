/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		xferrecv.h		Edit:  16-Mar-05	*/
/*	SCCS:		<@(#) xferrecv.h 1.4>				*/
/*	Language:	ANSI C						*/
/*	Project:	Rewritten RiscOS Library			*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	This is the header file for the data load & import facility.	*/
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

#ifndef __xferrecv_h
#define __xferrecv_h

/************************************************************************/
/*									*/
/*	Include files							*/
/*									*/
/************************************************************************/

#ifndef __wimp_h
#include "sys/wimp.h"					/* Window manager system calls */
#endif
#ifndef __flex_h
#include "riscos/flex.h"				/* Flexible memory allocation */
#endif

/************************************************************************/
/*									*/
/*	Special file type codes						*/
/*									*/
/************************************************************************/

#define xferrecv_typeDIR	0x00001000
#define xferrecv_typeAPP	0x00002000
#define xferrecv_typeUNTYPED	0xFFFFFFFF

/************************************************************************/
/*									*/
/*	Pointer value passed to file loading functions to indicate	*/
/*	that they should load via RAM transfer.				*/
/*									*/
/************************************************************************/

#define xferrecv_Recv	((char *) 1)			/* Load via RAM transfer */

/************************************************************************/
/*									*/
/*	XFERRECV_BUFFERPROC - Function type for the client-supplied	*/
/*	function to empty a full buffer during RAM transfer.		*/
/*									*/
/*	Parameters:	buffer		Buffer pointer			*/
/*			size		Current size of buffer		*/
/*									*/
/*	Returns:	TRUE if buffer emptied OK, FALSE if unable	*/
/*			to empty buffer or create a new one.		*/
/*									*/
/*	Other Info:	This is the function (supplied by the client),	*/
/*			which will be called when the transfer buffer	*/
/*			is full.  It should empty the current buffer,	*/
/*			or create more space and modify the address	*/
/*			and size accordingly, and return TRUE.  If	*/
/*			the buffer cannot be emptied or extended, it	*/
/*			should return FALSE and the transfer will be	*/
/*			aborted.					*/
/*									*/
/************************************************************************/

typedef BOOL (*xferrecv_bufferProc)(void **buffer,int *size);

/************************************************************************/
/*									*/
/*	XFERRECV_CHECKINSERT - Set up the acknowledge message for a	*/
/*	DATAOPEN or DATALOAD message, and get the file name from	*/
/*	which to load.							*/
/*									*/
/*	Parameters:	filename	File name to load		*/
/*									*/
/*	Returns:	The file's type, or -1 if not load message	*/
/*									*/
/*	Other Info:	This function checks to see if the last Wimp	*/
/*			event was a request to import a file.  If so	*/
/*			it returns the file type and fills in the	*/
/*			pointer to the file name.  If the event was	*/
/*			not a file import, it returns -1.		*/
/*									*/
/************************************************************************/

extern int xferrecv_checkinsert(char **filename);

/************************************************************************/
/*									*/
/*	XFERRECV_INSERTFILEOK - Deletes scrap file (if it was used	*/
/*	for transfer) and sends acknowledgement of DATALOAD message.	*/
/*									*/
/*	Other Info:	This should be called when the import is	*/
/*			complete and the file has been loaded.		*/
/*									*/
/************************************************************************/

extern void xferrecv_insertfileok(void);

/************************************************************************/
/*									*/
/*	XFERRECV_INSERTFILEOK1 - As above, but with the option of not	*/
/*	deleting the scrap file.					*/
/*									*/
/*	Parameters:	keep		TRUE => retain scrap file,	*/
/*					FALSE => delete it		*/
/*									*/
/*	Other Info:	This can be used where there has to be some	*/
/*			Wimp interaction, e.g. a dialogue box, before	*/
/*			the file is eventually loaded.  The client	*/
/*			must delete the scrap file (using the next	*/
/*			routine) when it has finished with it.		*/
/*									*/
/************************************************************************/

extern void xferrecv_insertfileok1(BOOL keep);

/************************************************************************/
/*									*/
/*	XFERRECV_DELETESCRAP - Delete the scrap file from the last	*/
/*	transfer.							*/
/*									*/
/************************************************************************/

extern void xferrecv_deletescrap(void);

/************************************************************************/
/*									*/
/*	XFERRECV_CHECKIMPORT - Set up acknowledgement message for a	*/
/*	DATASAVE message, ready to initiate RAM transfer if the		*/
/*	sender can also do it.						*/
/*									*/
/*	Parameters:	estsize		Sender's estimated file size	*/
/*									*/
/*	Returns:	File type of imported file			*/
/*									*/
/*	Other Info:	The estimated size sent by the sending task	*/
/*			is filled in on return.  If the client cannot	*/
/*			do RAM transfer, it should instead call		*/
/*			xferrecv_scraptransfer() to use a scrap file.	*/
/*									*/
/************************************************************************/

extern int xferrecv_checkimport(int *estsize);

/************************************************************************/
/*									*/
/*	XFERRECV_CHECKPRINT - Set up the acknowledge for a		*/
/*	PRINTTYPEODD message and get the file name to print.		*/
/*									*/
/*	Parameters:	filename	File name to print		*/
/*									*/
/*	Returns:	The file's type, or -1 if not print message	*/
/*									*/
/*	Other Info:	This function checks to see if the last Wimp	*/
/*			event was a request to print a file.  If so	*/
/*			it returns the file type and fills in the	*/
/*			pointer to the file name.  If the event was	*/
/*			not a print request, it returns -1.  The	*/
/*			application can either print the file		*/
/*			directly, or convert it to <Printer$Temp> for	*/
/*			spooling by the printer manager.		*/
/*									*/
/************************************************************************/

extern int xferrecv_checkprint(char **filename);

/************************************************************************/
/*									*/
/*	XFERRECV_PRINTFILEOK - Send an acknowledgement back to the	*/
/*	printer manager, saying that the file has either been printed	*/
/*	or converted to a spool file.  If the file was spooled then	*/
/*	this also informs the printer manager of its file type.		*/
/*									*/
/*	Parameters:	type		Type of file spooled		*/
/*									*/
/************************************************************************/

extern void xferrecv_printfileok(int type);

/************************************************************************/
/*									*/
/*	XFERRECV_DOIMPORT - Receives RAM transfer data into a buffer,	*/
/*	and calls the client-supplied function to empty the buffer	*/
/*	when it is full.						*/
/*									*/
/*	Parameters:	buf		Initial transfer buffer		*/
/*			size		Buffer's current size		*/
/*			proc		Function to be called when	*/
/*					  the buffer is full		*/
/*									*/
/*	Returns:	Number of bytes transferred on successful	*/
/*			completion, or -1 for error.			*/
/*									*/
/************************************************************************/

extern int xferrecv_doimport(void *buf,int size,xferrecv_bufferProc proc);

/************************************************************************/
/*									*/
/*	XFERRECV_FILE_IS_SAFE - Informs caller if file was received	*/
/*	from a "safe" source (i.e. a permanent file on disc).		*/
/*									*/
/*	Returns:	TRUE if file is safe				*/
/*									*/
/*	Other Info:	"Safe" in this context means that the		*/
/*			supplied filename will not change in the	*/
/*			foreseeable future.				*/
/*									*/
/************************************************************************/

extern BOOL xferrecv_file_is_safe(void);

/************************************************************************/
/*									*/
/*	XFERRECV_SCRAPTRANSFER - Turns around a DATASAVE message to	*/
/*	tell the sender to use the Wimp scrap file for a transfer.	*/
/*									*/
/*	Parameters:	e		Event data of DATASAVE message	*/
/*									*/
/************************************************************************/

extern void xferrecv_scraptransfer(wimp_eventstr *e);

/************************************************************************/
/*									*/
/*	XFERRECV_XFERSIZE - Reads the total size of data transferred	*/
/*	during the last RAM transfer exchange.				*/
/*									*/
/*	Returns:       Total data size in bytes				*/
/*									*/
/************************************************************************/

extern int xferrecv_xfersize(void);

/************************************************************************/
/*									*/
/*	XFERRECV_FLEXIMPORT - Performs a RAM transfer into a flex	*/
/*	block, automatically extending the block when necessary.  The	*/
/*	caller must have already done the initial flex allocation.	*/
/*									*/
/*	Parameters:	anchor		Anchor of flex block		*/
/*			size		Allocated size of block		*/
/*									*/
/*	Returns:	Size of last block, or -1 for failure		*/
/*									*/
/*	Other Info:	It is possible that the block could be moved.	*/
/*									*/
/************************************************************************/

extern int xferrecv_fleximport(mem_ptr anchor,int size);

/************************************************************************/
/*									*/
/*	XFERRECV_LEAFNAME - Read the leaf name that was suggested by	*/
/*	the sender in the last DATASAVE message.			*/
/*									*/
/*	Returns:	Leaf name, or NULL if no name has been seen	*/
/*			or none was supplied.				*/
/*									*/
/************************************************************************/

extern const char *xferrecv_leafname(void);

#endif

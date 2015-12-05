/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		drawobject.h		Edit:  22-Mar-05	*/
/*	SCCS:		<@(#) drawobject.h 1.4>				*/
/*	Language:	ANSI C						*/
/*	Project:	Draw file utility library			*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	This is the header file for the object-level routines.		*/
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
/*	Copyright © Jonathan Marten 1992-1994				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/

#ifndef __drawfobj_h
#define __drawfobj_h

/************************************************************************/
/*									*/
/*	Include files							*/
/*									*/
/************************************************************************/

#ifndef __drawtypes_h
#include "draw/drawtypes.h"				/* Extended Draw file data types */
#endif

/************************************************************************/
/*									*/
/*	Global variables						*/
/*									*/
/************************************************************************/

extern int *draw_transTable;				/* For translating fonts, etc. */

/************************************************************************/
/*									*/
/*	DRAW_VERIFYOBJECT - Verify the data for an existing object	*/
/*	in a diagram.							*/
/*									*/
/*	Parameters:	diag	Diagram containing object		*/
/*			object	Object to be verified			*/
/*			size	Pointer to object size for return	*/
/*									*/
/*	Returns:	Standard status					*/
/*									*/
/************************************************************************/

extern BOOL draw_verifyObject(draw_diag *diag,draw_object object,int *size);

/************************************************************************/
/*									*/
/*	DRAW_SETBBOX and DRAW_SETBBOX1 - Set the bounding box for	*/
/*	an existing object in a diagram.				*/
/*									*/
/*	Parameters:	diag	Diagram containing object		*/
/*			object	Object handle to set bounding box	*/
/*			op	Object pointer to set bounding box	*/
/*			recurse	TRUE => set for all included objects	*/
/*									*/
/*	Returns:	Standard status					*/
/*									*/
/************************************************************************/

extern BOOL draw_setBBox(draw_diag *diag,draw_object object,BOOL recurse);
extern BOOL draw_setBBox1(draw_objectType op,BOOL recurse);

/************************************************************************/
/*									*/
/*	DRAW_CREATEOBJECT and DRAW_CREATEAT - Create an object at a	*/
/*	specified position in a diagram.				*/
/*									*/
/*	Parameters:	diag	Diagram containing object		*/
/*			new	Object data to be added			*/
/*			after	Object after which to create new one	*/
/*			at	Object at which to create new one	*/
/*			rebind	TRUE => update diagram bounding box	*/
/*			object	Pointer to return handle of new object	*/
/*									*/
/*	Returns:	Standard status					*/
/*									*/
/*	Other Info:	All data after the insertion point is moved	*/
/*			down.  'after' or 'at' may be set to a special	*/
/*			value (draw_objectFIRST or draw_objectLAST)	*/
/*			to create at the start/end of the diagram.	*/
/*			The diagram must be large enough to accomodate	*/
/*			the new data;  its length field is updated.	*/
/*			The handle of the new object is returned.	*/
/*									*/
/************************************************************************/
 
extern BOOL draw_createObject(draw_diag *diag,draw_objectType new,
                              draw_object after,BOOL rebind,draw_object *object);
extern BOOL draw_createAt(draw_diag *diag,draw_objectType new,
                          draw_object at,BOOL rebind,draw_object *object);

/************************************************************************/
/*									*/
/*	DRAW_OPENOBJECT and DRAW_OPENAT - Create space for an object	*/
/*	at a specified position in a diagram.				*/
/*									*/
/*	Parameters:	diag	Diagram containing object		*/
/*			size	Size of the new object			*/
/*			after	Object after which to open new object	*/
/*			after	Object at which to open new object	*/
/*			object	Pointer to return handle of new object	*/
/*									*/
/*	Returns:	Standard status					*/
/*									*/
/************************************************************************/
 
extern BOOL draw_openObject(draw_diag *diag,draw_sizetyp size,
                            draw_object after,draw_object *object);
extern BOOL draw_openAt(draw_diag *diag,draw_sizetyp size,
                        draw_object at,draw_object *object);

/************************************************************************/
/*									*/
/*	DRAW_DELETEOBJECTS - Deletes a specified range of objects	*/
/*	from a diagram.							*/
/*									*/
/*	Parameters:	diag	Diagram containing objects		*/
/*			start	First object to be deleted		*/
/*			end	Last object to be deleted		*/
/*			rebind	TRUE => update diagram bounding box	*/
/*									*/
/*	Returns:	Standard status					*/
/*									*/
/************************************************************************/

extern BOOL draw_deleteObjects(draw_diag *diag,draw_object start,draw_object end,BOOL rebind);

/************************************************************************/
/*									*/
/*	DRAW_EXTRACTOBJECT - Extracts an object from a diagram into	*/
/*	a supplied buffer.						*/
/*									*/
/*	Parameters:	diag	Diagram containing objects		*/
/*			object	Object to be extracted			*/
/*			buf	Buffer to contain extracted object	*/
/*									*/
/*	Returns:	Standard status					*/
/*									*/
/************************************************************************/

extern BOOL draw_extractObject(draw_diag *diag,draw_object object,draw_objectType buf);

/************************************************************************/
/*									*/
/*	DRAW_OBJECTINIT - Initialise the object-level interface.	*/
/*									*/
/*	Returns:	Standard status					*/
/*									*/
/************************************************************************/

extern BOOL draw_objectInit(void);

#endif

/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		wimp.h			Edit:  12-Mar-05	*/
/*	SCCS:		<@(#) wimp.h 1.4>				*/
/*	Language:	ANSI C						*/
/*	Project:	Rewritten RiscOS Library (system)		*/
/*									*/
/*	This is the header file for the C interface to Wimp routines.	*/
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
/*	Copyright © Jonathan Marten 1992-1998				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/

#ifndef __wimp_h
#define __wimp_h

/************************************************************************/
/*									*/
/*	Include files							*/
/*									*/
/************************************************************************/

#ifndef __ostypes_h
#include "sys/ostypes.h"				/* Operating system data types */
#endif
#ifndef __sprite_h
#include "sys/sprite.h"					/* Sprite handling system calls */
#endif

/************************************************************************/
/*									*/
/*	General macros							*/
/*									*/
/************************************************************************/

#define wimp_NULL		((void *) -1)		/* Null pointer for OS calls */

/************************************************************************/
/*									*/
/*	Enumerations for flag bits, event types etc.			*/
/*									*/
/************************************************************************/

#ifdef CREL5

typedef int wimp_windowflags;				/* Window flags */

#define wimp_winMOVEABLE	0x00000002		/* Is moveable */
#define wimp_winAUTOREDRAW	0x00000010		/* Can be redrawn entirely by Wimp */
#define wimp_winPANE		0x00000020		/* Stuck over tool window */
#define wimp_winNOBOUNDS	0x00000040		/* Allowed to go outside main area */
#define wimp_winSCROLL1		0x00000100		/* Auto-repeat scroll request */
#define wimp_winSCROLL2		0x00000200		/* Debounced scroll request */
#define wimp_winREALCOLOURS	0x00000400		/* Use real window colours */
#define wimp_winBACKGROUND	0x00000800		/* This window is a background window */
#define wimp_winHOTKEYS		0x00001000		/* Generate events for 'hot keys' */

#define wimp_winRESTRICT	0x00002000		/* Always keep window on-screen */
#define wimp_winFREERIGHT	0x00004000		/* Ignore right extent when resizing */
#define wimp_winFREEBOTTOM	0x00008000		/* Ignore bottom extent when resizing */

#define wimp_winISOPEN		0x00010000		/* Current status: open */
#define wimp_winISTOP		0x00020000		/*                 on top (not covered) */
#define wimp_winISFULL		0x00040000		/*                 full size */
#define wimp_winISTOGGLE	0x00080000		/*                 toggle-size used */
#define wimp_winFOCUS		0x00100000		/*                 has input focus */ 
#define wimp_winSCREENSNAP	0x00200000		/*                 on-screen at next open */

#define wimp_winBACK		0x01000000		/* Controls:       back */
#define wimp_winQUIT		0x02000000		/*                 quit */
#define wimp_winTITLE		0x04000000		/*                 title bar */
#define wimp_winTOGGLE		0x08000000		/*                 toggle-size */
#define wimp_winVSCROLL		0x10000000		/*                 vertical scroll bar */
#define wimp_winRESIZE		0x20000000		/*                 resize */
#define wimp_winHSCROLL		0x40000000		/*                 horizontal scroll bar */
#define wimp_winNEW		~0x7FFFFFFF		/* Use these new flags */

#else
typedef enum						/* Window flags */
{
	wimp_winMOVEABLE	= 0x00000002,		/* Is moveable */
	wimp_winAUTOREDRAW	= 0x00000010,		/* Can be redrawn entirely by Wimp */
	wimp_winPANE    	= 0x00000020,		/* Stuck over tool window */
	wimp_winNOBOUNDS	= 0x00000040,		/* Allowed to go outside main area */
	wimp_winSCROLL1		= 0x00000100,		/* Auto-repeat scroll request */
	wimp_winSCROLL2		= 0x00000200,		/* Debounced scroll request */
	wimp_winREALCOLOURS	= 0x00000400,		/* Use real window colours */
	wimp_winBACKGROUND	= 0x00000800,		/* This window is a background window */
	wimp_winHOTKEYS		= 0x00001000,		/* Generate events for 'hot keys' */

	wimp_winRESTRICT	= 0x00002000,		/* Always keep window on-screen */
	wimp_winFREERIGHT	= 0x00004000,		/* Ignore right extent when resizing */
	wimp_winFREEBOTTOM	= 0x00008000,		/* Ignore bottom extent when resizing */

	wimp_winISOPEN 		= 0x00010000,		/* Current status: open */
	wimp_winISTOP  		= 0x00020000,		/*                 on top (not covered) */
	wimp_winISFULL 		= 0x00040000,		/*                 full size */
	wimp_winISTOGGLE	= 0x00080000,		/*                 toggle-size used */
	wimp_winFOCUS		= 0x00100000,		/*                 has input focus */ 
	wimp_winSCREENSNAP	= 0x00200000,		/*                 on-screen at next open */

	wimp_winBACK 		= 0x01000000,		/* Controls:       back */
	wimp_winQUIT 		= 0x02000000,		/*                 quit */
	wimp_winTITLE		= 0x04000000,		/*                 title bar */
	wimp_winTOGGLE		= 0x08000000,		/*                 toggle-size */
	wimp_winVSCROLL 	= 0x10000000,		/*                 vertical scroll bar */
	wimp_winRESIZE 		= 0x20000000,		/*                 resize */
	wimp_winHSCROLL 	= 0x40000000,		/*                 horizontal scroll bar */
	wimp_winNEW		= ~0x7FFFFFFF		/* Use these new flags */
} wimp_windowflags;
#endif

#ifdef CREL5

typedef int wimp_windowcolours;				/* Indexes into window colour array */

#define wimp_colTITLEFORE	0			/* Title bar */
#define wimp_colTITLEBACK	1
#define wimp_colWORKFORE	2			/* Work area */
#define wimp_colWORKBACK	3
#define wimp_colSCROLLOUTER	4			/* Scroll bar */
#define wimp_colSCROLLINNER	5
#define wimp_colFOCUS		6			/* Title bar when has input focus */

#else
typedef enum						/* Indexes into window colour array */
{
	wimp_colTITLEFORE, wimp_colTITLEBACK,		/* Title bar */
	wimp_colWORKFORE, wimp_colWORKBACK,		/* Work area */
	wimp_colSCROLLOUTER, wimp_colSCROLLINNER,	/* Scroll bar */
	wimp_colFOCUS,					/* Title bar when has input focus */
	wimp_colRESERVED				/* Reserved */
} wimp_windowcolours;
#endif

#ifdef CREL5

typedef int wimp_iconflags;				/* Icon flags */

#define wimp_iconTEXT		0x00000001		/* Contents: text */
#define wimp_iconSPRITE		0x00000002		/*           sprite */
#define wimp_iconBORDER		0x00000004		/*           border */
#define wimp_iconHCENTRE	0x00000008		/*           horizontally centred */
#define wimp_iconVCENTRE	0x00000010		/*           vertically centred */
#define wimp_iconFILLED		0x00000020		/*           filled background */
#define wimp_iconFONT		0x00000040		/*           anti-aliased font */
#define wimp_iconREDRAW		0x00000080		/*           complex redraw */
#define wimp_iconINDIRECT	0x00000100		/*           indirected */
#define wimp_iconRIGHT		0x00000200		/*           right justified */

#define wimp_iconADJUST		0x00000400		/* No ESG cancel with Adjust */
#define wimp_iconHALFSPRITE	0x00000800		/* Plot sprites half-size */

#define wimp_iconBUTTON		0x00001000		/* 4-bit field: button type */
#define wimp_iconESG		0x00010000		/* 5-bit field: selection group */

#define wimp_iconSELECTED	0x00200000		/* Status:   selected by user (inverted) */
#define wimp_iconSHADED		0x00400000		/*           cannot be selected (shaded) */
#define wimp_iconDELETED	0x00800000		/*           has been deleted */

#define wimp_iconFORECOL	0x01000000		/* 4-bit field: foreground colour */
#define wimp_iconBACKCOL	0x10000000		/* 4-bit field: background colour */

#define wimp_iconFONTHANDLE	0x01000000		/* 8-bit field: font handle */

#else
typedef enum						/* Icon flags */
{
	wimp_iconTEXT		= 0x00000001,		/* Contents: text */
	wimp_iconSPRITE		= 0x00000002,		/*           sprite */
	wimp_iconBORDER		= 0x00000004,		/*           border */
	wimp_iconHCENTRE	= 0x00000008,		/*           horizontally centred */
	wimp_iconVCENTRE	= 0x00000010,		/*           vertically centred */
	wimp_iconFILLED		= 0x00000020,		/*           filled background */
	wimp_iconFONT		= 0x00000040,		/*           anti-aliased font */
	wimp_iconREDRAW		= 0x00000080,		/*           complex redraw */
	wimp_iconINDIRECT	= 0x00000100,		/*           indirected */
	wimp_iconRIGHT		= 0x00000200,		/*           right justified */

	wimp_iconADJUST		= 0x00000400,		/* No ESG cancel with Adjust */
	wimp_iconHALFSPRITE	= 0x00000800,		/* Plot sprites half-size */

	wimp_iconBUTTON		= 0x00001000,		/* 4-bit field: button type */
	wimp_iconESG		= 0x00010000,		/* 5-bit field: selection group */

	wimp_iconSELECTED	= 0x00200000,		/* Status:   selected by user (inverted) */
	wimp_iconSHADED		= 0x00400000,		/*           cannot be selected (shaded) */
	wimp_iconDELETED	= 0x00800000,		/*           has been deleted */

	wimp_iconFORECOL	= 0x01000000,		/* 4-bit field: foreground colour */
	wimp_iconBACKCOL	= 0x10000000,		/* 4-bit field: background colour */

	wimp_iconFONTHANDLE	= 0x01000000		/* 8-bit field: font handle */
} wimp_iconflags;
#endif

#ifdef CREl5

typedef int wimp_buttontype;				/* Icon button types */

#define wimp_typeIGNORE			0
#define wimp_typeNOTIFY			1
#define wimp_typeCLICKAUTO		2
#define wimp_typeCLICKDEBOUNCE		3
#define wimp_typeSELREL			4
#define wimp_typeSELDOUBLE		5
#define wimp_typeDEBOUNCEDRAG		6
#define wimp_typeRELEASEDRAG		7
#define wimp_typeDOUBLEDRAG		8
#define wimp_typeMENU			9
#define wimp_typeCLICKDRAGDOUBLE	10
#define wimp_typeCLICKSEL		11
#define wimp_typeWRITECLICKDRAG		14
#define wimp_typeWRITEABLE		15

#else
typedef enum						/* Icon button types */
{
	wimp_typeIGNORE,
	wimp_typeNOTIFY,
	wimp_typeCLICKAUTO,
	wimp_typeCLICKDEBOUNCE,
	wimp_typeSELREL,
	wimp_typeSELDOUBLE,
	wimp_typeDEBOUNCEDRAG,
	wimp_typeRELEASEDRAG,
	wimp_typeDOUBLEDRAG,
	wimp_typeMENU,
	wimp_typeCLICKDRAGDOUBLE,
	wimp_typeCLICKSEL,
	wimp_typeRESERVED1,
	wimp_typeRESERVED2,
	wimp_typeWRITECLICKDRAG,
	wimp_typeWRITEABLE
} wimp_buttontype;
#endif

#ifdef CREL5

typedef int wimp_buttonstate;				/* Button state bits */

#define wimp_butADJUST		0x001
#define wimp_butMENU		0x002
#define wimp_butSELECT		0x004
#define wimp_butDRAGADJ		0x010
#define wimp_butDRAGSEL		0x040
#define wimp_butCLICKADJ	0x100
#define wimp_butCLICKSEL	0x400

#else
typedef enum						/* Button state bits */
{
	wimp_butADJUST		= 0x001,
	wimp_butMENU		= 0x002,
	wimp_butSELECT		= 0x004,
	wimp_butDRAGADJ		= 0x010,
	wimp_butDRAGSEL		= 0x040,
	wimp_butCLICKADJ	= 0x100,
	wimp_butCLICKSEL	= 0x400
} wimp_buttonstate;
#endif

#ifdef CREL5

typedef int wimp_pointericon;				/* Special pointer icons */

#define wimp_pointWORKAREA	-1
#define wimp_pointBACK		-2
#define wimp_pointCLOSE		-3
#define wimp_pointTITLE		-4
#define wimp_pointTOGGLE	-5
#define wimp_pointUPSCROLL	-6
#define wimp_pointVSCROLL	-7
#define wimp_pointDOWNSCROLL	-8
#define wimp_pointRESIZE	-9
#define wimp_pointLEFTSCROLL	-10
#define wimp_pointHSCROLL	-11
#define wimp_pointRIGHTSCROLL	-12
#define wimp_pointBORDER	-13

#else
typedef enum						/* Special pointer icons */
{
	wimp_pointWORKAREA	= -1,
	wimp_pointBACK		= -2,
	wimp_pointCLOSE		= -3,
	wimp_pointTITLE		= -4,
	wimp_pointTOGGLE	= -5,
	wimp_pointUPSCROLL	= -6,
	wimp_pointVSCROLL	= -7,
	wimp_pointDOWNSCROLL	= -8,
	wimp_pointRESIZE	= -9,
	wimp_pointLEFTSCROLL	= -10,
	wimp_pointHSCROLL	= -11,
	wimp_pointRIGHTSCROLL	= -12,
	wimp_pointBORDER	= -13
} wimp_pointericon;
#endif

#ifdef CREL5

typedef int wimp_dragtype;				/* Drag types */

#define wimp_dragMOVEWIND	1			/* System: move window */
#define wimp_dragSIZEWIND	2			/*         resize window */
#define wimp_dragHSCROLL	3			/*         horizontal scroll */
#define wimp_dragVSCROLL	4			/*         vertical scroll */

#define wimp_dragUSERFIXED	5			/* User:   fixed size */
#define wimp_dragUSERRUBBER	6			/*         rubber box */
#define wimp_dragUSERHIDDEN	7			/*         invisible box */

#else
typedef enum						/* Drag types */
{
	wimp_dragMOVEWIND	= 1,			/* System: move window */
	wimp_dragSIZEWIND	= 2,			/*         resize window */
	wimp_dragHSCROLL	= 3,			/*         horizontal scroll */
	wimp_dragVSCROLL	= 4,			/*         vertical scroll */

	wimp_dragUSERFIXED	= 5,			/* User:   fixed size */
	wimp_dragUSERRUBBER	= 6,			/*         rubber box */
	wimp_dragUSERHIDDEN	= 7			/*         invisible box */
} wimp_dragtype;
#endif

#ifdef CREL5

typedef int wimp_eventtype;				/* Poll event types */

#define wimp_eventNULL		0
#define wimp_eventREDRAW	1
#define wimp_eventOPEN		2
#define wimp_eventCLOSE		3
#define wimp_eventPTRLEAVE	4
#define wimp_eventPTRENTER	5
#define wimp_eventBUTTON	6
#define wimp_eventUSERDRAG	7
#define wimp_eventKEY		8
#define wimp_eventMENU		9
#define wimp_eventSCROLL	10
#define wimp_eventLOSECARET	11
#define wimp_eventGAINCARET	12
#define wimp_eventPOLLWORD	13
#define wimp_eventSEND		17
#define wimp_eventSENDACK	18
#define wimp_eventACK		19

#else
typedef enum						/* Poll event types */
{
	wimp_eventNULL		= 0,
	wimp_eventREDRAW	= 1,
	wimp_eventOPEN		= 2,
	wimp_eventCLOSE		= 3,
	wimp_eventPTRLEAVE	= 4,
	wimp_eventPTRENTER	= 5,
	wimp_eventBUTTON	= 6,
	wimp_eventUSERDRAG	= 7,
	wimp_eventKEY		= 8,
	wimp_eventMENU		= 9,
	wimp_eventSCROLL	= 10,
	wimp_eventLOSECARET	= 11,
	wimp_eventGAINCARET	= 12,
	wimp_eventPOLLWORD	= 13,
	wimp_eventSEND		= 17,
	wimp_eventSENDACK	= 18,
	wimp_eventACK		= 19
} wimp_eventtype;
#endif

#ifdef CREL5

typedef int wimp_eventmask;				/* Poll event masks */

#define wimp_maskNULL		(1<<wimp_eventNULL)
#define wimp_maskREDRAW		(1<<wimp_eventREDRAW)
#define wimp_maskOPEN		(1<<wimp_eventOPEN)
#define wimp_maskCLOSE		(1<<wimp_eventCLOSE)
#define wimp_maskPTRLEAVE	(1<<wimp_eventPTRLEAVE)
#define wimp_maskPTRENTER	(1<<wimp_eventPTRENTER)
#define wimp_maskBUTTON		(1<<wimp_eventBUTTON)
#define wimp_maskUSERDRAG	(1<<wimp_eventUSERDRAG)
#define wimp_maskKEY		(1<<wimp_eventKEY)
#define wimp_maskMENU		(1<<wimp_eventMENU)
#define wimp_maskSCROLL		(1<<wimp_eventSCROLL)
#define wimp_maskLOSECARET	(1<<wimp_eventLOSECARET)
#define wimp_maskGAINCARET	(1<<wimp_eventGAINCARET)
#define wimp_maskPOLLWORD	(1<<wimp_eventPOLLWORD)
#define wimp_maskSEND		(1<<wimp_eventSEND)
#define wimp_maskSENDACK	(1<<wimp_eventSENDACK)
#define wimp_maskACK		(1<<wimp_eventACK)

#else
typedef enum						/* Poll event masks */
{
	wimp_maskNULL		= (1<<wimp_eventNULL),
	wimp_maskREDRAW		= (1<<wimp_eventREDRAW),
	wimp_maskOPEN		= (1<<wimp_eventOPEN),
	wimp_maskCLOSE		= (1<<wimp_eventCLOSE),
	wimp_maskPTRLEAVE	= (1<<wimp_eventPTRLEAVE),
	wimp_maskPTRENTER	= (1<<wimp_eventPTRENTER),
	wimp_maskBUTTON		= (1<<wimp_eventBUTTON),
	wimp_maskUSERDRAG	= (1<<wimp_eventUSERDRAG),
	wimp_maskKEY		= (1<<wimp_eventKEY),
	wimp_maskMENU		= (1<<wimp_eventMENU),
	wimp_maskSCROLL		= (1<<wimp_eventSCROLL),
	wimp_maskLOSECARET	= (1<<wimp_eventLOSECARET),
	wimp_maskGAINCARET	= (1<<wimp_eventGAINCARET),
	wimp_maskPOLLWORD	= (1<<wimp_eventPOLLWORD),
	wimp_maskSEND		= (1<<wimp_eventSEND),
	wimp_maskSENDACK	= (1<<wimp_eventSENDACK),
	wimp_maskACK		= (1<<wimp_eventACK)
} wimp_eventmask;
#endif

#ifdef CREL5

typedef int wimp_pollflags;				/* Special polling flags */

#define wimp_pollSCANWORD	(1<<22)
#define wimp_pollPRIORITY	(1<<23)
#define wimp_pollFPSAVE		(1<<24)

#else
typedef enum						/* Special polling flags */
{
	wimp_pollSCANWORD	= (1<<22),
	wimp_pollPRIORITY	= (1<<23),
	wimp_pollFPSAVE		= (1<<24)
} wimp_pollflags;
#endif

#ifdef CREL5

typedef int wimp_msgaction;				/* Message action codes */

#define wimp_msgCLOSEDOWN	0			/* Must close down now */
#define wimp_msgDATASAVE	1			/* Request to identify directory */
#define wimp_msgDATASAVEACK	2			/* Reply to DATASAVE */
#define wimp_msgDATALOAD	3			/* Request to load/insert dragged icon */
#define wimp_msgDATALOADACK	4			/* Reply to DATALOAD or DATAOPEN */
#define wimp_msgDATAOPEN	5			/* Request that an object be opened */
#define wimp_msgRAMFETCH	6			/* Transfer data to buffer in my workspace */
#define wimp_msgRAMTRANSMIT	7			/* I have transferred some data */
#define wimp_msgPREQUIT		8			/* Confirm with user now */
#define wimp_msgPALETTECHANGE	9			/* Desktop palette has changed */
#define wimp_msgSAVEDESK	10			/* Desktop save request */
#define wimp_msgDEVICECLAIM	11			/* Application would like to claim device */
#define wimp_msgDEVICEINUSE	12			/* Application is already using device */
#define wimp_msgDATASAVED	13			/* A file previously saved has become 'safe' */

#define wimp_msgFILEROPENDIR	0x00400			/* Open Filer directory viewer */
#define wimp_msgFILERCLOSEDIR	0x00401			/* Close Filer directory viewer */
#define wimp_msgFILEROPENDIRAT	0x00402			/* Open Filer directory viewer with details */

#define wimp_msgFILERSELECTDIR	0x00403			/* Action selected directory */
#define wimp_msgFILERADDSELECT	0x00404			/* Action selected files */
#define wimp_msgFILERACTION	0x00405			/* Start filer operation */
#define wimp_msgFILERCONTROLACT	0x00406			/* Check/control filer action */

#define wimp_msgFILERSELECTION	0x00407			/* Filer selection is following */

#define wimp_msgHELPREQUEST	0x00502			/* Interactive help request */
#define wimp_msgHELPREPLY	0x00503			/* Interactive help information */

#define wimp_msgNOTIFY		0x40040			/* Network notify broadcast */

#define wimp_msgMENUWARN	0x400C0			/* Submenu to be opened */
#define wimp_msgMODECHANGE	0x400C1			/* Screen mode has changed */
#define wimp_msgTASKSTART	0x400C2			/* A task has started */
#define wimp_msgTASKEND		0x400C3			/* A task has ended */
#define wimp_msgSLOTCHANGE	0x400C4			/* Slot size has altered */
#define wimp_msgSETSLOT		0x400C5			/* Task manager requests slot size change */
#define wimp_msgTASKNAMERQ	0x400C6			/* Request task name */
#define wimp_msgTASKNAMEIS	0x400C7			/* Reply to task name request */
#define wimp_msgTASKSTARTED	0x400C8			/* Task has now fully started */
#define wimp_msgMENUSDELETED	0x400C9			/* Menu tree has been removed */

#define wimp_msgICONISE		0x400CA			/* Window is to be iconised */
#define wimp_msgWINDOWCLOSED	0x400CB			/* Window has been closed */
#define wimp_msgWINDOWINFO	0x400CC			/* Reply with iconising information */

#define wimp_msgPRINTFILE	0x80140			/* Printer's response to a DATASAVE */
#define wimp_msgWILLPRINT	0x80141			/* Application will print file */
#define wimp_msgPRINTSAVE	0x80142			/* Send file to printer */
#define wimp_msgPRINTINIT	0x80143			/* Printer driver starting up */
#define wimp_msgPRINTERROR	0x80144			/* Printer is busy or faulty */
#define wimp_msgPRINTTYPEODD	0x80145			/* Request to print unknown file */
#define wimp_msgPRINTTYPEKNOWN	0x80146			/* Applcation knows how to print file */
#define wimp_msgPRINTERCHANGE	0x80147			/* New printer application installed */

#define wimp_msgEEREQUEST	0x45D80			/* Request external edit */
#define wimp_msgEEACK		0x45D81			/* Acknowledge external edit */
#define wimp_msgEERETURN	0x45D82			/* Request return of edit data */
#define wimp_msgEEABORT		0x45D83			/* Abort editing session */
#define wimp_msgEEDATASAVE	0x45D84			/* Send data for editing */

#define wimp_msgCOLPICKCHOICE	0x47700			/* Colour picker colour choice */
#define wimp_msgCOLPICKCHANGED	0x47701			/* Colour picker colour changed */
#define wimp_msgCOLPICKCLOSE	0x47702			/* Colour picker closed */
#define wimp_msgCOLPICKPARENT	0x47703			/* Colour picker open parent */

#else
typedef enum						/* Message action codes */
{
	wimp_msgCLOSEDOWN	= 0,			/* Must close down now */
	wimp_msgDATASAVE	= 1,			/* Request to identify directory */
	wimp_msgDATASAVEACK	= 2,			/* Reply to DATASAVE */
	wimp_msgDATALOAD	= 3,			/* Request to load/insert dragged icon */
	wimp_msgDATALOADACK	= 4,			/* Reply to DATALOAD or DATAOPEN */
	wimp_msgDATAOPEN	= 5,			/* Request that an object be opened */
	wimp_msgRAMFETCH	= 6,			/* Transfer data to buffer in my workspace */
	wimp_msgRAMTRANSMIT	= 7,			/* I have transferred some data */
	wimp_msgPREQUIT		= 8,			/* Confirm with user now */
	wimp_msgPALETTECHANGE	= 9,			/* Desktop palette has changed */
	wimp_msgSAVEDESK	= 10,			/* Desktop save request */
	wimp_msgDEVICECLAIM	= 11,			/* Application would like to claim device */
	wimp_msgDEVICEINUSE	= 12,			/* Application is already using device */
	wimp_msgDATASAVED	= 13,			/* A file previously saved has become 'safe' */

	wimp_msgFILEROPENDIR	= 0x00400,		/* Open Filer directory viewer */
	wimp_msgFILERCLOSEDIR	= 0x00401,		/* Close Filer directory viewer */
	wimp_msgFILEROPENDIRAT	= 0x00402,		/* Open Filer directory viewer with details */

	wimp_msgFILERSELECTDIR	= 0x00403,		/* Action selected directory */
	wimp_msgFILERADDSELECT	= 0x00404,		/* Action selected files */
	wimp_msgFILERACTION	= 0x00405,		/* Start filer operation */
	wimp_msgFILERCONTROLACT	= 0x00406,		/* Check/control filer action */

	wimp_msgFILERSELECTION	= 0x00407,		/* Filer selection is following */

	wimp_msgHELPREQUEST	= 0x00502,		/* Interactive help request */
	wimp_msgHELPREPLY	= 0x00503,		/* Interactive help information */

	wimp_msgNOTIFY		= 0x40040,		/* Network notify broadcast */

	wimp_msgMENUWARN	= 0x400C0,		/* Submenu to be opened */
	wimp_msgMODECHANGE	= 0x400C1,		/* Screen mode has changed */
	wimp_msgTASKSTART	= 0x400C2,		/* A task has started */
	wimp_msgTASKEND		= 0x400C3,		/* A task has ended */
	wimp_msgSLOTCHANGE	= 0x400C4,		/* Slot size has altered */
	wimp_msgSETSLOT		= 0x400C5,		/* Task manager requests slot size change */
	wimp_msgTASKNAMERQ	= 0x400C6,		/* Request task name */
	wimp_msgTASKNAMEIS	= 0x400C7,		/* Reply to task name request */
	wimp_msgTASKSTARTED	= 0x400C8,		/* Task has now fully started */
	wimp_msgMENUSDELETED	= 0x400C9,		/* Menu tree has been removed */

	wimp_msgICONISE		= 0x400CA,		/* Window is to be iconised */
	wimp_msgWINDOWCLOSED	= 0x400CB,		/* Window has been closed */
	wimp_msgWINDOWINFO	= 0x400CC,		/* Reply with iconising information */

	wimp_msgEEREQUEST	= 0x45D80,		/* Request external edit */
	wimp_msgEEACK		= 0x45D81,		/* Acknowledge external edit */
	wimp_msgEERETURN	= 0x45D82,		/* Request return of edit data */
	wimp_msgEEABORT		= 0x45D83,		/* Abort editing session */
	wimp_msgEEDATASAVE	= 0x45D84,		/* Send data for editing */

	wimp_msgCOLPICKCHOICE	= 0x47700,		/* Colour picker colour choice */
	wimp_msgCOLPICKCHANGED	= 0x47701,		/* Colour picker colour changed */
	wimp_msgCOLPICKCLOSE	= 0x47702,		/* Colour picker closed */
	wimp_msgCOLPICKPARENT	= 0x47703,		/* Colour picker open parent */

	wimp_msgPRINTFILE	= 0x80140,		/* Printer's response to a DATASAVE */
	wimp_msgWILLPRINT	= 0x80141,		/* Application will print file */
	wimp_msgPRINTSAVE	= 0x80142,		/* Send file to printer */
	wimp_msgPRINTINIT	= 0x80143,		/* Printer driver starting up */
	wimp_msgPRINTERROR	= 0x80144,		/* Printer is busy or faulty */
	wimp_msgPRINTTYPEODD	= 0x80145,		/* Request to print unknown file */
	wimp_msgPRINTTYPEKNOWN	= 0x80146,		/* Applcation knows how to print file */
	wimp_msgPRINTERCHANGE	= 0x80147		/* New printer application installed */
} wimp_msgaction;
#endif

#ifdef CREL5

typedef int wimp_menuflags;				/* Menu item flags */

#define wimp_menuTICK		0x01			/* Tick entry */
#define wimp_menuSEPARATE	0x02			/* Dotted line after entry */
#define wimp_menuWRITEABLE	0x04			/* Writeable item */
#define wimp_menuNOTIFY		0x08			/* Open submenu via message */
#define wimp_menuVIEWSHADED	0x10			/* Allow shaded submenus to open */
#define wimp_menuLAST		0x80			/* Last item in menu */

#else
typedef enum						/* Menu item flags */
{
	wimp_menuTICK		= 0x01,			/* Tick entry */
	wimp_menuSEPARATE	= 0x02,			/* Dotted line after entry */
	wimp_menuWRITEABLE	= 0x04,			/* Writeable item */
	wimp_menuNOTIFY		= 0x08,			/* Open submenu via message */
	wimp_menuVIEWSHADED	= 0x10,			/* Allow shaded submenus to open */
	wimp_menuLAST		= 0x80			/* Last item in menu */
} wimp_menuflags;
#endif

#ifdef CREL5

typedef int wimp_errorflags;				/* Flags for error dialogue */

#define wimp_errorOK		0x001			/* Buttons: OK */
#define wimp_errorCANCEL	0x002			/*          CANCEL */
#define wimp_errorHICANCEL	0x004			/*          highlight CANCEL rather than OK */
#define wimp_errorCONT		0x008			/* Return after putting up box */
#define wimp_errorNONAME	0x010			/* No "Error from" in title bar */
#define wimp_errorIMMED		0x020			/* Return if button activated */
#define wimp_errorSELECT	0x040			/* Select button and remove */
#define wimp_errorNOBEEP	0x080			/* Never sound a beep */
#define wimp_errorNEW		0x100			/* New categorised errors */

#define wimp_reportOLD		0x000			/* Old-style report */
#define wimp_reportINFO		0x200			/* Information */
#define wimp_reportERROR	0x400			/* Error */
#define wimp_reportPROGRAM	0x600			/* Program (Stop) */
#define wimp_reportQUERY	0x800			/* Question */

#else
typedef enum						/* Flags for error dialogue */
{
	wimp_errorOK		= 0x001,		/* Buttons: OK */
	wimp_errorCANCEL	= 0x002,		/*          CANCEL */
	wimp_errorHICANCEL	= 0x004,		/*          highlight CANCEL rather than OK */
	wimp_errorCONT		= 0x008,		/* Return after putting up box */
	wimp_errorNONAME	= 0x010,		/* No "Error from" in title bar */
	wimp_errorIMMED		= 0x020,		/* Return if button activated */
	wimp_errorSELECT	= 0x040,		/* Select button and remove */
	wimp_errorNOBEEP	= 0x080,		/* Never sound a beep */
	wimp_errorNEW		= 0x100,		/* New categorised errors */

	wimp_reportOLD		= 0x000,		/* Old-style report */
	wimp_reportINFO		= 0x200,		/* Information */
	wimp_reportERROR	= 0x400,		/* Error */
	wimp_reportPROGRAM	= 0x600,		/* Program (Stop) */
	wimp_reportQUERY	= 0x800			/* Question */
} wimp_errorflags;
#endif

#ifdef CREL5

typedef int wimp_commandtag;				/* Tags for command window */

#define wimp_commandTITLE		0
#define wimp_commandACTIVE		1
#define wimp_commandCLOSEPROMPT		2
#define wimp_commandCLOSENOPROMPT	3

#else
typedef enum						/* Tags for command window */
{
	wimp_commandTITLE		= 0,
	wimp_commandACTIVE		= 1,
	wimp_commandCLOSEPROMPT		= 2,
	wimp_commandCLOSENOPROMPT	= 3
} wimp_commandtag;
#endif

#ifdef CREL5

typedef int wimp_caretflags;				/* Caret setting flags */

#define wimp_caretHEIGHT	0x00000001		/* 16-bit field: caret size */
#define wimp_caretCOLOUR	0x00010000		/* 8-bit field: caret colour */
#define wimp_caretVDU5		0x01000000		/* VDU-5 type caret */
#define wimp_caretINVISIBLE	0x02000000		/* Invisible caret */
#define wimp_caretUSECOLOUR	0x04000000		/* Colour is supplied */
#define wimp_caretREALCOLOUR	0x08000000		/* Colour is not to be translated */

#else
typedef enum						/* Caret setting flags */
{
	wimp_caretHEIGHT	= 0x00000001,		/* 16-bit field: caret size */
	wimp_caretCOLOUR	= 0x00010000,		/* 8-bit field: caret colour */
	wimp_caretVDU5		= 0x01000000,		/* VDU-5 type caret */
	wimp_caretINVISIBLE	= 0x02000000,		/* Invisible caret */
	wimp_caretUSECOLOUR	= 0x04000000,		/* Colour is supplied */
        wimp_caretREALCOLOUR	= 0x08000000		/* Colour is not to be translated */
} wimp_caretflags;
#endif

#ifdef CREL5

typedef int wimp_sysinfo;				/* Keys for ReadSysInfo */

#define wimp_sysTASKS		0			/* Number of active tasks */
#define wimp_sysWIMPMODE	1			/* Current Wimp screen mode */
#define wimp_sysMODESUFF	2			/* Sprite filename suffix */
#define wimp_sysDESKTOP		3			/* Text/desktop flag */
#define wimp_sysWRITEDIR	4			/* Text entry direction */
#define wimp_sysCURTASK		5			/* Current task handle */
#define wimp_sysTASKVERS	-5			/* Requested Wimp version */
#define wimp_sysRESERVED	6			/* Reserved */
#define wimp_sysVERSION		7			/* Current Wimp version */
#define wimp_sysDESKTOPFONT	8			/* Desktop font handle */
#define wimp_sysSYMBOLFONT	-8			/* Symbol font handle */
#define wimp_sysTOOLSPRITEAREA	9			/* Window tool sprite area */
#define wimp_sysMAXAPPSPACE	11			/* Maximum application space */

#else
typedef enum						/* Keys for ReadSysInfo */
{
	wimp_sysTASKS		= 0,			/* Number of active tasks */
	wimp_sysWIMPMODE	= 1,			/* Current Wimp screen mode */
	wimp_sysMODESUFF	= 2,			/* Sprite filename suffix */
	wimp_sysDESKTOP		= 3,			/* Text/desktop flag */
	wimp_sysWRITEDIR	= 4,			/* Text entry direction */
	wimp_sysCURTASK		= 5,			/* Current task handle */
	wimp_sysTASKVERS	= -5,			/* Requested Wimp version */
	wimp_sysRESERVED	= 6,			/* Reserved */
	wimp_sysVERSION		= 7,			/* Current Wimp version */
	wimp_sysDESKTOPFONT	= 8,			/* Desktop font handle */
	wimp_sysSYMBOLFONT	= -8,			/* Symbol font handle */
	wimp_sysTOOLSPRITEAREA	= 9,			/* Window tool sprite area */
	wimp_sysMAXAPPSPACE	= 11			/* Maximum application space */
} wimp_sysinfo;
#endif

/************************************************************************/
/*									*/
/*	Shifts and masks for icon flags and report category		*/
/*									*/
/************************************************************************/

#define wimp_imaskFORECOL	0x0F000000
#define wimp_imaskBACKCOL	0xF0000000
#define wimp_imaskFONT		0xFF000000
#define wimp_imaskBUTTON	0x0000F000
#define wimp_imaskESG		0x001F0000

#define wimp_ishiftBACKCOL	28
#define wimp_ishiftFORECOL	24
#define wimp_ishiftFONT		24
#define wimp_ishiftBUTTON	12
#define wimp_ishiftESG		16

#define wimp_rmaskCATEGORY	0xE00
#define wimp_rshiftCATEGORY	9

/************************************************************************/
/*									*/
/*	Values for sprite area pointer					*/
/*									*/
/************************************************************************/

#define wimp_spriteSYSTEM	((sprite_area *) 0)
#define wimp_spriteWIMP		((sprite_area *) 1)

/************************************************************************/
/*									*/
/*	Values for icon bar window handle				*/
/*									*/
/************************************************************************/

#define wimp_ibarRIGHT		((wimp_w) -1)
#define wimp_ibarLEFT		((wimp_w) -2)

#define wimp_ibarSPECIFYLEFT	((wimp_w) -3)
#define wimp_ibarSPECIFYRIGHT	((wimp_w) -4)
#define wimp_ibarLEFTPRILEFT	((wimp_w) -5)
#define wimp_ibarLEFTPRIRIGHT	((wimp_w) -6)
#define wimp_ibarRIGHTPRILEFT	((wimp_w) -7)
#define wimp_ibarRIGHTPRIRIGHT	((wimp_w) -8)

/************************************************************************/
/*									*/
/*	Values for window depth						*/
/*									*/
/************************************************************************/

#define wimp_depthTOP		((wimp_w) -1)
#define wimp_depthBOTTOM	((wimp_w) -2)
#define wimp_depthHIDDEN	((wimp_w) -3)

/************************************************************************/
/*									*/
/*	Fundamental data types						*/
/*									*/
/************************************************************************/

typedef int wimp_w;					/* Window handle */
typedef int wimp_i;					/* Icon handle */
typedef int wimp_t;					/* Task handle */

/************************************************************************/
/*									*/
/*	Fundamental data structures					*/
/*									*/
/************************************************************************/

typedef union						/* Icon data */
{
	char text[12];					/* Included text */
	char sprite[12];				/* Included sprite name */
	struct						/* Indirected text */
	{
		char *text;				/* Text buffer */
		char *valid;				/* Validation string */
		int length;				/* Length of text buffer */
	} indirecttext;
	struct						/* Indirected sprite */
	{
		char *name;				/* Sprite name */
		sprite_area *area;			/* Sprite area (0=system, 1=Wimp) */
		BOOL nameisname;			/* Sprite name/pointer flag */
	} indirectsprite;
} wimp_icondata;

typedef struct						/* Rectangle coordinates */
{
	int x0,y0;					/* Bottom left */
	int x1,y1;					/* Top right */
} wimp_box;

typedef struct						/* Window definition */
{
	wimp_box box;					/* Screen coordinates of work area */
	int scx,scy;					/* Scroll bar positions */
	wimp_w behind;					/* Window to open behind */
	wimp_windowflags flags;				/* Window flags */
	char colours[8];				/* Colour array */
	wimp_box ex;					/* Work area extent */
	wimp_iconflags titleflags;			/* Title bar icon flags */
	wimp_iconflags workflags;			/* Work area icon flags */
	sprite_area *spritearea;			/* Sprite area (0=system, 1=Wimp) */
	struct						/* Minimum window size */
	{
		unsigned int width : 16;
		unsigned int height : 16;
	} minsize;
	wimp_icondata title;				/* Icon data for title bar */
	int nicons;					/* Number of icons in window */
} wimp_wind;						/* Icon definitions follow */

typedef struct						/* Returned window information */
{
	wimp_w w;					/* Window handle */
	wimp_wind info;					/* Window definition */
} wimp_winfo;						/* Icon definitions follow */

typedef struct						/* Icon definition */
{
	wimp_box box;					/* Bounding box */
	wimp_iconflags flags;				/* Icon flags */
	wimp_icondata data;				/* Icon data */
} wimp_icon;

typedef struct						/* Data for creating an icon */
{
	wimp_w w;					/* Window handle */
	wimp_icon i;					/* Icon data */
} wimp_icreate;

typedef struct						/* Data for opening a window */
{
	wimp_w w;					/* Window handle */
	wimp_box box;					/* Screen position of visible work area */
	int scx,scy;					/* Scroll bar positions */
	wimp_w behind;					/* Window to open behind */
} wimp_openstr;

typedef struct						/* Returned window state information */
{
	wimp_openstr o;					/* Open data */
	wimp_windowflags flags;				/* Window flags */
} wimp_wstate;

typedef struct						/* Data for redrawing a window */
{
	wimp_w w;					/* Window handle */
	wimp_box box;					/* Screen position of visible work area */
	int scx,scy;					/* Scroll bar positions */
	wimp_box g;					/* Current graphics window */
} wimp_redrawstr;

typedef struct						/* Mouse state information */
{
	int x,y;					/* Active coordinates */
	wimp_buttonstate b;				/* Button state */
	wimp_w w;					/* Window handle */
	wimp_i i;					/* Icon handle */
} wimp_mousestr;

typedef struct						/* Caret information */
{
	wimp_w w;					/* Window handle */
	wimp_i i;					/* Icon handle */
	int x,y;					/* Position relative to window origin */
	wimp_caretflags height;				/* Size, or -1 for icon calculation */
	int index;					/* Icon index, or -1 for icon calculation */
} wimp_caretstr;

typedef struct						/* Menu header */
{
	char title[12];					/* Menu title */
	char title_fcol,title_bcol;			/* Title bar colours */
	char work_fcol,work_bcol;			/* Work area colours */
	int width,height;				/* Size of menu items */
	int gap;					/* Vertical gap between items */
} wimp_menuhdr;

typedef struct wimp_menustr *wimp_menuptr;		/* Circular reference */

typedef struct						/* Menu entry */
{
	wimp_menuflags menuflags;			/* Menu flags */
	wimp_menuptr submenu;				/* Submenu, subwindow or -1 for none */
	wimp_iconflags iconflags;			/* Icon flags */
	wimp_icondata data;				/* Icon data */
} wimp_menuitem;

typedef struct wimp_menustr1				/* Menu data with first entry */
{
	wimp_menuhdr hdr;				/* Header */
	wimp_menuitem item[1];				/* Menu entries */
} wimp_menustr1;

typedef struct wimp_menustr				/* Menu data header */
{
	wimp_menuhdr hdr;				/* Header (entries follow) */
} wimp_menustr;

typedef struct						/* Data for initiating a drag */
{
	wimp_w w;					/* Window handle */
	wimp_dragtype type;				/* Drag type */
	wimp_box box;					/* Initial drag box */
	wimp_box parent;				/* Parent box for drag box */
} wimp_dragstr;

typedef struct						/* Data for finding icons */
{
	wimp_w w;					/* Window handle */
	wimp_iconflags mask;				/* Mask for desired bits */
	wimp_iconflags set;				/* State of desired bits */
} wimp_which_block;

typedef struct						/* Data for setting mouse pointer */
{
	int num;					/* Pointer number (0 to turn off pointer) */
	void *data;					/* Pointer shape data */
	int width,height;				/* Size in pixels */
	int activex,activey;				/* Active point (pixels from top left) */
} wimp_pshapestr;

typedef struct						/* Font reference count array */
{
	char f[256];
} wimp_font_array;

typedef struct						/* Data for reading template */
{
	wimp_wind *buf;					/* Space for putting template in */
	char *work_free;				/* Start of indirect workspace */
	char *work_end;					/* End of indirect workspace */
	wimp_font_array *font;				/* Font reference count array */
	char *name;					/* Template name (can be wildcard) */
	int index;					/* Position where search starts */
} wimp_template;

typedef struct
{
	unsigned int winsize;				/* Window definition size */
	unsigned int indsize;				/* Indirect workspace size */
	int index;					/* Position in template file */
} wimp_tempinfo;

typedef union						/* Palette entry */
{
	struct						/* Byte fields */
	{
		char gcol;				/* GCOL that will be used */
		char red,green,blue;			/* RGB components */
	} bytes;
	os_colour word;					/* Entire entry */
} wimp_paletteword;

typedef struct						/* Palette information */
{
	wimp_paletteword c[16];				/* Wimp colours */
	wimp_paletteword border,mouse1,mouse2,mouse3;	/* Border and pointer colours */
} wimp_palettestr;

typedef struct						/* Data for a command window */
{
	wimp_commandtag tag;				/* Operation tag */
	char *title;					/* String for title */
} wimp_commandwind;

typedef struct						/* Colour value and model data */
{
	wimp_paletteword col;				/* Encoded colour */
	int extsize;					/* Size of extra data */
	int model;					/* Colour model */

	union						/* Extra data for models */
	{
		struct { unsigned int red,green,blue;		} rgb;
		struct { unsigned int cyan,magenta,yellow,key;	} cmyk;
		struct { unsigned int hue,saturation,value;	} hsv;
	} m;
} wimp_colourdescriptor;

/************************************************************************/
/*									*/
/*	Structures for message-specific data				*/
/*									*/
/************************************************************************/

typedef struct						/* Destination of save request */
{
	wimp_w w;					/* Window handle */
	wimp_i i;					/* Icon handle */
	int x,y;					/* Screen position */
	int estsize;					/* Estimated size of data */
	int type;					/* File type of data */
	char leaf[60];					/* Proposed leaf name of file */
} wimp_msgdatasave;

typedef struct						/* Acknowledge, save to here */
{
	wimp_w w;					/* Window handle */
	wimp_i i;					/* Icon handle */
	int x,y;					/* Screen position */
	int estsize;					/* Estimated size of data */
	int type;					/* File type of data */
	char name[256-44];				/* Pathname of file to save */
} wimp_msgdatasaveack;

typedef wimp_msgdatasaveack wimp_msgdataload;		/* Load file from here */
typedef wimp_msgdatasaveack wimp_msgdataloadack;	/* Loaded successfully */

typedef struct						/* Can anyone load this file? */
{
	wimp_w w;					/* Window handle of Filer window */
	wimp_i spare1;					/* Not used */
	int x,y;					/* Screen position */
	int spare2;					/* Not used */
	int type;					/* File type of data */
	char name[256-44];				/* Pathname of file to save */
} wimp_msgdataopen;

typedef struct						/* Transfer data in memory */
{
	char *addr;					/* Address of data to transfer */
	int size;					/* Number of bytes to transfer */
} wimp_msgramfetch;

typedef struct						/* I have transferred some data */
{
	char *addr;					/* Copy of value sent in RAMfetch */
	int size;					/* Number of bytes written */
} wimp_msgramtransmit;

typedef struct						/* Save desktop state */
{
	int filehandle;					/* RiscOS file handle */
} wimp_msgsavedesk;

typedef struct						/* PreQuit for RiscOS 3 */
{
	int flag;					/* TRUE if quit just this task */
} wimp_msgprequit;

typedef struct						/* Device claim or status */
{
	int major;					/* Major device number */
	int minor;					/* Minor device number */
	char information[256-28];			/* Information string */
} wimp_msgdevice;

typedef struct						/* Interactive help request */
{
	wimp_mousestr m;				/* Point where help is required */
} wimp_msghelprequest;

typedef struct						/* Interactive help reply */
{
	char text[200];					/* The help string */
} wimp_msghelpreply;

typedef struct						/* Error from printer */
{
	int errnum;					/* Error number */
	char errmess[256-24];				/* Error message */
} wimp_msgprinterror;

typedef struct						/* Printing unknown file type */
{
	int spare[5];					/* Not used */
	int type;					/* File type of data */
	char name[256-44];				/* Pathname of file to print */
} wimp_msgprintodd;

typedef struct						/* New task started */
{
	unsigned int cao;				/* Current Active Object */
	unsigned int memory;				/* Application memory used */
	char name[256-28];				/* Task name */
} wimp_msgtaskinit;

typedef struct						/* Slot size changed */
{
	unsigned int new;				/* Task's new slot size */
	unsigned int next;				/* Next slot size */
} wimp_msgslotsize;

typedef struct						/* Set slot size */
{
	unsigned int new;				/* New slot size */
	wimp_t task;					/* Task handle */
} wimp_msgsetslot;

typedef struct						/* Filer selection following */
{
	int x0,y0,x1,y1;				/* Selection bounding box */
	int width,height;				/* Size of a selected item */
	unsigned int viewmode;				/* Directory viewing mode */
	unsigned int c0,r0;				/* Start column,row */
	unsigned int c1,r1;				/* End column,row */
} wimp_msgfilersel;

typedef struct						/* Window iconise requested */
{
	wimp_w w;					/* Window handle */
	wimp_t t;					/* Owner task */
	char title[20];					/* Suggested title */
} wimp_msgiconise;

typedef struct						/* Window has been closed */
{
	wimp_w w;					/* Window handle */
} wimp_msgwinclosed;

typedef struct						/* Information for iconise */
{
	wimp_w w;					/* Window handle */
	int reserved;					/* Reserved, must be 0 */
	char sprite[8];					/* Sprite name to use */
	char title[20];					/* Icon title to use */
} wimp_msgwindowinfo;

typedef struct						/* Submenu warning */
{
	wimp_menustr *m;				/* Menu data */
	int x,y;					/* Menu opening coordinates */
	int hit[10];					/* Menu hits leading to this */
} wimp_msgmenuwarn;

typedef struct						/* Colour picker colour choice */
{
	int d;						/* Dialogue handle */
	int flags;					/* Selection flags */
	wimp_colourdescriptor col;			/* Colour selection */
} wimp_msgcolpickchoice;

typedef struct						/* Colour picker colour changed */
{
	int d;						/* Dialogue handle */
	int flags;					/* Selection/drag flags */
	wimp_colourdescriptor col;			/* Current colour */
} wimp_msgcolpickchanged;

typedef struct						/* Colour picker closed */
{
	int d;						/* Dialogue handle */
} wimp_msgcolpickclose;

typedef wimp_msgcolpickclose wimp_msgcolpickparent;	/* Colour picker open parent */

/************************************************************************/
/*									*/
/*	Message and event data structures				*/
/*									*/
/************************************************************************/

typedef struct						/* Message header */
{
	int size;					/* Size of message block */
	const wimp_t task;				/* Task handle of sender (filled in by Wimp) */
	const int msg_id;				/* Unique reference (filled in by Wimp) */
	int your_ref;					/* Sender's reference */
	wimp_msgaction action;				/* Message action code */
} wimp_msghdr;

typedef struct						/* Message block */
{
	wimp_msghdr hdr;				/* Header */
	union						/* Message data */
	{
		char			chars[236];
		int			words[59];
		wimp_msgdatasave	datasave;
		wimp_msgdatasaveack	datasaveack;
		wimp_msgdataload	dataload;
		wimp_msgdataloadack	dataloadack;
		wimp_msgdataopen	dataopen;
		wimp_msgramfetch	ramfetch;
		wimp_msgramtransmit	ramtransmit;
		wimp_msgprequit		prequit;
		wimp_msghelprequest	helprequest;
		wimp_msghelpreply	helpreply;
		wimp_msgprintodd	printodd;
		wimp_msgdatasave	printsave;
		wimp_msgdatasave	printfile;
		wimp_msgprinterror	printerror;
		wimp_msgsavedesk	savedesk;
		wimp_msgdevice		device;
		wimp_msgtaskinit	taskinit;
		wimp_msgslotsize	slotsize;
		wimp_msgsetslot		setslot;
		wimp_msgfilersel	filersel;
		wimp_msgiconise		iconise;
		wimp_msgwinclosed	winclose;
		wimp_msgwindowinfo	windowinfo;
		wimp_msgmenuwarn	menuwarn;
		wimp_msgcolpickchoice	colpickchoice;
		wimp_msgcolpickchanged	colpickchanged;
		wimp_msgcolpickclose	colpickclose;
		wimp_msgcolpickparent	colpickparent;
	} data;
} wimp_msgstr;

typedef union						/* Data from poll result */
{
	wimp_openstr o;					/* Redraw, Open, Close, Enter, Leave */
	struct						/* Button change */
	{
		wimp_mousestr m;			/* Current mouse state */
		wimp_buttonstate old;			/* Previous button state */
	} but;
	wimp_box dragbox;				/* User drag */
	struct						/* Key */
	{
		wimp_caretstr c;			/* Caret information */
		int code;				/* Key code */
	} key;
	int menu[10];					/* Menu selections */
	struct						/* Scroll request */
	{
		wimp_openstr o;				/* Current window open data */
		int x,y;				/* Direction and amount */
	} scroll;
	wimp_caretstr c;				/* Gain or Lose caret */
	wimp_msgstr msg;				/* Message */
} wimp_eventdata;

typedef struct						/* Poll result */
{
	wimp_eventtype e;				/* Event type */
	wimp_eventdata data;				/* Event data */
} wimp_eventstr;

/************************************************************************/
/*									*/
/*	External functions						*/
/*									*/
/************************************************************************/

extern os_error *wimp_Initialise(int *version);
extern os_error *wimp_TaskClose(wimp_t task);

extern os_error *wimp_TaskInit(const char *name,int *version,wimp_t *task);
extern os_error *wimp_CloseDown(void);

extern os_error *wimp_StartTask(const char *command);
extern os_error *wimp_StartTask1(const char *command,wimp_t *task);
extern os_error *wimp_CommandWindow(const wimp_commandwind *data);

extern os_error *wimp_CreateWindow(const wimp_wind *data,wimp_w *w);
extern os_error *wimp_CreateIcon(const wimp_icreate *data,wimp_i *i);
extern os_error *wimp_DeleteWindow(wimp_w w);
extern os_error *wimp_DeleteIcon(wimp_w w,wimp_i i);
extern os_error *wimp_OpenWindow(const wimp_openstr *data);
extern os_error *wimp_CloseWindow(wimp_w w);

extern os_error *wimp_Poll(wimp_eventmask mask,wimp_eventstr *result);
extern os_error *wimp_PollIdle(wimp_eventmask mask,wimp_eventstr *result,int earliest);
extern void wimp_SaveFPState(void);
extern void wimp_CorruptFPState(void);

extern os_error *wimp_RedrawWindow(wimp_redrawstr *rect,BOOL *more);
extern os_error *wimp_UpdateWindow(wimp_redrawstr *rect,BOOL *more);
extern os_error *wimp_GetRectangle(wimp_redrawstr *rect,BOOL *more);
extern os_error *wimp_ForceRedraw(const wimp_redrawstr *rect);

extern os_error *wimp_GetWindowState(wimp_w w,wimp_wstate *result);
extern os_error *wimp_GetWindowInfo(wimp_winfo *result);
extern os_error *wimp_GetWindowInfo1(wimp_winfo *result);

extern os_error *wimp_SetIconState(wimp_w w,wimp_i i,wimp_iconflags data,wimp_iconflags mask);
extern os_error *wimp_GetIconState(wimp_w w,wimp_i i,wimp_icon *result);

extern os_error *wimp_GetPointerInfo(wimp_mousestr *result);
extern os_error *wimp_SetCaretPosition(wimp_caretstr *data);
extern os_error *wimp_GetCaretPosition(wimp_caretstr *result);

extern os_error *wimp_DragBox(wimp_dragstr *data);
extern os_error *wimp_SetPointerShape(wimp_pshapestr *data);
extern os_error *wimp_ProcessKey(int code);

extern os_error *wimp_ReportError(const os_error *errblk,wimp_errorflags flags,const char *title);
extern int wimp_ReportError1(const os_error *errblk,wimp_errorflags flags,const char *title);
extern int wimp_ReportError2(const os_error *errblk,wimp_errorflags flags,const char *title,const char *sprite,const char *buttons);

extern os_error *wimp_CreateMenu(const wimp_menustr *m,int x,int y);
extern os_error *wimp_CreateMenu1(wimp_w w,int x,int y);
extern os_error *wimp_CreateSubMenu(const wimp_menustr *sub,int x,int y);
extern os_error *wimp_CreateSubMenu1(wimp_w w,int x,int y);
extern os_error *wimp_CreateMenu0(void);
extern os_error *wimp_DecodeMenu(const wimp_menustr *m,const int *selections,char *result);
extern os_error *wimp_GetMenuState(BOOL wi,int *result,wimp_w w,wimp_i i);

extern os_error *wimp_PlotIcon(const wimp_icon *data);
extern os_error *wimp_WhichIcon(const wimp_which_block *data,wimp_i *result);

extern os_error *wimp_OpenTemplate(const char *file);
extern os_error *wimp_CloseTemplate(void);
extern os_error *wimp_LoadTemplate(wimp_template *data);
extern os_error *wimp_LoadTemplate1(const wimp_template *data,wimp_tempinfo *info);

extern os_error *wimp_SetPalette(wimp_palettestr *data);
extern os_error *wimp_SetMode(int mode);
extern os_error *wimp_BlockCopy(wimp_w w,wimp_box *source,int x,int y);

extern os_error *wimp_ReadPalette(wimp_palettestr *result);
extern os_error *wimp_SetColour(int colour);
extern os_error *wimp_SetFontColours(int foreground,int background);
extern os_error *wimp_ReadPixTrans(sprite_area *area,sprite_id *id,sprite_factors *factors,sprite_pixtrans *pixtrans);

extern os_error *wimp_SpriteOp(int reason,char *name);
extern os_error *wimp_SpriteOpFull(os_regset *);

extern os_error *wimp_SendMessage(wimp_eventtype code,wimp_msgstr *msg,wimp_t task);
extern os_error *wimp_SendMessage1(wimp_eventtype code,wimp_msgstr *msg,wimp_w w,wimp_i i);

extern int wimp_ReadSysInfo(wimp_sysinfo key);
extern sprite_area *wimp_BaseOfSprites(void);

extern os_error *wimp_SlotSize(int *currentslot,int *nextslot,int *freepool);
extern os_error *wimp_TransferBlock(wimp_t sourcetask,const void *sourcebuf,wimp_t desttask,void *destbuf,int buflen);

extern os_error *wimp_GetWindowOutline(wimp_redrawstr *result);
extern os_error *wimp_SetExtent(wimp_redrawstr *rect);

extern int wimp_TextOp1(const char *str,int n);

/************************************************************************/
/*									*/
/*	Convenience macros						*/
/*									*/
/************************************************************************/

#define wimp_SelectIcon(w,i,state)	wimp_SetIconState(w,i,((state) ? wimp_iconSELECTED : 0),wimp_iconSELECTED)

#endif

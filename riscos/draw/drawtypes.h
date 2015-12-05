/* For Emacs' benefit:                -*- mode:c; mode:riscos-file; -*- */
/************************************************************************/
/*									*/
/*	File:		drawtypes.h		Edit:  23-Mar-05	*/
/*	SCCS:		<@(#) drawtypes.h 1.4>				*/
/*	Original:	riscos/drawftypes.h				*/
/*	Language:	ANSI C						*/
/*	Project:	Draw file utility library			*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	Derived from Acorn's drawftypes.h, this file defines the new	*/
/*	object tags and object type structures as used by the layer,	*/
/*	new format save and library facilities of the Draw 1½ family.	*/
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
/*	Copyright © Acorn Computers Limited 1990-1992			*/
/*	Copyright © Jonathan Marten 1990-1999				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/

#ifndef __drawtypes_h
#define __drawtypes_h

/************************************************************************/
/*									*/
/*	Include files							*/
/*									*/
/************************************************************************/

#ifndef __sprite_h
#include "sys/sprite.h"					/* Sprite handling system calls */
#endif
#ifndef __ostypes_h
#include "sys/ostypes.h"				/* Operating system data types */
#endif

/************************************************************************/
/*									*/
/*	General constants for file and object routines			*/
/*									*/
/************************************************************************/

#define draw_fileVERSION	201			/* Version of drawing file */
#define draw_textAREAMARGIN	2048			/* Margin of text area outside columns */
#define draw_pathMITRELIMIT	0xA0000			/* Mitre limit for paths */
#define draw_DASHBIT		2304			/* One dash dot (Draw units) */
#define draw_SCALE1		0x00010000		/* 1:1 in transform units */

#define setstr_VERSION		4			/* Version of settings structure */

/************************************************************************/
/*									*/
/*	Types for object details					*/
/*									*/
/************************************************************************/

typedef int draw_sizetyp;				/* Object size */
typedef os_colour draw_coltyp;				/* BBGGRRXX colour */
typedef int draw_pathwidth;				/* Line width */
typedef unsigned char draw_fontid;			/* Font reference */
typedef unsigned int draw_fontsize;			/* Font size */
typedef struct { int x,y; } draw_objcoord;		/* Base coordinate */
typedef struct { char ch[12]; } draw_groupnametyp;	/* Group name */
typedef int draw_transmat[6];				/* Transform matrix */

enum draw_jointyp					/* Path join types */
{
	draw_joinMITRED =	0,
	draw_joinROUND =	1,
	draw_joinBEVELLED =	2
};

enum draw_captyp					/* Path cap types */
{
	draw_capBUTT =		0,
	draw_capROUND =		1,
	draw_capSQUARE =	2,
	draw_capTRIANGLE =	3
};

enum draw_windtyp					/* Fill winding rule */
{
	draw_windNONZERO =	0,
	draw_windEVENODD =	1
};

enum draw_dashtyp					/* Dash pattern flags */
{
	draw_dashABSENT =	0,
	draw_dashPRESENT =	1
};

enum draw_reptyp					/* Replication type, must */
{							/* synchronise with replicate.c */
	draw_repHORIZONTAL =	1,
	draw_repVERTICAL =	2,
	draw_repDIAGONAL =	3,
	draw_repMATRIX =	4
};

enum draw_radtyp					/* Radiation centre, must */
{							/* synchronise with interpol.c */
	draw_radTOPLEFT =	1,
	draw_radTOPCENT =	2,
	draw_radTOPRIGHT =	3,
	draw_radCENTRIGHT =	4,
	draw_radBOTTRIGHT =	5,
	draw_radBOTTCENT =	6,
	draw_radBOTTLEFT =	7,
	draw_radCENTLEFT =	8,
	draw_radCENTRE =	9
};

typedef enum draw_jointyp draw_jointyp;
typedef enum draw_captyp draw_captyp;
typedef enum draw_windtyp draw_windtyp;
typedef enum draw_dashtyp draw_dashtyp;
typedef enum draw_reptyp draw_reptyp;
typedef enum draw_radtyp draw_radtyp;

#define draw_packmaskJOIN	0x03			/* draw_pathstyle.joincapwind fields */
#define draw_packmaskENDCAP	0x0C
#define draw_packmaskSTARTCAP	0x30
#define draw_packmaskWINDRULE	0x40
#define draw_packmaskDASHED	0x80

#define draw_packshftJOIN	0
#define draw_packshftENDCAP	2
#define draw_packshftSTARTCAP	4
#define draw_packshftWINDRULE	6
#define draw_packshftDASHED	7

#define draw_textKERN		0x01			/* Flags for text display */
#define draw_textREVERSE	0x02
#define draw_textBLEND		0x04

#define draw_textLEFT		0x00			/* Text line justification */
#define draw_textCENTRE		0x10
#define draw_textRIGHT		0x20

#define draw_textBLOCK		0x40			/* Display with background block */

#define draw_textmaskJUST	0x30			/* Masks for display/justification */
#define draw_textmaskDISP	0x07

#ifndef NAMELEN
#define NAMELEN		20				/* Name length for library object */
#endif

struct draw_box						/* Rectangle (bounding box, etc.) */
{
	int x0,y0;					/* Lower left */
	int x1,y1;					/* Upper right */
};

struct draw_pathstyle					/* Path style */
{
	unsigned char joincapwind;			/* Join/cap/winding/dashed */
	unsigned char overlay;				/* Overlay width fraction (1/256) */
	unsigned char tricapwid;			/* 1/16th of line width */
	unsigned char tricaphei;			/* 1/16th of line width */
};

struct draw_textstyle					/* Text style */
{
	draw_fontid fontref;				/* Font reference */
	unsigned char flags;				/* Kern/reverse flags */
	short spacing;					/* Block line spacing */
};

typedef struct draw_pathstyle draw_pathstyle;
typedef struct draw_textstyle draw_textstyle;
typedef struct draw_box draw_box;

struct draw_dashstrhdr					/* Dash pattern header */
{
	int dashstart;					/* Distance into pattern */
	int dashcount;					/* Number of elements */
};

struct draw_dashstr					/* Dash pattern */
{
	int dashstart;					/* Distance into pattern */
	int dashcount;					/* Number of elements */
	int dashelements[25];				/* Elements of pattern */
};

struct draw_jointspec					/* As passed to Draw module */
{
	char join;
	char endcap;
	char startcap;
	char reserved;
	int mitrelimit;
	short endtricapwid;
	short endtricaphei;
	short starttricapwid;
	short starttricaphei;
};

typedef struct draw_dashstrhdr draw_dashstrhdr;
typedef struct draw_dashstr draw_dashstr;
typedef struct draw_jointspec draw_jointspec;

/************************************************************************/
/*									*/
/*	Types for path data						*/
/*									*/
/************************************************************************/

enum draw_path_tagtype					/* Path elements */
{
	draw_pathTERM =		0,			/* end of path */
	draw_pathMOVE = 	2,			/* move, starts new subpath */
	draw_pathLINE = 	8,			/* line */
	draw_pathCURVE =	6,			/* bezier curve with 2 control points */
	draw_pathCLOSE =	5,			/* close subpath with a line */

	draw_pathARC =		100,			/* arc segment (while creating) */
	draw_pathFOUR =		102			/* four-point curve (ditto) */
};
							/* Direction cues for Vector */
#define draw_pathARCCW		((draw_path_tagtype) (draw_pathARC+0))
#define draw_pathARCACW		((draw_path_tagtype) (draw_pathARC+1))

typedef enum draw_path_tagtype draw_path_tagtype;

struct draw_moveElement					/* Move to */
{
	draw_path_tagtype tag;
	int x,y;
};

struct draw_lineElement					/* Line to */
{
	draw_path_tagtype tag;
	int x,y;
};

struct draw_curveElement				/* Curve to */
{
	draw_path_tagtype tag;
	int x1,y1;
	int x2,y2;
	int x3,y3;
};

struct draw_closeElement				/* Close subpath */
{
	draw_path_tagtype tag;
};

struct draw_termElement					/* Terminate path */
{
	draw_path_tagtype tag;
};

union draw_largestElement				/* For allocation */
{
	struct draw_moveElement a;
	struct draw_lineElement b;
	struct draw_curveElement c;
	struct draw_closeElement d;
	struct draw_termElement e;
};

union draw_pathElementPtr				/* Path element */
{
	struct draw_moveElement *move;
	struct draw_lineElement *line;
	struct draw_curveElement *curve;
	struct draw_closeElement *close;
	struct draw_termElement *term;
	char *bytep;
	int *wordp;
};

typedef union draw_largestElement draw_largestElement;
typedef union draw_pathElementPtr draw_pathElementPtr;

/************************************************************************/
/*									*/
/*	Data types and values for extended tags				*/
/*									*/
/************************************************************************/

typedef unsigned char draw_tagtyp;			/* Basic object tag */
typedef unsigned char draw_layer;			/* Layer number */
typedef unsigned char draw_objflags;			/* Object flags */

#define draw_objFONTLIST	0			/* Font table */
#define draw_objTEXT 		1			/* Text object */
#define draw_objPATH 		2			/* Path object */
#define draw_objSPRITE		5			/* Sprite object */
#define draw_objGROUP 		6			/* Group object */
#define draw_objTAGGED		7			/* Tagged object */
#define draw_objTEXTAREA	9			/* Text area */
#define draw_objTEXTCOL		10			/* Text column */

#define draw_objACORNSET	11			/* Acorn's settings object */
#define draw_objTRANSTEXT	12			/* Transformed text object */
#define draw_objTRANSPRITE	13			/* Transformed sprite object */
#define draw_objJPEG		16			/* JPEG image object */

#define draw_objLIB		100			/* Library object */
#define draw_objSET		101			/* Settings object */
#define draw_objSIMREP		102			/* Simple replicate */
#define draw_objGENREP		103			/* General replicate */
#define draw_objGENINST		104			/* General instance */
#define draw_objMASK		105			/* Background mask */
#define draw_objRADIATE		106			/* Angular replicate */
#define draw_objSIMSKEL		107			/* Simple-replicate skeleton */
#define draw_objGENSKEL		108			/* General-replicate skeleton */
#define draw_objPATTERN		109			/* Pattern data */

#define draw_flagNODISPLAY	1			/* Object is never displayed */
#define draw_flagLOCKED		2			/* Object is locked */
#define draw_flagHIDDEN		4			/* Object is temporarily hidden */

struct draw_extag					/* Extended object tag */
{
	draw_tagtyp tag;				/* Basic object type */
	draw_layer layer;				/* Drawing layer */
	draw_objflags flag;				/* Object flags */
	unsigned char spare;				/* For future expansion */
};

typedef struct draw_extag draw_extag;

/************************************************************************/
/*									*/
/*	Structures and types for the various sorts of objects		*/
/*									*/
/************************************************************************/

struct draw_fileheader					/* File header */
{
	char title[4];					/* Ident 'Draw' */
	int majorstamp;					/* Major version */
	int minorstamp;					/* Minor version */
	char progident[12];				/* Creator */
	draw_box bbox;					/* Bounding box */
};

typedef struct draw_fileheader draw_fileheader;

struct draw_objhdr					/* General object header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
};

typedef struct draw_objhdr draw_objhdr;

struct draw_fontliststrhdr				/* Font table header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
};

struct draw_fontliststr					/* Font table */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_fontid fontref;				/* Font reference */
	char fontname[1];				/* Font name */
};

typedef struct draw_fontliststrhdr draw_fontliststrhdr;
typedef struct draw_fontliststr draw_fontliststr;

struct draw_textinfo					/* Common information for text */
{
	draw_coltyp textcolour;				/* Foreground colour */
	draw_coltyp background;				/* Background colour hint */
	draw_textstyle textstyle;			/* Text style and font refernce */
	draw_fontsize fsizex;				/* X point size */
	draw_fontsize fsizey;				/* Y point size */
	draw_objcoord coord;				/* Base point */
};

typedef struct draw_textinfo draw_textinfo;

struct draw_textstrhdr					/* Text object header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_textinfo i;				/* Text information */
};

struct draw_textstr					/* Text object */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_textinfo i;				/* Text information */
	char text[1];					/* Text string */
};

typedef struct draw_textstrhdr draw_textstrhdr;
typedef struct draw_textstr draw_textstr;

struct draw_pathstrhdr					/* Path object header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_coltyp fillcolour;				/* Fill colour */
	draw_coltyp pathcolour;				/* Line colour */
	draw_pathwidth pathwidth;			/* Line width */
	draw_pathstyle pathstyle;			/* Line style */
};

struct draw_pathstr					/* Path object */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_coltyp fillcolour;				/* Fill colour */
	draw_coltyp pathcolour;				/* Line colour */
	draw_pathwidth pathwidth;			/* Line width */
	draw_pathstyle pathstyle;			/* Line style */
	draw_dashstr data;				/* Dash pattern, then path */
};

typedef struct draw_pathstrhdr draw_pathstrhdr;
typedef struct draw_pathstr draw_pathstr;

struct draw_spristrhdr					/* Sprite object header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
};

struct draw_spristr					/* Sprite object */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	sprite_header sprite;				/* Sprite data */
};

typedef struct draw_spristrhdr draw_spristrhdr;
typedef struct draw_spristr draw_spristr;

struct draw_groustrhdr					/* Group object header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_groupnametyp name;				/* Group name */
}; 

struct draw_groustr					/* Group object */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_groupnametyp name;				/* Group name */
	draw_objhdr object[1];				/* Enclosed objects */
};

typedef struct draw_groustrhdr draw_groustrhdr;
typedef struct draw_groustr draw_groustr;

struct draw_taggedstrhdr				/* Tagged object header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	int id;						/* Tag identifier */
};

struct draw_taggedstr					/* Tagged object */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	int id;						/* Tag identifier */
	draw_objhdr object;				/* Enclosed object */
};

typedef struct draw_taggedstrhdr draw_taggedstrhdr;
typedef struct draw_taggedstr draw_taggedstr;

struct draw_textcolstr					/* Text column */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
};

typedef struct draw_textcolstr draw_textcolstr;
typedef struct draw_textcolstr draw_textcolhdr;

struct draw_textareastrhdr				/* Text area header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
};

struct draw_textareahdr					/* Text area start */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_textcolstr column[1];			/* Text columns */
};

struct draw_textareaendhdr				/* Text area end header */
{
	int endmark;					/* Always 0 */
	int blank1;
	int blank2;
	draw_coltyp textcolour;				/* Foreground colour */
	draw_coltyp backcolour;				/* Background colour hint */
};

struct draw_textareaend					/* Text area end */
{
	int endmark;					/* Always 0 */
	int blank1;
	int blank2;
	draw_coltyp textcolour;				/* Foreground colour */
	draw_coltyp backcolour;				/* Background colour hint */
	char text[1];					/* Text string */
};

typedef struct draw_textareastrhdr draw_textareastrhdr;
typedef struct draw_textareahdr draw_textareahdr;
typedef struct draw_textareaendhdr draw_textareaendhdr;
typedef struct draw_textareaend draw_textareaend;

struct draw_setstrhdr					/* Saved settings header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	int version;					/* Structure version */
	int contents;					/* What this contains */
};

struct draw_setstr					/* Saved settings */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	int version;					/* Structure version */
	int contents;					/* What this contains */
	char d[1];					/* Settings data */
};

typedef struct draw_setstrhdr draw_setstrhdr;
typedef struct draw_setstr draw_setstr;

struct draw_simrepstrhdr				/* Simple-replicated object header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_reptyp type;				/* Replication type */
	int hor_num;					/* Number horizontally */
	int hor_sp;					/* Horizontal spacing */
	int vert_num;					/* Number vertically */
	int vert_sp;					/* Vertical spacing */
};

struct draw_simrepstr					/* Simple-replicated object */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_reptyp type;				/* Replication type */
	int hor_num;					/* Number horizontally */
	int hor_sp;					/* Horizontal spacing */
	int vert_num;					/* Number vertically */
	int vert_sp;					/* Vertical spacing */
	draw_objhdr object;				/* Object */
};

typedef struct draw_simrepstrhdr draw_simrepstrhdr;
typedef struct draw_simrepstr draw_simrepstr;

struct draw_geninststr					/* General instance */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
#ifdef NEWGENREP
	draw_transmat mat;				/* Transform matrix */
#endif
};

typedef struct draw_geninststr draw_geninststr;

struct draw_genrepstrhdr				/* General replicate header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
#ifdef NEWGENREP
	int pad[6];					/* Space for matrix in instances */
#endif
};

struct draw_genrephdr					/* General replicate start */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
#ifdef NEWGENREP
	int pad[6];					/* Space for matrix in instances */
#endif
	draw_geninststr inst[1];			/* Object instances */
};

struct draw_genrependhdr				/* General replicate end header */
{
	int endmark;					/* Always 0 */
};

struct draw_genrepend					/* General replicate end */
{
	int endmark;					/* Always 0 */
	draw_objhdr object;				/* Replicated object */
};

typedef struct draw_genrepstrhdr draw_genrepstrhdr;
typedef struct draw_genrephdr draw_genrephdr;
typedef struct draw_genrependhdr draw_genrependhdr;
typedef struct draw_genrepend draw_genrepend;

struct draw_radiatestrhdr				/* Angle-replicated path header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	double angle;					/* Signed angle (radians) */
	int number;					/* Number of copies */
	draw_radtyp centre;				/* Rotation centre */
};

struct draw_radiatestr					/* Angle-replicated path */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	double angle;					/* Signed angle (radians) */
	int number;					/* Number of copies */
	draw_radtyp centre;				/* Rotation centre */
	draw_pathstrhdr path;				/* Object */
};

typedef struct draw_radiatestrhdr draw_radiatestrhdr;
typedef struct draw_radiatestr draw_radiatestr;

struct draw_simskelstr					/* Simple-replicate skeleton */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_reptyp type;				/* Replication type */
	int hor_num;					/* Number horizontally */
	int hor_sp;					/* Horizontal spacing */
	int vert_num;					/* Number vertically */
	int vert_sp;					/* Vertical spacing */
	draw_objhdr object;				/* Object, for bounding box only */
};

typedef struct draw_simskelstr draw_simskelstr;

struct draw_genskelstrhdr				/* General-replicate skeleton header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
#ifdef NEWGENREP
	int pad[6];					/* Space for matrix in instances */
#endif
};

struct draw_genskelhdr					/* General-skeleton start */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
#ifdef NEWGENREP
	int pad[6];					/* Space for matrix in instances */
#endif
	draw_geninststr inst[1];			/* Object instances */
};

struct draw_genskelendhdr				/* General-skeketon end header */
{
	int endmark;					/* Always 0 */
};

struct draw_genskelend					/* General-skeleton end */
{
	int endmark;					/* Always 0 */
	draw_objhdr object;				/* Object, for bounding box only */
};

typedef struct draw_genskelstrhdr draw_genskelstrhdr;
typedef struct draw_genskelhdr draw_genskelhdr;
typedef struct draw_genskelendhdr draw_genskelendhdr;
typedef struct draw_genskelend draw_genskelend;

struct draw_maskstrhdr					/* Path-mask header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
};

struct draw_maskstr					/* Path-mask */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_objhdr object;				/* Background object */
	draw_pathstr path;				/* Masking path */
};

typedef struct draw_maskstrhdr draw_maskstrhdr;
typedef struct draw_maskstr draw_maskstr;

struct draw_transtextstrhdr				/* Transformed text object header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_transmat mat;				/* Transform matrix */
	int flags;					/* Font flags */
	draw_textinfo i;				/* Text information */
};

struct draw_transtextstr				/* Transformed text object */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_transmat mat;				/* Transform matrix */
	int flags;					/* Font flags */
	draw_textinfo i;				/* Text information */
	char text[1];					/* Text string */
};

typedef struct draw_transtextstrhdr draw_transtextstrhdr;
typedef struct draw_transtextstr draw_transtextstr;

struct draw_transpristrhdr				/* Transformed sprite object header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_transmat mat;				/* Transform matrix */
};

struct draw_transpristr					/* Transformed sprite object */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Object size */
	draw_box bbox;					/* Bounding box */
	draw_transmat mat;				/* Transform matrix */
	sprite_header sprite;				/* Sprite data */
};

typedef struct draw_transpristrhdr draw_transpristrhdr;
typedef struct draw_transpristr draw_transpristr;

struct draw_libstr					/* Library object */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Size (including contained object) */
	draw_box bbox;					/* Copy of object's bounding box */
	char name[NAMELEN+1];				/* Object name */
	unsigned int time;				/* Time last updated */
	draw_objhdr object;				/* Object data */
};

struct draw_libstrhdr					/* Library object header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Size (including contained object) */
	draw_box bbox;					/* Copy of object's bounding box */
	char name[NAMELEN+1];				/* Object name */
	unsigned int time;				/* Time last updated */
};

typedef struct draw_libstr draw_libstr;
typedef struct draw_libstrhdr draw_libstrhdr;

struct draw_pattstrhdr					/* Pattern object header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Size (including pattern data) */
	draw_box bbox;					/* Pattern bounding box */
	int id;						/* Pattern ID */
};

struct draw_pattstr					/* Pattern object */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Size (including pattern data) */
	draw_box bbox;					/* Pattern bounding box */
	int id;						/* Pattern ID */
	int data[1];					/* Pattern data */
};

typedef struct draw_pattstrhdr draw_pattstrhdr;
typedef struct draw_pattstr draw_pattstr;

struct draw_jpegstrhdr					/* JPEG object header */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Size (including image data) */
	draw_box bbox;					/* Pattern bounding box */
	unsigned int width,height;			/* Untransformed size of image */
	unsigned int pdx,pdy;				/* Pixel density of image */
	draw_transmat mat;				/* Transform matrix */
	unsigned int datasize;				/* Size of image data */
};

struct draw_jpegstr					/* JPEG object */
{
	draw_extag t;					/* Extended tag */
	draw_sizetyp size;				/* Size (including image data) */
	draw_box bbox;					/* Pattern bounding box */
	unsigned int width,height;			/* Untransformed size of image */
	unsigned int pdx,pdy;				/* Pixel density of image */
	draw_transmat mat;				/* Transform matrix */
	unsigned int datasize;				/* Size of image data */
	int data[1];					/* Image data */
};

typedef struct draw_jpegstrhdr draw_jpegstrhdr;
typedef struct draw_jpegstr draw_jpegstr;

/************************************************************************/
/*									*/
/*	Types for object-level interface				*/
/*									*/
/************************************************************************/

#define draw_objectNONE		((draw_object) -3)	/* Special object location codes */
#define draw_objectFIRST	((draw_object) -1)
#define draw_objectLAST		((draw_object) -2)

typedef void *draw_objectType;				/* General object pointer */

/************************************************************************/
/*									*/
/*	Types for diagram-level interface				*/
/*									*/
/************************************************************************/

struct draw_diag					/* Diagram data */
{
	void *data;					/* Pointer to data */
	int length;					/* Current length */
};

typedef struct draw_diag draw_diag;			/* Diagram data */
typedef int draw_object;				/* Object handle (diagram offset) */

/************************************************************************/
/*									*/
/*	Macros for access to structures from pointers			*/
/*									*/
/************************************************************************/

#define draw_objTag(p)		(((draw_objhdr *) (p))->t)
#define draw_objSize(p)		(((draw_objhdr *) (p))->size)
#define draw_objBbox(p)		(((draw_objhdr *) (p))->bbox)

#define draw_fileBbox(p)	(((draw_fileheader *) (p))->bbox)

#endif

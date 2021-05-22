/* Emacs:          -*- mode:c; mode:riscos-file; c-basic-offset: 4; -*- */
/************************************************************************/
/*									*/
/*	File:		decompress.c		Edit:  22-May-21	*/
/*	Language:	ANSI C						*/
/*	Project:	Draw file compression library			*/
/*	Developed on:	Vector						*/
/*									*/
/*	Purpose:							*/
/*									*/
/*	The functions in this file implement loading & decompression	*/
/*	of Draw files.  Plain Draw files are loaded into memory		*/
/*	in the same format as the file.  Compressed files are		*/
/*	read into memory and decompressed from there, in place if	*/
/*	there is a shortage of memory or we don't know the size of	*/
/*	the decompressed data.						*/
/*									*/
/************************************************************************/
/*									*/
/*	Written by Paul Lebeau and possibly others as part of the	*/
/*	development of Poster, smArt and other products for		*/
/*	4Mation Educational Resources.  Updated and extended by		*/
/*	Jonathan Marten in development of DrawPlus, Vector and		*/
/*	associated products.						*/
/*									*/
/*	Copyright © Paul Lebeau 1991-1992				*/
/*	Copyright © Jonathan Marten 1992-1997				*/
/*									*/
/*	Released under the GNU General Public License;  see the files	*/
/*	README and COPYING in the distribution archive, or		*/
/*	<http://www.gnu.org/licenses/gpl.html> for more information.	*/
/*	Downloads and further information are available at		*/
/*	<http://www.keelhaul.demon.co.uk/acorn/oss/>.			*/
/*									*/
/************************************************************************/


/************************************************************************/
/*									*/
/*	Include files							*/
/*									*/
/************************************************************************/

#include <string.h>					/* String handling routines */

#include "jjm/program.h"				/* Useful macros and definitions */

#include "draw/drawtypes.h"				/* Extended Draw file data types */
#include "draw/drawobject.h"				/* Draw file object-level routines */

#include "riscos/flex.h"				/* Flexible memory allocation */
#include "riscos/err.h"					/* Desktop error reporting */
#include "riscos/xferrecv.h"				/* Data transfer receiving */

#include "sys/visdelay.h"				/* Hourglass system calls */
#include "sys/io.h"					/* Low level I/O utilities */

#include "compress00.h"					/* Internal definitions */
#include "drawLZW.h"					/* TIFF-compatible compression */

#include "drawcomp.h"					/* Draw file compression/depression */

/************************************************************************/
/*									*/
/*	Options								*/
/*									*/
/************************************************************************/

#undef DEBUG_MSGS					/* Keep quiet while decompressing */
#undef STRIP_TAGS					/* Retain tag data when loading */

/************************************************************************/
/*									*/
/*	Macros								*/
/*									*/
/************************************************************************/

#define Error1(s)		err_generateLookup(s)
#define Error2()		err_lastError()
#define Error3(s,a)		err_generateLookup(s,a)
#define Error4(s,a)		return (err_generateLookup(s,a))
#define Index(handle)		((handle)/4)

/************************************************************************/
/*									*/
/*	Local variables							*/
/*									*/
/************************************************************************/

static int mid_diag_gap;				/* Gap left by recently-decompressed object */

/************************************************************************/
/*									*/
/*	DRAWCOMP_GET_COMP_XY - Read two coordinates in compressed	*/
/*	form from the source data using the number of bytes indicated	*/
/*	in the analysis flag word.  Fill in the result data, update	*/
/*	the source offset.						*/
/*									*/
/************************************************************************/

static void drawcomp_get_comp_xy(draw_diag *diag,int *o,int anal,int *dx,int *dy)
{
    register char *from = diag->data;
    register int i,n;
    register unsigned int cd = 0;			/* Need unsigned shift later */

    n = (anal >> 4) & 3;				/* Get the X coordinate */
    for (i = n; i>=0; --i)
    {
	cd >>= 8;
	cd |= from[(*o)++]<<24;				/* Build up from the top byte down */
    }

    i = cd;
    if (n!=3) i >>= 24-n*8;				/* Shift down to correct value */
    *dx = i;

    n = (anal >> 6) & 3;				/* Get the Y coordinate */
    for (i = n; i>=0; --i)
    {
	cd >>= 8;
	cd |= from[(*o)++]<<24;
    }

    i = cd;
    if (n!=3) i >>= 24-n*8;				/* Shift down to correct value */
    *dy = i;
}

/************************************************************************/
/*									*/
/*	DRAWCOMP_DECOMPRESS_DRAW_OBJECT - Decompress the object in	*/
/*	the input (diagram+offset) and store it in the output diagram	*/
/*	at the indicated offset.					*/
/*									*/
/*	Update the input and output offsets.  '*more' is set to FALSE	*/
/*	if we have reached the end of the compressed diagram (no data	*/
/*	will have been stored in this case).				*/
/*									*/
/************************************************************************/

static os_error *drawcomp_decompress_draw_object(draw_diag *diag_in,int *o_in,
                                                draw_diag *diag_out,int *o_out,
                                                int cflags,BOOL *more)
{
#ifndef RISCOS
    unsigned char *from,*c;
#else
    char *from,*c;
#endif
    int *to;
    int p;
    int i,j,k,m;
    int lx,ly,dx,dy;
    int obj_type,obj_comp_vers;
    int obj_comp_size,orig_size_p;
    int obj_end;
    int obj_bbox;
    int obj_sz;
    unsigned int u,anal;
    BOOL end;
    BOOL redo_bbox;
    os_error *ep = NULL;				/* For error return */

    from = diag_in->data;				/* Source pointer */
    to = (int *) diag_out->data;			/* Destination pointer */

    obj_sz = (cflags & CFLAGS_OBJBYTES)+1;		/* Size of object tag */
    obj_type = 0;
    for (i = j = 0; i<obj_sz; ++i)
    {
	obj_type |= from[(*o_in)++]<<j;
	j += 8;
    }

    i = obj_type<<(32-(obj_sz*8));			/* Have we reached the end? */
    i = i>>(32-(obj_sz*8));
    if (i==-1)
    {
	*more = FALSE;					/* Yes, indicate to client */
	return (NULL);
    }

    *more = TRUE;					/* No, more to come */
    redo_bbox = FALSE;					/* Don't need bounding box yet */

    obj_comp_vers = from[(*o_in)++];			/* Object compression type */
    obj_comp_size = get4(from,o_in);			/* Original object size */
    obj_end = *o_in-(5+obj_sz)+obj_comp_size;		/* End of compressed object */
    p = *o_out/4;

    to[p++] = obj_type;					/* Store object type */
    orig_size_p = p++;					/* Note where to put expanded size */
#ifndef RISCOS
    c = (unsigned char *) &to[p];
#else
    c = (char *) &to[p];
#endif

    if (obj_comp_vers<CC_SPECIALS_START)		/* General compression type */
    {
	switch (obj_comp_vers)
	{
case CC_NO_COMPRESSION:					/* No compression at all */
	    to[orig_size_p] = obj_comp_size+(3-obj_sz);
	    i = obj_comp_size-(5+obj_sz);
	    memcpy(c,from+(*o_in),i);
	    *o_in += i;
	    p += i>>2;
	    break;

case CC_LZW_ALL:					/* All LZW'd from +8 onwards */
	    i = get4(from,o_in);			/* Original object size */
	    to[orig_size_p] = i;			/* Store expanded size */
	    lzw_decompress(c,0,from+(*o_in));
	    p += (i-8)>>2;
	    *o_in += obj_comp_size-(9+obj_sz);
	    break;

case CC_LZW_BBOX:					/* Bounding box kept, rest LZW'd */
	    i = get4(from,o_in);			/* Original object size */
	    to[orig_size_p] = i;			/* Store expanded size */
	    for (j = 1; j<=16; j++) *c++ = from[(*o_in)++];
	    p += 4;

	    lzw_decompress(c,0,from+(*o_in));
	    p += (i-24)>>2;
	    *o_in += obj_comp_size-(25+obj_sz);
	    break;

case CC_LZW_NO_BBOX:					/* Recreate bounding box, rest LZW'd */
	    i = get4(from,o_in);			/* Original object size */
	    to[orig_size_p] = i;			/* Store expanded size */
	    obj_bbox = p;				/* Reserve space for bounding box */
	    redo_bbox = TRUE;
	    p += 4;
	    c += 16;

	    lzw_decompress(c,0,from+(*o_in));
	    p += (i-24)>>2;
	    *o_in += obj_comp_size-(9+obj_sz);
	    break;

default:    Error4("decomp4:Unknown object compression type %d",obj_comp_vers);
	}
    }
    else						/* Object-specific compression type */
    {
	switch (obj_type)
	{
case draw_objPATH:
	    switch (obj_comp_vers)			/* Path object */
	    {
case CC_PATH_PACK_COORDS:
		i = get4(from,o_in);			/* Read the orig size of the object */
		to[orig_size_p] = i;			/* Write out the 'expanded' size */

		obj_bbox = p;				/* Provide a spot for the bbox to go later */
		redo_bbox = TRUE;
		p += 4;
		c += 16;				/* Copy the first three path parameters */
		for (i = 0; i<12; i++) *c++ = from[(*o_in)++];
		p += 3;

		u = 1;
		if (from[*o_in] & (1<<7))		/* Is there a dash pattern ? */
		{
		    i = fetch4(from,*o_in+8);
		    u += (2 + i);
		}
		p += u;
		u *= 4;
							/* Copy path style word and any dash pattern data */
		for (; u>0; u--) *c++ = from[(*o_in)++];

		u = from[(*o_in)++];			/* Element type */
		lx = 0;
		ly = 0;
		end = FALSE;
		while (!end)				/* Decompress the path elements */
		{
		    switch (u & 0xf)
		    {
case 0:							/* end */
			end = TRUE;			/* Flag the end of path but still fall */
							/* through and copy element tag.   */

case 5:							/* close */
			to[p++] = (u & 0xf);		/* No x,y only the element tag */
			break;

case 2:							/* move */
			lx = 0;				/* Reset last x/y and fall through to next part */
			ly = 0;

case 8:							/* line */
			anal = u & 0xf0;		/* Read element tag */
			u = (u & 0xf);

			to[p++] = u;			/* Get the x & y coords */
			drawcomp_get_comp_xy(diag_in, o_in, anal, &dx, &dy);
#ifdef FLEX_BUDGING					/* Calling a fn can cause flex pointers to change so... */
			from = diag_in->data;
			to = (int *) diag_out->data;
#endif
			lx += dx;  to[p++] = lx;
			ly += dy;  to[p++] = ly;
			break;

case 6:							/* bezier */
			anal = u & 0xf0;		/* Read element tag */
			u = (u & 0xf);

			to[p++] = u;			/* Get the x & y coords */
			drawcomp_get_comp_xy(diag_in, o_in, anal, &dx, &dy);
#ifdef FLEX_BUDGING
			from = diag_in->data;
			to = (int *) diag_out->data;
#endif
			lx += dx;  to[p++] = lx;
			ly += dy;  to[p++] = ly;

			drawcomp_get_comp_xy(diag_in, o_in, anal, &dx, &dy);
#ifdef FLEX_BUDGING
			from = diag_in->data;
			to = (int *) diag_out->data;
#endif
			lx += dx;  to[p++] = lx;
			ly += dy;  to[p++] = ly;

			drawcomp_get_comp_xy(diag_in, o_in, anal, &dx, &dy);
#ifdef FLEX_BUDGING
			from = diag_in->data;
			to = (int *) diag_out->data;
#endif
			lx += dx;  to[p++] = lx;
			ly += dy;  to[p++] = ly;
			break;

default:		Error4("comp3:Unknown path element type %d",u);
		    }

		    if (!end) u = from[(*o_in)++];	/* Element type */
		}
		break;

default:	Error4("decomp4:Unknown path object compression type %d",obj_comp_vers);
	    }
	    break;

case draw_objGROUP:
	    switch (obj_comp_vers)			/* group object */
	    {
case CC_GROUP_SIMPLE:
    		obj_bbox = p;				/* Provide a spot for the bbox to go later */
		redo_bbox = TRUE;
		p += 4;
		c += 16;
							/* Put in a blank group name to replace the skipped one */
#ifdef __GNUC__						/* 2 warnings about the below, */
		memset(&to[p],' ',12); p += 3;		/* not surprising really */
#else
		to[p++] = to[p++] = to[p++] = '    ';
#endif
		k = obj_comp_size;			/* Actually 'no. of sub-objects' in group case */
		j = 0;

		i = p*4;
		while (j<k)
		{
		    visdelay_percent(*o_in*100/diag_in->length);
							/* Run through the group attempting to */
							/* decompress the objects it contains */
		    ep = drawcomp_decompress_draw_object(diag_in,o_in,diag_out,&i,cflags,&m);
		    if (ep!=NULL) return (ep);
		    j++;
		}
		p = i/4;

		to[orig_size_p] = (i-obj_bbox*4+8);	/* Write out the 'expanded' size */
		break;

default:	Error4("decomp4:Unknown group object compression type %d",obj_comp_vers);
	    }
	    break;

case draw_objTAGGED:
	    if ((obj_comp_vers!=CC_TAGGED_SIMPLE) && (obj_comp_vers!=CC_TAGGED_EXTRALZW))
	    {
		Error4("decomp4:Unknown tagged object compression type %d",obj_comp_vers);
	    }

	    obj_bbox = p;				/* Provide a spot for the bbox to go later */
	    redo_bbox = TRUE;
	    p += 4;

	    to[p++] = obj_comp_size;			/* Copy tagged object identifier */
							/* (tagged objects have no 'compressed size' word, */
							/* so 'obj_comp_size' is actually the tag I.D. */
	    i = p*4;					/* Decompress the 'draw-able' part */
	    ep = drawcomp_decompress_draw_object(diag_in, o_in, diag_out, &i, cflags, &m);
	    if (ep!=NULL) return (ep);
	    p = i/4;

#ifdef FLEX_BUDGING
	    from = diag_in->data;
	    to = (int *) diag_out->data;
#endif
#ifndef RISCOS
	    c = (unsigned char *) &to[p];
#else
	    c = (char *) &to[p];
#endif
	    obj_comp_size = get4(from,o_in);		/* Get 'compressed size of extra data' word */
	    obj_end = *o_in + obj_comp_size;

	    if (obj_comp_size!=0)			/* Now transfer across the tagged object 'extra data' */
	    {
		switch (obj_comp_vers)
		{
case CC_TAGGED_SIMPLE:
    		    memcpy(c,from+(*o_in),obj_comp_size);
		    p += (obj_comp_size/4);
		    *o_in = obj_end;
		    break;

case CC_TAGGED_EXTRALZW:
    		    j = get4(from,o_in);		/* Get 'original size of extra data' word */
		    obj_end += 4;

		    lzw_decompress(c,0,from+(*o_in));
		    p += (j/4);
		    *o_in = obj_end;
		    break;
		}
	    }

	    to[orig_size_p] = ((p-obj_bbox)*4+8);	/* Write out the 'expanded' size */
	    break;

case draw_objTEXTAREA:
	    if ((obj_comp_vers!=CC_TEXTAREA_SIMPLE) && (obj_comp_vers!=CC_TEXTAREA_TEXTLZW))
	    {
		Error4("decomp4:Unknown text area compression type %d",obj_comp_vers);
	    }

	    j = get4(from,o_in);			/* Get the 'expanded' (original) size */
	    to[orig_size_p] = j;

	    obj_bbox = p;				/* Provide a spot for the bbox to go later */
	    redo_bbox = TRUE;
	    p += 4;
	    c += 16;

	    i = p*4;					/* Decompress the column objects */
	    while (((char *) diag_in->data)[*o_in]==draw_objTEXTCOL)
	    {
		ep = drawcomp_decompress_draw_object(diag_in,o_in,diag_out,&i,cflags,&m);
		if (ep!=NULL) return (ep);
	    }
	    p = i/4;

#ifdef FLEX_BUDGING
	    from = diag_in->data;
	    to = (int *) diag_out->data;
#endif
#ifndef RISCOS
	    c = (unsigned char *) &to[p];
#else
	    c = (char *) &to[p];
#endif
	    to[p++] = 0;				/* Add a zero word as an 'end of columns' indicator */
	    c += 4;
	    *o_in += 1;					/* Skip the compressed version of this indicator */

	    switch (obj_comp_vers)			/* Now transfer across the text-area data and text */
	    {
case CC_TEXTAREA_SIMPLE:
		i = obj_end - *o_in;			/* Aside: i should be multiple of 4 bytes */
		memcpy(c,from+(*o_in),i);
		p += (i/4);
		*o_in = obj_end;
		break;

case CC_TEXTAREA_TEXTLZW:
		lzw_decompress(c,0,from+(*o_in));
		p = (*o_out+j)/4;
		*o_in = obj_end;
		break;
	    }
	    break;
	}
    }

    if (redo_bbox) draw_setBBox(diag_out,*o_out,FALSE);	/* Regenerate bounding box */
    *o_out = p*4;					/* Update output offset */
    return (NULL);					/* Decompression successful */
}

/************************************************************************/
/*									*/
/*	DRAWCOMP_MIDEXTEND - Open up a gap in the drawing, to make	*/
/*	room for some extra data, and update the length accordingly.	*/
/*	Return TRUE if all is well.					*/
/*									*/
/************************************************************************/

static BOOL drawcomp_midextend(register draw_diag *diag,int offset,int size)
{
    if (!flex_midextend((mem_ptr) &diag->data,offset,size))
    {							/* Extend block and open up gap */
	Error3("loadmem2:No memory to decompress file, size +%d",size);
	return (FALSE);
    }

    diag->length += size;				/* Update diagram length */
    return (TRUE);
}

/************************************************************************/
/*									*/
/*	DRAWCOMP_DECOMPRESS_IN_PLACE - Decompress an object into the	*/
/*	same diagram as it came from, opening up a gap to accommodate	*/
/*	it.  This can reuse a gap that was left behind by an earlier	*/
/*	operation.							*/
/*									*/
/************************************************************************/

static os_error *drawcomp_decompress_in_place(draw_diag *diag_in,int *o_in,int cflags,BOOL *more)
{
#ifndef RISCOS
    unsigned char *from = diag_in->data;		/* Source pointer */
#else
    char *from = diag_in->data;				/* Source pointer */
#endif
    int o_temp = *o_in+mid_diag_gap;
    os_error *ep = NULL;				/* For error return */

    int *to;
    int i,j,k;
    int obj_sz;
    int obj_type,obj_comp_vers;
    int obj_comp_size,obj_full_size;
    int obj_end;
    int obj_bbox;

    obj_sz = (cflags & CFLAGS_OBJBYTES)+1;		/* Number of bytes in tag */
    obj_type = 0;
    for (i = j = 0; i<obj_sz; ++i)
    {
	obj_type |= from[o_temp++]<<j;
	j += 8;
    }

    i = obj_type<<(32-(obj_sz*8));			/* Have we reached the end? */
    i = i>>(32-(obj_sz*8));
    if (i==-1)
    {
	*more = FALSE;					/* Yes, indicate to client */
	return (NULL);
    }
    else *more = TRUE;					/* No, more to come */

    obj_comp_vers = from[o_temp++];			/* Compression type */
    obj_comp_size = get4(from,&o_temp);			/* Compressed size */
    obj_full_size = get4(from,&o_temp);			/* Expanded size */

    obj_end = o_temp+obj_comp_size;
							/* Get the true expanded size of the object */
    if (obj_comp_vers<CC_SPECIALS_START)		/* General compression type */
    {
	switch (obj_comp_vers)
	{
case CC_LZW_ALL:
case CC_LZW_BBOX:
case CC_LZW_NO_BBOX:
	    break;

case CC_NO_COMPRESSION:
	    obj_full_size = obj_comp_size+(3-obj_sz);
	    break;

default:    Error4("decomp4:Unknown object compression type %d",obj_comp_vers);
	}
    }
    else						/* Object-specific compression */
    {
	switch (obj_type)
	{
case draw_objPATH:
	    if (obj_comp_vers!=CC_PATH_PACK_COORDS) Error4("decomp4:Unknown path object compression type %d",obj_comp_vers);
	    break;					/* 'obj_full_size' is as is already read */

case draw_objGROUP:
	    if (obj_comp_vers!=CC_GROUP_SIMPLE) Error4("decomp4:Unknown group object compression type %d",obj_comp_vers);
	    if (!drawcomp_midextend(diag_in,*o_in,sizeof(draw_groustrhdr))) return (Error2());

	    to = (int *) diag_in->data;
	    k  = Index(*o_in);

	    to[k++] = draw_objGROUP;
	    obj_bbox = k+1;				/* Remember spot for the bbox to go later */
	    k += 5;					/* Skip 'size' and 'bbox' for now */
#ifdef __GNUC__						/* 2 warnings about the below, */
	    memset(&to[k],' ',12); k += 3;		/* not surprising really */
#else
	    to[k++] = to[k++] = to[k++] = '    ';	/* Put in blank group name */
#endif
	    o_temp = *o_in;				/* Remember start of group for size later */
	    *o_in += sizeof(draw_groustrhdr);		/* Point to start of group objects */
	    mid_diag_gap += 5+obj_sz;			/* Note we have transferred group header */
	    k = obj_comp_size;				/* Actually '# of sub-objects' in group case */
	    j = 0;

	    while (j<k)					/* Decompress the group objects */
	    {
		visdelay_percent(((*o_in)*100)/diag_in->length);
		ep = drawcomp_decompress_in_place(diag_in,o_in,cflags,more);
		if (ep!=NULL) return (ep);
		++j;
	    }

	    to = (int *) diag_in->data;
	    to[obj_bbox-1] = (*o_in)-o_temp;		/* Set expanded size & generate bounding box */
	    draw_setBBox(diag_in,(obj_bbox*4)-8,FALSE);
	    return (NULL);				/* Escape now, ready for first object after group */

case draw_objTAGGED:
    	    if ((obj_comp_vers!=CC_TAGGED_SIMPLE) && (obj_comp_vers!=CC_TAGGED_EXTRALZW)) Error4("decomp4:Unknown tagged object compression type %d",obj_comp_vers);
	    if (!drawcomp_midextend(diag_in,*o_in,sizeof(draw_taggedstrhdr))) return (Error2());

	    to = (int *) diag_in->data;
	    k  = Index(*o_in);

	    to[k++] = draw_objTAGGED;			/* Insert a tagged object header */
	    obj_bbox = k+1;				/* Remember spot for the bbox to go later */
	    k += 5;					/* Skip 'size' and 'bbox' for now */

	    to[k++] = obj_comp_size;			/* Copy tagged object identifier */
	    o_temp = *o_in;				/* Remember start to calculate size later */
	    *o_in += sizeof(draw_taggedstrhdr);		/* Point to start of included object */
	    mid_diag_gap += 5+obj_sz;			/* Note we have transferred tagged header */

	    ep = drawcomp_decompress_in_place(diag_in,o_in,cflags,more);
	    if (ep!=NULL) return (ep);

	    from = diag_in->data;
	    to = (int *) diag_in->data;

	    i = *o_in+mid_diag_gap;			/* Decompress the 'extra data' part */

	    obj_comp_size = get4(from,&i);		/* 'Compressed size of extra data' word */
	    mid_diag_gap += 4;				/* Account for the word we have just read */

	    if (obj_comp_size!=0)			/* There is some 'extra data' */
	    {
		switch (obj_comp_vers)
		{
case CC_TAGGED_SIMPLE:
		    if (!drawcomp_midextend(diag_in,*o_in,obj_comp_size)) return (Error2());
		    mid_diag_gap += obj_comp_size;
		    i += obj_comp_size;

		    memcpy(from+*o_in,from+i,obj_comp_size);
		    *o_in += obj_comp_size;
		    break;

case CC_TAGGED_EXTRALZW:
		    j = get4(from,&i);			/* 'Original size of extra data' word */
		    if (!drawcomp_midextend(diag_in,*o_in,j)) return (Error2());
		    mid_diag_gap += obj_comp_size+4;
		    i += j;

		    lzw_decompress(from+*o_in,0,from+i);
		    *o_in += j;
		    break;
		}
	    }

	    to[obj_bbox-1] = (*o_in)-o_temp;
	    memcpy(&to[obj_bbox],&to[obj_bbox+7],16);
	    return (NULL);

case draw_objTEXTAREA:
	    if ((obj_comp_vers!=CC_TEXTAREA_SIMPLE) && (obj_comp_vers!=CC_TEXTAREA_TEXTLZW)) Error4("decomp4:Unknown text area compression type %d",obj_comp_vers);
	    break;					/* 'obj_full_size' is as is already read */
	}
    }

    if (mid_diag_gap<obj_full_size)			/* Flex space to decompress object into */
    {
	if (!drawcomp_midextend(diag_in,*o_in,(obj_full_size-mid_diag_gap))) return (Error2());
	mid_diag_gap = obj_full_size;
    }

    o_temp = *o_in+mid_diag_gap;			/* Position of the object to be decompressed */
    obj_comp_size = o_temp;

    k = *o_in;						/* Decompress the object into the gap */
    ep = drawcomp_decompress_draw_object(diag_in,&o_temp,diag_in,o_in,cflags,more);
    if (ep!=NULL) return (ep);

    if (*o_in>k) mid_diag_gap -= obj_full_size;		/* Lose space that has just been decompressed into */
    obj_comp_size = o_temp-obj_comp_size;		/* Work out how big the compressed object was */
    mid_diag_gap += obj_comp_size;			/* Reserve space that compressed object occupied */

    return (NULL);					/* Decompression successful */
}

/************************************************************************/
/*									*/
/*	DRAWCOMP_DECOMPRESS_DIAG - Decompress the compressed Draw	*/
/*	file at 'diag_in' and store it at 'diag_out'.  This is		*/
/*	assumed to have enough memory to store the decompressed		*/
/*	diagram.  If 'diag_out' is NULL, use in-place decompression	*/
/*	and extend 'diag_in' as necessary during the decompression.	*/
/*									*/
/************************************************************************/

static os_error *drawcomp_decompress_diag(draw_diag *diag_in,draw_diag *diag_out)
{
    int o_in,o_out;					/* Offsets into diagram */
    int cflags;						/* Compression flags */
    BOOL more = TRUE;					/* For checking end of diagram */
    os_error *ep;					/* For error return */

    cflags = ((int *) diag_in->data)[5];		/* Original compression flags */

    memcpy(diag_in->data,DRAW_FILE_ID,4);		/* Identifier & creator ID in header */
    memcpy(ptroff(diag_in->data,12),"DrawCompress",12);
    if (diag_out!=NULL)					/* Copy file header */
    {
	memcpy(diag_out->data,diag_in->data,sizeof(draw_fileheader));
    }

    o_in = o_out = sizeof(draw_fileheader);		/* Objects start here */
    mid_diag_gap = 0;					/* No extra space yet */

    while (more)					/* Process objects */
    {
	visdelay_percent(o_in*100/diag_in->length);

	if (diag_out==NULL) ep = drawcomp_decompress_in_place(diag_in,&o_in,cflags,&more);
	else ep = drawcomp_decompress_draw_object(diag_in,&o_in,diag_out,&o_out,cflags,&more);
	if (ep!=NULL) return (ep);
    }

    if (diag_out==NULL)					/* Decompressed in place */
    {
	diag_in->length = o_in;				/* Set diagram length */
	if (mid_diag_gap>0) flex_extend((mem_ptr) &diag_in->data,o_in);
    }
    else diag_out->length = o_out;			/* Set diagram length */

    return (NULL);					/* Decompression successful */
}

/************************************************************************/
/*									*/
/*	DRAWCOMP_READDRAWFILE - Read a standard Draw file, compressed	*/
/*	Draw file, library file or file of any other type from the	*/
/*	given file 'name' and store it in the flex block at 'diag'.	*/
/*	Return -1 if there is a problem;  otherwise, fill in the	*/
/*	file's load and execute addresses, the loaded file type and	*/
/*	compression indicator.						*/
/*									*/
/*	If 'name' is xferrecv_Recv, load via RAM transfer:  in this	*/
/*	case the last Wimp event must have been the DATASAVE.  It is	*/
/*	not necessary for the 'checkimport' to have been done (but it	*/
/*	doesn't matter if it has been), since we do it again here.	*/
/*									*/
/*	If the file is uncompressed, simply load or receive it to the	*/
/*	flex block.  If it is compressed, first try decompressing it	*/
/*	from one diagram to another.  If there is not enough memory	*/
/*	available for this, decompress it in place.			*/
/*									*/
/*	Entry:	diag	Pointer to diagram structure (data pointer	*/
/*			& length).  The data pointer must either be	*/
/*			NULL or point to an allocated flex block.	*/
/*									*/
/*		name	File name to read from, or xfersend_Recv to	*/
/*			do RAM transfer.				*/
/*									*/
/*		any	TRUE if any type of file should be accepted.	*/
/*									*/
/*		cf	Compression flags file was saved with;		*/
/*			nothing stored if this is NULL.			*/
/*									*/
/*		addr	Pointer to 2 longwords, to receive the file's	*/
/*			load and execute addresses;  nothing stored	*/
/*			if this is NULL.				*/
/*									*/
/*	Return:		RiscOS file type of file read,			*/
/*			-1 if error occurred,				*/
/*			-2 if RAM transfer refused.			*/
/*									*/
/*	Note:		This routine can also be told to read a file	*/
/*			of any type, so that it can be used in		*/
/*			"SpriteFix" and others.	 If the flag is TRUE,	*/
/*			then a file which is not a Draw file is simply	*/
/*			loaded into the memory block, and a special	*/
/*			'cflags' code is returned.  Otherwise, an	*/
/*			error is given if the file is not a Draw file.	*/
/*									*/
/************************************************************************/

int drawcomp_readDrawFile(draw_diag *diag,const char *name,BOOL any,int *cf,unsigned int *addr)
{
#ifdef RISCOS
    struct stat st;					/* File information */
#else
    struct iostat st;					/* File information */
#endif
    int type;						/* File type */
    int size;						/* File size */
    register int *hp;					/* File header */
    draw_diag temp;					/* Diagram for decompressing */
    os_error *ep;					/* For error checking */

    if (name==xferrecv_Recv)				/* Load via RAM transfer */
    {
	type = xferrecv_checkimport(&size);		/* Estimated size here */
	if (type==-1) return (drawcomp_typeERROR);
	if (addr!=NULL) addr[0] = addr[1] = 0;		/* Store dummy addresses */
    }
    else						/* Load from file, so get details */
    {
	if ((ep = io_stat(name,&st))!=NULL)		/* Get file information */
	{
err:	    if (diag->data!=NULL) flex_free((mem_ptr) &diag->data);
	    return (drawcomp_typeERROR);
	}

	if (!(st.obj & 0x01))				/* Must be a file */
	{
	    io_generr(name,0x01,st.obj);
	    goto err;
	}

	type = st.type;					/* Note file type */
	size = st.size;					/* and file size */

	if (addr!=NULL)					/* If information required, */
	{
	    addr[0] = st.load;				/* save file addresses */
	    addr[1] = st.exec;
	}
    }
							/* Allocate memory and load/transfer the file */
    if (!flex_extend((mem_ptr) &diag->data,size))	/* Grab memory for it */
    {
	ep = Error3("loadmem:No memory to load file, size %d",size);
	goto err;
    }

    if (name==xferrecv_Recv)				/* Load via RAM transfer */
    {
	size = xferrecv_fleximport((mem_ptr) &diag->data,size);
	if (size==-1) return (drawcomp_typeRAMXFER);	/* RAM transfer refused, will load via file */
	size = xferrecv_xfersize();			/* Get total size */
	name = "";					/* Dummy for error messages */
    }
    else						/* Load from file */
    {							/* into diagram memory */
	if ((ep = io_load(name,diag->data))!=NULL) goto err;
    }
    diag->length = size;				/* Set diagram length */

							/* Check the file type, from the drawing header */
    hp = (int *) diag->data;				/* Point to file header */
    if (memcmp(&hp[0],DRAW_FILE_ID,4)==0)		/* Standard (uncompressed) file */
    {							/* Saved with no compression */
	if (cf!=NULL) *cf = drawcomp_compNOCOMP;
	return (type);					/* Nothing more to do */
    }

    if (memcmp(&hp[0],DRWC_FILE_ID,4)!=0)		/* Must be a compressed file */
    {
	if (any)					/* Accept arbitrary file */
	{
	    if (cf!=NULL) *cf = drawcomp_compNOTDRAW;	/* Not a Draw file */
	    return (type);				/* Nothing more to do */
	}

	ep = Error1("decomp9:Not a standard or compressed Draw file");
	goto err;
    }
							/* Allocate memory and decompress the file */
    if (cf!=NULL) *cf = hp[5];				/* Original compression flags */
    size = hp[3];					/* Tentative expanded size */
    if (size!=0)					/* Original size available */
    {							/* Try to allocate memory */
	if (!flex_alloc((mem_ptr) &temp.data,size)) size = 0;
    }

    if (size!=0)					/* Second diagram available, */
    {							/* do (faster) a-to-b decompression */
	ep = drawcomp_decompress_diag(diag,&temp);
	if (ep!=NULL) goto err;

	flex_free((mem_ptr) &diag->data);		/* Finished with compressed file */
	flex_repoint((mem_ptr) &temp.data,(mem_ptr) &diag->data);
	diag->length = temp.length;			/* Set expanded size */
    }
    else						/* Do (slower) in-place decompression */
    {
	ep = drawcomp_decompress_diag(diag,NULL);
	if (ep!=NULL) goto err;
    }

    return (type);					/* Load successful */
}

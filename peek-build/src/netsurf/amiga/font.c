/*
 * Copyright 2008 - 2010 Chris Young <chris@unsatisfactorysoftware.co.uk>
 *
 * This file is part of NetSurf, http://www.netsurf-browser.org/
 *
 * NetSurf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * NetSurf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include "css/css.h"
#include "render/font.h"
#include "amiga/gui.h"
#include <proto/graphics.h>
#include <proto/diskfont.h>
#include <graphics/rpattr.h>
#include "amiga/font.h"
#include "desktop/options.h"
#include "amiga/utf8.h"
#include "utils/utf8.h"
#include <diskfont/diskfonttag.h>
#include <diskfont/oterrors.h>
#include <proto/Picasso96API.h>
#include <proto/exec.h>
#include <graphics/blitattr.h>
#include "amiga/options.h"
#include <proto/utility.h>
#include "utils/utils.h"

#define NSA_UNICODE_FONT PLOT_FONT_FAMILY_COUNT

static struct OutlineFont *of[PLOT_FONT_FAMILY_COUNT+1];
static struct OutlineFont *ofb[PLOT_FONT_FAMILY_COUNT+1];
static struct OutlineFont *ofi[PLOT_FONT_FAMILY_COUNT+1];
static struct OutlineFont *ofbi[PLOT_FONT_FAMILY_COUNT+1];

int32 ami_font_plot_glyph(struct OutlineFont *ofont, struct RastPort *rp,
		uint16 char1, uint16 char2, uint32 x, uint32 y, uint32 emwidth);
struct OutlineFont *ami_open_outline_font(const plot_font_style_t *fstyle, BOOL fallback);

static bool nsfont_width(const plot_font_style_t *fstyle,
	  const char *string, size_t length,
    int *width);
       
static bool nsfont_position_in_string(const plot_font_style_t *fstyle,
	       const char *string, size_t length,
	  int x, size_t *char_offset, int *actual_x);
       
static bool nsfont_split(const plot_font_style_t *fstyle,
	  const char *string, size_t length,
    int x, size_t *char_offset, int *actual_x);

const struct font_functions nsfont = {
	nsfont_width,
	nsfont_position_in_string,
	nsfont_split
};

bool nsfont_width(const plot_font_style_t *fstyle,
		const char *string, size_t length,
		int *width)
{
	*width = ami_unicode_text(NULL,string,length,fstyle,0,0);

	if(*width <= 0) *width == length; // fudge

	return true;
}

/**
 * Find the position in a string where an x coordinate falls.
 *
 * \param  fstyle       style for this text
 * \param  string       UTF-8 string to measure
 * \param  length       length of string
 * \param  x            x coordinate to search for
 * \param  char_offset  updated to offset in string of actual_x, [0..length]
 * \param  actual_x     updated to x coordinate of character closest to x
 * \return  true on success, false on error and error reported
 */

bool nsfont_position_in_string(const plot_font_style_t *fstyle,
		const char *string, size_t length,
		int x, size_t *char_offset, int *actual_x)
{
	struct TextExtent extent;
	struct TextFont *tfont;
	uint16 *utf16 = NULL, *outf16 = NULL;
	uint16 utf16next = NULL;
	FIXED kern = 0;
	struct OutlineFont *ofont, *ufont = NULL;
	struct GlyphMap *glyph;
	uint32 tx=0,i=0;
	size_t len, utf8len = 0;
	uint8 *utf8;
	uint32 co = 0;
	int utf16charlen;
	ULONG emwidth = (ULONG)((fstyle->size / FONT_SIZE_SCALE) * glob->scale);
	int32 tempx;

	len = utf8_bounded_length(string, length);
	if(utf8_to_enc(string,"UTF-16",length,(char **)&utf16) != UTF8_CONVERT_OK) return false;
	outf16 = utf16;

	if(!(ofont = ami_open_outline_font(fstyle, FALSE))) return false;

	*char_offset = length;

	for(i=0;i<len;i++)
	{
		if (*utf16 < 0xD800 || 0xDFFF < *utf16)
			utf16charlen = 1;
		else
			utf16charlen = 2;

		utf8len = utf8_char_byte_length(string);

		utf16next = utf16[utf16charlen];

		tempx = ami_font_plot_glyph(ofont, NULL, *utf16, utf16next,
					0, 0, emwidth);

		if(tempx == 0)
		{
			if(ufont == NULL)
			{
				ufont = ami_open_outline_font(fstyle, TRUE);
			}

			if(ufont)
			{
				tempx = ami_font_plot_glyph(ufont, NULL, *utf16, utf16next,
							0, 0, emwidth);
			}
/*
			if(tempx == 0)
			{
				tempx = ami_font_plot_glyph(ofont, NULL, 0xfffd, utf16next,
							0, 0, emwidth);
			}
*/
		}

		if(x < (tx + tempx))
		{
			*actual_x = tx;
			i = len+1;
		}
		else
		{
			co += utf8len;
		}

		tx += tempx;
		string += utf8len;
		utf16 += utf16charlen;
	}

	if(co >= (length))
	{
		*actual_x = tx;
		co = length;
	}

	*char_offset = co;

	free(outf16);

	return true;
}


/**
 * Find where to split a string to make it fit a width.
 *
 * \param  fstyle       style for this text
 * \param  string       UTF-8 string to measure
 * \param  length       length of string
 * \param  x            width available
 * \param  char_offset  updated to offset in string of actual_x, [0..length]
 * \param  actual_x     updated to x coordinate of character closest to x
 * \return  true on success, false on error and error reported
 *
 * On exit, [char_offset == 0 ||
 *           string[char_offset] == ' ' ||
 *           char_offset == length]
 */

bool nsfont_split(const plot_font_style_t *fstyle,
		const char *string, size_t length,
		int x, size_t *char_offset, int *actual_x)
{
	struct TextExtent extent;
	ULONG co;
	char *ostr = string;
	struct TextFont *tfont;
	uint16 *utf16 = NULL,*outf16 = NULL;
	uint16 utf16next = 0;
	FIXED kern = 0;
	int utf16charlen = 0;
	struct OutlineFont *ofont, *ufont = NULL;
	struct GlyphMap *glyph;
	uint32 tx=0,i=0;
	size_t len;
	int utf8len, utf8clen = 0;
	int32 tempx = 0;
	ULONG emwidth = (ULONG)((fstyle->size / FONT_SIZE_SCALE) * glob->scale);

	len = utf8_bounded_length(string, length);
	if(utf8_to_enc((char *)string,"UTF-16",length,(char **)&utf16) != UTF8_CONVERT_OK) return false;
	outf16 = utf16;
	if(!(ofont = ami_open_outline_font(fstyle, FALSE))) return false;

	*char_offset = 0;
	*actual_x = 0;

	for(i=0;i<len;i++)
	{
		utf8len = utf8_char_byte_length(string+utf8clen);

		if (*utf16 < 0xD800 || 0xDFFF < *utf16)
			utf16charlen = 1;
		else
			utf16charlen = 2;

		utf16next = utf16[utf16charlen];

		if(x < tx)
		{
			i = length+1;
		}
		else
		{
			if(string[utf8clen] == ' ') //*utf16 == 0x0020)
			{
				*actual_x = tx;
				*char_offset = utf8clen;
			}
		}

		tempx = ami_font_plot_glyph(ofont, NULL, *utf16, utf16next, 0, 0, emwidth);

		if(tempx == 0)
		{
			if(ufont == NULL)
			{
				ufont = ami_open_outline_font(fstyle, TRUE);
			}

			if(ufont)
			{
				tempx = ami_font_plot_glyph(ufont, NULL, *utf16, utf16next,
							0, 0, emwidth);
			}
/*
			if(tempx == 0)
			{
				tempx = ami_font_plot_glyph(ofont, NULL, 0xfffd, utf16next,
							0, 0, emwidth);
			}
*/
		}

		tx += tempx;
		utf16 += utf16charlen;
		utf8clen += utf8len;
	}

	free(outf16);

	return true;
}

/**
 * Open an outline font in the specified size and style
 *
 * \param fstyle font style structure
 * \param default open a default font instead of the one specified by fstyle
 * \return outline font or NULL on error
 */
struct OutlineFont *ami_open_outline_font(const plot_font_style_t *fstyle, BOOL fallback)
{
	struct OutlineFont *ofont;
	char *fontname;
	ULONG ysize;
	int tstyle = 0;
	plot_font_generic_family_t fontfamily;

	if(fallback) fontfamily = NSA_UNICODE_FONT;
		else fontfamily = fstyle->family;

	if ((fstyle->flags & FONTF_ITALIC) || (fstyle->flags & FONTF_OBLIQUE))
		tstyle += NSA_ITALIC;

	if (fstyle->weight >= 700)
		tstyle += NSA_BOLD;

	switch(tstyle)
	{
		case NSA_ITALIC:
			if(ofi[fontfamily]) ofont = ofi[fontfamily];
				else ofont = of[fontfamily];
		break;

		case NSA_BOLD:
			if(ofb[fontfamily]) ofont = ofb[fontfamily];
				else ofont = of[fontfamily];
		break;

		case NSA_BOLDITALIC:
			if(ofbi[fontfamily]) ofont = ofbi[fontfamily];
				else ofont = of[fontfamily];
		break;

		default:
			ofont = of[fontfamily];
		break;
	}

	/* Scale to 16.16 fixed point */
	ysize = fstyle->size * ((1 << 16) / FONT_SIZE_SCALE) * glob->scale;

	if(ESetInfo(&ofont->olf_EEngine,
			OT_DeviceDPI,(72<<16) | 72,
			OT_PointHeight,ysize,
			TAG_END) == OTERR_Success)
	{
		return ofont;
	}

	return NULL;
}

int32 ami_font_plot_glyph(struct OutlineFont *ofont, struct RastPort *rp,
		uint16 char1, uint16 char2, uint32 x, uint32 y, uint32 emwidth)
{
	struct GlyphMap *glyph;
	UBYTE *glyphbm;
	int32 char_advance = 0;
	FIXED kern = 0;

	if(ESetInfo(&ofont->olf_EEngine,
			OT_GlyphCode, char1,
			OT_GlyphCode2, char2,
			TAG_END) == OTERR_Success)
	{
		if(EObtainInfo(&ofont->olf_EEngine,
			OT_GlyphMap8Bit,&glyph,
			TAG_END) == 0)
		{
			glyphbm = glyph->glm_BitMap;
			if(!glyphbm) return 0;

			if(rp)
			{
				BltBitMapTags(BLITA_SrcX, glyph->glm_BlackLeft,
					BLITA_SrcY, glyph->glm_BlackTop,
					BLITA_DestX, x - glyph->glm_X0 + glyph->glm_BlackLeft,
					BLITA_DestY, y - glyph->glm_Y0 + glyph->glm_BlackTop,
					BLITA_Width, glyph->glm_BlackWidth,
					BLITA_Height, glyph->glm_BlackHeight,
					BLITA_Source, glyphbm,
					BLITA_SrcType, BLITT_ALPHATEMPLATE,
					BLITA_Dest, rp,
					BLITA_DestType, BLITT_RASTPORT,
					BLITA_SrcBytesPerRow, glyph->glm_BMModulo,
					TAG_DONE);
			}

			kern = 0;

			if(char2) EObtainInfo(&ofont->olf_EEngine,
								OT_TextKernPair, &kern,
								TAG_END);

			char_advance = (ULONG)(((glyph->glm_Width - kern) * emwidth) / 65536);

			EReleaseInfo(&ofont->olf_EEngine,
				OT_GlyphMap8Bit,glyph,
				TAG_END);
		}
	}

	return char_advance;
}

ULONG ami_unicode_text(struct RastPort *rp,const char *string,ULONG length,const plot_font_style_t *fstyle,ULONG dx, ULONG dy)
{
	uint16 *utf16 = NULL, *outf16 = NULL;
	uint16 utf16next = 0;
	int utf16charlen;
	struct OutlineFont *ofont, *ufont = NULL;
	ULONG i,gx,gy;
	UWORD posn;
	uint32 x=0;
	uint8 co = 0;
	int32 tempx = 0;
	ULONG emwidth = (ULONG)((fstyle->size / FONT_SIZE_SCALE) * glob->scale);

	if(!string || string[0]=='\0') return 0;
	if(!length) return 0;

	if(utf8_to_enc(string,"UTF-16",length,(char **)&utf16) != UTF8_CONVERT_OK) return 0;
	outf16 = utf16;
	if(!(ofont = ami_open_outline_font(fstyle, FALSE))) return 0;

	if(rp) SetRPAttrs(rp,RPTAG_APenColor,p96EncodeColor(RGBFB_A8B8G8R8,fstyle->foreground),TAG_DONE);

	while(*utf16 != 0)
	{
		if (*utf16 < 0xD800 || 0xDFFF < *utf16)
			utf16charlen = 1;
		else
			utf16charlen = 2;

		utf16next = utf16[utf16charlen];

		tempx = ami_font_plot_glyph(ofont, rp, *utf16, utf16next, dx + x, dy, emwidth);

		if(tempx == 0)
		{
			if(ufont == NULL)
			{
				ufont = ami_open_outline_font(fstyle, TRUE);
			}

			if(ufont)
			{
				tempx = ami_font_plot_glyph(ufont, rp, *utf16, utf16next,
							dx + x, dy, emwidth);
			}
/*
			if(tempx == 0)
			{
				tempx = ami_font_plot_glyph(ofont, rp, 0xfffd, utf16next,
							dx + x, dy, emwidth);
			}
*/
		}

		x += tempx;

		utf16 += utf16charlen;
	}

	free(outf16);
	return x;
}

void ami_init_fonts(void)
{
	int i;
	char *bname,*iname,*biname;
	char *deffont;

	switch(option_font_default)
	{
		case PLOT_FONT_FAMILY_SANS_SERIF:
			deffont = strdup(option_font_sans);
		break;
		case PLOT_FONT_FAMILY_SERIF:
			deffont = strdup(option_font_serif);
		break;
		case PLOT_FONT_FAMILY_MONOSPACE:
			deffont = strdup(option_font_mono);
		break;
		case PLOT_FONT_FAMILY_CURSIVE:
			deffont = strdup(option_font_cursive);
		break;
		case PLOT_FONT_FAMILY_FANTASY:
			deffont = strdup(option_font_fantasy);
		break;
		default:
			deffont = strdup(option_font_sans);
		break;
	}

	of[PLOT_FONT_FAMILY_SANS_SERIF] = OpenOutlineFont(option_font_sans,NULL,OFF_OPEN);
	of[PLOT_FONT_FAMILY_SERIF] = OpenOutlineFont(option_font_serif,NULL,OFF_OPEN);
	of[PLOT_FONT_FAMILY_MONOSPACE] = OpenOutlineFont(option_font_mono,NULL,OFF_OPEN);
	of[PLOT_FONT_FAMILY_CURSIVE] = OpenOutlineFont(option_font_cursive,NULL,OFF_OPEN);
	of[PLOT_FONT_FAMILY_FANTASY] = OpenOutlineFont(option_font_fantasy,NULL,OFF_OPEN);
	of[NSA_UNICODE_FONT] = OpenOutlineFont(option_font_unicode,NULL,OFF_OPEN);

	for(i=PLOT_FONT_FAMILY_SANS_SERIF;i<=NSA_UNICODE_FONT;i++)
	{
		if(!of[i])
		{
			char *tmpfontname = NULL;
			switch(i)
			{
				case PLOT_FONT_FAMILY_SANS_SERIF:
					tmpfontname = option_font_sans;
				break;
				case PLOT_FONT_FAMILY_SERIF:
					tmpfontname = option_font_serif;
				break;
				case PLOT_FONT_FAMILY_MONOSPACE:
					tmpfontname = option_font_mono;
				break;
				case PLOT_FONT_FAMILY_CURSIVE:
					tmpfontname = option_font_cursive;
				break;
				case PLOT_FONT_FAMILY_FANTASY:
					tmpfontname = option_font_fantasy;
				break;
				case NSA_UNICODE_FONT:
					tmpfontname = option_font_unicode;
				break;
				default:
					/* should never get here, but just in case */
					tmpfontname = strdup("{unknown font}");
				break;
			}
			warn_user("CompError",tmpfontname);
		}

		if(bname = (char *)GetTagData(OT_BName,0,of[i]->olf_OTagList))
		{
			ofb[i] = OpenOutlineFont(bname,NULL,OFF_OPEN);
		}
		else
		{
			ofb[i] = NULL;
		}

		if(iname = (char *)GetTagData(OT_IName,0,of[i]->olf_OTagList))
		{
			ofi[i] = OpenOutlineFont(iname,NULL,OFF_OPEN);
		}
		else
		{
			ofi[i] = NULL;
		}

		if(biname = (char *)GetTagData(OT_BIName,0,of[i]->olf_OTagList))
		{
			ofbi[i] = OpenOutlineFont(biname,NULL,OFF_OPEN);
		}
		else
		{
			ofbi[i] = NULL;
		}
	}
	if(deffont) free(deffont);
}

void ami_close_fonts(void)
{
	int i=0;

	for(i=PLOT_FONT_FAMILY_SANS_SERIF;i<=NSA_UNICODE_FONT;i++)
	{
		if(of[i]) CloseOutlineFont(of[i],NULL);
		if(ofb[i]) CloseOutlineFont(ofb[i],NULL);
		if(ofi[i]) CloseOutlineFont(ofi[i],NULL);
		if(ofbi[i]) CloseOutlineFont(ofbi[i],NULL);
	}
}

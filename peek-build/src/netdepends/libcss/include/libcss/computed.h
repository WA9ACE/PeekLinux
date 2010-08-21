/*
 * This file is part of LibCSS
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef libcss_computed_h_
#define libcss_computed_h_

#include <libwapcaplet/libwapcaplet.h>

#include <libcss/errors.h>
#include <libcss/functypes.h>
#include <libcss/properties.h>
#include <libcss/types.h>

#include <stdint.h>

struct css_hint;
struct css_select_handler;

enum css_computed_content_type {
	CSS_COMPUTED_CONTENT_NONE		= 0,
	CSS_COMPUTED_CONTENT_STRING		= 1,
	CSS_COMPUTED_CONTENT_URI		= 2,
	CSS_COMPUTED_CONTENT_COUNTER		= 3,
	CSS_COMPUTED_CONTENT_COUNTERS		= 4,
	CSS_COMPUTED_CONTENT_ATTR		= 5,
	CSS_COMPUTED_CONTENT_OPEN_QUOTE		= 6,
	CSS_COMPUTED_CONTENT_CLOSE_QUOTE	= 7,
	CSS_COMPUTED_CONTENT_NO_OPEN_QUOTE	= 8,
	CSS_COMPUTED_CONTENT_NO_CLOSE_QUOTE	= 9
};

typedef struct css_computed_content_item {
	uint8_t type;
	union {
		lwc_string *string;
		lwc_string *uri;
		lwc_string *attr;
		struct {
			lwc_string *name;
			uint8_t style;
		} counter;
		struct {
			lwc_string *name;
			lwc_string *sep;
			uint8_t style;
		} counters;
	} data;	
} css_computed_content_item;

typedef struct css_computed_counter {
	lwc_string *name;
	css_fixed value;
} css_computed_counter;

typedef struct css_computed_clip_rect {
	css_fixed top;
	css_fixed right;
	css_fixed bottom;
	css_fixed left;

	css_unit tunit;
	css_unit runit;
	css_unit bunit;
	css_unit lunit;

	bool top_auto;
	bool right_auto;
	bool bottom_auto;
	bool left_auto;
} css_computed_clip_rect;

typedef struct css_computed_uncommon {
/*
 * border_spacing		  1 + 2(4)	  2(4)
 * clip				  2 + 4(4) + 4	  4(4)
 * letter_spacing		  2 + 4		  4
 * outline_color		  2		  4
 * outline_width		  3 + 4		  4
 * word_spacing			  2 + 4		  4
 * 				---		---
 * 				 52 bits	 40 bytes
 *
 * Encode counter_increment and _reset as an array of name, value pairs,
 * terminated with a blank entry.
 *
 * counter_increment		  1		  sizeof(ptr)
 * counter_reset		  1		  sizeof(ptr)
 * 				---		---
 * 				  2 bits	  2sizeof(ptr) bytes
 *
 * Encode cursor uri(s) as an array of string objects, terminated with a
 * blank entry.
 *
 * cursor			  5		  sizeof(ptr)
 * 				---		---
 * 				  5 bits	  sizeof(ptr) bytes
 *
 * Encode content as an array of content items, terminated with a blank entry.
 *
 * content			  2		  sizeof(ptr)
 * 				---		---
 * 				  2 bits	  sizeof(ptr)
 *
 * 				___		___
 * 				 61 bits	 40 + 4sizeof(ptr) bytes
 *
 * 				  8 bytes	 40 + 4sizeof(ptr) bytes
 * 				===================
 * 				 48 + 4sizeof(ptr) bytes
 *
 * Bit allocations:
 *
 *    76543210
 *  1 llllllcc	letter-spacing | outline-color
 *  2 ooooooob	outline-width  | border-spacing
 *  3 bbbbbbbb	border-spacing
 *  4 wwwwwwir	word-spacing   | counter-increment | counter-reset
 *  5 uuuuu...	cursor         | <unused>
 *  6 cccccccc	clip
 *  7 cccccccc	clip
 *  8 ccccccoo	clip           | content
 */
	uint8_t bits[8];

	css_fixed border_spacing[2];

	css_fixed clip[4];

	css_fixed letter_spacing;

	css_color outline_color;
	css_fixed outline_width;

	css_fixed word_spacing;

	css_computed_counter *counter_increment;
	css_computed_counter *counter_reset;

	lwc_string **cursor;

	css_computed_content_item *content;
} css_computed_uncommon;

struct css_computed_style {
/*
 * background_attachment	  2
 * background_repeat		  3
 * border_collapse		  2
 * border_top_style		  4
 * border_right_style		  4
 * border_bottom_style		  4
 * border_left_style		  4
 * caption_side			  2
 * clear			  3
 * direction			  2
 * display			  5
 * empty_cells			  2
 * float			  2
 * font_style			  2
 * font_variant			  2
 * font_weight			  4
 * list_style_position		  2
 * list_style_type		  4
 * overflow			  3
 * outline_style		  4
 * position			  3
 * table_layout			  2
 * text_align			  4
 * text_decoration		  5
 * text_transform		  3
 * unicode_bidi			  2
 * visibility			  2
 * white_space			  3
 *				---
 *				 84 bits
 *
 * Colours are 32bits of RRGGBBAA
 * Dimensions are encoded as a fixed point value + 4 bits of unit data
 *
 * background_color		  2		  4
 * background_image		  1		  sizeof(ptr)
 * background_position		  1 + 2(4)	  2(4)
 * border_top_color		  2		  4
 * border_right_color		  2		  4
 * border_bottom_color		  2		  4
 * border_left_color		  2		  4
 * border_top_width		  3 + 4		  4
 * border_right_width		  3 + 4		  4
 * border_bottom_width		  3 + 4		  4
 * border_left_width		  3 + 4		  4
 * top				  2 + 4		  4
 * right			  2 + 4		  4
 * bottom			  2 + 4		  4
 * left				  2 + 4		  4
 * color			  1		  4
 * font_size			  4 + 4		  4
 * height			  2 + 4		  4
 * line_height			  2 + 4		  4
 * list_style_image		  1		  sizeof(ptr)
 * margin_top			  2 + 4		  4
 * margin_right			  2 + 4		  4
 * margin_bottom		  2 + 4		  4
 * margin_left			  2 + 4		  4
 * max_height			  2 + 4		  4
 * max_width			  2 + 4		  4
 * min_height			  1 + 4		  4
 * min_width			  1 + 4		  4
 * padding_top			  1 + 4		  4
 * padding_right		  1 + 4		  4
 * padding_bottom		  1 + 4		  4
 * padding_left			  1 + 4		  4
 * text_indent			  1 + 4		  4
 * vertical_align		  4 + 4		  4
 * width			  2 + 4		  4
 * z_index			  2		  4
 * 				---		---
 *				181 bits	140 + 2sizeof(ptr) bytes
 *
 * Encode font family as an array of string objects, terminated with a 
 * blank entry.
 *
 * font_family			  3		  sizeof(ptr)
 * 				---		---
 * 				  3 bits	  sizeof(ptr)
 *
 * Encode quotes as an array of string objects, terminated with a blank entry.
 *
 * quotes			  1		  sizeof(ptr)
 * 				---		---
 * 				  1 bit		  sizeof(ptr) bytes
 *
 * 				___		___
 *				269 bits	140 + 4sizeof(ptr) bytes
 *
 *				 34 bytes	140 + 4sizeof(ptr) bytes
 *				===================
 *				174 + 4sizeof(ptr) bytes
 *
 * Bit allocations:
 *
 *    76543210
 *  1 vvvvvvvv	vertical-align
 *  2 ffffffff	font-size
 *  3 ttttttti	border-top-width    | background-image
 *  4 rrrrrrrc	border-right-width  | color
 *  5 bbbbbbbl	border-bottom-width | list-style-image
 *  6 lllllllq	border-left-width   | quotes
 *  7 ttttttcc	top                 | border-top-color
 *  8 rrrrrrcc	right               | border-right-color
 *  9 bbbbbbcc	bottom              | border-bottom-color
 * 10 llllllcc	left                | border-left-color
 * 11 hhhhhhbb	height              | background-color
 * 12 llllllzz	line-height         | z-index
 * 13 ttttttbb	margin-top          | background-attachment
 * 14 rrrrrrbb	margin-right        | border-collapse
 * 15 bbbbbbcc	margin-bottom       | caption-side
 * 16 lllllldd	margin-left         | direction
 * 17 mmmmmmee	max-height          | empty-cells
 * 18 mmmmmmff	max-width           | float
 * 19 wwwwwwff	width               | font-style
 * 20 mmmmmbbb	min-height          | background-repeat
 * 21 mmmmmccc	min-width           | clear
 * 22 tttttooo	padding-top         | overflow
 * 23 rrrrrppp	padding-right       | position
 * 24 bbbbb...	padding-bottom      | <unused>
 * 25 lllllttt	padding-left        | text-transform
 * 26 tttttwww	text-indent         | white-space
 * 27 bbbbbbbb	background-position
 * 28 bdddddff	background-position | display               | font-variant
 * 29 tttttfff	text-decoration     | font-family
 * 30 ttttrrrr	border-top-style    | border-right-style
 * 31 bbbbllll	border-bottom-style | border-left-style
 * 32 ffffllll	font-weight         | list-style-type
 * 33 oooottuu	outline-style       | table-layout          | unicode-bidi
 * 34 vvlltttt	visibility          | list-style-position   | text-align
 */
	uint8_t bits[34];

	uint8_t unused[2];

	css_color background_color;
	lwc_string *background_image;
	css_fixed background_position[2];

	css_color border_color[4];
	css_fixed border_width[4];

	css_fixed top;
	css_fixed right;
	css_fixed bottom;
	css_fixed left;

	css_color color;

	css_fixed font_size;

	css_fixed height;

	css_fixed line_height;

	lwc_string *list_style_image;

	css_fixed margin[4];

	css_fixed max_height;
	css_fixed max_width;

	css_fixed min_height;
	css_fixed min_width;

	css_fixed padding[4];

	css_fixed text_indent;

	css_fixed vertical_align;

	css_fixed width;

	int32_t z_index;

	lwc_string **font_family;

	lwc_string **quotes;

	css_computed_uncommon *uncommon;/**< Uncommon properties */
	void *aural;			/**< Aural properties */
	void *page;			/**< Page properties */

	css_allocator_fn alloc;
	void *pw;
};

css_error css_computed_style_create(css_allocator_fn alloc, void *pw,
		css_computed_style **result);
css_error css_computed_style_destroy(css_computed_style *style);

css_error css_computed_style_initialise(css_computed_style *style,
		struct css_select_handler *handler, void *pw);

css_error css_computed_style_compose(const css_computed_style *parent,
		const css_computed_style *child,
		css_error (*compute_font_size)(void *pw,
				const struct css_hint *parent, 
				struct css_hint *size),
		void *pw,
		css_computed_style *result);

/******************************************************************************
 * Property accessors below here                                              *
 ******************************************************************************/

static inline uint8_t css_computed_font_size(const css_computed_style *style, 
		css_fixed *length, css_unit *unit);
static inline uint8_t css_computed_line_height(const css_computed_style *style, 
		css_fixed *length, css_unit *unit);
static inline uint8_t css_computed_position(const css_computed_style *style);


#define CSS_LETTER_SPACING_INDEX 0
#define CSS_LETTER_SPACING_SHIFT 2
#define CSS_LETTER_SPACING_MASK  0xfc
static inline uint8_t css_computed_letter_spacing(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	if (style->uncommon != NULL) {
		uint8_t bits = style->uncommon->bits[CSS_LETTER_SPACING_INDEX];
		bits &= CSS_LETTER_SPACING_MASK;
		bits >>= CSS_LETTER_SPACING_SHIFT;

		/* 6bits: uuuutt : unit | type */

		if ((bits & 3) == CSS_LETTER_SPACING_SET) {
			*length = style->uncommon->letter_spacing;
			*unit = (css_unit) (bits >> 2);
		}

		return (bits & 3);
	}

	return CSS_LETTER_SPACING_NORMAL;
}
#undef CSS_LETTER_SPACING_MASK
#undef CSS_LETTER_SPACING_SHIFT
#undef CSS_LETTER_SPACING_INDEX

#define CSS_OUTLINE_COLOR_INDEX 0
#define CSS_OUTLINE_COLOR_SHIFT 0
#define CSS_OUTLINE_COLOR_MASK  0x3
static inline uint8_t css_computed_outline_color(
		const css_computed_style *style, css_color *color)
{
	if (style->uncommon != NULL) {
		uint8_t bits = style->uncommon->bits[CSS_OUTLINE_COLOR_INDEX];
		bits &= CSS_OUTLINE_COLOR_MASK;
		bits >>= CSS_OUTLINE_COLOR_SHIFT;

		/* 2bits: tt : type */

		if ((bits & 3) == CSS_OUTLINE_COLOR_COLOR) {
			*color = style->uncommon->outline_color;
		}

		return (bits & 3);
	}

	return CSS_OUTLINE_COLOR_INVERT;
}
#undef CSS_OUTLINE_COLOR_MASK
#undef CSS_OUTLINE_COLOR_SHIFT
#undef CSS_OUTLINE_COLOR_INDEX

#define CSS_OUTLINE_WIDTH_INDEX 1
#define CSS_OUTLINE_WIDTH_SHIFT 1
#define CSS_OUTLINE_WIDTH_MASK  0xfe
static inline uint8_t css_computed_outline_width(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	if (style->uncommon != NULL) {
		uint8_t bits = style->uncommon->bits[CSS_OUTLINE_WIDTH_INDEX];
		bits &= CSS_OUTLINE_WIDTH_MASK;
		bits >>= CSS_OUTLINE_WIDTH_SHIFT;

		/* 7bits: uuuuttt : unit | type */

		if ((bits & 7) == CSS_OUTLINE_WIDTH_WIDTH) {
			*length = style->uncommon->outline_width;
			*unit = (css_unit) (bits >> 3);
		}

		return (bits & 7);
	}

	*length = INTTOFIX(2);
	*unit = CSS_UNIT_PX;

	return CSS_OUTLINE_WIDTH_WIDTH;
}
#undef CSS_OUTLINE_WIDTH_MASK
#undef CSS_OUTLINE_WIDTH_SHIFT
#undef CSS_OUTLINE_WIDTH_INDEX

#define CSS_BORDER_SPACING_INDEX 1
#define CSS_BORDER_SPACING_SHIFT 0
#define CSS_BORDER_SPACING_MASK  0x1
#define CSS_BORDER_SPACING_INDEX1 2
#define CSS_BORDER_SPACING_SHIFT1 0
#define CSS_BORDER_SPACING_MASK1 0xff
static inline uint8_t css_computed_border_spacing(
		const css_computed_style *style, 
		css_fixed *hlength, css_unit *hunit,
		css_fixed *vlength, css_unit *vunit)
{
	if (style->uncommon != NULL) {
		uint8_t bits = style->uncommon->bits[CSS_BORDER_SPACING_INDEX];
		bits &= CSS_BORDER_SPACING_MASK;
		bits >>= CSS_BORDER_SPACING_SHIFT;

		/* 1 bit: type */
		if (bits == CSS_BORDER_SPACING_SET) {
			uint8_t bits1 = 
				style->uncommon->bits[CSS_BORDER_SPACING_INDEX1];
			bits1 &= CSS_BORDER_SPACING_MASK1;
			bits1 >>= CSS_BORDER_SPACING_SHIFT1;

			/* 8bits: hhhhvvvv : hunit | vunit */

			*hlength = style->uncommon->border_spacing[0];
			*hunit = (css_unit) (bits1 >> 4);

			*vlength = style->uncommon->border_spacing[1];
			*vunit = (css_unit) (bits1 & 0xf);
		}

		return bits;
	} else {
		*hlength = *vlength = 0;
		*hunit = *vunit = CSS_UNIT_PX;
	}

	return CSS_BORDER_SPACING_SET;
}
#undef CSS_BORDER_SPACING_MASK1
#undef CSS_BORDER_SPACING_SHIFT1
#undef CSS_BORDER_SPACING_INDEX1
#undef CSS_BORDER_SPACING_MASK
#undef CSS_BORDER_SPACING_SHIFT
#undef CSS_BORDER_SPACING_INDEX

#define CSS_WORD_SPACING_INDEX 3
#define CSS_WORD_SPACING_SHIFT 2
#define CSS_WORD_SPACING_MASK  0xfc
static inline uint8_t css_computed_word_spacing(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	if (style->uncommon != NULL) {
		uint8_t bits = style->uncommon->bits[CSS_WORD_SPACING_INDEX];
		bits &= CSS_WORD_SPACING_MASK;
		bits >>= CSS_WORD_SPACING_SHIFT;

		/* 6bits: uuuutt : unit | type */

		if ((bits & 3) == CSS_WORD_SPACING_SET) {
			*length = style->uncommon->word_spacing;
			*unit = (css_unit) (bits >> 2);
		}

		return (bits & 3);
	}

	return CSS_WORD_SPACING_NORMAL;
}
#undef CSS_WORD_SPACING_MASK
#undef CSS_WORD_SPACING_SHIFT
#undef CSS_WORD_SPACING_INDEX

#define CSS_COUNTER_INCREMENT_INDEX 3
#define CSS_COUNTER_INCREMENT_SHIFT 1
#define CSS_COUNTER_INCREMENT_MASK  0x2
static inline uint8_t css_computed_counter_increment(
		const css_computed_style *style, 
		const css_computed_counter **counters)
{
	if (style->uncommon != NULL) {
		uint8_t bits = 
			style->uncommon->bits[CSS_COUNTER_INCREMENT_INDEX];
		bits &= CSS_COUNTER_INCREMENT_MASK;
		bits >>= CSS_COUNTER_INCREMENT_SHIFT;

		/* 1bit: type */
		*counters = style->uncommon->counter_increment;

		return bits;
	}

	return CSS_COUNTER_INCREMENT_NONE;
}
#undef CSS_COUNTER_INCREMENT_MASK
#undef CSS_COUNTER_INCREMENT_SHIFT
#undef CSS_COUNTER_INCREMENT_INDEX

#define CSS_COUNTER_RESET_INDEX 3
#define CSS_COUNTER_RESET_SHIFT 0
#define CSS_COUNTER_RESET_MASK  0x1
static inline uint8_t css_computed_counter_reset(
		const css_computed_style *style, 
		const css_computed_counter **counters)
{
	if (style->uncommon != NULL) {
		uint8_t bits = style->uncommon->bits[CSS_COUNTER_RESET_INDEX];
		bits &= CSS_COUNTER_RESET_MASK;
		bits >>= CSS_COUNTER_RESET_SHIFT;

		/* 1bit: type */
		*counters = style->uncommon->counter_reset;

		return bits;
	}

	return CSS_COUNTER_RESET_NONE;
}
#undef CSS_COUNTER_RESET_MASK
#undef CSS_COUNTER_RESET_SHIFT
#undef CSS_COUNTER_RESET_INDEX

#define CSS_CURSOR_INDEX 4
#define CSS_CURSOR_SHIFT 3
#define CSS_CURSOR_MASK  0xf8
static inline uint8_t css_computed_cursor(
		const css_computed_style *style, 
		lwc_string ***urls)
{
	if (style->uncommon != NULL) {
		uint8_t bits = style->uncommon->bits[CSS_CURSOR_INDEX];
		bits &= CSS_CURSOR_MASK;
		bits >>= CSS_CURSOR_SHIFT;

		/* 5bits: type */
		*urls = style->uncommon->cursor;

		return bits;
	}

	return CSS_CURSOR_AUTO;
}
#undef CSS_CURSOR_MASK
#undef CSS_CURSOR_SHIFT
#undef CSS_CURSOR_INDEX

#define CSS_CLIP_INDEX 7
#define CSS_CLIP_SHIFT 2
#define CSS_CLIP_MASK  0xfc
#define CSS_CLIP_INDEX1 5
#define CSS_CLIP_SHIFT1 0
#define CSS_CLIP_MASK1 0xff
#define CSS_CLIP_INDEX2 6
#define CSS_CLIP_SHIFT2 0
#define CSS_CLIP_MASK2 0xff
static inline uint8_t css_computed_clip(
		const css_computed_style *style, 
		css_computed_clip_rect *rect)
{
	if (style->uncommon != NULL) {
		uint8_t bits = style->uncommon->bits[CSS_CLIP_INDEX];
		bits &= CSS_CLIP_MASK;
		bits >>= CSS_CLIP_SHIFT;

		/* 6bits: trblyy : top | right | bottom | left | type */
		if ((bits & 0x3) == CSS_CLIP_RECT) {
			uint8_t bits1; 

			rect->left_auto = (bits & 0x4);
			rect->bottom_auto = (bits & 0x8);
			rect->right_auto = (bits & 0x10);
			rect->top_auto = (bits & 0x20);

			if (rect->top_auto == false ||
					rect->right_auto == false) {
				/* 8bits: ttttrrrr : top | right */
				bits1 = style->uncommon->bits[CSS_CLIP_INDEX1];
				bits1 &= CSS_CLIP_MASK1;
				bits1 >>= CSS_CLIP_SHIFT1;
			} else {
				bits1 = 0;
			}

			rect->top = style->uncommon->clip[0];
			rect->tunit = (css_unit) (bits1 >> 4);

			rect->right = style->uncommon->clip[1];
			rect->runit = (css_unit) (bits1 & 0xf);

			if (rect->bottom_auto == false ||
					rect->left_auto == false) {
				/* 8bits: bbbbllll : bottom | left */
				bits1 = style->uncommon->bits[CSS_CLIP_INDEX2];
				bits1 &= CSS_CLIP_MASK2;
				bits1 >>= CSS_CLIP_SHIFT2;
			} else {
				bits1 = 0;
			}

			rect->bottom = style->uncommon->clip[2];
			rect->bunit = (css_unit) (bits1 >> 4);

			rect->left = style->uncommon->clip[3];
			rect->lunit = (css_unit) (bits1 & 0xf);
		}

		return (bits & 0x3);
	}

	return CSS_CLIP_AUTO;
}
#undef CSS_CLIP_MASK2
#undef CSS_CLIP_SHIFT2
#undef CSS_CLIP_INDEX2
#undef CSS_CLIP_MASK1
#undef CSS_CLIP_SHIFT1
#undef CSS_CLIP_INDEX1
#undef CSS_CLIP_MASK
#undef CSS_CLIP_SHIFT
#undef CSS_CLIP_INDEX

#define CSS_CONTENT_INDEX 7
#define CSS_CONTENT_SHIFT 0
#define CSS_CONTENT_MASK  0x3
static inline uint8_t css_computed_content(
		const css_computed_style *style, 
		const css_computed_content_item **content)
{
	if (style->uncommon != NULL) {
		uint8_t bits = style->uncommon->bits[CSS_CONTENT_INDEX];
		bits &= CSS_CONTENT_MASK;
		bits >>= CSS_CONTENT_SHIFT;

		/* 2bits: type */
		*content = style->uncommon->content;

		return bits;
	}

	return CSS_CONTENT_NORMAL;
}
#undef CSS_CONTENT_MASK
#undef CSS_CONTENT_SHIFT
#undef CSS_CONTENT_INDEX

#define CSS_VERTICAL_ALIGN_INDEX 0
#define CSS_VERTICAL_ALIGN_SHIFT 0
#define CSS_VERTICAL_ALIGN_MASK  0xff
static inline uint8_t css_computed_vertical_align(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_VERTICAL_ALIGN_INDEX];
	bits &= CSS_VERTICAL_ALIGN_MASK;
	bits >>= CSS_VERTICAL_ALIGN_SHIFT;

	/* 8bits: uuuutttt : units | type */
	if ((bits & 0xf) == CSS_VERTICAL_ALIGN_SET) {
		*length = style->vertical_align;
		*unit = (css_unit) (bits >> 4);
	}

	return (bits & 0xf);
}
#undef CSS_VERTICAL_ALIGN_MASK
#undef CSS_VERTICAL_ALIGN_SHIFT
#undef CSS_VERTICAL_ALIGN_INDEX

#define CSS_FONT_SIZE_INDEX 1
#define CSS_FONT_SIZE_SHIFT 0
#define CSS_FONT_SIZE_MASK  0xff
static inline uint8_t css_computed_font_size(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_FONT_SIZE_INDEX];
	bits &= CSS_FONT_SIZE_MASK;
	bits >>= CSS_FONT_SIZE_SHIFT;

	/* 8bits: uuuutttt : units | type */
	if ((bits & 0xf) == CSS_FONT_SIZE_DIMENSION) {
		*length = style->font_size;
		*unit = (css_unit) (bits >> 4);
	}

	return (bits & 0xf);
}
#undef CSS_FONT_SIZE_MASK
#undef CSS_FONT_SIZE_SHIFT
#undef CSS_FONT_SIZE_INDEX

#define CSS_BORDER_TOP_WIDTH_INDEX 2
#define CSS_BORDER_TOP_WIDTH_SHIFT 1
#define CSS_BORDER_TOP_WIDTH_MASK  0xfe
static inline uint8_t css_computed_border_top_width(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_BORDER_TOP_WIDTH_INDEX];
	bits &= CSS_BORDER_TOP_WIDTH_MASK;
	bits >>= CSS_BORDER_TOP_WIDTH_SHIFT;

	/* 7bits: uuuuttt : units | type */
	if ((bits & 0x7) == CSS_BORDER_WIDTH_WIDTH) {
		*length = style->border_width[0];
		*unit = (css_unit) (bits >> 3);
	}

	return (bits & 0x7);
}
#undef CSS_BORDER_TOP_WIDTH_MASK
#undef CSS_BORDER_TOP_WIDTH_SHIFT
#undef CSS_BORDER_TOP_WIDTH_INDEX

#define CSS_BORDER_RIGHT_WIDTH_INDEX 3
#define CSS_BORDER_RIGHT_WIDTH_SHIFT 1
#define CSS_BORDER_RIGHT_WIDTH_MASK  0xfe
static inline uint8_t css_computed_border_right_width(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_BORDER_RIGHT_WIDTH_INDEX];
	bits &= CSS_BORDER_RIGHT_WIDTH_MASK;
	bits >>= CSS_BORDER_RIGHT_WIDTH_SHIFT;

	/* 7bits: uuuuttt : units | type */
	if ((bits & 0x7) == CSS_BORDER_WIDTH_WIDTH) {
		*length = style->border_width[1];
		*unit = (css_unit) (bits >> 3);
	}

	return (bits & 0x7);
}
#undef CSS_BORDER_RIGHT_WIDTH_MASK
#undef CSS_BORDER_RIGHT_WIDTH_SHIFT
#undef CSS_BORDER_RIGHT_WIDTH_INDEX

#define CSS_BORDER_BOTTOM_WIDTH_INDEX 4
#define CSS_BORDER_BOTTOM_WIDTH_SHIFT 1
#define CSS_BORDER_BOTTOM_WIDTH_MASK  0xfe
static inline uint8_t css_computed_border_bottom_width(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_BORDER_BOTTOM_WIDTH_INDEX];
	bits &= CSS_BORDER_BOTTOM_WIDTH_MASK;
	bits >>= CSS_BORDER_BOTTOM_WIDTH_SHIFT;

	/* 7bits: uuuuttt : units | type */
	if ((bits & 0x7) == CSS_BORDER_WIDTH_WIDTH) {
		*length = style->border_width[2];
		*unit = (css_unit) (bits >> 3);
	}

	return (bits & 0x7);
}
#undef CSS_BORDER_BOTTOM_WIDTH_MASK
#undef CSS_BORDER_BOTTOM_WIDTH_SHIFT
#undef CSS_BORDER_BOTTOM_WIDTH_INDEX

#define CSS_BORDER_LEFT_WIDTH_INDEX 5
#define CSS_BORDER_LEFT_WIDTH_SHIFT 1
#define CSS_BORDER_LEFT_WIDTH_MASK  0xfe
static inline uint8_t css_computed_border_left_width(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_BORDER_LEFT_WIDTH_INDEX];
	bits &= CSS_BORDER_LEFT_WIDTH_MASK;
	bits >>= CSS_BORDER_LEFT_WIDTH_SHIFT;

	/* 7bits: uuuuttt : units | type */
	if ((bits & 0x7) == CSS_BORDER_WIDTH_WIDTH) {
		*length = style->border_width[3];
		*unit = (css_unit) (bits >> 3);
	}

	return (bits & 0x7);
}
#undef CSS_BORDER_LEFT_WIDTH_MASK
#undef CSS_BORDER_LEFT_WIDTH_SHIFT
#undef CSS_BORDER_LEFT_WIDTH_INDEX

#define CSS_BACKGROUND_IMAGE_INDEX 2
#define CSS_BACKGROUND_IMAGE_SHIFT 0
#define CSS_BACKGROUND_IMAGE_MASK  0x1
static inline uint8_t css_computed_background_image(
		const css_computed_style *style, 
		lwc_string **url)
{
	uint8_t bits = style->bits[CSS_BACKGROUND_IMAGE_INDEX];
	bits &= CSS_BACKGROUND_IMAGE_MASK;
	bits >>= CSS_BACKGROUND_IMAGE_SHIFT;

	/* 1bit: type */
	*url = style->background_image;

	return bits;
}
#undef CSS_BACKGROUND_IMAGE_MASK
#undef CSS_BACKGROUND_IMAGE_SHIFT
#undef CSS_BACKGROUND_IMAGE_INDEX

#define CSS_COLOR_INDEX 3
#define CSS_COLOR_SHIFT 0
#define CSS_COLOR_MASK  0x1
static inline uint8_t css_computed_color(
		const css_computed_style *style, 
		css_color *color)
{
	uint8_t bits = style->bits[CSS_COLOR_INDEX];
	bits &= CSS_COLOR_MASK;
	bits >>= CSS_COLOR_SHIFT;

	/* 1bit: type */
	*color = style->color;

	return bits;
}
#undef CSS_COLOR_MASK
#undef CSS_COLOR_SHIFT
#undef CSS_COLOR_INDEX

#define CSS_LIST_STYLE_IMAGE_INDEX 4
#define CSS_LIST_STYLE_IMAGE_SHIFT 0
#define CSS_LIST_STYLE_IMAGE_MASK  0x1
static inline uint8_t css_computed_list_style_image(
		const css_computed_style *style, 
		lwc_string **url)
{
	uint8_t bits = style->bits[CSS_LIST_STYLE_IMAGE_INDEX];
	bits &= CSS_LIST_STYLE_IMAGE_MASK;
	bits >>= CSS_LIST_STYLE_IMAGE_SHIFT;

	/* 1bit: type */
	*url = style->list_style_image;

	return bits;
}
#undef CSS_LIST_STYLE_IMAGE_MASK
#undef CSS_LIST_STYLE_IMAGE_SHIFT
#undef CSS_LIST_STYLE_IMAGE_INDEX

#define CSS_QUOTES_INDEX 5
#define CSS_QUOTES_SHIFT 0
#define CSS_QUOTES_MASK  0x1
static inline uint8_t css_computed_quotes(
		const css_computed_style *style, 
		lwc_string ***quotes)
{
	uint8_t bits = style->bits[CSS_QUOTES_INDEX];
	bits &= CSS_QUOTES_MASK;
	bits >>= CSS_QUOTES_SHIFT;

	/* 1bit: type */
	*quotes = style->quotes;

	return bits;
}
#undef CSS_QUOTES_MASK
#undef CSS_QUOTES_SHIFT
#undef CSS_QUOTES_INDEX

#define CSS_TOP_INDEX 6
#define CSS_TOP_SHIFT 2
#define CSS_TOP_MASK  0xfc
#define CSS_RIGHT_INDEX 7
#define CSS_RIGHT_SHIFT 2
#define CSS_RIGHT_MASK  0xfc
#define CSS_BOTTOM_INDEX 8
#define CSS_BOTTOM_SHIFT 2
#define CSS_BOTTOM_MASK  0xfc
#define CSS_LEFT_INDEX 9
#define CSS_LEFT_SHIFT 2
#define CSS_LEFT_MASK  0xfc
static inline uint8_t css_computed_top(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_TOP_INDEX];
	bits &= CSS_TOP_MASK;
	bits >>= CSS_TOP_SHIFT;

	/* Fix up, based on computed position */
	if (css_computed_position(style) == CSS_POSITION_STATIC) {
		/* Static -> auto */
		bits = CSS_TOP_AUTO;
	} else if (css_computed_position(style) == CSS_POSITION_RELATIVE) {
		/* Relative -> follow $9.4.3 */
		uint8_t bottom = style->bits[CSS_BOTTOM_INDEX];
		bottom &= CSS_BOTTOM_MASK;
		bottom >>= CSS_BOTTOM_SHIFT;

		if ((bits & 0x3) == CSS_TOP_AUTO && 
				(bottom & 0x3) == CSS_BOTTOM_AUTO) {
			/* Both auto => 0px */
			*length = 0;
			*unit = CSS_UNIT_PX;
		} else if ((bits & 0x3) == CSS_TOP_AUTO) {
			/* Top is auto => -bottom */
			*length = -style->bottom;
			*unit = (css_unit) (bottom >> 2);
		} else {
			*length = style->top;
			*unit = (css_unit) (bits >> 2);
		}

		bits = CSS_TOP_SET;
	} else if ((bits & 0x3) == CSS_TOP_SET) {
		*length = style->top;
		*unit = (css_unit) (bits >> 2);
	}

	/* 6bits: uuuutt : units | type */
	return (bits & 0x3);
}

static inline uint8_t css_computed_right(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_RIGHT_INDEX];
	bits &= CSS_RIGHT_MASK;
	bits >>= CSS_RIGHT_SHIFT;

	/* Fix up, based on computed position */
	if (css_computed_position(style) == CSS_POSITION_STATIC) {
		/* Static -> auto */
		bits = CSS_RIGHT_AUTO;
	} else if (css_computed_position(style) == CSS_POSITION_RELATIVE) {
		/* Relative -> follow $9.4.3 */
		uint8_t left = style->bits[CSS_LEFT_INDEX];
		left &= CSS_LEFT_MASK;
		left >>= CSS_LEFT_SHIFT;

		if ((bits & 0x3) == CSS_RIGHT_AUTO && 
				(left & 0x3) == CSS_LEFT_AUTO) {
			/* Both auto => 0px */
			*length = 0;
			*unit = CSS_UNIT_PX;
		} else if ((bits & 0x3) == CSS_RIGHT_AUTO) {
			/* Right is auto => -left */
			*length = -style->left;
			*unit = (css_unit) (left >> 2);
		} else {
			/** \todo Consider containing block's direction 
			 * if overconstrained */
			*length = style->right;
			*unit = (css_unit) (bits >> 2);
		}

		bits = CSS_RIGHT_SET;
	} else if ((bits & 0x3) == CSS_RIGHT_SET) {
		*length = style->right;
		*unit = (css_unit) (bits >> 2);
	}

	/* 6bits: uuuutt : units | type */
	return (bits & 0x3);
}

static inline uint8_t css_computed_bottom(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_BOTTOM_INDEX];
	bits &= CSS_BOTTOM_MASK;
	bits >>= CSS_BOTTOM_SHIFT;

	/* Fix up, based on computed position */
	if (css_computed_position(style) == CSS_POSITION_STATIC) {
		/* Static -> auto */
		bits = CSS_BOTTOM_AUTO;
	} else if (css_computed_position(style) == CSS_POSITION_RELATIVE) {
		/* Relative -> follow $9.4.3 */
		uint8_t top = style->bits[CSS_TOP_INDEX];
		top &= CSS_TOP_MASK;
		top >>= CSS_TOP_SHIFT;

		if ((bits & 0x3) == CSS_BOTTOM_AUTO &&
				(top & 0x3) == CSS_TOP_AUTO) {
			/* Both auto => 0px */
			*length = 0;
			*unit = CSS_UNIT_PX;
		} else if ((bits & 0x3) == CSS_BOTTOM_AUTO || 
				(top & 0x3) != CSS_TOP_AUTO) {
			/* Bottom is auto or top is not auto => -top */
			*length = -style->top;
			*unit = (css_unit) (top >> 2);
		} else {
			*length = style->bottom;
			*unit = (css_unit) (bits >> 2);
		}

		bits = CSS_BOTTOM_SET;
	} else if ((bits & 0x3) == CSS_BOTTOM_SET) {
		*length = style->bottom;
		*unit = (css_unit) (bits >> 2);
	}

	/* 6bits: uuuutt : units | type */
	return (bits & 0x3);
}

static inline uint8_t css_computed_left(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_LEFT_INDEX];
	bits &= CSS_LEFT_MASK;
	bits >>= CSS_LEFT_SHIFT;

	/* Fix up, based on computed position */
	if (css_computed_position(style) == CSS_POSITION_STATIC) {
		/* Static -> auto */
		bits = CSS_LEFT_AUTO;
	} else if (css_computed_position(style) == CSS_POSITION_RELATIVE) {
		/* Relative -> follow $9.4.3 */
		uint8_t right = style->bits[CSS_RIGHT_INDEX];
		right &= CSS_RIGHT_MASK;
		right >>= CSS_RIGHT_SHIFT;

		if ((bits & 0x3) == CSS_LEFT_AUTO && 
				(right & 0x3) == CSS_RIGHT_AUTO) {
			/* Both auto => 0px */
			*length = 0;
			*unit = CSS_UNIT_PX;
		} else if ((bits & 0x3) == CSS_LEFT_AUTO) {
			/* Left is auto => -right */
			*length = -style->right;
			*unit = (css_unit) (right >> 2);
		} else {
			/** \todo Consider containing block's direction 
			 * if overconstrained */
			*length = style->left;
			*unit = (css_unit) (bits >> 2);
		}

		bits = CSS_LEFT_SET;
	} else if ((bits & 0x3) == CSS_LEFT_SET) {
		*length = style->left;
		*unit = (css_unit) (bits >> 2);
	}

	/* 6bits: uuuutt : units | type */
	return (bits & 0x3);
}
#undef CSS_LEFT_MASK
#undef CSS_LEFT_SHIFT
#undef CSS_LEFT_INDEX
#undef CSS_BOTTOM_MASK
#undef CSS_BOTTOM_SHIFT
#undef CSS_BOTTOM_INDEX
#undef CSS_RIGHT_MASK
#undef CSS_RIGHT_SHIFT
#undef CSS_RIGHT_INDEX
#undef CSS_TOP_MASK
#undef CSS_TOP_SHIFT
#undef CSS_TOP_INDEX

#define CSS_BORDER_TOP_COLOR_INDEX 6
#define CSS_BORDER_TOP_COLOR_SHIFT 0
#define CSS_BORDER_TOP_COLOR_MASK  0x3
static inline uint8_t css_computed_border_top_color(
		const css_computed_style *style, 
		css_color *color)
{
	uint8_t bits = style->bits[CSS_BORDER_TOP_COLOR_INDEX];
	bits &= CSS_BORDER_TOP_COLOR_MASK;
	bits >>= CSS_BORDER_TOP_COLOR_SHIFT;

	/* 2bits: type */
	*color = style->border_color[0];

	return bits;
}
#undef CSS_BORDER_TOP_COLOR_MASK
#undef CSS_BORDER_TOP_COLOR_SHIFT
#undef CSS_BORDER_TOP_COLOR_INDEX

#define CSS_BORDER_RIGHT_COLOR_INDEX 7
#define CSS_BORDER_RIGHT_COLOR_SHIFT 0
#define CSS_BORDER_RIGHT_COLOR_MASK  0x3
static inline uint8_t css_computed_border_right_color(
		const css_computed_style *style, 
		css_color *color)
{
	uint8_t bits = style->bits[CSS_BORDER_RIGHT_COLOR_INDEX];
	bits &= CSS_BORDER_RIGHT_COLOR_MASK;
	bits >>= CSS_BORDER_RIGHT_COLOR_SHIFT;

	/* 2bits: type */
	*color = style->border_color[1];

	return bits;
}
#undef CSS_BORDER_RIGHT_COLOR_MASK
#undef CSS_BORDER_RIGHT_COLOR_SHIFT
#undef CSS_BORDER_RIGHT_COLOR_INDEX

#define CSS_BORDER_BOTTOM_COLOR_INDEX 8
#define CSS_BORDER_BOTTOM_COLOR_SHIFT 0
#define CSS_BORDER_BOTTOM_COLOR_MASK  0x3
static inline uint8_t css_computed_border_bottom_color(
		const css_computed_style *style, 
		css_color *color)
{
	uint8_t bits = style->bits[CSS_BORDER_BOTTOM_COLOR_INDEX];
	bits &= CSS_BORDER_BOTTOM_COLOR_MASK;
	bits >>= CSS_BORDER_BOTTOM_COLOR_SHIFT;

	/* 2bits: type */
	*color = style->border_color[2];

	return bits;
}
#undef CSS_BORDER_BOTTOM_COLOR_MASK
#undef CSS_BORDER_BOTTOM_COLOR_SHIFT
#undef CSS_BORDER_BOTTOM_COLOR_INDEX

#define CSS_BORDER_LEFT_COLOR_INDEX 9
#define CSS_BORDER_LEFT_COLOR_SHIFT 0
#define CSS_BORDER_LEFT_COLOR_MASK  0x3
static inline uint8_t css_computed_border_left_color(
		const css_computed_style *style, 
		css_color *color)
{
	uint8_t bits = style->bits[CSS_BORDER_LEFT_COLOR_INDEX];
	bits &= CSS_BORDER_LEFT_COLOR_MASK;
	bits >>= CSS_BORDER_LEFT_COLOR_SHIFT;

	/* 2bits: type */
	*color = style->border_color[3];

	return bits;
}
#undef CSS_BORDER_LEFT_COLOR_MASK
#undef CSS_BORDER_LEFT_COLOR_SHIFT
#undef CSS_BORDER_LEFT_COLOR_INDEX

#define CSS_HEIGHT_INDEX 10
#define CSS_HEIGHT_SHIFT 2
#define CSS_HEIGHT_MASK  0xfc
static inline uint8_t css_computed_height(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_HEIGHT_INDEX];
	bits &= CSS_HEIGHT_MASK;
	bits >>= CSS_HEIGHT_SHIFT;

	/* 6bits: uuuutt : units | type */
	if ((bits & 0x3) == CSS_HEIGHT_SET) {
		*length = style->height;
		*unit = (css_unit) (bits >> 2);
	}

	return (bits & 0x3);
}
#undef CSS_HEIGHT_MASK
#undef CSS_HEIGHT_SHIFT
#undef CSS_HEIGHT_INDEX

#define CSS_LINE_HEIGHT_INDEX 11
#define CSS_LINE_HEIGHT_SHIFT 2
#define CSS_LINE_HEIGHT_MASK  0xfc
static inline uint8_t css_computed_line_height(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_LINE_HEIGHT_INDEX];
	bits &= CSS_LINE_HEIGHT_MASK;
	bits >>= CSS_LINE_HEIGHT_SHIFT;

	/* 6bits: uuuutt : units | type */
	if ((bits & 0x3) == CSS_LINE_HEIGHT_NUMBER || 
			(bits & 0x3) == CSS_LINE_HEIGHT_DIMENSION) {
		*length = style->line_height;
	}

	if ((bits & 0x3) == CSS_LINE_HEIGHT_DIMENSION) {
		*unit = (css_unit) (bits >> 2);
	}

	return (bits & 0x3);
}
#undef CSS_LINE_HEIGHT_MASK
#undef CSS_LINE_HEIGHT_SHIFT
#undef CSS_LINE_HEIGHT_INDEX

#define CSS_BACKGROUND_COLOR_INDEX 10
#define CSS_BACKGROUND_COLOR_SHIFT 0
#define CSS_BACKGROUND_COLOR_MASK  0x3
static inline uint8_t css_computed_background_color(
		const css_computed_style *style, 
		css_color *color)
{
	uint8_t bits = style->bits[CSS_BACKGROUND_COLOR_INDEX];
	bits &= CSS_BACKGROUND_COLOR_MASK;
	bits >>= CSS_BACKGROUND_COLOR_SHIFT;

	/* 2bits: type */
	*color = style->background_color;

	return bits;
}
#undef CSS_BACKGROUND_COLOR_MASK
#undef CSS_BACKGROUND_COLOR_SHIFT
#undef CSS_BACKGROUND_COLOR_INDEX

#define CSS_Z_INDEX_INDEX 11
#define CSS_Z_INDEX_SHIFT 0
#define CSS_Z_INDEX_MASK  0x3
static inline uint8_t css_computed_z_index(
		const css_computed_style *style, 
		int32_t *z_index)
{
	uint8_t bits = style->bits[CSS_Z_INDEX_INDEX];
	bits &= CSS_Z_INDEX_MASK;
	bits >>= CSS_Z_INDEX_SHIFT;

	/* 2bits: type */
	*z_index = style->z_index;

	return bits;
}
#undef CSS_Z_INDEX_MASK
#undef CSS_Z_INDEX_SHIFT
#undef CSS_Z_INDEX_INDEX

#define CSS_MARGIN_TOP_INDEX 12
#define CSS_MARGIN_TOP_SHIFT 2
#define CSS_MARGIN_TOP_MASK  0xfc
static inline uint8_t css_computed_margin_top(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_MARGIN_TOP_INDEX];
	bits &= CSS_MARGIN_TOP_MASK;
	bits >>= CSS_MARGIN_TOP_SHIFT;

	/* 6bits: uuuutt : units | type */
	if ((bits & 0x3) == CSS_MARGIN_SET) {
		*length = style->margin[0];
		*unit = (css_unit) (bits >> 2);
	}

	return (bits & 0x3);
}
#undef CSS_MARGIN_TOP_MASK
#undef CSS_MARGIN_TOP_SHIFT
#undef CSS_MARGIN_TOP_INDEX

#define CSS_MARGIN_RIGHT_INDEX 13
#define CSS_MARGIN_RIGHT_SHIFT 2
#define CSS_MARGIN_RIGHT_MASK  0xfc
static inline uint8_t css_computed_margin_right(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_MARGIN_RIGHT_INDEX];
	bits &= CSS_MARGIN_RIGHT_MASK;
	bits >>= CSS_MARGIN_RIGHT_SHIFT;

	/* 6bits: uuuutt : units | type */
	if ((bits & 0x3) == CSS_MARGIN_SET) {
		*length = style->margin[1];
		*unit = (css_unit) (bits >> 2);
	}

	return (bits & 0x3);
}
#undef CSS_MARGIN_RIGHT_MASK
#undef CSS_MARGIN_RIGHT_SHIFT
#undef CSS_MARGIN_RIGHT_INDEX

#define CSS_MARGIN_BOTTOM_INDEX 14
#define CSS_MARGIN_BOTTOM_SHIFT 2
#define CSS_MARGIN_BOTTOM_MASK  0xfc
static inline uint8_t css_computed_margin_bottom(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_MARGIN_BOTTOM_INDEX];
	bits &= CSS_MARGIN_BOTTOM_MASK;
	bits >>= CSS_MARGIN_BOTTOM_SHIFT;

	/* 6bits: uuuutt : units | type */
	if ((bits & 0x3) == CSS_MARGIN_SET) {
		*length = style->margin[2];
		*unit = (css_unit) (bits >> 2);
	}

	return (bits & 0x3);
}
#undef CSS_MARGIN_BOTTOM_MASK
#undef CSS_MARGIN_BOTTOM_SHIFT
#undef CSS_MARGIN_BOTTOM_INDEX

#define CSS_MARGIN_LEFT_INDEX 15
#define CSS_MARGIN_LEFT_SHIFT 2
#define CSS_MARGIN_LEFT_MASK  0xfc
static inline uint8_t css_computed_margin_left(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_MARGIN_LEFT_INDEX];
	bits &= CSS_MARGIN_LEFT_MASK;
	bits >>= CSS_MARGIN_LEFT_SHIFT;

	/* 6bits: uuuutt : units | type */
	if ((bits & 0x3) == CSS_MARGIN_SET) {
		*length = style->margin[3];
		*unit = (css_unit) (bits >> 2);
	}

	return (bits & 0x3);
}
#undef CSS_MARGIN_LEFT_MASK
#undef CSS_MARGIN_LEFT_SHIFT
#undef CSS_MARGIN_LEFT_INDEX

#define CSS_BACKGROUND_ATTACHMENT_INDEX 12
#define CSS_BACKGROUND_ATTACHMENT_SHIFT 0
#define CSS_BACKGROUND_ATTACHMENT_MASK  0x3
static inline uint8_t css_computed_background_attachment(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_BACKGROUND_ATTACHMENT_INDEX];
	bits &= CSS_BACKGROUND_ATTACHMENT_MASK;
	bits >>= CSS_BACKGROUND_ATTACHMENT_SHIFT;

	/* 2bits: type */
	return bits;
}
#undef CSS_BACKGROUND_ATTACHMENT_MASK
#undef CSS_BACKGROUND_ATTACHMENT_SHIFT
#undef CSS_BACKGROUND_ATTACHMENT_INDEX

#define CSS_BORDER_COLLAPSE_INDEX 13
#define CSS_BORDER_COLLAPSE_SHIFT 0
#define CSS_BORDER_COLLAPSE_MASK  0x3
static inline uint8_t css_computed_border_collapse(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_BORDER_COLLAPSE_INDEX];
	bits &= CSS_BORDER_COLLAPSE_MASK;
	bits >>= CSS_BORDER_COLLAPSE_SHIFT;

	/* 2bits: type */
	return bits;
}
#undef CSS_BORDER_COLLAPSE_MASK
#undef CSS_BORDER_COLLAPSE_SHIFT
#undef CSS_BORDER_COLLAPSE_INDEX

#define CSS_CAPTION_SIDE_INDEX 14
#define CSS_CAPTION_SIDE_SHIFT 0
#define CSS_CAPTION_SIDE_MASK  0x3
static inline uint8_t css_computed_caption_side(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_CAPTION_SIDE_INDEX];
	bits &= CSS_CAPTION_SIDE_MASK;
	bits >>= CSS_CAPTION_SIDE_SHIFT;

	/* 2bits: type */
	return bits;
}
#undef CSS_CAPTION_SIDE_MASK
#undef CSS_CAPTION_SIDE_SHIFT
#undef CSS_CAPTION_SIDE_INDEX

#define CSS_DIRECTION_INDEX 15
#define CSS_DIRECTION_SHIFT 0
#define CSS_DIRECTION_MASK  0x3
static inline uint8_t css_computed_direction(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_DIRECTION_INDEX];
	bits &= CSS_DIRECTION_MASK;
	bits >>= CSS_DIRECTION_SHIFT;

	/* 2bits: type */
	return bits;
}
#undef CSS_DIRECTION_MASK
#undef CSS_DIRECTION_SHIFT
#undef CSS_DIRECTION_INDEX

#define CSS_MAX_HEIGHT_INDEX 16
#define CSS_MAX_HEIGHT_SHIFT 2
#define CSS_MAX_HEIGHT_MASK  0xfc
static inline uint8_t css_computed_max_height(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_MAX_HEIGHT_INDEX];
	bits &= CSS_MAX_HEIGHT_MASK;
	bits >>= CSS_MAX_HEIGHT_SHIFT;

	/* 6bits: uuuutt : units | type */
	if ((bits & 0x3) == CSS_MAX_HEIGHT_SET) {
		*length = style->max_height;
		*unit = (css_unit) (bits >> 2);
	}

	return (bits & 0x3);
}
#undef CSS_MAX_HEIGHT_MASK
#undef CSS_MAX_HEIGHT_SHIFT
#undef CSS_MAX_HEIGHT_INDEX

#define CSS_MAX_WIDTH_INDEX 17
#define CSS_MAX_WIDTH_SHIFT 2
#define CSS_MAX_WIDTH_MASK  0xfc
static inline uint8_t css_computed_max_width(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_MAX_WIDTH_INDEX];
	bits &= CSS_MAX_WIDTH_MASK;
	bits >>= CSS_MAX_WIDTH_SHIFT;

	/* 6bits: uuuutt : units | type */
	if ((bits & 0x3) == CSS_MAX_WIDTH_SET) {
		*length = style->max_width;
		*unit = (css_unit) (bits >> 2);
	}

	return (bits & 0x3);
}
#undef CSS_MAX_WIDTH_MASK
#undef CSS_MAX_WIDTH_SHIFT
#undef CSS_MAX_WIDTH_INDEX

#define CSS_WIDTH_INDEX 18
#define CSS_WIDTH_SHIFT 2
#define CSS_WIDTH_MASK  0xfc
static inline uint8_t css_computed_width(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_WIDTH_INDEX];
	bits &= CSS_WIDTH_MASK;
	bits >>= CSS_WIDTH_SHIFT;

	/* 6bits: uuuutt : units | type */
	if ((bits & 0x3) == CSS_WIDTH_SET) {
		*length = style->width;
		*unit = (css_unit) (bits >> 2);
	}

	return (bits & 0x3);
}
#undef CSS_WIDTH_MASK
#undef CSS_WIDTH_SHIFT
#undef CSS_WIDTH_INDEX

#define CSS_EMPTY_CELLS_INDEX 16
#define CSS_EMPTY_CELLS_SHIFT 0
#define CSS_EMPTY_CELLS_MASK  0x3
static inline uint8_t css_computed_empty_cells(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_EMPTY_CELLS_INDEX];
	bits &= CSS_EMPTY_CELLS_MASK;
	bits >>= CSS_EMPTY_CELLS_SHIFT;

	/* 2bits: type */
	return bits;
}
#undef CSS_EMPTY_CELLS_MASK
#undef CSS_EMPTY_CELLS_SHIFT
#undef CSS_EMPTY_CELLS_INDEX

#define CSS_FLOAT_INDEX 17
#define CSS_FLOAT_SHIFT 0
#define CSS_FLOAT_MASK  0x3
static inline uint8_t css_computed_float(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_FLOAT_INDEX];
	bits &= CSS_FLOAT_MASK;
	bits >>= CSS_FLOAT_SHIFT;

	/* Fix up as per $9.7:2 */
	if (css_computed_position(style) == CSS_POSITION_ABSOLUTE ||
			css_computed_position(style) == CSS_POSITION_FIXED)
		return CSS_FLOAT_NONE;

	/* 2bits: type */
	return bits;
}
#undef CSS_FLOAT_MASK
#undef CSS_FLOAT_SHIFT
#undef CSS_FLOAT_INDEX

#define CSS_FONT_STYLE_INDEX 18
#define CSS_FONT_STYLE_SHIFT 0
#define CSS_FONT_STYLE_MASK  0x3
static inline uint8_t css_computed_font_style(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_FONT_STYLE_INDEX];
	bits &= CSS_FONT_STYLE_MASK;
	bits >>= CSS_FONT_STYLE_SHIFT;

	/* 2bits: type */
	return bits;
}
#undef CSS_FONT_STYLE_MASK
#undef CSS_FONT_STYLE_SHIFT
#undef CSS_FONT_STYLE_INDEX

#define CSS_MIN_HEIGHT_INDEX 19
#define CSS_MIN_HEIGHT_SHIFT 3
#define CSS_MIN_HEIGHT_MASK  0xf8
static inline uint8_t css_computed_min_height(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_MIN_HEIGHT_INDEX];
	bits &= CSS_MIN_HEIGHT_MASK;
	bits >>= CSS_MIN_HEIGHT_SHIFT;

	/* 5bits: uuuut : units | type */
	if ((bits & 0x1) == CSS_MIN_HEIGHT_SET) {
		*length = style->min_height;
		*unit = (css_unit) (bits >> 1);
	}

	return (bits & 0x1);
}
#undef CSS_MIN_HEIGHT_MASK
#undef CSS_MIN_HEIGHT_SHIFT
#undef CSS_MIN_HEIGHT_INDEX

#define CSS_MIN_WIDTH_INDEX 20
#define CSS_MIN_WIDTH_SHIFT 3
#define CSS_MIN_WIDTH_MASK  0xf8
static inline uint8_t css_computed_min_width(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_MIN_WIDTH_INDEX];
	bits &= CSS_MIN_WIDTH_MASK;
	bits >>= CSS_MIN_WIDTH_SHIFT;

	/* 5bits: uuuut : units | type */
	if ((bits & 0x1) == CSS_MIN_WIDTH_SET) {
		*length = style->min_width;
		*unit = (css_unit) (bits >> 1);
	}

	return (bits & 0x1);
}
#undef CSS_MIN_WIDTH_MASK
#undef CSS_MIN_WIDTH_SHIFT
#undef CSS_MIN_WIDTH_INDEX

#define CSS_BACKGROUND_REPEAT_INDEX 19
#define CSS_BACKGROUND_REPEAT_SHIFT 0
#define CSS_BACKGROUND_REPEAT_MASK  0x7
static inline uint8_t css_computed_background_repeat(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_BACKGROUND_REPEAT_INDEX];
	bits &= CSS_BACKGROUND_REPEAT_MASK;
	bits >>= CSS_BACKGROUND_REPEAT_SHIFT;

	/* 3bits: type */
	return bits;
}
#undef CSS_BACKGROUND_REPEAT_MASK
#undef CSS_BACKGROUND_REPEAT_SHIFT
#undef CSS_BACKGROUND_REPEAT_INDEX

#define CSS_CLEAR_INDEX 20
#define CSS_CLEAR_SHIFT 0
#define CSS_CLEAR_MASK  0x7
static inline uint8_t css_computed_clear(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_CLEAR_INDEX];
	bits &= CSS_CLEAR_MASK;
	bits >>= CSS_CLEAR_SHIFT;

	/* 3bits: type */
	return bits;
}
#undef CSS_CLEAR_MASK
#undef CSS_CLEAR_SHIFT
#undef CSS_CLEAR_INDEX

#define CSS_PADDING_TOP_INDEX 21
#define CSS_PADDING_TOP_SHIFT 3
#define CSS_PADDING_TOP_MASK  0xf8
static inline uint8_t css_computed_padding_top(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_PADDING_TOP_INDEX];
	bits &= CSS_PADDING_TOP_MASK;
	bits >>= CSS_PADDING_TOP_SHIFT;

	/* 5bits: uuuut : units | type */
	if ((bits & 0x1) == CSS_PADDING_SET) {
		*length = style->padding[0];
		*unit = (css_unit) (bits >> 1);
	}

	return (bits & 0x1);
}
#undef CSS_PADDING_TOP_MASK
#undef CSS_PADDING_TOP_SHIFT
#undef CSS_PADDING_TOP_INDEX

#define CSS_PADDING_RIGHT_INDEX 22
#define CSS_PADDING_RIGHT_SHIFT 3
#define CSS_PADDING_RIGHT_MASK  0xf8
static inline uint8_t css_computed_padding_right(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_PADDING_RIGHT_INDEX];
	bits &= CSS_PADDING_RIGHT_MASK;
	bits >>= CSS_PADDING_RIGHT_SHIFT;

	/* 5bits: uuuut : units | type */
	if ((bits & 0x1) == CSS_PADDING_SET) {
		*length = style->padding[1];
		*unit = (css_unit) (bits >> 1);
	}

	return (bits & 0x1);
}
#undef CSS_PADDING_RIGHT_MASK
#undef CSS_PADDING_RIGHT_SHIFT
#undef CSS_PADDING_RIGHT_INDEX

#define CSS_PADDING_BOTTOM_INDEX 23
#define CSS_PADDING_BOTTOM_SHIFT 3
#define CSS_PADDING_BOTTOM_MASK  0xf8
static inline uint8_t css_computed_padding_bottom(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_PADDING_BOTTOM_INDEX];
	bits &= CSS_PADDING_BOTTOM_MASK;
	bits >>= CSS_PADDING_BOTTOM_SHIFT;

	/* 5bits: uuuut : units | type */
	if ((bits & 0x1) == CSS_PADDING_SET) {
		*length = style->padding[2];
		*unit = (css_unit) (bits >> 1);
	}

	return (bits & 0x1);
}
#undef CSS_PADDING_BOTTOM_MASK
#undef CSS_PADDING_BOTTOM_SHIFT
#undef CSS_PADDING_BOTTOM_INDEX

#define CSS_PADDING_LEFT_INDEX 24
#define CSS_PADDING_LEFT_SHIFT 3
#define CSS_PADDING_LEFT_MASK  0xf8
static inline uint8_t css_computed_padding_left(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_PADDING_LEFT_INDEX];
	bits &= CSS_PADDING_LEFT_MASK;
	bits >>= CSS_PADDING_LEFT_SHIFT;

	/* 5bits: uuuut : units | type */
	if ((bits & 0x1) == CSS_PADDING_SET) {
		*length = style->padding[3];
		*unit = (css_unit) (bits >> 1);
	}

	return (bits & 0x1);
}
#undef CSS_PADDING_LEFT_MASK
#undef CSS_PADDING_LEFT_SHIFT
#undef CSS_PADDING_LEFT_INDEX

#define CSS_OVERFLOW_INDEX 21
#define CSS_OVERFLOW_SHIFT 0
#define CSS_OVERFLOW_MASK  0x7
static inline uint8_t css_computed_overflow(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_OVERFLOW_INDEX];
	bits &= CSS_OVERFLOW_MASK;
	bits >>= CSS_OVERFLOW_SHIFT;

	/* 3bits: type */
	return bits;
}
#undef CSS_OVERFLOW_MASK
#undef CSS_OVERFLOW_SHIFT
#undef CSS_OVERFLOW_INDEX

#define CSS_POSITION_INDEX 22
#define CSS_POSITION_SHIFT 0
#define CSS_POSITION_MASK  0x7
static inline uint8_t css_computed_position(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_POSITION_INDEX];
	bits &= CSS_POSITION_MASK;
	bits >>= CSS_POSITION_SHIFT;

	/* 3bits: type */
	return bits;
}
#undef CSS_POSITION_MASK
#undef CSS_POSITION_SHIFT
#undef CSS_POSITION_INDEX

#define CSS_TEXT_TRANSFORM_INDEX 24
#define CSS_TEXT_TRANSFORM_SHIFT 0
#define CSS_TEXT_TRANSFORM_MASK  0x7
static inline uint8_t css_computed_text_transform(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_TEXT_TRANSFORM_INDEX];
	bits &= CSS_TEXT_TRANSFORM_MASK;
	bits >>= CSS_TEXT_TRANSFORM_SHIFT;

	/* 3bits: type */
	return bits;
}
#undef CSS_TEXT_TRANSFORM_MASK
#undef CSS_TEXT_TRANSFORM_SHIFT
#undef CSS_TEXT_TRANSFORM_INDEX

#define CSS_TEXT_INDENT_INDEX 25
#define CSS_TEXT_INDENT_SHIFT 3
#define CSS_TEXT_INDENT_MASK  0xf8
static inline uint8_t css_computed_text_indent(
		const css_computed_style *style, 
		css_fixed *length, css_unit *unit)
{
	uint8_t bits = style->bits[CSS_TEXT_INDENT_INDEX];
	bits &= CSS_TEXT_INDENT_MASK;
	bits >>= CSS_TEXT_INDENT_SHIFT;

	/* 5bits: uuuut : units | type */
	if ((bits & 0x1) == CSS_TEXT_INDENT_SET) {
		*length = style->text_indent;
		*unit = (css_unit) (bits >> 1);
	}

	return (bits & 0x1);
}
#undef CSS_TEXT_INDENT_MASK
#undef CSS_TEXT_INDENT_SHIFT
#undef CSS_TEXT_INDENT_INDEX

#define CSS_WHITE_SPACE_INDEX 25
#define CSS_WHITE_SPACE_SHIFT 0
#define CSS_WHITE_SPACE_MASK  0x7
static inline uint8_t css_computed_white_space(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_WHITE_SPACE_INDEX];
	bits &= CSS_WHITE_SPACE_MASK;
	bits >>= CSS_WHITE_SPACE_SHIFT;

	/* 3bits: type */
	return bits;
}
#undef CSS_WHITE_SPACE_MASK
#undef CSS_WHITE_SPACE_SHIFT
#undef CSS_WHITE_SPACE_INDEX

#define CSS_BACKGROUND_POSITION_INDEX 27
#define CSS_BACKGROUND_POSITION_SHIFT 7
#define CSS_BACKGROUND_POSITION_MASK  0x80
#define CSS_BACKGROUND_POSITION_INDEX1 26
#define CSS_BACKGROUND_POSITION_SHIFT1 0
#define CSS_BACKGROUND_POSITION_MASK1 0xff
static inline uint8_t css_computed_background_position(
		const css_computed_style *style, 
		css_fixed *hlength, css_unit *hunit,
		css_fixed *vlength, css_unit *vunit)
{
	uint8_t bits = style->bits[CSS_BACKGROUND_POSITION_INDEX];
	bits &= CSS_BACKGROUND_POSITION_MASK;
	bits >>= CSS_BACKGROUND_POSITION_SHIFT;

	/* 1bit: type */
	if (bits == CSS_BACKGROUND_POSITION_SET) {
		uint8_t bits1 = style->bits[CSS_BACKGROUND_POSITION_INDEX1];
		bits1 &= CSS_BACKGROUND_POSITION_MASK1;
		bits1 >>= CSS_BACKGROUND_POSITION_SHIFT1;

		/* 8bits: hhhhvvvv : hunit | vunit */
		*hlength = style->background_position[0];
		*hunit = (css_unit) (bits1 >> 4);

		*vlength = style->background_position[1];
		*vunit = (css_unit) (bits1 & 0xf);
	}

	return bits;
}
#undef CSS_BACKGROUND_POSITION_MASK1
#undef CSS_BACKGROUND_POSITION_SHIFT1
#undef CSS_BACKGROUND_POSITION_INDEX1
#undef CSS_BACKGROUND_POSITION_MASK
#undef CSS_BACKGROUND_POSITION_SHIFT
#undef CSS_BACKGROUND_POSITION_INDEX

#define CSS_DISPLAY_INDEX 27
#define CSS_DISPLAY_SHIFT 2
#define CSS_DISPLAY_MASK  0x7c
static inline uint8_t css_computed_display(
		const css_computed_style *style, bool root)
{
	uint8_t position;
	uint8_t bits = style->bits[CSS_DISPLAY_INDEX];
	bits &= CSS_DISPLAY_MASK;
	bits >>= CSS_DISPLAY_SHIFT;

	/* Return computed display as per $9.7 */
	position = css_computed_position(style);

	/* 5bits: type */
	if (bits == CSS_DISPLAY_NONE)
		return bits; /* 1. */

	if ((position == CSS_POSITION_ABSOLUTE || 
			position == CSS_POSITION_FIXED) /* 2. */ ||
			css_computed_float(style) != CSS_FLOAT_NONE /* 3. */ ||
			root /* 4. */) {
		if (bits == CSS_DISPLAY_INLINE_TABLE) {
			return CSS_DISPLAY_TABLE;
		} else if (bits == CSS_DISPLAY_INLINE ||
				bits == CSS_DISPLAY_RUN_IN ||
				bits == CSS_DISPLAY_TABLE_ROW_GROUP ||
				bits == CSS_DISPLAY_TABLE_COLUMN ||
				bits == CSS_DISPLAY_TABLE_COLUMN_GROUP ||
				bits == CSS_DISPLAY_TABLE_HEADER_GROUP ||
				bits == CSS_DISPLAY_TABLE_FOOTER_GROUP ||
				bits == CSS_DISPLAY_TABLE_ROW ||
				bits == CSS_DISPLAY_TABLE_CELL ||
				bits == CSS_DISPLAY_TABLE_CAPTION ||
				bits == CSS_DISPLAY_INLINE_BLOCK) {
			return CSS_DISPLAY_BLOCK;
		}
	}

	/* 5. */
	return bits;
}

static inline uint8_t css_computed_display_static(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_DISPLAY_INDEX];
	bits &= CSS_DISPLAY_MASK;
	bits >>= CSS_DISPLAY_SHIFT;

	/* 5bits: type */
	return bits;
}

#undef CSS_DISPLAY_MASK
#undef CSS_DISPLAY_SHIFT
#undef CSS_DISPLAY_INDEX

#define CSS_FONT_VARIANT_INDEX 27
#define CSS_FONT_VARIANT_SHIFT 0
#define CSS_FONT_VARIANT_MASK  0x3
static inline uint8_t css_computed_font_variant(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_FONT_VARIANT_INDEX];
	bits &= CSS_FONT_VARIANT_MASK;
	bits >>= CSS_FONT_VARIANT_SHIFT;

	/* 2bits: type */
	return bits;
}
#undef CSS_FONT_VARIANT_MASK
#undef CSS_FONT_VARIANT_SHIFT
#undef CSS_FONT_VARIANT_INDEX

#define CSS_TEXT_DECORATION_INDEX 28
#define CSS_TEXT_DECORATION_SHIFT 3
#define CSS_TEXT_DECORATION_MASK  0xf8
static inline uint8_t css_computed_text_decoration(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_TEXT_DECORATION_INDEX];
	bits &= CSS_TEXT_DECORATION_MASK;
	bits >>= CSS_TEXT_DECORATION_SHIFT;

	/* 5bits: type */
	return bits;
}
#undef CSS_TEXT_DECORATION_MASK
#undef CSS_TEXT_DECORATION_SHIFT
#undef CSS_TEXT_DECORATION_INDEX

#define CSS_FONT_FAMILY_INDEX 28
#define CSS_FONT_FAMILY_SHIFT 0
#define CSS_FONT_FAMILY_MASK  0x7
static inline uint8_t css_computed_font_family(
		const css_computed_style *style, 
		lwc_string ***names)
{
	uint8_t bits = style->bits[CSS_FONT_FAMILY_INDEX];
	bits &= CSS_FONT_FAMILY_MASK;
	bits >>= CSS_FONT_FAMILY_SHIFT;

	/* 3bits: type */
	*names = style->font_family;

	return bits;
}
#undef CSS_FONT_FAMILY_MASK
#undef CSS_FONT_FAMILY_SHIFT
#undef CSS_FONT_FAMILY_INDEX

#define CSS_BORDER_TOP_STYLE_INDEX 29
#define CSS_BORDER_TOP_STYLE_SHIFT 4
#define CSS_BORDER_TOP_STYLE_MASK  0xf0
static inline uint8_t css_computed_border_top_style(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_BORDER_TOP_STYLE_INDEX];
	bits &= CSS_BORDER_TOP_STYLE_MASK;
	bits >>= CSS_BORDER_TOP_STYLE_SHIFT;

	/* 4bits: type */
	return bits;
}
#undef CSS_BORDER_TOP_STYLE_MASK
#undef CSS_BORDER_TOP_STYLE_SHIFT
#undef CSS_BORDER_TOP_STYLE_INDEX

#define CSS_BORDER_RIGHT_STYLE_INDEX 29
#define CSS_BORDER_RIGHT_STYLE_SHIFT 0
#define CSS_BORDER_RIGHT_STYLE_MASK  0xf
static inline uint8_t css_computed_border_right_style(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_BORDER_RIGHT_STYLE_INDEX];
	bits &= CSS_BORDER_RIGHT_STYLE_MASK;
	bits >>= CSS_BORDER_RIGHT_STYLE_SHIFT;

	/* 4bits: type */
	return bits;
}
#undef CSS_BORDER_RIGHT_STYLE_MASK
#undef CSS_BORDER_RIGHT_STYLE_SHIFT
#undef CSS_BORDER_RIGHT_STYLE_INDEX

#define CSS_BORDER_BOTTOM_STYLE_INDEX 30
#define CSS_BORDER_BOTTOM_STYLE_SHIFT 4
#define CSS_BORDER_BOTTOM_STYLE_MASK  0xf0
static inline uint8_t css_computed_border_bottom_style(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_BORDER_BOTTOM_STYLE_INDEX];
	bits &= CSS_BORDER_BOTTOM_STYLE_MASK;
	bits >>= CSS_BORDER_BOTTOM_STYLE_SHIFT;

	/* 4bits: type */
	return bits;
}
#undef CSS_BORDER_BOTTOM_STYLE_MASK
#undef CSS_BORDER_BOTTOM_STYLE_SHIFT
#undef CSS_BORDER_BOTTOM_STYLE_INDEX

#define CSS_BORDER_LEFT_STYLE_INDEX 30
#define CSS_BORDER_LEFT_STYLE_SHIFT 0
#define CSS_BORDER_LEFT_STYLE_MASK  0xf
static inline uint8_t css_computed_border_left_style(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_BORDER_LEFT_STYLE_INDEX];
	bits &= CSS_BORDER_LEFT_STYLE_MASK;
	bits >>= CSS_BORDER_LEFT_STYLE_SHIFT;

	/* 4bits: type */
	return bits;
}
#undef CSS_BORDER_LEFT_STYLE_MASK
#undef CSS_BORDER_LEFT_STYLE_SHIFT
#undef CSS_BORDER_LEFT_STYLE_INDEX

#define CSS_FONT_WEIGHT_INDEX 31
#define CSS_FONT_WEIGHT_SHIFT 4
#define CSS_FONT_WEIGHT_MASK  0xf0
static inline uint8_t css_computed_font_weight(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_FONT_WEIGHT_INDEX];
	bits &= CSS_FONT_WEIGHT_MASK;
	bits >>= CSS_FONT_WEIGHT_SHIFT;

	/* 4bits: type */
	return bits;
}
#undef CSS_FONT_WEIGHT_MASK
#undef CSS_FONT_WEIGHT_SHIFT
#undef CSS_FONT_WEIGHT_INDEX

#define CSS_LIST_STYLE_TYPE_INDEX 31
#define CSS_LIST_STYLE_TYPE_SHIFT 0
#define CSS_LIST_STYLE_TYPE_MASK  0xf
static inline uint8_t css_computed_list_style_type(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_LIST_STYLE_TYPE_INDEX];
	bits &= CSS_LIST_STYLE_TYPE_MASK;
	bits >>= CSS_LIST_STYLE_TYPE_SHIFT;

	/* 4bits: type */
	return bits;
}
#undef CSS_LIST_STYLE_TYPE_MASK
#undef CSS_LIST_STYLE_TYPE_SHIFT
#undef CSS_LIST_STYLE_TYPE_INDEX

#define CSS_OUTLINE_STYLE_INDEX 32
#define CSS_OUTLINE_STYLE_SHIFT 4
#define CSS_OUTLINE_STYLE_MASK  0xf0
static inline uint8_t css_computed_outline_style(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_OUTLINE_STYLE_INDEX];
	bits &= CSS_OUTLINE_STYLE_MASK;
	bits >>= CSS_OUTLINE_STYLE_SHIFT;

	/* 4bits: type */
	return bits;
}
#undef CSS_OUTLINE_STYLE_MASK
#undef CSS_OUTLINE_STYLE_SHIFT
#undef CSS_OUTLINE_STYLE_INDEX

#define CSS_TABLE_LAYOUT_INDEX 32
#define CSS_TABLE_LAYOUT_SHIFT 2
#define CSS_TABLE_LAYOUT_MASK  0xc
static inline uint8_t css_computed_table_layout(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_TABLE_LAYOUT_INDEX];
	bits &= CSS_TABLE_LAYOUT_MASK;
	bits >>= CSS_TABLE_LAYOUT_SHIFT;

	/* 2bits: type */
	return bits;
}
#undef CSS_TABLE_LAYOUT_MASK
#undef CSS_TABLE_LAYOUT_SHIFT
#undef CSS_TABLE_LAYOUT_INDEX

#define CSS_UNICODE_BIDI_INDEX 32
#define CSS_UNICODE_BIDI_SHIFT 0
#define CSS_UNICODE_BIDI_MASK  0x3
static inline uint8_t css_computed_unicode_bidi(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_UNICODE_BIDI_INDEX];
	bits &= CSS_UNICODE_BIDI_MASK;
	bits >>= CSS_UNICODE_BIDI_SHIFT;

	/* 2bits: type */
	return bits;
}
#undef CSS_UNICODE_BIDI_MASK
#undef CSS_UNICODE_BIDI_SHIFT
#undef CSS_UNICODE_BIDI_INDEX

#define CSS_VISIBILITY_INDEX 33
#define CSS_VISIBILITY_SHIFT 6
#define CSS_VISIBILITY_MASK  0xc0
static inline uint8_t css_computed_visibility(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_VISIBILITY_INDEX];
	bits &= CSS_VISIBILITY_MASK;
	bits >>= CSS_VISIBILITY_SHIFT;

	/* 2bits: type */
	return bits;
}
#undef CSS_VISIBILITY_MASK
#undef CSS_VISIBILITY_SHIFT
#undef CSS_VISIBILITY_INDEX

#define CSS_LIST_STYLE_POSITION_INDEX 33
#define CSS_LIST_STYLE_POSITION_SHIFT 4
#define CSS_LIST_STYLE_POSITION_MASK  0x30
static inline uint8_t css_computed_list_style_position(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_LIST_STYLE_POSITION_INDEX];
	bits &= CSS_LIST_STYLE_POSITION_MASK;
	bits >>= CSS_LIST_STYLE_POSITION_SHIFT;

	/* 2bits: type */
	return bits;
}
#undef CSS_LIST_STYLE_POSITION_MASK
#undef CSS_LIST_STYLE_POSITION_SHIFT
#undef CSS_LIST_STYLE_POSITION_INDEX

#define CSS_TEXT_ALIGN_INDEX 33
#define CSS_TEXT_ALIGN_SHIFT 0
#define CSS_TEXT_ALIGN_MASK  0xf
static inline uint8_t css_computed_text_align(
		const css_computed_style *style)
{
	uint8_t bits = style->bits[CSS_TEXT_ALIGN_INDEX];
	bits &= CSS_TEXT_ALIGN_MASK;
	bits >>= CSS_TEXT_ALIGN_SHIFT;

	/* 4bits: type */
	return bits;
}
#undef CSS_TEXT_ALIGN_MASK
#undef CSS_TEXT_ALIGN_SHIFT
#undef CSS_TEXT_ALIGN_INDEX

#endif

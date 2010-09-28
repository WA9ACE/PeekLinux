/*
 * Copyright 2009 Vincent Sanders <vince@kyllikki.org>
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

/** \file Plotter global styles.
 *
 * These plot styles are globaly available and used in many places.
 */

#include "desktop/plotters.h"

static plot_style_t plot_style_fill_white_static = {
	0,
	0,
	0,
	PLOT_OP_TYPE_SOLID,
	0xffffff
};
plot_style_t *plot_style_fill_white = &plot_style_fill_white_static;

static plot_style_t plot_style_fill_black_static = {
	0,
	0,
	0,
	PLOT_OP_TYPE_SOLID,
	0x0
};
plot_style_t *plot_style_fill_black = &plot_style_fill_black_static;

static plot_style_t plot_style_fill_red_static = {
	0,
	0,
	0,
	PLOT_OP_TYPE_SOLID,
	0x000000ff
};
plot_style_t *plot_style_fill_red = &plot_style_fill_red_static;

/* Box model debug outline styles for content, padding and margin edges */
static const plot_style_t plot_style_content_edge_static = {
	PLOT_OP_TYPE_SOLID,
	1,
	0x00ff0000,
	0,
	0
};
plot_style_t const * const plot_style_content_edge =
		&plot_style_content_edge_static;

static const plot_style_t plot_style_padding_edge_static = {
	PLOT_OP_TYPE_SOLID,
	1,
	0x000000ff,
	0,
	0
};
plot_style_t const * const plot_style_padding_edge =
		&plot_style_padding_edge_static;

static const plot_style_t plot_style_margin_edge_static = {
	PLOT_OP_TYPE_SOLID,
	1,
	0x0000ffff,
	0,
	0
};
plot_style_t const * const plot_style_margin_edge =
		&plot_style_margin_edge_static;

/* caret style used in html_redraw_caret */
static plot_style_t plot_style_caret_static = {
	PLOT_OP_TYPE_SOLID,
	0,
	0x808080,  /* todo - choose a proper colour */
	0,
	0
};
plot_style_t *plot_style_caret = &plot_style_caret_static;



/* html redraw widget styles */

/** plot style for filled widget base colour. */
static plot_style_t plot_style_fill_wbasec_static = {
	0,
	0,
	0,
	PLOT_OP_TYPE_SOLID,
	WIDGET_BASEC
};
plot_style_t *plot_style_fill_wbasec = &plot_style_fill_wbasec_static;

/** plot style for dark filled widget base colour . */
static plot_style_t plot_style_fill_darkwbasec_static = {
	0,
	0,
	0,
	PLOT_OP_TYPE_SOLID,
	double_darken_colour(WIDGET_BASEC)
};
plot_style_t *plot_style_fill_darkwbasec = &plot_style_fill_darkwbasec_static;

/** plot style for light filled widget base colour. */
static plot_style_t plot_style_fill_lightwbasec_static = {
	0,
	0,
	0,
	PLOT_OP_TYPE_SOLID,
	double_lighten_colour(WIDGET_BASEC)
};
plot_style_t *plot_style_fill_lightwbasec = &plot_style_fill_lightwbasec_static;


/** plot style for widget background. */
static plot_style_t plot_style_fill_wblobc_static = {
	0,
	0,
	0,
	PLOT_OP_TYPE_SOLID,
	WIDGET_BLOBC
};
plot_style_t *plot_style_fill_wblobc = &plot_style_fill_wblobc_static;

/** plot style for checkbox cross. */
static plot_style_t plot_style_stroke_wblobc_static = {
	PLOT_OP_TYPE_SOLID,
	2,
	WIDGET_BLOBC,
	0,
	0
};
plot_style_t *plot_style_stroke_wblobc = &plot_style_stroke_wblobc_static;

/** stroke style for widget double dark colour. */
static plot_style_t plot_style_stroke_darkwbasec_static = {
	PLOT_OP_TYPE_SOLID,
	0,
	double_darken_colour(WIDGET_BASEC),
	0,
	0
};
plot_style_t *plot_style_stroke_darkwbasec = &plot_style_stroke_darkwbasec_static;

/** stroke style for widget double light colour. */
static plot_style_t plot_style_stroke_lightwbasec_static = {
	PLOT_OP_TYPE_SOLID,
	0,
	double_lighten_colour(WIDGET_BASEC),
	0,
	0
};
plot_style_t *plot_style_stroke_lightwbasec = &plot_style_stroke_lightwbasec_static;

/* history styles */

/** stroke style for history core. */
static plot_style_t plot_style_stroke_history_static = {
	PLOT_OP_TYPE_SOLID,
	0,
	0x333333,
	0,
	0
};
plot_style_t *plot_style_stroke_history = &plot_style_stroke_history_static;

/* Generic font style */
static const plot_font_style_t plot_style_font_static = {
	PLOT_FONT_FAMILY_SANS_SERIF,
	10 * FONT_SIZE_SCALE,
	400,
	FONTF_NONE,
	0xffffff,
	0x000000
};
plot_font_style_t const * const plot_style_font = &plot_style_font_static;


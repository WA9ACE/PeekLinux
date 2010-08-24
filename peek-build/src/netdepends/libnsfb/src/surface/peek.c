/*
 * Copyright 2009 Vincent Sanders <vince@simtec.co.uk>
 *
 * This file is part of libnsfb, http://www.netsurf-browser.org/
 * Licenced under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 */

#include <stdbool.h>
#include <stdlib.h>

#include "libnsfb.h"
#include "libnsfb_event.h"
#include "libnsfb_plot.h"
#include "libnsfb_plot_util.h"

#include "frontend.h"
#include "plot.h"
#include "cursor.h"

extern uint8_t* get_LCD_bitmap(void);
extern void dspl_Enable(uint8_t);

static int peek_geometry(nsfb_t *nsfb, int width, int height, int bpp)
{
	if (nsfb->frontend_priv != NULL)
		return -1; /* if were already initialised fail */

	nsfb->width = width;
	nsfb->height = height;
	nsfb->bpp = bpp;

	/* select default sw plotters for bpp */
	select_plotters(nsfb);

	return 0;
}

static int peek_initialise(nsfb_t *nsfb)
{
	if (nsfb->frontend_priv != NULL)
		return -1;

	if (nsfb->bpp != 16 || nsfb->width != 320 || nsfb->height != 240)
		return -1;

	nsfb->ptr = (void *)get_LCD_bitmap();
	nsfb->linelen = nsfb->width * (nsfb->bpp >> 3);

	return 0;
}

static int peek_finalise(nsfb_t *nsfb)
{
	return 0;
}

static bool peek_input(nsfb_t *nsfb, nsfb_event_t *event, int timeout)
{
	return false;
}

static int peek_claim(nsfb_t *nsfb, nsfb_bbox_t *box)
{
	struct nsfb_cursor_s *cursor = nsfb->cursor;

	dspl_Enable(0);
	if ((cursor != NULL) && 
			(cursor->plotted == true) && 
			(nsfb_plot_bbox_intersect(box, &cursor->loc))) {

		nsfb->plotter_fns->bitmap(nsfb, 
				&cursor->savloc,  
				cursor->sav, 
				cursor->sav_width, 
				cursor->sav_height, 
				cursor->sav_width, 
				false);
		cursor->plotted = false;
	}

	dspl_Enable(1);
	return 0;
}

static int peek_cursor(nsfb_t *nsfb, struct nsfb_cursor_s *cursor)
{
	nsfb_bbox_t sclip;

	dspl_Enable(0);

	if ((cursor != NULL) && (cursor->plotted == true)) {
		sclip = nsfb->clip;

		nsfb->plotter_fns->set_clip(nsfb, NULL);

		nsfb->plotter_fns->bitmap(nsfb, 
				&cursor->savloc,  
				cursor->sav, 
				cursor->sav_width, 
				cursor->sav_height, 
				cursor->sav_width, 
				false);

		nsfb_cursor_plot(nsfb, cursor);

		nsfb->clip = sclip;
	}

	dspl_Enable(1);
	return true;
}


static int peek_update(nsfb_t *nsfb, nsfb_bbox_t *box)
{
	struct nsfb_cursor_s *cursor = nsfb->cursor;

	dspl_Enable(0);

	if ((cursor != NULL) && (cursor->plotted == false)) {
		nsfb_cursor_plot(nsfb, cursor);
	}

	dspl_Enable(1);
	return 0;
}

const nsfb_frontend_rtns_t peek_rtns = {
	0, 								/* defaults */
	peek_initialise, 	/* initialize */
	peek_finalise, 		/* finalize */
	peek_geometry,		/* geometry */
	peek_input,				/* input */
	peek_claim,				/* claim */
	peek_update,			/* update */
	peek_cursor				/* cursor */
};

//NSFB_FRONTEND_DEF(peek, NSFB_FRONTEND_PEEK, &peek_rtns)
void peek_register_frontend(void) 
{ 
	_nsfb_register_frontend(NSFB_FRONTEND_PEEK, &peek_rtns, "peek"); 
}


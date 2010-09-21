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
#include "KeyMappings.h"
#include "frontend.h"
#include "plot.h"
#include "cursor.h"

#define KPD_NB_PHYSICAL_KEYS 52
#include "kpd_api.h"

#include "List.h"

extern uint8_t* get_LCD_bitmap(void);
extern void dspl_Enable(uint8_t);
extern List *netsurf_get_queue();

#ifdef EMO_SIM
enum nsfb_key_code_e peek_nsfb_map[] = {
	NSFB_KEY_UNKNOWN,
	NSFB_KEY_0,
	NSFB_KEY_1,
	NSFB_KEY_2,
	NSFB_KEY_3,
	NSFB_KEY_4,
	NSFB_KEY_5,
	NSFB_KEY_6,
	NSFB_KEY_7,
	NSFB_KEY_8,
	NSFB_KEY_9,
	NSFB_KEY_a,
	NSFB_KEY_b,
	NSFB_KEY_c,
	NSFB_KEY_d,
	NSFB_KEY_e,
	NSFB_KEY_f,
	NSFB_KEY_g,
	NSFB_KEY_h,
	NSFB_KEY_i,
	NSFB_KEY_j,
	NSFB_KEY_k,
	NSFB_KEY_l,
	NSFB_KEY_m,
	NSFB_KEY_n,
	NSFB_KEY_o,
	NSFB_KEY_p,
	NSFB_KEY_q,
	NSFB_KEY_r,
	NSFB_KEY_s,
	NSFB_KEY_t,
	NSFB_KEY_u,
	NSFB_KEY_v,
	NSFB_KEY_w,
	NSFB_KEY_x,
	NSFB_KEY_y,
	NSFB_KEY_z,
	NSFB_KEY_AT,
	NSFB_KEY_SPACE,
	NSFB_KEY_LSHIFT,
	NSFB_KEY_RSHIFT,
	NSFB_KEY_RETURN,
	NSFB_KEY_TAB,
	NSFB_KEY_MINUS,
	NSFB_KEY_PERIOD,
	NSFB_KEY_COMMA,
	NSFB_KEY_QUOTE,
	NSFB_KEY_RETURN,
	NSFB_KEY_ESCAPE,
	NSFB_KEY_BACKSPACE,
	NSFB_KEY_UNKNOWN,
	NSFB_KEY_UP,
	NSFB_KEY_DOWN
};
#else
enum nsfb_key_code_e peek_nsfb_map[] = {
    NSFB_KEY_UNKNOWN,
	NSFB_KEY_RSHIFT,
    NSFB_KEY_0,
    NSFB_KEY_1,
    NSFB_KEY_2,
    NSFB_KEY_3,
    NSFB_KEY_4,
    NSFB_KEY_5,
    NSFB_KEY_6,
    NSFB_KEY_7,
    NSFB_KEY_8,
    NSFB_KEY_9,

    NSFB_KEY_a,
    NSFB_KEY_b,
    NSFB_KEY_c,
    NSFB_KEY_d,
    NSFB_KEY_e,
    NSFB_KEY_f,
    NSFB_KEY_g,
    NSFB_KEY_h,
    NSFB_KEY_i,
    NSFB_KEY_j,
    NSFB_KEY_k,
    NSFB_KEY_l,
    NSFB_KEY_m,
    NSFB_KEY_n,
    NSFB_KEY_o,
    NSFB_KEY_p,
    NSFB_KEY_q,
    NSFB_KEY_r,
    NSFB_KEY_s,
    NSFB_KEY_t,
    NSFB_KEY_u,
    NSFB_KEY_v,
    NSFB_KEY_w,
    NSFB_KEY_x,
    NSFB_KEY_y,
    NSFB_KEY_z,

	NSFB_KEY_RETURN, /* Enter Key */
	NSFB_KEY_PERIOD,
	NSFB_KEY_AT,
	NSFB_KEY_QUOTE,
	NSFB_KEY_UNKNOWN, /* Lock Key */
	NSFB_KEY_MINUS,
	NSFB_KEY_SPACE,
	NSFB_KEY_COMMA,
	NSFB_KEY_RETURN, /* Wheel Click */
	NSFB_KEY_ESCAPE, /* Back button */
	NSFB_KEY_BACKSPACE,
	NSFB_KEY_UNKNOWN, /* Power key */
	
    NSFB_KEY_a,
    NSFB_KEY_b,
    NSFB_KEY_c,
    NSFB_KEY_d,
    NSFB_KEY_e,
    NSFB_KEY_f,
    NSFB_KEY_g,
    NSFB_KEY_h,
    NSFB_KEY_i,
    NSFB_KEY_j,
    NSFB_KEY_k,
    NSFB_KEY_l,
    NSFB_KEY_m,
    NSFB_KEY_n,
    NSFB_KEY_o,
    NSFB_KEY_p,
    NSFB_KEY_q,
    NSFB_KEY_r,
    NSFB_KEY_s,
    NSFB_KEY_t,
    NSFB_KEY_u,
    NSFB_KEY_v,
    NSFB_KEY_w,
    NSFB_KEY_x,
    NSFB_KEY_y,
    NSFB_KEY_z,
	
	NSFB_KEY_EXCLAIM,
	NSFB_KEY_QUESTION,
	NSFB_KEY_HASH,
	NSFB_KEY_DOLLAR,
	NSFB_KEY_UNKNOWN, /* Do they have % key? */
	NSFB_KEY_SLASH,
	NSFB_KEY_AMPERSAND,
	NSFB_KEY_ASTERISK,
	NSFB_KEY_LEFTPAREN,
	NSFB_KEY_RIGHTPAREN,
	NSFB_KEY_UNKNOWN, /* sys wheel forward */
	NSFB_KEY_UNKNOWN, /* sys wheel back */
	NSFB_KEY_COLON,
	NSFB_KEY_SEMICOLON,
	NSFB_KEY_QUOTEDBL,
	NSFB_KEY_UNDERSCORE,
	NSFB_KEY_EQUALS,
	NSFB_KEY_UNKNOWN, /* shift wheel forward */
	NSFB_KEY_UNKNOWN, /* shift wheel back */
	NSFB_KEY_UNKNOWN, /* shift wheel click */
};
#endif

static int peek_geometry(nsfb_t *nsfb, int width, int height, int bpp)
{
	emo_printf("peek_geometry()");

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
	emo_printf("peek_finalise()");
	return 0;
}

static unsigned char state = 0;

#ifdef EMO_SIM
static bool peek_input(nsfb_t *nsfb, nsfb_event_t *event, int timeout)
{
	int kpd_key;
	struct nsfb_cursor_s *cursor = nsfb->cursor;

	kpd_key = SimReadKey();
	state = SimReadKeyState();

	if(kpd_key)
	{
    	emo_printf("peek_input kpd_key: %d - state %d\n", kpd_key, state);
	} else {
        event->type = NSFB_EVENT_NONE;
		return false;
	}
/*
	if(state) {
		event->type = NSFB_EVENT_KEY_DOWN;
		state = 0;
	} else { //else if((!state) && (kpd_key > 0)) {
		event->type = NSFB_EVENT_KEY_UP;
		state = 1;
	}
*/
	switch(kpd_key) {
		case 1: 
			event->value.keycode = NSFB_KEY_MOUSE_1;
			event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
			break;
		case 2:
            event->value.keycode = NSFB_KEY_MOUSE_2;
            event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
			break;
		case 3:
            event->value.keycode = NSFB_KEY_MOUSE_3;
            event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
			break;
		case 8:
			event->value.keycode = NSFB_KEY_SLASH;
            event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
            break;
		case 51:// up
            event->value.keycode = NSFB_KEY_MOUSE_4;
			event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
            break;
		case 52:// down
			event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
            event->value.keycode = NSFB_KEY_MOUSE_5;
            break;
		case 53:// left
			event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
			event->value.keycode = NSFB_KEY_LEFT;
			break;
		case 54:// right
			event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
			event->value.keycode = NSFB_KEY_RIGHT;
			break;
		case 4: /* cursor left */
			event->type = NSFB_EVENT_MOVE_RELATIVE;
            event->value.vector.x = -4;
            event->value.vector.y = 0;
            event->value.vector.z = 0;
			break;
        case 5: /* cursor right */
            event->type = NSFB_EVENT_MOVE_RELATIVE;
            event->value.vector.x = 4;
            event->value.vector.y = 0;
            event->value.vector.z = 0;
			break;
        case 6: /* cursor up */
            event->type = NSFB_EVENT_MOVE_RELATIVE;
            event->value.vector.x = 0;
            event->value.vector.y = -4;
            event->value.vector.z = 0;
			break;
        case 7: /* cusor down */
            event->type = NSFB_EVENT_MOVE_RELATIVE;
            event->value.vector.x = 0;
            event->value.vector.y = 4;
            event->value.vector.z = 0;
			break;
		 default:
            event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
            event->value.keycode = peek_nsfb_map[kpd_key];
			break;
    }
	if(kpd_key) {
		emo_printf("kpd_key event->value.keycode %d\n", event->value.keycode);
	}
    kpd_key = 0;
	
	return true;
}
#else
extern int netsurf_start_flag;
static bool wheel_mode = 0;
static bool peek_input(nsfb_t *nsfb, nsfb_event_t *event, int timeout)
{
    int kpd_key, state;
    struct nsfb_cursor_s *cursor = nsfb->cursor;
	List *keyqueue = netsurf_get_queue();
	ListIterator keyItem;
	int keystate = 0;

	if (list_size(keyqueue) > 0)
	{
		list_begin(keyqueue, &keyItem);
		keystate = (unsigned int)listIterator_item(&keyItem);
		listIterator_remove(&keyItem);

		kpd_key = keystate & 0xFFFF;
		state = (keystate >> 16);
	} else {
		return false;
	}

	emo_printf("peek_input() key %d state %d", kpd_key, state);

	switch(kpd_key) {
		case SYS_SHIFT: /* Ignore shift for now */
			return;
		case SYS_LOCK:
			/* Invert wheel mode */
			if(state)
				wheel_mode = wheel_mode ? 0 : 1;	
			break;
		case SYS_CANCEL_KEY:
			event->type = NSFB_EVENT_CONTROL;
			event->value.controlcode = NSFB_CONTROL_QUIT;
			netsurf_start_flag = 0;
			break;
		case SYS_WHEEL:
            event->value.keycode = NSFB_KEY_MOUSE_1;
            event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
            break;
        case SYS_WHEEL_BACK: // up
			if(!wheel_mode) {  // default scroll mode
            	event->value.keycode = NSFB_KEY_MOUSE_4;
            	event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
			} else { // cursor mode
            	event->type = NSFB_EVENT_MOVE_RELATIVE;
            	event->value.vector.x = 0;
            	event->value.vector.y = -8;
            	event->value.vector.z = 0;
			}
            break;
        case SYS_WHEEL_FORWARD: // down
			if(!wheel_mode) {
            	event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
            	event->value.keycode = NSFB_KEY_MOUSE_5;
			} else {
            	event->type = NSFB_EVENT_MOVE_RELATIVE;
            	event->value.vector.x = 0;
            	event->value.vector.y = 8;
            	event->value.vector.z = 0;
			}
            break;
        case SYS_WHEEL_BACK_SHIFT: // left
			if(!wheel_mode) {
            	event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
            	event->value.keycode = NSFB_KEY_LEFT;
			} else {
            	event->type = NSFB_EVENT_MOVE_RELATIVE;
            	event->value.vector.x = -8;
            	event->value.vector.y = 0;
            	event->value.vector.z = 0;
			}
			break;
        case SYS_WHEEL_FORWARD_SHIFT: // right
			if(!wheel_mode) {
            	event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
            	event->value.keycode = NSFB_KEY_RIGHT;
			} else {
            	event->type = NSFB_EVENT_MOVE_RELATIVE;
            	event->value.vector.x = 8;
            	event->value.vector.y = 0;
            	event->value.vector.z = 0;
			}
            break;
		default: /* Pass through the rest to the key map */
            event->type = state ? NSFB_EVENT_KEY_DOWN : NSFB_EVENT_KEY_UP;
            event->value.keycode = peek_nsfb_map[kpd_key];
            break;
	}

	return true;
}
#endif
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
	emo_printf("peek_update()\n");
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


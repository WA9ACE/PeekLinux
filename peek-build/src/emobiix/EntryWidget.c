#include "EntryWidget.h"

#include "DataObject.h"
#include "Widget.h"
#include "lgui.h"
#include "Style.h"
#include "WidgetRenderer.h"

#include "p_malloc.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* TODO, handle UTF8 instead of just ascii */

int entryWidget_handleKey(Widget *w, int key, Style *s)
{
	DataObject *dobj;
	DataObjectField *field, *cursorfield;
	int slen;
	int keylen;
	int cursorindex, cursorbytes, i, adv;
	char *newstring, *pos, *lastpos;
	Rectangle rect;

	if (key > 0xFFFFFF00 || !isprint(key) && key != '\b')
		return 0;

	dobj = widget_getDataObject(w);
	field = dataobject_getValue(dobj, "data");
	if (field == NULL) {
		field = dataobjectfield_string("");
		dataobject_setValue(dobj, "data", field);
	}

	cursorfield = dataobject_getValueAsInt(w, "cursor");
	cursorindex = cursorfield->field.integer;
	cursorbytes = 0;
	pos = field->field.string;
	for (i = 0; i < cursorindex; ++i) {
		lastpos = pos;
		if (UTF8toUTF32(pos, &adv) == 0)
			return 1;
		pos += adv;
		cursorbytes += adv;
	}

	slen = strlen(field->field.string);

	keylen = 1;
	if (key & 0xFF00)
		keylen = 2;
	if (key & 0xFF0000)
		keylen = 3;
	if (key & 0xFF000000)
		keylen = 4;

	if (key == '\b') {
		if (slen > 0 && cursorindex > 0) {
			memmove(lastpos, lastpos+adv, strlen(lastpos+adv)+1);
			cursorfield->field.integer;
			--cursorfield->field.integer;
		}
	} else {
		newstring = p_malloc(slen+keylen+1);
		memcpy(newstring, field->field.string, cursorbytes);
		newstring[cursorbytes] = key & 0xFF;
		if (keylen >= 2)
			newstring[cursorbytes+1] = (key >> 8) & 0xFF;
		if (keylen >= 3)
			newstring[cursorbytes+2] = (key >> 16) & 0xFF;
		if (keylen >= 4)
			newstring[cursorbytes+3] = (key >> 24) & 0xFF;
		memcpy(newstring+cursorbytes+keylen, field->field.string+cursorbytes, slen-cursorbytes);
		newstring[slen+keylen] = 0;
		p_free(field->field.string);
		field->field.string = newstring;
		++cursorfield->field.integer;
	}
	lgui_clip_identity();
	widget_getClipRectangle(w, &rect);
	lgui_clip_set(&rect);
	lgui_push_region();
	widget_markDirty(w);
	style_renderWidgetTree(s, dataobject_superparent(w));
	return 1;
}

extern Font *defaultFont;

/* entry renderer */
static void entry_renderer(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj) {
	Rectangle *box, *margin, cursorBox;
	DataObject *text;
	DataObjectField *field, *cursor, *startindex, *multiline;
	/*Style *style;*/
	Font *f;
	Color c, cursorColor;/*, background, line;*/
	/*const char *dtype;
	const char *ltype;*/
	const char *str;
	int percent, offset;/*, hasFocus;*/
	WidgetRenderer *iwr;

	box = widget_getBox(w);
	margin = widget_getMargin(w);
	/*dtype = (const char *)dataobject_getValue(dobj, "type")->field.string;
	ltype = widget_getID(w);*/

	iwr = NULL;
	style_getRenderer(s, w, "box-renderer", &iwr);
	if (iwr != NULL)
		iwr->render(wr, s, w, dobj);
#if 0
	hasFocus = widget_hasFocus(w);
	style = style_getID(s, ltype, hasFocus);
	f = (Font *)defaultFont;/*style_getProperty(s, NULL, ltype, dtype, "font");*/
	/*c.value = (unsigned int)style_getProperty(s, NULL, ltype, dtype, "color");*/
	c.value = 0;
	style_getColor(s, w, "color", &c.value);
	/*background.value = (unsigned int)style_getProperty(s, NULL, ltype, dtype, "background");*/
	background.value = 0;
	style_getColor(s, w, "background", &background.value);
	/*line.value = (unsigned int)style_getProperty(s, NULL, ltype, dtype, "focusline");*/
	if (hsaFocus)
		style_getColor(s, w, "focusline", &line.value);
	else
		line.value = (unsigned int)style_getProperty(s, NULL, ltype, dtype, "line");
	text = widget_getDataObject(w);
	field = dataobject_getValue(text, "data");
	if (field != NULL) {
		str = field->field.string;
	} else {
		str = "";
	}
#endif

	text = widget_getDataObject(w);
	field = dataobject_getValue(text, "data");
	if (field != NULL) {
		str = field->field.string;
	} else {
		str = "";
	}

	f = (Font *)defaultFont;
	c.value = 0;
	style_getColor(s, w, "font-color", &c.value);

	cursor = dataobject_getValueAsInt(w, "cursor");
	if (cursor == NULL) {
		cursor = dataobjectfield_int(strlen(str));
		dataobject_setValue(w, "cursor", cursor);
	}

	startindex = dataobject_getValueAsInt(w, "startindex");
	if (startindex == NULL) {
		startindex = dataobjectfield_int(0);
		dataobject_setValue(w, "startindex", startindex);
	}

	multiline = dataobject_getValue(w, "multiline");
	if (multiline == NULL) {
		multiline = dataobjectfield_string("false");
		dataobject_setValue(w, "multiline", multiline);
	}

	/*lgui_roundedbox_fill(box->x+margin->x, box->y+margin->y, box->width-1, box->height-1, 7,
			background.rgba.red, background.rgba.green, background.rgba.blue);
	lgui_roundedbox_line(box->x+margin->x, box->y+margin->y, box->width-1, box->height-1, 7,
			line.rgba.red, line.rgba.green, line.rgba.blue);*/

	lgui_complex_draw_font(box->x+4+margin->x, box->y+margin->y, box->width-9, box->height, str,
			f, c, cursor->field.integer, startindex->field.integer, &percent, &offset, 0,
			&cursorBox);

	if (dataobjectfield_isTrue(multiline)) {
		lgui_vline(box->x+margin->x+box->width-8, box->y+margin->y+1, box->height-1, 4, 0x44, 0x44, 0xFF);
		lgui_vline(box->x+margin->x+box->width-8, box->y+margin->y+1+offset*(box->height-1)/100,
				percent*(box->height-1)/100, 4, 0xCC, 0xCC, 0xFF);
	}

	if (style_getColor(s, w, "cursor-color", &cursorColor.value) != NULL) {
		lgui_hline(cursorBox.x, cursorBox.y, cursorBox.width, cursorBox.height,
				cursorColor.rgba.red, cursorColor.rgba.green, cursorColor.rgba.blue);
	}
}

static void entry_measure(WidgetRenderer *wr, Style *s, Widget *w,
		DataObject *dobj, IPoint *p)
{
	Font *f;
	const char *dtype;
	const char *ltype;
	const char *str;
	DataObjectField *field;

	dtype = (const char *)dataobject_getValue(dobj, "type")->field.string;
	ltype = widget_getID(w);
	f = (Font *)defaultFont;
	field = dataobject_getValue(dobj, "data");
	if (field == NULL) {
		field = dataobjectfield_string("");	
		dataobject_setValue(dobj, "data", field);
	}
	str = (const char *)field->field.data.bytes;

	lgui_measure_font(str, f, 0, p);
	p->y += 4;
}

WidgetRenderer *widgetrenderer_entry(void)
{
	static WidgetRenderer *output = NULL;

	if (output != NULL)
		return output;

	output = (WidgetRenderer *)p_malloc(sizeof(WidgetRenderer));
	output->render = entry_renderer;
	output->measure = entry_measure;
	output->margin = widgetrenderer_zeroMargin;

	return output;
}


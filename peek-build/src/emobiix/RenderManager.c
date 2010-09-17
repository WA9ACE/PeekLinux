#include "RenderManager.h"

#include "Array.h"
#include "ApplicationManager.h"
#include "DataObject.h"
#include "Rectangle.h"
#include "Widget.h"
#include "lgui.h"
#include "Debug.h"

#include "p_malloc.h"

struct RenderManager_t {
	Array *queue;
	Array *rectBefore;
	int flags;
};
typedef struct RenderManager_t RenderManager;

static RenderManager *renderman = NULL;

#define RM_NEEDS_RELAYOUT		0x01
#define RM_COMPLETE_REDRAW		0x02

void renderman_init(void)
{
	if (renderman != NULL)
		return;

	renderman = (RenderManager *)p_malloc(sizeof(RenderManager));
	renderman->queue = array_newWithSize(sizeof(DataObject *), 10);
	renderman->rectBefore = array_newWithSize(sizeof(Rectangle), 10);
	renderman->flags = 0;
}

void renderman_queue(DataObject *widget)
{
	int len, i;

	EMO_ASSERT(widget != NULL, "renderman can't queue NULL widget")

	if (renderman->flags & RM_COMPLETE_REDRAW)
		return;

	len = array_length(renderman->queue);

	for (i = 0; i < len; ++i) {
		if (widget == *(DataObject **)array_item(renderman->queue, i))
			return;
	}
	array_append(renderman->queue, &widget);
}

void renderman_dequeue(DataObject *widget)
{
	int len, i;

	EMO_ASSERT(widget != NULL, "renderman can't dequeue NULL widget")

	if (renderman->flags & RM_COMPLETE_REDRAW)
		return;

	len = array_length(renderman->queue);

	for (i = 0; i < len; ++i) {
		if (widget == *(DataObject **)array_item(renderman->queue, i)) {
			array_removeItem(renderman->queue, i);
			return;
		}
	}
}

void renderman_markLayoutChanged(void)
{
	renderman->flags |= RM_NEEDS_RELAYOUT;
}

void renderman_flush(void)
{
	Application *app;
	DataObject *appObj, *scrolled;
	DataObject *sparent, *widget;
	DataObjectField *ytrans;
	Rectangle rect;
	Rectangle *before;
	int len, i, needsLayout;

	app = manager_getFocusedApplication();
	if (app == NULL)
		return;
	appObj = application_getDataObject(app);

	if (renderman->flags & RM_COMPLETE_REDRAW) {
		manager_resolveLayout();
		goto cleanup_flush;
	}

	len = array_length(renderman->queue);
	if (len == 0)
		goto cleanup_flush;

	needsLayout = renderman->flags & RM_NEEDS_RELAYOUT;

	if (needsLayout) {
		for (i = 0; i < len; ++i) {
			widget = *(DataObject **)array_item(renderman->queue, i);
			widget_getClipRectangle(widget, &rect);
			scrolled = widget_findStringFieldParent(widget, "type", "scrolled");
			if (scrolled != NULL) {
				ytrans = dataobject_getValueAsInt(scrolled, "yoffset");
				if (ytrans != NULL) {
					emo_printf("YTrans %d -> %d" NL, rect.y, rect.y-ytrans->field.integer);
					rect.y -= ytrans->field.integer;
				}
			}
			array_append(renderman->rectBefore, &rect);
		}

		manager_resolveLayout();
	}
	
	for (i = 0; i < len; ++i) {
		widget = *(DataObject **)array_item(renderman->queue, i);
		sparent = dataobject_superparent(widget);
		if (sparent != appObj)
			continue;

		widget_markDirty(widget);
		lgui_clip_identity();
		widget_getClipRectangle(widget, &rect);
		scrolled = widget_findStringFieldParent(widget, "type", "scrolled");
		if (scrolled != NULL) {
			ytrans = dataobject_getValueAsInt(scrolled, "yoffset");
			if (ytrans != NULL) {
				emo_printf("YTrans %d -> %d" NL, rect.y, rect.y-ytrans->field.integer);
				rect.y -= ytrans->field.integer;
			}
		}
		if (needsLayout) {
			before = (Rectangle *)array_item(renderman->rectBefore, i);
			rectangle_union(&rect, before);
		}
		lgui_clip_set(&rect);
		lgui_push_region();
		manager_drawScreenPartial();
	}

cleanup_flush:
	array_empty(renderman->queue);
	array_empty(renderman->rectBefore);
	renderman->flags = 0;
}

void renderman_clearQueue(void)
{
	renderman->flags |= RM_COMPLETE_REDRAW;
}

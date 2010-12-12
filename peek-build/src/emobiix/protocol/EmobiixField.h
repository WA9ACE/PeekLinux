#ifndef __EMOBIIX_FIELD_H__
#define __EMOBIIX_FIELD_H__

#ifdef __cplusplus
extern "C" {
#endif

#define EMO_FIELD_ALIGNMENT_STR "alignment"
#define EMO_FIELD_ALPHA_STR "alpha"
#define EMO_FIELD_ARRAY_STR "array"
#define EMO_FIELD_ARRAYSOURCE_STR "arraysource"
#define EMO_FIELD_AUTOSCROLL_STR "autoscroll"
#define EMO_FIELD_BACKGROUND_COLOR_STR "background-color"
#define EMO_FIELD_BOLD_STR "bold"
#define EMO_FIELD_BORDER_STR "border"
#define EMO_FIELD_BORDER_COLOR_STR "border-color"
#define EMO_FIELD_BORDER_CORNERS_STR "border-corners"
#define EMO_FIELD_BOTTOM_STR "bottom"
#define EMO_FIELD_BOX_STR "box"
#define EMO_FIELD_BOX_RENDERER_STR "box-renderer"
#define EMO_FIELD_BUTTON_STR "button"
#define EMO_FIELD_CANFOCUS_STR "canfocus"
#define EMO_FIELD_CENTER_STR "center"
#define EMO_FIELD_CHECK_COLOR_STR "check-color"
#define EMO_FIELD_CHECKBOX_STR "checkbox"
#define EMO_FIELD_CHECKFIELD_STR "checkfield"
#define EMO_FIELD_CHECKVALUE_STR "checkvalue"
#define EMO_FIELD_COLOR_STR "color"
#define EMO_FIELD_CURSOR_STR "cursor"
#define EMO_FIELD_CURSOR_COLOR_STR "cursor-color"
#define EMO_FIELD_DATA_STR "data"
#define EMO_FIELD_DATAFIELD_STR "datafield"
#define EMO_FIELD_DIRECTION_STR "direction"
#define EMO_FIELD_ENDINDEX_STR "endindex"
#define EMO_FIELD_ENTRY_STR "entry"
#define EMO_FIELD_FALSE_STR "false"
#define EMO_FIELD_FIELDNAME_STR "fieldname"
#define EMO_FIELD_FIELDVALUE_STR "fieldvalue"
#define EMO_FIELD_FILL_STR "fill"
#define EMO_FIELD_FILTERFIELD_STR "filterfield"
#define EMO_FIELD_FILTERVALUE_STR "filtervalue"
#define EMO_FIELD_FIXED_STR "fixed"
#define EMO_FIELD_FOCUSINDEX_STR "focusindex"
#define EMO_FIELD_FOCUSSTACK_STR "focusstack"
#define EMO_FIELD_FONT_STR "font"
#define EMO_FIELD_FONT_ALIGNMENT_STR "font-alignment"
#define EMO_FIELD_FONT_COLOR_STR "font-color"
#define EMO_FIELD_FRAME_STR "frame"
#define EMO_FIELD_GRADBOX_STR "gradbox"
#define EMO_FIELD_GRADIENT_STR "gradient"
#define EMO_FIELD_HASFOCUS_STR "hasfocus"
#define EMO_FIELD_HEIGHT_STR "height"
#define EMO_FIELD_HGRID_STR "hgrid"
#define EMO_FIELD_HORIZONTAL_STR "horizontal"
#define EMO_FIELD_ID_STR "id"
#define EMO_FIELD_IMAGE_STR "image"
#define EMO_FIELD_IMGHEIGHT_STR "imgheight"
#define EMO_FIELD_IMGWIDTH_STR "imgwidth"
#define EMO_FIELD_IMGX_STR "imgx"
#define EMO_FIELD_IMGY_STR "imgy"
#define EMO_FIELD_LABEL_STR "label"
#define EMO_FIELD_LEFT_STR "left"
#define EMO_FIELD_MARGIN_STR "margin"
#define EMO_FIELD_MARGINBOTTOM_STR "marginbottom"
#define EMO_FIELD_MARGINLEFT_STR "marginleft"
#define EMO_FIELD_MARGINRIGHT_STR "marginright"
#define EMO_FIELD_MARGINTOP_STR "margintop"
#define EMO_FIELD_MULTILINE_STR "multiline"
#define EMO_FIELD_NAME_STR "name"
#define EMO_FIELD_NONE_STR "none"
#define EMO_FIELD_ORIENTATION_STR "orientation"
#define EMO_FIELD_PACKING_STR "packing"
#define EMO_FIELD_PIXELFORMAT_STR "pixelformat"
#define EMO_FIELD_POSITION_STR "position"
#define EMO_FIELD_PREPEND_STR "prepend"
#define EMO_FIELD_RADIUS_STR "radius"
#define EMO_FIELD_REFERENCE_STR "reference"
#define EMO_FIELD_RENDERER_STR "renderer"
#define EMO_FIELD_RIGHT_STR "right"
#define EMO_FIELD_ROUNDED_STR "rounded"
#define EMO_FIELD_SCROLLBAR_STR "scrollbar"
#define EMO_FIELD_SCROLLED_STR "scrolled"
#define EMO_FIELD_SET_STR "set"
#define EMO_FIELD_SOLID_STR "solid"
#define EMO_FIELD_STACK_STR "stack"
#define EMO_FIELD_STARTCOUNT_STR "startcount"
#define EMO_FIELD_STARTINDEX_STR "startindex"
#define EMO_FIELD_STENCIL_STR "stencil"
#define EMO_FIELD_TEXT_STR "text"
#define EMO_FIELD_THUMB_COLOR_STR "thumb-color"
#define EMO_FIELD_TOP_STR "top"
#define EMO_FIELD_TRACK_COLOR_STR "track-color"
#define EMO_FIELD_TRANSPARENCY_STR "transparency"
#define EMO_FIELD_TYPE_STR "type"
#define EMO_FIELD_VERTICAL_STR "vertical"
#define EMO_FIELD_VGRID_STR "vgrid"
#define EMO_FIELD_VIEW_STR "view"
#define EMO_FIELD_WEIGHT_STR "weight"
#define EMO_FIELD_WIDTH_STR "width"
#define EMO_FIELD_X_STR "x"
#define EMO_FIELD_XOFFSET_STR "xoffset"
#define EMO_FIELD_Y_STR "y"
#define EMO_FIELD_YOFFSET_STR "yoffset"

typedef enum EmoField_t
{
	EMO_FIELD_ALIGNMENT,
	EMO_FIELD_ALPHA,
	EMO_FIELD_ARRAY,
	EMO_FIELD_ARRAYSOURCE,
	EMO_FIELD_AUTOSCROLL,
	EMO_FIELD_BACKGROUND_COLOR,
	EMO_FIELD_BOLD,
	EMO_FIELD_BORDER,
	EMO_FIELD_BORDER_COLOR,
	EMO_FIELD_BORDER_CORNERS,
	EMO_FIELD_BOTTOM,
	EMO_FIELD_BOX,
	EMO_FIELD_BOX_RENDERER,
	EMO_FIELD_BUTTON,
	EMO_FIELD_CANFOCUS,
	EMO_FIELD_CENTER,
	EMO_FIELD_CHECK_COLOR,
	EMO_FIELD_CHECKBOX,
	EMO_FIELD_CHECKFIELD,
	EMO_FIELD_CHECKVALUE,
	EMO_FIELD_COLOR,
	EMO_FIELD_CURSOR,
	EMO_FIELD_CURSOR_COLOR,
	EMO_FIELD_DATA,
	EMO_FIELD_DATAFIELD,
	EMO_FIELD_DIRECTION,
	EMO_FIELD_ENDINDEX,
	EMO_FIELD_ENTRY,
	EMO_FIELD_FALSE,
	EMO_FIELD_FIELDNAME,
	EMO_FIELD_FIELDVALUE,
	EMO_FIELD_FILL,
	EMO_FIELD_FILTERFIELD,
	EMO_FIELD_FILTERVALUE,
	EMO_FIELD_FIXED,
	EMO_FIELD_FOCUSINDEX,
	EMO_FIELD_FOCUSSTACK,
	EMO_FIELD_FONT,
	EMO_FIELD_FONT_ALIGNMENT,
	EMO_FIELD_FONT_COLOR,
	EMO_FIELD_FRAME,
	EMO_FIELD_GRADBOX,
	EMO_FIELD_GRADIENT,
	EMO_FIELD_HASFOCUS,
	EMO_FIELD_HEIGHT,
	EMO_FIELD_HGRID,
	EMO_FIELD_HORIZONTAL,
	EMO_FIELD_ID,
	EMO_FIELD_IMAGE,
	EMO_FIELD_IMGHEIGHT,
	EMO_FIELD_IMGWIDTH,
	EMO_FIELD_IMGX,
	EMO_FIELD_IMGY,
	EMO_FIELD_LABEL,
	EMO_FIELD_LEFT,
	EMO_FIELD_MARGIN,
	EMO_FIELD_MARGINBOTTOM,
	EMO_FIELD_MARGINLEFT,
	EMO_FIELD_MARGINRIGHT,
	EMO_FIELD_MARGINTOP,
	EMO_FIELD_MULTILINE,
	EMO_FIELD_NAME,
	EMO_FIELD_NONE,
	EMO_FIELD_ORIENTATION,
	EMO_FIELD_PACKING,
	EMO_FIELD_PIXELFORMAT,
	EMO_FIELD_POSITION,
	EMO_FIELD_PREPEND,
	EMO_FIELD_RADIUS,
	EMO_FIELD_REFERENCE,
	EMO_FIELD_RENDERER,
	EMO_FIELD_RIGHT,
	EMO_FIELD_ROUNDED,
	EMO_FIELD_SCROLLBAR,
	EMO_FIELD_SCROLLED,
	EMO_FIELD_SET,
	EMO_FIELD_SOLID,
	EMO_FIELD_STACK,
	EMO_FIELD_STARTCOUNT,
	EMO_FIELD_STARTINDEX,
	EMO_FIELD_STENCIL,
	EMO_FIELD_TEXT,
	EMO_FIELD_THUMB_COLOR,
	EMO_FIELD_TOP,
	EMO_FIELD_TRACK_COLOR,
	EMO_FIELD_TRANSPARENCY,
	EMO_FIELD_TYPE,
	EMO_FIELD_VERTICAL,
	EMO_FIELD_VGRID,
	EMO_FIELD_VIEW,
	EMO_FIELD_WEIGHT,
	EMO_FIELD_WIDTH,
	EMO_FIELD_X,
	EMO_FIELD_XOFFSET,
	EMO_FIELD_Y,
	EMO_FIELD_YOFFSET,

	EMO_FIELD_UNKNOWN_FIELD
} EmoField;

extern const char *emo_field_to_string(int fieldId);
extern int emo_field_to_int(const char *fieldName);

#ifdef __cplusplus
}
#endif

#endif 


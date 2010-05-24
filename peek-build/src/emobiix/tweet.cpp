#include "tweet.h"
#include "lgui.h"
#include "p_malloc.h"
#include "EntryWidget.h"
#include "DataObject.h"
#include "Script.h"
#include "ConnectionContext.h"
#include "Debug.h"

#ifndef SIMULATOR
#include "bal_def.h"
#endif

#include "buikeymap.h"

/* new assets*/
/*#include "assets/landscape565.c"
#include "assets/sans-regular.h"
#include "assets/letterA4.c"
#include "assets/sunA4.c"
#include "assets/smsA4.c"*/
#include "assets/DroidSans.c"

/* new includes */
#include "Widget.h"
#include "DataObject.h"
#include "Style.h"
#include "WidgetRenderer.h"
#include "Gradient.h"
#include "Font.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int tweetDidInit = 0;

extern void handleSmsSend(const char *pszNumber, const char *pszMsg);
extern void recv_sms(void);
void makeHomeScreen(void);
void makeLockScreen(void);
void makeComposeSMSScreen(void);
 
Widget *lockScreen, *homeScreen, *smsComposeScreen;
extern "C" {
	Widget *currentScreen = NULL;
	Style *defaultStyle, *greenStyle, *currentStyle = NULL;
	//extern ConnectionContext *connectionContext = NULL;
}
/*DataObject *landscapeBackground;*/
DataObject *fontObject1, *fontObject2, *fontObject3;
/*DataObject *letterObject, *sunObject, *smsObject;*/

Font *defaultFont, *hugeFont, *consoleFont;

Widget *emailPrv[4], *smsPrv[2], *emailPreview, *smsPreview; 
Widget *composeTo, *composeMsg, *composeSend;

static char toString[64];
static char msgString[512];

static char from1[512];
static char from2[512];
	Widget *smsBox;

#if  0
#define PROGRESS \
	--val;\
	if (val == 0) {\
		++bitsDone; \
		return 0; \
	}
#else
#define PROGRESS
#endif

int tweetInit(void)
{
	static int bitsDone = 1;
	int val;
	int progress = 0;



	if (tweetDidInit) {
#ifdef EMO_DEBUG
		emo_printf(" RYAN tweet init called when already initd\n");
#endif
		return 1;
	}
	toString[0] = 0;
	msgString[0] = 0;
	strcpy(from1, "Annie Jacks - Hey call me when your done");
	strcpy(from2, "Tim Schoff - nope not yet");

	val = bitsDone;

#if 0
	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- background \n");
#endif
	landscapeBackground = dataobject_new();

	if (!landscapeBackground)
	{
#ifdef EMO_DEBUG
		emo_printf("EMOBIIX: Failed to create background");
#endif
		return 0;
	}

	dataobject_setValue(landscapeBackground, "type", dataobjectfield_string("image"));
	dataobject_setValue(landscapeBackground, "width", dataobjectfield_int(landscape_image.width));
	dataobject_setValue(landscapeBackground, "height", dataobjectfield_int(landscape_image.height));
	dataobject_setValue(landscapeBackground, "pixelformat", dataobjectfield_int(RGB565));
	dataobject_setValue(landscapeBackground, "data", dataobjectfield_data((void *)landscape_image.pixel_data, landscape_image.bytes_per_pixel*landscape_image.height*landscape_image.width));
#ifdef EMO_DEBUG
	emo_printf("\n -- background done \n");
#endif
	}
	PROGRESS

	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- letter \n");
#endif
	letterObject = dataobject_new();
	dataobject_setValue(letterObject, "type", dataobjectfield_string("image"));
	dataobject_setValue(letterObject, "width", dataobjectfield_int(letter_image.width));
	dataobject_setValue(letterObject, "height", dataobjectfield_int(letter_image.height));
	dataobject_setValue(letterObject, "pixelformat", dataobjectfield_int(A4));
	dataobject_setValue(letterObject, "data", dataobjectfield_data((void *)letter_image.pixel_data, letter_image.bytes_per_pixel*letter_image.height*letter_image.width));
#ifdef EMO_DEBUG
	emo_printf("\n -- letter done \n");
#endif
	}
	PROGRESS

	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- sms \n");
#endif
	smsObject = dataobject_new();
	dataobject_setValue(smsObject, "type", dataobjectfield_string("image"));
	dataobject_setValue(smsObject, "width", dataobjectfield_int(sms_image.width));
	dataobject_setValue(smsObject, "height", dataobjectfield_int(sms_image.height));
	dataobject_setValue(smsObject, "pixelformat", dataobjectfield_int(A4));
	dataobject_setValue(smsObject, "data", dataobjectfield_data((void *)sms_image.pixel_data, sms_image.bytes_per_pixel*sms_image.height*sms_image.width));
#ifdef EMO_DEBUG
	emo_printf("\n -- sms done \n");
#endif
	}
	PROGRESS


	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- sun \n");
#endif
	sunObject = dataobject_new();
	dataobject_setValue(sunObject, "type", dataobjectfield_string("image"));
	dataobject_setValue(sunObject, "width", dataobjectfield_int(sun_image.width));
	dataobject_setValue(sunObject, "height", dataobjectfield_int(sun_image.height));
	dataobject_setValue(sunObject, "pixelformat", dataobjectfield_int(A4));
	dataobject_setValue(sunObject, "data", dataobjectfield_data((void *)sun_image.pixel_data, sun_image.bytes_per_pixel*sun_image.height*sun_image.width));
#ifdef EMO_DEBUG
	emo_printf("\n -- sun done \n");
#endif
	}
	PROGRESS
#endif

	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- sans \n");
#endif
	fontObject1 = dataobject_new();
#if 1
	dataobject_setValue(fontObject1, "type", dataobjectfield_string("font"));
	dataobject_setValue(fontObject1, "data", dataobjectfield_data((void *)DroidSans, DroidSansSize));
#endif
#ifdef EMO_DEBUG
	emo_printf("\n -- sans done \n");
#endif
	}
	PROGRESS

#if 1
	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- font \n");
#endif
	defaultFont = font_load(fontObject1);
	if (defaultFont != NULL)
		font_setHeight(defaultFont, 12);
#ifdef EMO_DEBUG
	emo_printf("\n -- font done \n");
#endif
	}
	PROGRESS

#if 0
	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- sansbold \n");
#endif
	fontObject2 = dataobject_new();
#if 1
	dataobject_setValue(fontObject2, "type", dataobjectfield_string("font"));
	dataobject_setValue(fontObject2, "data", dataobjectfield_data((void *)DroidSansBold, DroidSansBoldSize));
#endif
#ifdef EMO_DEBUG
	emo_printf("\n -- sansbold done \n");
#endif
	}
	PROGRESS
#endif

	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- hugefont \n");
#endif
	hugeFont = font_load(fontObject1);
	font_setHeight(hugeFont, 20);
#ifdef EMO_DEBUG
	emo_printf("\n -- font done \n");
#endif
	}
	PROGRESS
#endif

#if 0
	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- topaz \n");
#endif
	fontObject3 = dataobject_new();
#if 1
	dataobject_setValue(fontObject3, "type", dataobjectfield_string("font"));
	dataobject_setValue(fontObject3, "data", dataobjectfield_data((void *)Topaz8, Topaz8Size));
#endif
#ifdef EMO_DEBUG
	emo_printf("\n -- topaz done \n");
#endif
	}
	PROGRESS
#endif


	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- hugefont \n");
#endif
	consoleFont = font_load(fontObject1);
	font_setHeight(consoleFont, 8);
#ifdef EMO_DEBUG
	emo_printf("\n -- font done \n");
#endif
	}
	PROGRESS

	static Color color;
	static Gradient *grad1;
	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- g1 \n");
#endif
	grad1 = gradient_new();
	color.value = 0xF9F9F9FF;
	gradient_addStop(grad1, 0, color);
	color.value = 0xC9C9C9FF;
	gradient_addStop(grad1, 50, color);
	color.value = 0xB1B1B1FF;
	gradient_addStop(grad1, 50, color);
	color.value = 0xDEDEDEFF;
	gradient_addStop(grad1, 100, color);
#ifdef EMO_DEBUG
	emo_printf("\n -- g1 done \n");
#endif
	}
	PROGRESS

	static Gradient *greengrad1;
	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- gg1 \n");
#endif
	greengrad1 = gradient_new();
	if (greengrad1 == NULL)
	{
#ifdef EMO_DEBUG
		emo_printf("gg1 is NULL\n\n");
#endif
	}
	color.value = 0x10CF0DFF;
	gradient_addStop(greengrad1, 0, color);
	color.value = 0x008300FF;
	gradient_addStop(greengrad1, 50, color);
	color.value = 0x007000FF;
	gradient_addStop(greengrad1, 50, color);
	color.value = 0x005823FF;
	gradient_addStop(greengrad1, 100, color);
#ifdef EMO_DEBUG
	emo_printf("\n -- gg1 done \n");
#endif
	}
	PROGRESS

	static Gradient *grad2;
	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- g2 \n");
#endif
	grad2 = gradient_new();
	color.value = 0xF9F9F9FF;
	gradient_addStop(grad2, 0, color);
	color.value = 0x59DFFEFF;
	gradient_addStop(grad2, 50, color);
	color.value = 0x35dcfbFF;
	gradient_addStop(grad2, 50, color);
	color.value = 0xb7f1ffFF;
	gradient_addStop(grad2, 100, color);
#ifdef EMO_DEBUG
	emo_printf("\n -- g2 done \n");
#endif
	}
	PROGRESS

	static Gradient *greengrad2;
	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- gg2 \n");
#endif
	greengrad2 = gradient_new();
	color.value = 0xE4FD13FF;
	gradient_addStop(greengrad2, 0, color);
	color.value = 0x8dc809FF;
	gradient_addStop(greengrad2, 50, color);
	color.value = 0x72b909FF;
	gradient_addStop(greengrad2, 50, color);
	color.value = 0xbfe412FF;
	gradient_addStop(greengrad2, 100, color);
#ifdef EMO_DEBUG
	emo_printf("\n -- gg2 done \n");
#endif
	}
	PROGRESS

	if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- style \n");
#endif
	defaultStyle = style_new();
	greenStyle = style_new();
	style_setProperty(defaultStyle, NULL, NULL, "image",
			"renderer", (void *)widgetrenderer_image());
	style_setProperty(greenStyle, NULL, NULL, "image",
			"renderer", (void *)widgetrenderer_image());
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, "image", NULL,
			"color", (void *)color.value);
	color.value = 0xFFFFFFFF;
	style_setProperty(greenStyle, NULL, "image", NULL,
			"color", (void *)color.value);
	style_setProperty(defaultStyle, NULL, "gradbox", NULL,
			"renderer", (void *)widgetrenderer_gradbox());
	style_setProperty(greenStyle, NULL, "gradbox", NULL,
			"renderer", (void *)widgetrenderer_gradbox());
	style_setProperty(defaultStyle, NULL, "gradbox", NULL,
			"gradient", (void *)grad1);
	style_setProperty(greenStyle, NULL, "gradbox", NULL,
			"gradient", (void *)greengrad1);

	style_setProperty(defaultStyle, NULL, "gradboxr", NULL,
			"renderer", (void *)widgetrenderer_gradboxr());
	style_setProperty(greenStyle, NULL, "gradboxr", NULL,
			"renderer", (void *)widgetrenderer_gradboxr());
	style_setProperty(defaultStyle, NULL, "gradboxr", NULL,
			"gradient", (void *)grad1);
	style_setProperty(greenStyle, NULL, "gradboxr", NULL,
			"gradient", (void *)greengrad1);
	style_setProperty(defaultStyle, NULL, "gradboxr", NULL,
			"focusgradient", (void *)grad2);
	style_setProperty(greenStyle, NULL, "gradboxr", NULL,
			"focusgradient", (void *)greengrad2);
	style_setProperty(defaultStyle, NULL, "gradboxr", NULL,
			"radius", (void *)7);
	style_setProperty(greenStyle, NULL, "gradboxr", NULL,
			"radius", (void *)7);
	color.value = 0x696969FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", NULL,
			"outline", (void *)color.value);
	color.value = 0x69F969FF;
	style_setProperty(greenStyle, NULL, "gradboxr", NULL,
			"outline", (void *)color.value);
	color.value = 0x696989FF;
	style_setProperty(defaultStyle, NULL, "gradboxr", NULL,
			"focusoutline", (void *)color.value);
	style_setProperty(greenStyle, NULL, "gradboxr", NULL,
			"focusoutline", (void *)color.value);

	style_setProperty(defaultStyle, NULL, "selbox", NULL,
			"renderer", (void *)widgetrenderer_gradboxr());
	style_setProperty(greenStyle, NULL, "selbox", NULL,
			"renderer", (void *)widgetrenderer_gradboxr());
	style_setProperty(defaultStyle, NULL, "selbox", NULL,
			"gradient", NULL);
	style_setProperty(greenStyle, NULL, "selbox", NULL,
			"gradient", NULL);
	style_setProperty(defaultStyle, NULL, "selbox", NULL,
			"focusgradient", (void *)grad2);
	style_setProperty(greenStyle, NULL, "selbox", NULL,
			"focusgradient", (void *)greengrad2);
	style_setProperty(defaultStyle, NULL, "selbox", NULL,
			"radius", (void *)7);
	style_setProperty(greenStyle, NULL, "selbox", NULL,
			"radius", (void *)7);
	color.value = 0x696969FF;
	style_setProperty(defaultStyle, NULL, "selbox", NULL,
			"outline", (void *)color.value);
	color.value = 0x69F969FF;
	style_setProperty(greenStyle, NULL, "selbox", NULL,
			"outline", (void *)color.value);
	color.value = 0x696989FF;
	style_setProperty(defaultStyle, NULL, "selbox", NULL,
			"focusoutline", (void *)color.value);
	color.value = 0x69F989FF;
	style_setProperty(greenStyle, NULL, "selbox", NULL,
			"focusoutline", (void *)color.value);

	style_setProperty(defaultStyle, NULL, "solid", NULL,
			"renderer", (void *)widgetrenderer_solid());
	style_setProperty(greenStyle, NULL, "solid", NULL,
			"renderer", (void *)widgetrenderer_solid());
	color.value = 0x878787FF;
	style_setProperty(defaultStyle, NULL, "solid", NULL,
			"color", (void *)color.value);
	color.value = 0x263914FF;
	style_setProperty(greenStyle, NULL, "solid", NULL,
			"color", (void *)color.value);
	style_setProperty(defaultStyle, NULL, "label", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(greenStyle, NULL, "label", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "label", "string",
			"font", (void *)defaultFont);
	style_setProperty(greenStyle, NULL, "label", "string",
			"font", (void *)defaultFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(greenStyle, NULL, "label", "string",
			"color", (void *)color.value);
	style_setProperty(defaultStyle, NULL, "biglabel", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(greenStyle, NULL, "biglabel", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "biglabel", "string",
			"font", (void *)hugeFont);
	style_setProperty(greenStyle, NULL, "biglabel", "string",
			"font", (void *)hugeFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(greenStyle, NULL, "biglabel", "string",
			"color", (void *)color.value);

	style_setProperty(defaultStyle, NULL, "biglabelw", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(greenStyle, NULL, "biglabelw", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "biglabelw", "string",
			"font", (void *)hugeFont);
	style_setProperty(greenStyle, NULL, "biglabelw", "string",
			"font", (void *)hugeFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "biglabelw", "string",
			"color", (void *)color.value);
	style_setProperty(greenStyle, NULL, "biglabelw", "string",
			"color", (void *)color.value);

	style_setProperty(defaultStyle, NULL, "labelw", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(greenStyle, NULL, "labelw", "string",
			"renderer", (void *)widgetrenderer_string());
	style_setProperty(defaultStyle, NULL, "labelw", "string",
			"font", (void *)defaultFont);
	style_setProperty(greenStyle, NULL, "labelw", "string",
			"font", (void *)defaultFont);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "labelw", "string",
			"color", (void *)color.value);
	color.value = 0xFFFFFFFF;
	style_setProperty(greenStyle, NULL, "labelw", "string",
			"color", (void *)color.value);

	style_setProperty(defaultStyle, NULL, "entry", "entry",
			"renderer", (void *)widgetrenderer_entry());
	style_setProperty(greenStyle, NULL, "entry", "entry",
			"renderer", (void *)widgetrenderer_entry());
	style_setProperty(defaultStyle, NULL, "entry", "entry",
			"font", (void *)defaultFont);
	style_setProperty(greenStyle, NULL, "entry", "entry",
			"font", (void *)defaultFont);
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, "entry", "entry",
			"color", (void *)color.value);
	color.value = 0x000000FF;
	style_setProperty(greenStyle, NULL, "entry", "entry",
			"color", (void *)color.value);
	color.value = 0xFFFFFFFF;
	style_setProperty(defaultStyle, NULL, "entry", "entry",
			"background", (void *)color.value);
	color.value = 0xCFFFCFFF;
	style_setProperty(greenStyle, NULL, "entry", "entry",
			"background", (void *)color.value);
	color.value = 0x000000FF;
	style_setProperty(defaultStyle, NULL, "entry", "entry",
			"line", (void *)color.value);
	color.value = 0x207020FF;
	style_setProperty(greenStyle, NULL, "entry", "entry",
			"line", (void *)color.value);
	
	color.value = 0x8080F0FF;
	style_setProperty(defaultStyle, NULL, "entry", "entry",
			"focusline", (void *)color.value);
	color.value = 0xFFFF00FF;
	style_setProperty(greenStyle, NULL, "entry", "entry",
			"focusline", (void *)color.value);

	currentStyle = defaultStyle;
	
#ifdef EMO_DEBUG
	emo_printf("\n -- style done \n");
#endif
	}

		if (val == 1) {
#ifdef EMO_DEBUG
	emo_printf("\n -- screen \n");
#endif
	makeLockScreen();
	makeHomeScreen();
	makeComposeSMSScreen();
	currentScreen = lockScreen;
#ifdef EMO_DEBUG
	emo_printf("\n -- screen done \n");
#endif
	}
	PROGRESS

	if (val == 1) {
 	void *regaddr;
	regaddr = (void*)((unsigned int)0xFFFE4800);
	//*(unsigned int*) regaddr = 0xFFFFFFFF;
	}
	PROGRESS

	tweetDidInit = 1;

	return 1;
}

void tweetDrawScreen(void)
{
	if (!tweetDidInit) {
#ifdef EMO_DEBUG
		emo_printf(" RYAN Draw screen called before init, returning\n");
#endif
		return;
	}

	if (currentStyle == NULL || currentScreen == NULL) {
#ifdef EMO_DEBUG
		emo_printf("\n RYAN - style or screen is NULL\n\n");
#endif
		return;
	}

		style_renderWidgetTree(currentStyle, currentScreen);
}

void handleComposeKey(int key)
{
	Widget *focus;

	if (key == KP_BACKSPACE_KEY)
		currentScreen = homeScreen;

	focus = NULL;
	if (currentScreen != NULL)
		focus = widget_focusWhichOneNF(currentScreen);
	if (focus == NULL)
		return;
	if (key == 46 && focus == composeSend) {
#ifdef EMO_DEBUG
		emo_printf("Send SMS\n");
#endif
		currentScreen = homeScreen;
		style_renderWidgetTree(currentStyle, currentScreen);
		handleSmsSend(toString, msgString);
	}

#ifndef SIMULATOR
	switch (key) {
		case KP_0_KEY: key = '0'; break;
		case KP_1_KEY: key = '1'; break;
		case KP_2_KEY: key = '2'; break;
		case KP_3_KEY: key = '3'; break;
		case KP_4_KEY: key = '4'; break;
		case KP_5_KEY: key = '5'; break;
		case KP_6_KEY: key = '6'; break;
		case KP_7_KEY: key = '7'; break;
		case KP_8_KEY: key = '8'; break;
		case KP_9_KEY: key = '9'; break;
		case KP_a_KEY: key = 'a'; break;
		case KP_b_KEY: key = 'b'; break;
		case KP_c_KEY: key = 'c'; break;
		case KP_d_KEY: key = 'd'; break;
		case KP_e_KEY: key = 'e'; break;
		case KP_f_KEY: key = 'f'; break;
		case KP_g_KEY: key = 'g'; break;
		case KP_h_KEY: key = 'h'; break;
		case KP_i_KEY: key = 'i'; break;
		case KP_j_KEY: key = 'j'; break;
		case KP_k_KEY: key = 'k'; break;
		case KP_l_KEY: key = 'l'; break;
		case KP_m_KEY: key = 'm'; break;
		case KP_n_KEY: key = 'n'; break;
		case KP_o_KEY: key = 'o'; break;
		case KP_p_KEY: key = 'p'; break;
		case KP_q_KEY: key = 'q'; break;
		case KP_r_KEY: key = 'r'; break;
		case KP_s_KEY: key = 's'; break;
		case KP_t_KEY: key = 't'; break;
		case KP_u_KEY: key = 'u'; break;
		case KP_v_KEY: key = 'v'; break;
		case KP_w_KEY: key = 'w'; break;
		case KP_x_KEY: key = 'x'; break;
		case KP_y_KEY: key = 'y'; break;
		case KP_z_KEY: key = 'z'; break;
		case KP_A_KEY: key = 'A'; break;
		case KP_B_KEY: key = 'B'; break;
		case KP_C_KEY: key = 'C'; break;
		case KP_D_KEY: key = 'D'; break;
		case KP_E_KEY: key = 'E'; break;
		case KP_F_KEY: key = 'F'; break;
		case KP_G_KEY: key = 'G'; break;
		case KP_H_KEY: key = 'H'; break;
		case KP_I_KEY: key = 'I'; break;
		case KP_J_KEY: key = 'J'; break;
		case KP_K_KEY: key = 'K'; break;
		case KP_L_KEY: key = 'L'; break;
		case KP_M_KEY: key = 'M'; break;
		case KP_N_KEY: key = 'N'; break;
		case KP_O_KEY: key = 'O'; break;
		case KP_P_KEY: key = 'P'; break;
		case KP_Q_KEY: key = 'Q'; break;
		case KP_R_KEY: key = 'R'; break;
		case KP_S_KEY: key = 'S'; break;
		case KP_T_KEY: key = 'T'; break;
		case KP_U_KEY: key = 'U'; break;
		case KP_V_KEY: key = 'V'; break;
		case KP_W_KEY: key = 'W'; break;
		case KP_X_KEY: key = 'X'; break;
		case KP_Y_KEY: key = 'Y'; break;
		case KP_Z_KEY: key = 'Z'; break;
		case KP_SPACE_KEY: key = ' '; break;
	}
#endif

	char *str;
	Widget *w;
	static char nstr[2];
	int slen, blen;

	nstr[0] = key;
	nstr[1] = 0;

	if (focus == composeTo) {
		w = composeTo;
		str = toString;
		slen = strlen(toString);
		blen = 10;
	} else if (focus == composeMsg) {
		w = composeMsg;
		str = msgString;
		slen = strlen(msgString);
		blen = 512;
	} else
		return;

	if (slen >= blen)
		return;
	strcat(str, nstr);

	style_renderWidgetTree(currentStyle, w);
}

int tweetKey(int key)
{
	int isDirty = 0;
	Widget *focus;

	if (!tweetDidInit) {
#ifdef EMO_DEBUG
		emo_printf(" RYAN tweetKey called before Init, returning\n");
#endif
		return 1;
	}
#ifdef EMO_DEBUG
	emo_printf("Key %d pressed\n", key);
#endif

	focus = widget_focusWhichOneNF(currentScreen);
	if (focus != NULL) {
		DataObjectField *field;
		field = dataobject_getValue(focus, "type");
		if (field != NULL && field->type == DataObjectField::DOF_STRING &&
				strcmp(field->field.string, "entry") == 0) {
			entryWidget_handleKey(focus, key);
		}
	}

	switch (key) {
		case KP_1_KEY:
		case KP_2_KEY:
		case 46: // Push down on wheel
			script_event(focus != NULL ? focus : currentScreen, "onreturn");
			widget_resolveLayout(currentScreen, currentStyle);
			break;
		case 87: // up on scroll
		/*case 103:*/ /* GLUT a */
			widget_focusPrev(currentScreen, currentStyle);
			break;
		case 86: // down on scroll
		/*case 104:*/ /* GLUT z */
			widget_focusNext(currentScreen, currentStyle);
			break;
#if 0
		case KP_ENTER_KEY:
		case 46:
			isDirty = 1;
#ifdef EMO_DEBUG
			emo_printf("tweet enter\n");
#endif
			if (currentScreen == lockScreen) {
				currentScreen = homeScreen;
				/*style_renderWidgetTree(currentStyle, currentScreen);*/
			} else if (currentScreen == homeScreen) {
				currentScreen = smsComposeScreen;
				/*style_renderWidgetTree(currentStyle, currentScreen);*/
			} else if (currentScreen == smsComposeScreen)
				handleComposeKey(key);
			break;
		case KP_BACKSPACE_KEY:
		case 47:
			isDirty = 1;
#ifdef EMO_DEBUG
			emo_printf("tweet back\n");
#endif
			if (currentScreen == homeScreen) {
				currentScreen = lockScreen;
				/*style_renderWidgetTree(currentStyle, currentScreen);*/
			} else if (currentScreen == smsComposeScreen) {
				currentScreen = homeScreen;
				/*style_renderWidgetTree(currentStyle, currentScreen);*/
			}
			break;
		case KP_1_KEY:
			if (currentScreen == lockScreen)
				currentStyle = defaultStyle;
			else if (currentScreen == smsComposeScreen)
				handleComposeKey(key);
			break;
		case KP_2_KEY:
			if (currentScreen == lockScreen)
				currentStyle = greenStyle;
			else if (currentScreen == smsComposeScreen)
				handleComposeKey(key);
			break;
		case KP_3_KEY:
			if (currentScreen == smsComposeScreen)
				handleComposeKey(key);
			break;
		default:
			if (currentScreen == smsComposeScreen)
				handleComposeKey(key);
			break;
#endif
		default:
			break;
	}
/*	Widget *focus;
	focus = widget_focusWhichOneNF(currentScreen);
	if (focus == smsBox)
			recv_sms();*/
	return 1;
}

#define WIDGET(n, ix, iy, iw, ih, id) \
	n = widget_new();\
	if (id != 0) widget_setID(n, id);\
	box = widget_getBox(n);\
	box->x = (ix); box->y = (iy); box->width = (iw); box->height = (ih);


void makeLockScreen(void)
{
	lockScreen = widget_new();
	widget_setPacking(lockScreen, WP_VERTICAL);
#if 0
	widget_setID(lockScreen, "solid");
	dataobject_setValue(lockScreen, "color", dataobjectfield_uint(0x11883300));
#else
	widget_setID(lockScreen, "gradbox");
#endif

	DataObject *dobj;
	dobj = widget_new();
	dataobject_setValue(dobj, "data", dataobjectfield_string("emobiix"));
	dataobject_setValue(dobj, "type", dataobjectfield_string("string"));
	widget_setAlignment(dobj, WA_CENTER);
	widget_setID(dobj, "biglabel");
	widget_pack(dobj, lockScreen);

	dobj = widget_new();
	dataobject_setValue(dobj, "data", dataobjectfield_string("Application Boot Environment"));
	dataobject_setValue(dobj, "type", dataobjectfield_string("string"));
	widget_setAlignment(dobj, WA_CENTER);
	widget_setID(dobj, "label");
	widget_pack(dobj, lockScreen);

	DataObject *bobj;
	bobj = widget_new();
	widget_setPacking(bobj, WP_VERTICAL);
	widget_setID(bobj, "gradboxr");
	widget_pack(bobj, lockScreen);
	widget_setAlignment(bobj, WA_CENTER);
	dataobject_setValue(bobj, "onreturn", dataobjectfield_string(
			"dobj = DataObject.locate(\"tcp://69.114.111.9:12345/dataobject\"); dobj:toScreen();"));


	dobj = widget_new();
	dataobject_setValue(dobj, "data", dataobjectfield_string("Hello World"));
	dataobject_setValue(dobj, "type", dataobjectfield_string("string"));
	widget_setCanFocus(bobj, 1);
	widget_setAlignment(dobj, WA_CENTER);
	widget_setID(dobj, "label");
	widget_pack(dobj, bobj);

	bobj = widget_new();
	widget_setPacking(bobj, WP_VERTICAL);
	widget_setID(bobj, "gradboxr");
	widget_pack(bobj, lockScreen);
	widget_setAlignment(bobj, WA_CENTER);
	dataobject_setValue(bobj, "onreturn", dataobjectfield_string(
		"dobj = DataObject.locate(\"tcp://69.114.111.9:2501/dataobject\"); dobj:toScreen();"));
	
	dobj = widget_new();
	dataobject_setValue(dobj, "data", dataobjectfield_string("Calculator"));
	dataobject_setValue(dobj, "type", dataobjectfield_string("string"));
	widget_setCanFocus(bobj, 1);
	widget_setAlignment(dobj, WA_CENTER);
	widget_setID(dobj, "label");
	widget_pack(dobj, bobj);

	dobj = widget_new();
	dataobject_setValue(dobj, "data", dataobjectfield_string("tcp://69.114.111.9:12345/"));
	dataobject_setValue(dobj, "type", dataobjectfield_string("entry"));
	dataobject_setValue(dobj, "onreturn", dataobjectfield_string(
			"str = getValue(); dobj = DataObject.locate(str); dobj:toScreen();"));
	widget_setCanFocus(bobj, 1);
	widget_setAlignment(dobj, WA_CENTER);
	widget_setID(dobj, "entry");
	widget_setCanFocus(dobj, 1);
	dataobject_setValue(dobj, "width", dataobjectfield_string("90%"));
	widget_pack(dobj, lockScreen);

	widget_resolveLayout(lockScreen, currentStyle);
#if 0
	lockScreen = widget_new();
	/*widget_setDataObject(lockScreen, landscapeBackground);*/
	widget_setPacking(lockScreen, WP_FIXED);
	Rectangle *box;
	box = widget_getBox(lockScreen);
	box->x = 0;
	box->y = 0;
	box->width = 320;
	box->height = 240;

	DataObject *fromObj;
	fromObj = dataobject_new();
	dataobject_setValue(fromObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(fromObj, "data", dataobjectfield_string("2:30pm"));

	Widget *fromW;
	WIDGET(fromW, 24, 20, 50, 10, "biglabelw");
	widget_setDataObject(fromW, fromObj);
	widget_pack(fromW, lockScreen);

	fromObj = dataobject_new();
	dataobject_setValue(fromObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(fromObj, "data", dataobjectfield_string("Monday, March 29, 2010"));

	WIDGET(fromW, 24, 40, 50, 10, "labelw");
	widget_setDataObject(fromW, fromObj);
	widget_pack(fromW, lockScreen);
#endif
}

Widget *inboxEntry(int x, int y, const char *from, const char *subject, const char *stime)
{
	Widget *output;
	Rectangle *box;

	output = widget_new();
	WIDGET(output, x, y+2, 312, 24, "selbox");
	widget_setCanFocus(output, 1);

	Widget *letterW;
	WIDGET(letterW, x+6, y+10, 16, 10, 0);
	/*widget_setDataObject(letterW, letterObject);*/
	widget_pack(letterW, output);

	DataObject *fromObj;
	fromObj = dataobject_new();
	dataobject_setValue(fromObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(fromObj, "data", dataobjectfield_string(from));

	Widget *fromW;
	WIDGET(fromW, x+24, y+7, 50, 10, "label");
	widget_setDataObject(fromW, fromObj);
	widget_pack(fromW, output);

	fromObj = dataobject_new();
	dataobject_setValue(fromObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(fromObj, "data", dataobjectfield_string(subject));

	WIDGET(fromW, x+125, y+7, 50, 10, "label");
	widget_setDataObject(fromW, fromObj);
	widget_pack(fromW, output);

	fromObj = dataobject_new();
	dataobject_setValue(fromObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(fromObj, "data", dataobjectfield_string(stime));

	WIDGET(fromW, x+272, y+7, 50, 10, "label");
	widget_setDataObject(fromW, fromObj);
	widget_pack(fromW, output);

	return output;
}

static int entryCount = 0;

Widget *smsEntry(int x, int y, const char *from, const char *stime)
{
	Widget *output;
	Rectangle *box;

	output = widget_new();
	WIDGET(output, x, y+2, 312, 24, "selbox");
	widget_setCanFocus(output, 1);

	Widget *letterW;
	WIDGET(letterW, x+6, y+10, 16, 10, 0);
	/*widget_setDataObject(letterW, smsObject);*/
	widget_pack(letterW, output);

	DataObject *fromObj;
	fromObj = dataobject_new();
	dataobject_setValue(fromObj, "type", dataobjectfield_string("string"));
	if (entryCount == 0)
		dataobject_setValue(fromObj, "data", dataobjectfield_string(from1));
	else
		dataobject_setValue(fromObj, "data", dataobjectfield_string(from2));

	Widget *fromW;
	WIDGET(fromW, x+24, y+7, 50, 10, "label");
	widget_setDataObject(fromW, fromObj);
	widget_pack(fromW, output);

	fromObj = dataobject_new();
	dataobject_setValue(fromObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(fromObj, "data", dataobjectfield_string(stime));

	WIDGET(fromW, x+272, y+7, 50, 10, "label");
	widget_setDataObject(fromW, fromObj);
	widget_pack(fromW, output);

	++entryCount;

	return output;
}

void makeHomeScreen(void)
{
	Rectangle *box;

	homeScreen = widget_new();
	widget_setPacking(homeScreen, WP_VERTICAL);
	widget_setID(homeScreen, "solid");
	box = widget_getBox(homeScreen);
	box->x = 0;
	box->y = 0;
	box->width = 320;
	box->height = 240;

	Widget *topBar;
	topBar = widget_new();
	widget_setPacking(topBar, WP_HORIZONTAL);
	widget_setID(topBar, "gradbox");
	box = widget_getBox(topBar);
	box->x = 0;
	box->y = 0;
	box->width = 320;
	box->height = 17;
	widget_pack(topBar, homeScreen);

	Widget *timeBox;
	timeBox = widget_new();
	widget_setPacking(timeBox, WP_VERTICAL);
	widget_setID(timeBox, "gradboxr");
	box = widget_getBox(timeBox);
	box->x = 4;
	box->y = 22;
	box->width = 154;
	box->height = 52;
	widget_pack(timeBox, homeScreen);

	DataObject *labelObj;
	labelObj = dataobject_new();
	dataobject_setValue(labelObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(labelObj, "data", dataobjectfield_string("2:30pm"));

	Widget *timeW;
	timeW = widget_new();
	widget_setDataObject(timeW, labelObj);
	widget_setID(timeW, "biglabel");
	box = widget_getBox(timeW);
	box->x = 28;
	box->y = 26;
	box->width = 74;
	box->height = 20;
	widget_pack(timeW, timeBox);

	labelObj = dataobject_new();
	dataobject_setValue(labelObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(labelObj, "data", dataobjectfield_string("Mon March 29"));

	Widget *dateW;
	dateW = widget_new();
	widget_setDataObject(dateW, labelObj);
	widget_setID(dateW, "label");
	box = widget_getBox(dateW);
	box->x = 28;
	box->y = 50;
	box->width = 74;
	box->height = 10;
	widget_pack(dateW, timeBox);

	Widget *weatherBox;
	weatherBox = widget_new();
	widget_setPacking(weatherBox, WP_VERTICAL);
	widget_setID(weatherBox, "gradboxr");
	box = widget_getBox(weatherBox);
	box->x = 162;
	box->y = 22;
	box->width = 154;
	box->height = 52;
	widget_pack(weatherBox, homeScreen);

	Widget *sunW;
	WIDGET(sunW, 170, 32, 16, 10, 0);
	/*widget_setDataObject(sunW, sunObject);*/
	widget_pack(sunW, weatherBox);

	labelObj = dataobject_new();
	dataobject_setValue(labelObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(labelObj, "data", dataobjectfield_string("72\xc2\xb0"));

	Widget *degW;
	degW = widget_new();
	widget_setDataObject(degW, labelObj);
	widget_setID(degW, "biglabel");
	box = widget_getBox(degW);
	box->x = 210;
	box->y = 26;
	box->width = 74;
	box->height = 20;
	widget_pack(degW, weatherBox);

	labelObj = dataobject_new();
	dataobject_setValue(labelObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(labelObj, "data", dataobjectfield_string("New York, NY"));

	Widget *locW;
	locW = widget_new();
	widget_setDataObject(locW, labelObj);
	widget_setID(locW, "label");
	box = widget_getBox(locW);
	box->x = 210;
	box->y = 50;
	box->width = 74;
	box->height = 10;
	widget_pack(locW, weatherBox);

	Widget *emailBox;
	emailBox = widget_new();
	widget_setPacking(emailBox, WP_VERTICAL);
	widget_setID(emailBox, "gradboxr");
	box = widget_getBox(emailBox);
	box->x = 4;
	box->y = 79;
	box->width = 312;
	box->height = 99;
	widget_pack(emailBox, homeScreen);
	widget_setCanFocus(emailBox, 1);

#define POS_D 25
	int pos = 0;
	Widget *inbox;
	inbox = inboxEntry(4, 78+pos, "Todd Carter", "Re: TPS Reports", "1:12p");
	widget_pack(inbox, emailBox); pos += POS_D; emailPrv[0] = inbox;
	inbox = inboxEntry(4, 78+pos, "Dagan Bensen", "Re: Status update", "11:12a");
	widget_pack(inbox, emailBox); pos += POS_D; emailPrv[1] = inbox;
	inbox = inboxEntry(4, 78+pos, "Ryan Krumins", "TPS Reports", "10:12a");
	widget_pack(inbox, emailBox); pos += POS_D; emailPrv[2] = inbox;
	inbox = inboxEntry(4, 78+pos, "Andy Bryant", "Emobiix", "9:16a");
	widget_pack(inbox, emailBox); pos += POS_D; emailPrv[3] = inbox;
	emailPreview = emailBox;

	smsBox = widget_new();
	widget_setPacking(smsBox, WP_VERTICAL);
	widget_setID(smsBox, "gradboxr");
	widget_setCanFocus(smsBox, 1);
	box = widget_getBox(smsBox);
	box->x = 4;
	box->y = 182;
	box->width = 312;
	box->height = 52;
	widget_pack(smsBox, homeScreen);

	Widget *sms;
	pos = 0;
	sms = smsEntry(4, 181+pos, "Annie Jacks - Hey call me when your done", "1:14p");
	widget_pack(sms, smsBox); pos += POS_D; smsPrv[0] = sms;
	sms = smsEntry(4, 181+pos, "Tim Schoff - nope not yet", "11:37a");
	widget_pack(sms, smsBox); pos += POS_D; smsPrv[1] = sms;
	smsPreview = smsBox;

	//dataobject_debugPrint(homeScreen);
}

void makeComposeSMSScreen(void)
{
	Widget *output;
	Rectangle *box;
	DataObject *labelObj;
	Widget *labelW, *label2;

	WIDGET(output, 0, 0, 320, 240, "solid");

	Widget *topBar;
	WIDGET(topBar, 0, 0, 320, 17, "gradbox");
	widget_pack(topBar, output);

	Widget *mainBar;
	WIDGET(mainBar, 4, 20, 312, 240, "gradboxr");
	widget_pack(mainBar, output);

	labelObj = dataobject_new();
	dataobject_setValue(labelObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(labelObj, "data", dataobjectfield_string("Compose Message"));

	WIDGET(labelW, 105, 22, 100, 22, "label");
	widget_setDataObject(labelW, labelObj);
	widget_pack(labelW, mainBar);

	labelObj = dataobject_new();
	dataobject_setValue(labelObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(labelObj, "data", dataobjectfield_string("To"));

	WIDGET(labelW, 14, 42, 100, 22, "label");
	widget_setDataObject(labelW, labelObj);
	widget_pack(labelW, mainBar);

	labelObj = dataobject_new();
	dataobject_setValue(labelObj, "type", dataobjectfield_string("entry"));
	dataobject_setValue(labelObj, "data", dataobjectfield_string(toString));

	WIDGET(labelW, 34, 42, 150, 15, "entry");
	widget_setDataObject(labelW, labelObj);
	widget_pack(labelW, mainBar);
	widget_setCanFocus(labelW, 1);
	composeTo = labelW;

	labelObj = dataobject_new();
	dataobject_setValue(labelObj, "type", dataobjectfield_string("entry"));
	dataobject_setValue(labelObj, "data", dataobjectfield_string(msgString));

	WIDGET(labelW, 14, 62, 290, 13*10+4, "entry");
	widget_setDataObject(labelW, labelObj);
	widget_pack(labelW, mainBar);
	widget_setCanFocus(labelW, 1);
	composeMsg = labelW;

	WIDGET(labelW, 139, 200, 39, 21, "gradboxr");
	widget_pack(labelW, mainBar);
	widget_setCanFocus(labelW, 1);

	labelObj = dataobject_new();
	dataobject_setValue(labelObj, "type", dataobjectfield_string("string"));
	dataobject_setValue(labelObj, "data", dataobjectfield_string("Send"));

	WIDGET(label2, 144, 204, 30, 15, "label");
	widget_setDataObject(label2, labelObj);
	widget_pack(label2, labelW);
	composeSend = labelW;

	smsComposeScreen = output;
}

void tweetRecvSMS(char *number, char *text)
{
	memcpy(from2, from1, 64);
	sprintf(from1, "%s - %s", number, text);
	from1[64] = 0;
}

#ifdef SIMULATOR
void handleSmsSend(const char *pszNumber, const char *pszMsg) {}
#endif

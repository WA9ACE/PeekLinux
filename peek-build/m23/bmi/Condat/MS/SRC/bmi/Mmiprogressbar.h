
#ifndef _DEF_MMI_PROGRESSBAR_H_
#define _DEF_MMI_PROGRESSBAR_H_


#include "MmiDialogs.h"
#include "mfw_mnu.h"

//#define bar_width 150
#define bar_width 100
#define bar_height 20

#define x_screen 35
#define y_screen 150

#define download_percentage 100
#define playback_percentage 25

#define COLOR_RGB_DARK_RED         "#8B0000"
#define COLOR_RGB_DARK_BLUE        "#00008B"
#define COLOR_RGB_DARK_YELLOW      "#FFD700"
#define COLOR_RGB_DARK_GREEN       "#006400"
#define COLOR_RGB_DARK_GREY        "#A9A9A9"
#define COLOR_RGB_DARK_CYAN        "#008B8B"
#define COLOR_RGB_DARK_MAGENTA     "#8B008B"
#define COLOR_RGB_BLACK            "#000000"
#define COLOR_RGB_GREY             "#808080"
#define COLOR_RGB_WHITE            "#FFFFFF"
#define COLOR_RGB_BRIGHT_RED       "#FF0000"
#define COLOR_RGB_BRIGHT_YELLOW    "#FFFF00"
#define COLOR_RGB_BRIGHT_GREEN     "#008000"
#define COLOR_RGB_BRIGHT_CYAN      "#00FFFF"
#define COLOR_RGB_BRIGHT_BLUE      "#0000FF"
#define COLOR_RGB_BRIGHT_MAGENTA   "#FF00FF"
typedef float				Float;

typedef struct _tProgressData_
{
    /* administrative data */

    T_MMI_CONTROL     mmi_control;
    T_MFW_HND            win;
    T_MFW_HND            parent_win;
    MfwHnd   kbd; 

    /* associated handlers */

} tProgressData;



/********************************************************************

					Prototypes

********************************************************************/

T_MFW_HND progress_bar(T_MFW_HND	 parent );
static int ProgressBar_cb(MfwEvt , MfwWin * );

void MmiProgressIndication (UBYTE , UBYTE , UBYTE , UBYTE, int , int );


#endif /* _DEF_MMI_PROGRESSBAR */





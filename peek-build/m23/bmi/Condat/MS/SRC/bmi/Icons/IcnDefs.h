#ifndef _MMIICNDEFS_H_
#define _MMIICNDEFS_H_ 1

/*******************************************************************************

					CONDAT (UK)

********************************************************************************                                                                              

 This software product is the property of Condat (UK) Ltd and may not be
 disclosed to any third party without the express permission of the owner.                                 
                                                                              
********************************************************************************

 $Project name:	Basic MMI                                                      
 $Project code:	BMI (6349)                                                           
 $Module:		Icons
 $File:		    IcnDefs.h
 $Revision:		1.0                                                       
                                                                              
 $Author:		Condat(UK)                                                         
 $Date:		    01/11/02                                                      
                                                                               
********************************************************************************
                                                                              
 Description:

    This provides the references to the colour icon data arrays
                        
********************************************************************************
 $History: 

	01/11/02			Original Condat(UK) BMI version.	
	   
 $End

*******************************************************************************/
#include "typedefs.h"
enum {
//Idle screen
	ICON_IDLE_Background,

	ICON_IDLE_Battery4,
	ICON_IDLE_Battery14,
	ICON_IDLE_Battery24,
	ICON_IDLE_Battery49,
	ICON_IDLE_Battery100,
	ICON_IDLE_forwarding,
	ICON_IDLE_keylock,
	ICON_IDLE_message,
	ICON_IDLE_signalStr49, 	
	ICON_IDLE_signalStr99, 	
	ICON_IDLE_signalStr149, 	
	ICON_IDLE_signalStr199,
	ICON_IDLE_signalStr255,
	ICON_IDLE_homeZone,
	ICON_IDLE_gprs,
	ICON_IDLE_voicemail,


//'main' icons
	ICON_MAIN_Application,
	ICON_MAIN_Divert,
	ICON_MAIN_Messages,
	ICON_MAIN_Phonebook,
	ICON_MAIN_RecentCalls,
	ICON_MAIN_Security,
	ICON_MAIN_Settings,
	ICON_MAIN_SIM_Toolkit,
	ICON_MAIN_WAP,

//popup
	ICON_POPUP_CLOUDS,

//network
	ICON_Bouygues,
	ICON_Vodafone,
	ICON_SFR,
	ICON_VodafoneD2,
	ICON_Chinamobile,
	ICON_Fareast,
	ICON_orange,
	ICON_sonofon,
	ICON_Tmobile,

//Background
	ICON_BgdSquares,
#if (BOARD == 61)
/* OMAPS00074454 - 24-06-2006*/
	ICON_IDLE_edge, 
/* END */
#endif
	
	ICON_END
	
};

enum {
	ICON_TYPE_bw,
	ICON_TYPE_256Colour,			//R=0xE0, G=0x1C, B=0x03
	ICON_TYPE_24BitColour,			//R=0xFF0000, G=0x00FF00, B=0x0000FF
	ICON_TYPE_32BitColour,			//riviera format with alpha=0xFF000000, 
	ICON_TYPE_256PaletteColour,		//separate palette needed 

	ICON_TYPE_LCD16bit				//data matches D-sample LCD format 32 bits with 16bits used.
};

typedef struct {
  U32    width;
  U32    height;
  U32    bytes_per_pixel; /* 3:RGB, 4:RGBA */
  U8     pixel_data[20 * 10 * 3 + 1];
} I_Bat;

/*
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0g\377\1g\377\1g\377\1g\377\1g\377\1g"
  "\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377"
  "\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1""5\203\1\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1"
  "g\377\1g\377\1g\377\1g\377\1g\377\1""5\203\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g"
  "\377\1g\377\1g\377\1g\377\1""5\203\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377"
  "\1g\377\1g\377\1""5\203\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0g\377\1g\377"
  "\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1g\377\1"
  "g\377\1g\377\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
  "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
*/
int icnGPRS_dataformat( char* *bmp, int *sx, int *sy, int *format, char **palette);
#if (BOARD == 61)
/*OMAPS00074454 - 24-06-2006 */
int icnEDGE_dataformat( char* *bmp, int *sx, int *sy, int *format, char **palette);
/*END */
#endif
int icnBgdSquares_dataformat( char* *bmp, int *sx, int *sy, int *format, char **palette);
int icnSignalStr_dataformat( int iconId, char* *bmp, int *sx, int *sy, int *format, char **palette);

//#define USE_110X110

//Logo for powerup screen
extern const unsigned char TIlogColour[];
//Main icons
#ifndef USE_110X110
extern const unsigned char applications97x97_bmp[];
extern const unsigned char netWorkServices96x96_bmp[];
extern const unsigned char messages96x96_bmp[];
extern const unsigned char phonebook96x96_bmp[];
extern const unsigned char recentcalls96x96_bmp[];
extern const unsigned char security96x96_bmp[];
extern const unsigned char phoneSettings96x96_bmp[];
extern const unsigned char stkColour[];
extern const unsigned char wap96x96_bmp[];
#else
extern const unsigned char applications110x110_bmp[];
extern const unsigned char recentcalls110x110_bmp[] ;
extern const unsigned char messages110x110_bmp[] ;
extern const unsigned char phonebook110x110_bmp[] ;
extern const unsigned char security110x110_bmp[] ;
extern const unsigned char wap110x110_bmp[] ;
extern const unsigned char networkservices110x110_bmp[];
extern const unsigned char phonesettings110x110_bmp[] ;
extern const unsigned char stkColour[];
#endif
//Popup + background
extern const unsigned char bmpDataBubbles[];
extern const unsigned char bmpDataCloud[];
extern const unsigned char bmpDataSquares[];
extern const unsigned char Edinburgh1_bgd_bmp[];
extern const unsigned char Edinburgh2_bgd_bmp[];
extern const unsigned char Edinburgh3_bgd_bmp[];
extern const unsigned char Edinburgh4_bgd_bmp[];

//Network bitmaps
extern const unsigned char Bouygues_bmp[];
extern const unsigned char Vodafone_bmp[];
extern const unsigned char SFR_bmp[];
extern const unsigned char VodafoneD2_bmp[];
extern const unsigned char Chinamobile_bmp[]; 
extern const unsigned char Fareast_bmp[];
extern const unsigned char orange_bmp[];
extern const unsigned char SFR_bmp[];
extern const unsigned char sonofon_bmp[];
extern const unsigned char  Tmobile_bmp[];

//Idle Icons
extern const unsigned char I_forColour[] ;
extern const unsigned char I_klkColour[] ;

//Idle Bitmaps
extern const unsigned char Ti_bgd_bmp[] ;
extern const unsigned char I_batColour[] ;
extern const unsigned char iconInCallBackDrop[];

//Animation Bitmaps
extern const unsigned char callAnimation1[] ;
extern const unsigned char callAnimation2[] ;
extern const unsigned char callAnimation3[] ;

#ifdef WIDEICONS
#define I_batColour4 	I_batColour
#define I_batColour14 	I_batColour
#define I_batColour24 	I_batColour
#define I_batColour49 	I_batColour
#define I_batColour100 	I_batColour
#else
extern const unsigned char I_batColour4[] ;
extern const unsigned char I_batColour14[] ;
extern const unsigned char I_batColour24[] ;
extern const unsigned char I_batColour49[] ;
extern const unsigned char I_batColour100[] ;
#endif
extern const unsigned char I_msgColour[] ;
extern const unsigned char I_sigColour[] ;
#ifdef WIDEICONS
#define I_sigColour49 	I_sigColour
#define I_sigColour99 	I_sigColour
#define I_sigColour149 	I_sigColour
#define I_sigColour199 	I_sigColour
#define I_sigColour255	I_sigColour
#else
extern const unsigned char I_sigColour49[] ;
extern const unsigned char I_sigColour99[] ;
extern const unsigned char I_sigColour149[] ;
extern const unsigned char I_sigColour199[] ;
extern const unsigned char I_sigColour255[] ;
#endif
extern const unsigned char I_homeZoneColour[] ;
extern const unsigned char I_gprsColour[] ;

#if (BOARD == 61)
/* OMAPS00074454 - 24-04-2006 */
extern const unsigned char I_edgeColour[] ;
/* END */
#endif

extern const unsigned char I_voicemailColour[];
extern const unsigned char I_cipheringDeactivColour[];
extern const unsigned char I_cipheringActivColour[];

#endif

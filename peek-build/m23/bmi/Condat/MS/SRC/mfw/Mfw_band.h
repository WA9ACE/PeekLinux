#ifndef _MFW_BAND_H_
#define _MFW_BAND_H_

typedef enum
{	MFW_BAND_Busy= -2,
	MFW_BAND_Fail = -1,
	MFW_BAND_OK,
	MFW_BAND_InProgress
} T_MFW_BAND_RET;

typedef enum
{
	MFW_BAND_MODE_Auto	=  0,
	MFW_BAND_MODE_Manual	=  1
} T_MFW_BAND_MODE;

/* SPR919 - SH - Band is now a bit field*/
/*a0393213 compiler warnings removal - typedef removed*/
enum
{
	MFW_BAND_GSM_900		=	1,
	MFW_BAND_DCS_1800		=	2,
	MFW_BAND_PCS_1900		=	4,
	MFW_BAND_E_GSM			=	8,
	MFW_BAND_GSM_850		=	16
};

T_MFW_BAND_RET band_radio_mode_switch(T_MFW_BAND_MODE mode, UBYTE band);

T_MFW_BAND_RET band_get_radio_modes(T_MFW_BAND_MODE* maxMode, UBYTE *band);

T_MFW_BAND_RET band_get_current_radio_mode(T_MFW_BAND_MODE* mode, UBYTE *band);
#endif /* _MFW_BAND_H_*/

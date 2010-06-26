#include "IcnDefs.h"

#if 0

char NO_PALETTE[] = {0x00,0x00,0x00};
char NO_BMP_DATA[] = {0x00,0x00,0x00};

//Function to get icon information
int icn_getIconData( int iconId, int * op_sx, int * op_sy, char* *op_bmpData, int *op_format)
{
	char**palette;
	*op_sx = 0;	
	*op_sy = 0;
	*op_bmpData = NO_BMP_DATA;
	*palette = NO_PALETTE;
#ifdef COLOURDISPLAY
	switch (iconId)
	{
	case ICON_IDLE_Background:

	case ICON_IDLE_Battery4:		icnBattery_dataformat(iconId,op_bmpData, op_sx, op_sy, op_format, palette);	break;
	case ICON_IDLE_Battery14:		icnBattery_dataformat(iconId,op_bmpData, op_sx, op_sy, op_format, palette);	break;
	case ICON_IDLE_Battery24:		icnBattery_dataformat(iconId,op_bmpData, op_sx, op_sy, op_format, palette);	break;
	case ICON_IDLE_Battery49:		icnBattery_dataformat(iconId,op_bmpData, op_sx, op_sy, op_format, palette);	break;
	case ICON_IDLE_Battery100:		icnBattery_dataformat(iconId,op_bmpData, op_sx, op_sy, op_format, palette);	break;
	case ICON_IDLE_forwarding:
	case ICON_IDLE_keylock:
	case ICON_IDLE_message:
	case ICON_IDLE_signalStr49: 	icnSignalStr_dataformat(iconId,op_bmpData, op_sx, op_sy, op_format, palette);	break;
	case ICON_IDLE_signalStr99: 	icnSignalStr_dataformat(iconId,op_bmpData, op_sx, op_sy, op_format, palette);	break;
	case ICON_IDLE_signalStr149: 	icnSignalStr_dataformat(iconId,op_bmpData, op_sx, op_sy, op_format, palette);	break;
	case ICON_IDLE_signalStr199:	icnSignalStr_dataformat(iconId,op_bmpData, op_sx, op_sy, op_format, palette);	break;
	case ICON_IDLE_signalStr255:	icnSignalStr_dataformat(iconId,op_bmpData, op_sx, op_sy, op_format, palette);	break;
	case ICON_IDLE_homeZone:
	case ICON_IDLE_gprs:			icnGPRS_dataformat(op_bmpData, op_sx, op_sy, op_format, palette);	break;

	case ICON_IDLE_voicemail:


//'main' icons
	case ICON_MAIN_Application:
	case ICON_MAIN_Divert:
	case ICON_MAIN_Messages:
	case ICON_MAIN_Phonebook:
	case ICON_MAIN_RecentCalls:
	case ICON_MAIN_Security:
	case ICON_MAIN_Settings:
	case ICON_MAIN_SIM_Toolkit:
	case ICON_MAIN_WAP:

//popup
	case ICON_POPUP_CLOUDS:

//network
	case ICON_Bouygues:
	case ICON_Vodafone:
	case ICON_SFR:
	case ICON_VodafoneD2:
	case ICON_Chinamobile:
	case ICON_Fareast:
	case ICON_orange:
	case ICON_sonofon:
	case ICON_Tmobile:

	case ICON_BgdSquares: 		icnBgdSquares_dataformat(op_bmpData, op_sx, op_sy, op_format, palette);	break;
	default:
		return(-1);
		break;
		
	}
#endif
	return (0);	
}
#endif

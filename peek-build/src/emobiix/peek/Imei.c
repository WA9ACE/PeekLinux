#include "stddefs.h"
#include "cl_imei.h"

#include "Debug.h"

extern BYTE cl_get_imeisv(USHORT imeiBufSize, UBYTE *imeiBufPtr, UBYTE imeiType);
extern UBYTE getCdByteFromImei(UBYTE *imei);

const char* EmoGetImei()
{
  static UBYTE rawIMEI[CL_IMEI_SIZE];
	static char imei[32] = "";

	if (!imei[0])
	{
		cl_get_imeisv(sizeof(rawIMEI), rawIMEI, CL_IMEI_GET_STORED_IMEI);

		imei[0]  = ((rawIMEI [0] >> 4) & 0x0F)+0x30;
		imei[1]  = ( rawIMEI [0] & 0x0F) +0x30;
		imei[2]  = ((rawIMEI [1] >> 4) & 0x0F)+0x30;
		imei[3]  = ( rawIMEI [1] & 0x0F)+0x30;
		imei[4]  = ((rawIMEI [2] >> 4) & 0x0F)+0x30;
		imei[5]  = ( rawIMEI [2] & 0x0F)+0x30;
		imei[6]  = ((rawIMEI [3] >> 4) & 0x0F)+0x30;
		imei[7]  = ( rawIMEI [3] & 0x0F)+0x30;
		imei[8]  = ((rawIMEI [4] >> 4) & 0x0F)+0x30;
		imei[9]  = ( rawIMEI [4] & 0x0F)+0x30;
		imei[10] = ((rawIMEI [5] >> 4) & 0x0F)+0x30;
		imei[11] = ( rawIMEI [5] & 0x0F)+0x30;
		imei[12] = ((rawIMEI [6] >> 4) & 0x0F)+0x30;
		imei[13] = ( rawIMEI [6] & 0x0F)+0x30;
		imei[14] = ((getCdByteFromImei(rawIMEI)) & 0x0F)+0x30;
		imei[15] = (( rawIMEI [7] >> 4) & 0x0F)+0x30;
		imei[16] = ( rawIMEI [7] & 0x0F)+0x30;
		imei[17] = 0;

		emo_printf("%s(): got imei=%s", __FUNCTION__, imei);
	}

	return imei;
}

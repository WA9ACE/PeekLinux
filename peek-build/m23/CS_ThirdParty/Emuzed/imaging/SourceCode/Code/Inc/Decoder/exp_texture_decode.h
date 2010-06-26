/*
******************************************************************************
                    Copyright eMuzed Inc., 2001-2004.
All rights Reserved,Licensed Software Confidential and Proprietary Information 
of eMuzed Incorporation Made available under Non-Disclosure Agreement OR 
                        License as applicable.
******************************************************************************
*/

/*
******************************************************************************
Product        : JPEG-Decoder
Module         : Texture Decoding
File           : texture_decode.h
Description	   : This file contains the texture decoding functions definitions.


Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date               Id        Author              Comment    
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
July 6, 2001				 Rajiv Kumar Bajpai	  Initial Code				
******************************************************************************
*/

#ifndef INCLUDE_EXP_TEXTURE_DECODE
#define	INCLUDE_EXP_TEXTURE_DECODE

extern const uint8 sInvZigzagScanMtx[64]; 
extern const uint16 sZigzagPreMult[64];

int32 JDEmz_gDecodeMCU_patched(tJpegDecoder_JDEmz *jpegDecoder,  tMcu_JDEmz *mcu, int32 *flag);

int32 JDEmz_gParseMCU (tJpegDecoder_JDEmz *jpegDecoder, tMcu_JDEmz *mcu, int32 *flag);

/* new inclusion */
int32 JDEmz_gRestPDecodeDcMCU (tJpegDecoder_JDEmz *jpegDecoder,
                         tMcu_JDEmz *mcu, int32 *flag);

int32 JDEmz_gPDecodeDcMCU (tJpegDecoder_JDEmz *jpegDecoder, 
                     tMcu_JDEmz *mcu, int32 *flag );
int32 JDEmz_gSPDecodeDcMCU (tJpegDecoder_JDEmz *jpegDecoder, int32 *count, uint8 *byte,
                     tMcu_JDEmz *mcu);
int32 JDEmz_gPDecodeDCUnit (tJpegDecoder_JDEmz *jpegDecoder, tComponent_JDEmz *scanComponent,
                       int32 dataUnitX, int32 dataUnitY, 
                        int32 *flag);
int32 JDEmz_gPReconstructFrame(tJpegDecoder_JDEmz *jpegDecoder);

#endif /* INCLUDE_EXP_TEXTURE_DECODE */

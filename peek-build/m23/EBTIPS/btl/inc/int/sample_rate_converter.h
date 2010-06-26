/*******************************************************************************\
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      sample_rate_converter.h
*
*   BRIEF:          This file defines the API of the Sample Rate Convereter.
*                  
*
*   DESCRIPTION:    General
*
*              		This file defines the API of the Sample Rate Convereter.
*					It coverts a given PCM block in a given sample frequency to
*					another PCM block in given sample frequency according to 
*					the conversion rules.
*
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/


#ifndef __SAMPLERATECONVERTER_H
#define __SAMPLERATECONVERTER_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "bthal_types.h"
#include "bthal_common.h"


/*-------------------------------------------------------------------------------
 * SrcSamplingFreq type
 *
 *     	Describes the sampling frequency in Hz of the PCM block.
 */
typedef BTHAL_U8 SrcSamplingFreq;

#define SRC_SAMPLING_FREQ_8000			    						(0x01)
#define SRC_SAMPLING_FREQ_11025    									(0x02)
#define SRC_SAMPLING_FREQ_12000	  									(0x03)
#define SRC_SAMPLING_FREQ_16000	  									(0x04)
#define SRC_SAMPLING_FREQ_22050	  									(0x05)
#define SRC_SAMPLING_FREQ_24000    									(0x06)
#define SRC_SAMPLING_FREQ_32000    									(0x07)
#define SRC_SAMPLING_FREQ_44100	  									(0x08)
#define SRC_SAMPLING_FREQ_48000    									(0x09)


/*-------------------------------------------------------------------------------
 * SRC_ConfigureNewSong()
 *
 * Brief:  
 *		This function configures the SRC with the input parameters.
 *
 * Description:
 *		This function configures the SRC with the input parameters: input PCM 
 *		sample frequency, input PCM number of channels (1 or 2), and max length of 
 *		the given 'outBlk' argument in SRC_Convert.
 *		It must be called at least once before calling SRC_Convert.
 *		It can be called again to reconfigure SRC for a new input parameters.
 *
 *		In addition, this function returns the output sample frequency according to 
 *		the input sample frequency according to the conversion rules.
 *
 * Parameters:
 *
 *		inSampleFreq [in] - input PCM block sample frequency.
 *
 *		inNumOfChannels [in] - input PCM block number of channels (1 or 2).
 *
 *		outBlkMaxLen [in] - max length of the given 'outBlk' argument in SRC_Convert().
 *
 *		outSampleFreq [out] - output PCM sample frequency according to conversion rules
 *								(SRC_SAMPLING_FREQ_44100 or SRC_SAMPLING_FREQ_48000).
 *		
 * Returns:
 *		BTHAL_STATUS_SUCCESS - Operation is successful.
 *
 *		BTHAL_STATUS_FAILED - The operation failed.
 */
BthalStatus SRC_ConfigureNewSong(SrcSamplingFreq inSampleFreq, BTHAL_U8 inNumOfChannels, BTHAL_U16 outBlkMaxLen, SrcSamplingFreq *outSampleFreq);


/*-------------------------------------------------------------------------------
 * SRC_ResetSongHistory()
 *
 * Brief:  
 *		This function indicate to the SRC that a new song is going to start 
 *		and the history buffer needs to be reset.
 *
 * Description:
 *		This function indicate to the SRC that a new song is going to start 
 *		and the history buffer needs to be reset.
 *
 * Parameters:
 *		void
 *		
 * Returns:
 *		BTHAL_STATUS_SUCCESS - Operation is successful.
 *
 *		BTHAL_STATUS_FAILED - The operation failed.
 */
BthalStatus SRC_ResetSongHistory(void);


/*-------------------------------------------------------------------------------
 * SRC_Convert()
 *
 * Brief:  
 *		This function coverts a given PCM block to another PCM block.
 *
 * Description:
 *		This function coverts a given PCM block in a sample frequency (given in 
 *		last call to SRC_Configure) to	another PCM block in sample frequency 
 *		according to the conversion rules.
 *
 * Parameters:
 *		inBlk [in] - input PCM block.
 *
 *		inBlkLen [in] - input PCM block length.
 *
 *		outBlk [out] - output PCM block.
 *
 *		outBlkLen [out] - length of the output PCM block.
 *		
 * Returns:
 *		BTHAL_STATUS_SUCCESS - Operation is successful.
 *
 *		BTHAL_STATUS_FAILED - The operation failed (for example, 'outBlkMaxLen' is too small).
 */
BthalStatus SRC_Convert(BTHAL_U8 *inBlk, BTHAL_U16 inBlkLen, BTHAL_U8 *outBlk, BTHAL_U16 *outBlkLen);


#endif /* __SAMPLERATECONVERTER_H */



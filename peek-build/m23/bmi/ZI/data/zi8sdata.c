/* zi8sdata.c
*/
/*****************************************************************************
* NOTICE:  ZI CORPORATION PROPRIETARY AND CONFIDENTIAL                       *
*                                                                            *
* This file is the confidential and proprietary property of Zi Corporation   *
* Canada, Inc. or one of its affiliates. It is not intended for distribution *
* to any party except in accordance with a license for use granted by Zi     *
* Corporation. No portion of this file may be copied or distributed to any   *
* party without the express written consent of an officer of Zi Corporation  *
*****************************************************************************/
#include "zi8api.h" 
#ifndef CHINESE_MMI
	extern ZI8UCHAR ZI8ROMDATA Zi8DatDE10k[];
#else
	extern ZI8UCHAR ZI8ROMDATA Zi8DatZH[];
	extern ZI8UCHAR ZI8ROMDATA Zi8DatZ1[];
#endif
//extern ZI8UCHAR ZI8ROMDATA Zi8DatES10k[];
extern ZI8UCHAR ZI8ROMDATA Zi8DatEN10k[];
//extern ZI8UCHAR ZI8ROMDATA Zi8DatEN20k[];
//extern ZI8UCHAR ZI8ROMDATA Zi8DatPT10k[];
//extern ZI8UCHAR ZI8ROMDATA Zi8DatPT20k[];


 /*SPR 1434, added conditional compilation*/
ZI8_LANGUAGE_ENTRY ZI8ROMDATA Zi8StaticLanguageTable[] =
{
    {ZI8_LANG_EN,     (void ZI8ROMPOINTER)&Zi8DatEN10k[0]},
 #ifndef CHINESE_MMI
     {ZI8_LANG_DE,     (void ZI8ROMPOINTER)&Zi8DatDE10k[0]},
 #else
    {ZI8_LANG_ZH,     (void ZI8ROMPOINTER)&Zi8DatZH[0]},
    {ZI8_AUXTABLE_ZH1,(void ZI8ROMPOINTER)&Zi8DatZ1[0]},
 #endif

    {0,0}
};

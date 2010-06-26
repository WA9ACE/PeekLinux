/************* Revision Controle System Header *************
 *                  GSM Layer 1 software
 * L1MP3_ASYNC.C
 *
 *        Filename l1mp3_func.c
 *  Copyright 2004 (C) Texas Instruments
 *
 ************* Revision Controle System Header *************/
#include "l1_confg.h"
#include "l1_types.h"
#include "l1_const.h"
#include "l1mp3_const.h"

#if (L1_MP3 == 1)

/*-------------------------------------------------------*/
/* l1mp3_get_frequencyrate()                             */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : MP3 header                               */
/*                                                       */
/* Return     : Frequency rate in Hertz                  */
/*                                                       */
/* Description : Return the frequency rate of MP3 file   */
/*                                                       */
/*-------------------------------------------------------*/
UWORD16 l1mp3_get_frequencyrate(UWORD32 header)
{
  UWORD16 version =0; //omaps00090550

  // MPEG  version
  switch(header&0x00180000)
  {
    case 0x00000000:
      version=2;    // version 2.5
    break;
    case 0x00100000:
      version=1;    // version 2
    break;
    case 0x00180000:
      version=0;    // version 1
    break;
  }

  // Frequency rate
  switch((header&0x0000000C00)>>10)
  {
    case 0:
    {
      switch(version)
      {
        case 0:
          return 44100;
//omaps00090550        break;
        case 1:
          return 22050;
//omaps00090550        break;
        case 2:
          return 11025;
//omaps00090550        break;
      }
    } 
    break;
    case 1:
    {
      switch(version)
      {
        case 0:
          return 48000;
//omaps00090550        break;
        case 1:
          return 24000;
//omaps00090550        break;
        case 2:
          return 12000;
//omaps00090550        break;
      }
    } 
    break;
    case 2:
    {
     switch(version)
     {
       case 0:
         return 32000;
//omaps00090550       break;
       case 1:
         return 16000;
//omaps00090550       break;
       case 2:
         return 8000;
//omaps00090550       break;
     }
    }
    break;
  } 

  // Part of code that should never be reached
  return 0;
}


/*-------------------------------------------------------*/
/* l1mp3_get_bitrate()                                   */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : MP3 header                               */
/*                                                       */
/* Return     : bitrate in kbits/s                       */
/*                                                       */
/* Description : Return the bitrate of MP3 frame         */
/*                                                       */
/*-------------------------------------------------------*/
UWORD16 l1mp3_get_bitrate(UWORD32 header)
{
  UWORD16 version = 0; //omaps00090550

  // MPEG  version
  switch(header&0x00180000)
  {
    case 0x00000000:
      version=2;    // version 2.5
    break;
    case 0x00100000:
      version=1;    // version 2
    break;
    case 0x00180000:
      version=0;    // version 1
    break;
  }

  // Bitrate
  switch((header&0x0000F000)>>12)
  {
    case 1:
    {
      switch(version)
      {
        case 0:
          return 32;
//omaps00090550        break;
        case 1:
        case 2:
           return 8;
//omaps00090550        break;
      }
    } 
    break;
    case 2:
    {
      switch(version)
      {
        case 0:
          return 40;
//omaps00090550        break;
        case 1:
        case 2:
          return 16;
//omaps00090550        break;
      }
    } 
    break;
    case 3:
    {
      switch(version)
      {
        case 0:
          return 48;
//omaps00090550        break;
       case 1:
       case 2:
        return 24;
//omaps00090550        break;
      }
    } 
    break;
    case 4:
    {
      switch(version)
      {
        case 0:
          return 56;
//omaps00090550        break;
        case 1:
        case 2:
          return 32;
//omaps00090550        break;
      }
    } 
    break;
    case 5:
    {
      switch(version)
      {
        case 0:
          return 64;
//omaps00090550        break;
        case 1:
        case 2:
          return 40;
//omaps00090550        break;
      }
    } 
    break;
    case 6:
    {
      switch(version)
      {
        case 0:
          return 80;
//omaps00090550        break;
        case 1:
        case 2:
          return 48;
//omaps00090550        break;
      }
    } 
    break;
    case 7:
    {
      switch(version)
      {
        case 0:
          return 96;
//omaps00090550        break;
        case 1:
        case 2:
          return 56;
//omaps00090550        break;
      }
    } 
    break;
    case 8:
    {
      switch(version)
      {
        case 0:
          return 112;
//omaps00090550        break;
        case 1:
        case 2:
          return 64;
//omaps00090550        break;
      }
    } 
    break;
    case 9:
    {
      switch(version)
      {
        case 0:
          return 128;
//omaps00090550        break;
        case 1:
        case 2:
          return 80;
//omaps00090550        break;
      }
    } 
    break;
    case 10:
    {
      switch(version)
      {
        case 0:
          return 160;
//omaps00090550        break;
        case 1:
        case 2:
          return 96;
//omaps00090550        break;
      }
    } 
    break;
    case 11:
    {
      switch(version)
      {
        case 0:
          return 192;
//omaps00090550        break;
        case 1:
        case 2:
          return 112;
//omaps00090550        break;
     }
    } 
    break;
    case 12:
    {
      switch(version)
      {
        case 0:
          return 224;
//omaps00090550        break;
        case 1:
        case 2:
          return 128;
//omaps00090550        break;
      }
    } 
    break;
    case 13:
    {
      switch(version)
      {
        case 0:
          return 256;
//omaps00090550        break;
        case 1:
        case 2:
          return 144;
//omaps00090550        break;
      }
    } 
    break;
    case 14:
    {
      switch(version)
      {
        case 0:
          return 320;
//omaps00090550        break;
        case 1:
        case 2:
          return 160;
//omaps00090550        break;
      }
    } 
    break;
  }

  // Part of code that should never be reached
  return 0;
}

/*-------------------------------------------------------*/
/* l1mp3_get_mpeg_id()                                   */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : MP3 header                               */
/*                                                       */
/* Return     : MPEG version of the MP3 frame            */
/*                                                       */
/* Description : Return the MPEG version of the MP3      */
/*                                                       */
/*-------------------------------------------------------*/
UWORD8 l1mp3_get_mpeg_id(UWORD32 header)
{
  switch((header>>19) & 3)
  {
    case 0:
      return C_MP3_HEADER_MPEGID_2_5;
//omaps00090550    break;
    case 2:
      return C_MP3_HEADER_MPEGID_2;
//omaps00090550    break;
    case 3:
      return C_MP3_HEADER_MPEGID_1;
//omaps00090550    break;
    default:
      return C_MP3_HEADER_ERROR;
//omaps00090550    break;
  }
}

/*-------------------------------------------------------*/
/* l1mp3_get_layer()                                     */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : MP3 header                               */
/*                                                       */
/* Return     : Layer of MP3 file (I, II or III)         */
/*                                                       */
/* Description : Return the layer of MP3 file            */
/*                                                       */
/*-------------------------------------------------------*/
UWORD8 l1mp3_get_layer(UWORD32 header)
{
  switch((header>>17) & 3)
  {
    case 1:
      return C_MP3_HEADER_LAYER_III;
//omaps00090550    break;
    case 2:
      return C_MP3_HEADER_LAYER_II;
//omaps00090550    break;
    case 3:
      return C_MP3_HEADER_LAYER_I;
//omaps00090550    break;
    default:
      return C_MP3_HEADER_ERROR;
//omaps00090550    break;
  }
}

/*-------------------------------------------------------*/
/* l1mp3_get_padding()                                   */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : MP3 header                               */
/*                                                       */
/* Return     : TRUE if padding is set, false otherwise  */
/*                                                       */
/* Description : Return padding used in MP3 frame        */
/*                                                       */
/*-------------------------------------------------------*/
BOOL l1mp3_get_padding(UWORD32 header)
{
  if(((header>>9) & 1)==1)
    return TRUE;
  else
    return FALSE;
}

/*-------------------------------------------------------*/
/* l1mp3_get_private()                                   */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : MP3 header                               */
/*                                                       */
/* Return     : Private bit                              */
/*                                                       */
/* Description : Return private bit of MP3 frame         */
/*                                                       */
/*-------------------------------------------------------*/
UWORD8 l1mp3_get_private(UWORD32 header)
{
  return (UWORD8)((header>>8) & 1);
}

/*-------------------------------------------------------*/
/* l1mp3_get_channel_mode()                              */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : MP3 header                               */
/*                                                       */
/* Return     : Channel mode (mono, stereo, ...)         */
/*                                                       */
/* Description : Return channel mode of MP3 frame        */
/*                                                       */
/*-------------------------------------------------------*/
UWORD8 l1mp3_get_channel_mode(UWORD32 header)
{
  switch((header>>6) & 3)
  {
    case 0:
      return C_MP3_HEADER_STEREO;
//omaps00090550    break;
    case 1:
      return C_MP3_HEADER_JSTEREO;
//omaps00090550    break;
    case 2:
      return C_MP3_HEADER_DUAL_MONO;
//omaps00090550    break;
    case 3:
      return C_MP3_HEADER_MONO;
//omaps00090550    break;
    default:
      return C_MP3_HEADER_ERROR;
//omaps00090550    break;
  }
}

/*-------------------------------------------------------*/
/* l1mp3_get_copyright()                                 */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : MP3 header                               */
/*                                                       */
/* Return     : TRUE if MP3 is copyrighted               */
/*                                                       */
/* Description : Return copyright bit of MP3 frame       */
/*                                                       */
/*-------------------------------------------------------*/
BOOL l1mp3_get_copyright(UWORD32 header)
{
  if(((header>>3) & 1)==1)
    return TRUE;
  else
    return FALSE;
}

/*-------------------------------------------------------*/
/* l1mp3_get_original()                                  */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : MP3 header                               */
/*                                                       */
/* Return     : TRUE if MP3 is original                  */
/*                                                       */
/* Description : Return original bit of MP3 frame        */
/*                                                       */
/*-------------------------------------------------------*/
BOOL l1mp3_get_original(UWORD32 header)
{
  if(((header>>2) & 1)==1)
    return TRUE;
  else
    return FALSE;
}

/*-------------------------------------------------------*/
/* l1mp3_get_emphasis()                                  */
/*-------------------------------------------------------*/
/*                                                       */
/* Parameters : MP3 header                               */
/*                                                       */
/* Return     : Emphasis used in MP3 frame               */
/*                                                       */
/* Description : Return emphasis of MP3 frame            */
/*                                                       */
/*-------------------------------------------------------*/
UWORD8 l1mp3_get_emphasis(UWORD32 header)
{
  switch(header & 3)
  {
    case 0:
      return C_MP3_HEADER_EMPHASIS_NONE;
//omaps00090550    break;
    case 1:
      return C_MP3_HEADER_EMPHASIS_50_15;
//omaps00090550    break;
    case 3:
      return C_MP3_HEADER_EMPHASIS_CCIT_J17;
//omaps00090550    break;
    default:
      return C_MP3_HEADER_ERROR;
//omaps00090550    break;
  }
}
#endif    // L1_MP3

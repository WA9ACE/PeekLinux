/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_error_hdlr.c                                          */
/*                                                                          */
/*  Purpose:  This file contains routines used to report unrecoverable      */
/*                 memory errors that might occur.                          */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------------------------------------------*/
/*  14 May 2001  Create                                                      */
/*                                                                          */
/*  Author      Francois Mazard - Stephanie Gerthoux                        */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"
#ifdef RVM_AUDIO_MAIN_SWE
  #ifndef _WINDOWS
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
  #endif

  #include "l1sw.cfg"
  #include "l1_confg.h"
  #include "rv/rv_general.h"
  #include "rvf/rvf_api.h"
  #include "audio/audio_features_i.h"
  #include "audio/audio_api.h"
  #include "audio/audio_error_hdlr_i.h"
  #include "audio/audio_macro_i.h"

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_error_trace                                        */
  /*                                                                              */
  /*    Purpose:         This function is used to report error occured during the */
  /*                     audio entity execution                                   */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        status       - Contains the error code to be reported.                */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Global Parameters:                                                        */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*       14 May 01           Create                                             */
  /*                                                                              */
  /********************************************************************************/
  void audio_error_trace(UINT8 error_id)
  {
    switch(error_id)
    {
      case AUDIO_ENTITY_NOT_START:
        {
          AUDIO_SEND_TRACE("AUDIO entity not started.",RV_TRACE_LEVEL_ERROR);
          break;
        }

      case AUDIO_ENTITY_NO_MEMORY:
        {
          AUDIO_SEND_TRACE("AUDIO entity has not enough memory",RV_TRACE_LEVEL_ERROR);
          break;
        }

      case AUDIO_ENTITY_BAD_PARAMETER:
        {
          AUDIO_SEND_TRACE("AUDIO entity has bad parameters",RV_TRACE_LEVEL_ERROR);
          break;
        }

      case AUDIO_ERROR_STOP_EVENT:
        {
          AUDIO_SEND_TRACE("AUDIO entity has received a stop error event",RV_TRACE_LEVEL_ERROR);
          break;
        }

      case AUDIO_ERROR_START_EVENT:
        {
          AUDIO_SEND_TRACE("AUDIO entity has received a start error event",RV_TRACE_LEVEL_ERROR);
          break;
        }

      case AUDIO_ENTITY_BAD_MESSAGE:
        {
          AUDIO_SEND_TRACE("AUDIO entity has received a bad message",RV_TRACE_LEVEL_ERROR);
          break;
        }
    }
  }
  /*********************** End of audio_error_trace function **********************/

  #if (KEYBEEP)
    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_keybeep_error_trace                                */
    /*                                                                              */
    /*    Purpose:         This function is used to report error occured during the */
    /*                     audio entity execution                                   */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        status       - Contains the error code to be reported.                */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Global Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Revision History:                                                         */
    /*       14 May 01           Create                                             */
    /*                                                                              */
    /********************************************************************************/
    void audio_keybeep_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NOT_START:
          {
            AUDIO_SEND_TRACE("AUDIO Keybeep entity not started.",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_NO_MEMORY:
          {
            AUDIO_SEND_TRACE("AUDIO Keybeep entity has not enough memory",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_BAD_PARAMETER:
          {
            AUDIO_SEND_TRACE("AUDIO Keybeep entity has bad parameters",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_STOP_EVENT:
          {
            AUDIO_SEND_TRACE("AUDIO Keybeep entity has received a stop error event",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_START_EVENT:
          {
            AUDIO_SEND_TRACE("AUDIO Keybeep entity has received a start error event",RV_TRACE_LEVEL_ERROR);
            break;
          }
      }
    }
  #endif /* #if (KEYBEEP) */
  /*********************** End of audio_keybeep_error_trace function **********************/

  #if (TONE)
    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_tones_error_trace                                  */
    /*                                                                              */
    /*    Purpose:         This function is used to report error occured during the */
    /*                     audio entity execution                                   */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        status       - Contains the error code to be reported.                */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Global Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Revision History:                                                         */
    /*       14 May 01           Create                                             */
    /*                                                                              */
    /********************************************************************************/
    void audio_tones_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NOT_START:
          {
            AUDIO_SEND_TRACE("AUDIO Tones entity not started.",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_NO_MEMORY:
          {
            AUDIO_SEND_TRACE("AUDIO Tones entity has not enough memory",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_BAD_PARAMETER:
          {
            AUDIO_SEND_TRACE("AUDIO Tones entity has bad parameters",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_STOP_EVENT:
          {
            AUDIO_SEND_TRACE("AUDIO Tones entity has received a stop error event",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_START_EVENT:
          {
            AUDIO_SEND_TRACE("AUDIO Tones entity has received a start error event",RV_TRACE_LEVEL_ERROR);
            break;
          }
      }
    }
  #endif /* tone */
  /*********************** End of audio_tones_error_trace function **********************/

  #if (MELODY_E1)
    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_melody_E1_error_trace                              */
    /*                                                                              */
    /*    Purpose:         This function is used to report error occured during the */
    /*                     audio entity execution                                   */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        status       - Contains the error code to be reported.                */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Global Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Revision History:                                                         */
    /*       14 May 01           Create                                             */
    /*                                                                              */
    /********************************************************************************/
    void audio_melody_E1_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NOT_START:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E1 entity not started.",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_NO_MEMORY:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E1 entity has not enough memory",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_BAD_PARAMETER:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E1 entity has bad parameters",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_STOP_EVENT:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E1 entity has received a stop error event",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_START_EVENT:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E1 entity has received a start error event",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_MODE:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E1 entity has received a start with a wrong mode",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_FILE_ERROR:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E1 entity has received wrong file name",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_FILE_NO_CLOSE:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E1 entity can't close a FFS file ",RV_TRACE_LEVEL_ERROR);
            break;
          }
      }
    }
  #endif /* #if (Melody E1) */

  #if (MELODY_E2)
    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_melody_E2_error_trace                              */
    /*                                                                              */
    /*    Purpose:         This function is used to report error occured during the */
    /*                     audio entity execution                                   */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        status       - Contains the error code to be reported.                */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Global Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Revision History:                                                         */
    /*       7 December 01           Create                                         */
    /*                                                                              */
    /********************************************************************************/
    void audio_melody_E2_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NOT_START:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E2 entity not started.",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_NO_MEMORY:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E2 entity has not enough memory",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_BAD_PARAMETER:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E2 entity has bad parameters",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_STOP_EVENT:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E2 entity has received a stop error event",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_START_EVENT:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E2 entity has received a start error event",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_MODE:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E2 entity has received a start with a wrong mode",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_DOWNLOAD:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E2 entity has received a download error event",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_NOT_DOWNLOAD:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E2 entity has not download the instrument file",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_FILE_ERROR:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E2 entity has received wrong file name",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_FILE_NO_CLOSE:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E2 entity can't close a FFS file ",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_LOAD_FILE_INSTR_ERROR:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E2 entity has received wrong file name to load instruments",RV_TRACE_LEVEL_ERROR);
            break;
          }
        case AUDIO_ERROR_INSTRUMENT_SIZE:
          {
            AUDIO_SEND_TRACE("AUDIO Melody E2 not enough memory for the instrument of this melody",RV_TRACE_LEVEL_ERROR);
            break;
          }
      }
    }
  #endif /* #if (Melody E2) */

 #if (VOICE_MEMO)
    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_voice_memo_error_trace                              */
    /*                                                                              */
    /*    Purpose:         This function is used to report error occured during the */
    /*                     audio entity execution                                   */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        status       - Contains the error code to be reported.                */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Global Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Revision History:                                                         */
    /*       14 May 01           Create                                             */
    /*                                                                              */
    /********************************************************************************/
    void audio_voice_memo_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NOT_START:
          {
            AUDIO_SEND_TRACE("AUDIO Voice Memo entity not started.",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_NO_MEMORY:
          {
            AUDIO_SEND_TRACE("AUDIO Voice Memo entity has not enough memory",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_BAD_PARAMETER:
          {
            AUDIO_SEND_TRACE("AUDIO Voice Memo entity has bad parameters",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_STOP_EVENT:
          {
            AUDIO_SEND_TRACE("AUDIO Voice Memo entity has received a stop error event",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_START_EVENT:
          {
            AUDIO_SEND_TRACE("AUDIO Voice Memo entity has received a start error event",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_MODE:
          {
            AUDIO_SEND_TRACE("AUDIO Voice Memo entity has received a start with a wrong mode",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_FILE_ERROR:
          {
            AUDIO_SEND_TRACE("AUDIO Voice Memo entity has received wrong file name",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_FILE_NO_CLOSE:
          {
            AUDIO_SEND_TRACE("AUDIO Voice Memo entity can't close a FFS file",RV_TRACE_LEVEL_ERROR);
            break;
          }
      }
    }
  #endif /* #if (Voice Memo) */

  #if (L1_VOICE_MEMO_AMR)
    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_voice_memo_amr_error_trace                         */
    /*                                                                              */
    /*    Purpose:         This function is used to report error occured during the */
    /*                     audio entity execution                                   */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        status       - Contains the error code to be reported.                */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Global Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Revision History:                                                         */
    /*       14 May 01           Create                                             */
    /*                                                                              */
    /********************************************************************************/
    void audio_voice_memo_amr_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NOT_START:
          AUDIO_SEND_TRACE("AUDIO Voice Memo AMR not started.",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_NO_MEMORY:
          AUDIO_SEND_TRACE("AUDIO Voice Memo AMR entity has not enough memory",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_BAD_PARAMETER:
          AUDIO_SEND_TRACE("AUDIO Voice Memo AMR entity has bad parameters",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_STOP_EVENT:
          AUDIO_SEND_TRACE("AUDIO Voice Memo AMR entity has received a stop error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_START_EVENT:
          AUDIO_SEND_TRACE("AUDIO Voice Memo AMR entity has received a start error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_MODE:
          AUDIO_SEND_TRACE("AUDIO Voice Memo AMR entity has received a start with a wrong mode",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_ERROR:
          AUDIO_SEND_TRACE("AUDIO Voice Memo AMR entity has received wrong file name",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_NO_CLOSE:
          AUDIO_SEND_TRACE("AUDIO Voice Memo AMR entity can't close a FFS file",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_FEATURE_NOT_COMPILED:
          AUDIO_SEND_TRACE("AUDIO Voice Memo AMR entity: feature not compiled",RV_TRACE_LEVEL_ERROR);
        break;
        default:
          AUDIO_SEND_TRACE_PARAM("AUDIO Voice Memo AMR entity: unknown error", error_id, RV_TRACE_LEVEL_ERROR);
        break;
      }
    }
  #endif /* #if (Voice Memo AMR) */

  #if (MELODY_E1) || (VOICE_MEMO) || (SPEECH_RECO) || (L1_MP3) || (L1_AAC)
    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_ffs_error_trace                                    */
    /*                                                                              */
    /*    Purpose:         This function is used to report error occured during the */
    /*                     audio entity execution                                   */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        status       - Contains the error code to be reported.                */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Global Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Revision History:                                                         */
    /*       25 May 01           Create                                             */
    /*                                                                              */
    /********************************************************************************/
    void audio_ffs_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NO_MEMORY:
          AUDIO_SEND_TRACE("AUDIO FFS entity has not enough memory",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_NO_SAVED:
          AUDIO_SEND_TRACE("AUDIO FFS entity can't saved the file in flash",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_NO_CLOSE:
          AUDIO_SEND_TRACE("AUDIO FFS entity can't close the file",RV_TRACE_LEVEL_ERROR);
        break;
      }
    }
  #endif /* #if (MELODY_E1) || (VOICE_MEMO) */

  #if (AUDIO_NEW_FFS_MANAGER)
    void audio_new_ffs_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NO_MEMORY:
          AUDIO_SEND_TRACE("AUDIO FFS entity has not enough memory",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_NO_SAVED:
     AUDIO_SEND_TRACE("AUDIO FFS entity can't save the file in flash",RV_TRACE_LEVEL_ERROR);
            break;
        case AUDIO_ENTITY_FILE_NO_CLOSE:
          AUDIO_SEND_TRACE("AUDIO FFS entity can't close the file",RV_TRACE_LEVEL_ERROR);
        break;
      }
    }
  #endif

  #if (AUDIO_RAM_MANAGER)
    void audio_ram_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NO_MEMORY:
          AUDIO_SEND_TRACE("AUDIO RAM entity has not enough memory",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_NO_SAVED:
          AUDIO_SEND_TRACE("AUDIO RAM entity can't save the file in flash",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_NO_CLOSE:
          AUDIO_SEND_TRACE("AUDIO RAM entity can't close the file",RV_TRACE_LEVEL_ERROR);
        break;
      }
    }
  #endif

  #if (SPEECH_RECO)
    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_sr_error_trace                                     */
    /*                                                                              */
    /*    Purpose:         This function is used to report error occured during the */
    /*                     audio entity execution                                   */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        status       - Contains the error code to be reported.                */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Global Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Revision History:                                                         */
    /*       25 May 01           Create                                             */
    /*                                                                              */
    /********************************************************************************/
    void audio_sr_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NOT_START:
          {
            AUDIO_SEND_TRACE("AUDIO Speech reco entity not started.",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_FILE_ALREADY_EXIST:
          {
            AUDIO_SEND_TRACE("AUDIO Speech reco file already exists.",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_NO_MEMORY:
          {
            AUDIO_SEND_TRACE("AUDIO Speech reco entity has not enough memory",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_STOP_EVENT:
          {
            AUDIO_SEND_TRACE("AUDIO Speech reco entity has received a stop error event",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ERROR_START_EVENT:
          {
            AUDIO_SEND_TRACE("AUDIO Speech reco entity has received a start error event",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_SPEECH_NO_SAVED:
          {
            AUDIO_SEND_TRACE("AUDIO Speech reco entity can't save the speech sample",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_MODEL_NO_SAVED:
          {
            AUDIO_SEND_TRACE("AUDIO Speech reco entity can't save the model sample",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_BAD_PARAMETER:
          {
            AUDIO_SEND_TRACE("AUDIO Speech reco entity has bad parameters",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_BAD_DATABASE:
          {
            AUDIO_SEND_TRACE("AUDIO Speech reco entity has bad database",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_MODEL_NO_LOAD:
          {
            AUDIO_SEND_TRACE("AUDIO Speech reco entity model isn't loaded",RV_TRACE_LEVEL_ERROR);
            break;
          }

      }
    }
  #endif /* #if (SPEECH_RECO) */

    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_mode_error_trace                                   */
    /*                                                                              */
    /*    Purpose:         This function is used to report error occured during the */
    /*                     audio entity execution                                   */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        status       - Contains the error code to be reported.                */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Global Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Revision History:                                                         */
    /*       25 May 01           Create                                             */
    /*                                                                              */
    /********************************************************************************/
    void audio_mode_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NOT_START:
          {
          AUDIO_SEND_TRACE("AUDIO Mode entity not started.",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_NO_MEMORY:
          {
          AUDIO_SEND_TRACE("AUDIO Mode entity has not enough memory",RV_TRACE_LEVEL_ERROR);
            break;
          }

        case AUDIO_ENTITY_BAD_PARAMETER:
          {
          AUDIO_SEND_TRACE("AUDIO Mode entity has bad parameters",RV_TRACE_LEVEL_ERROR);
            break;
          }

      case AUDIO_ENTITY_AUDIO_MODE_NO_LOADED:
        {
          AUDIO_SEND_TRACE("AUDIO Mode entity: no audio mode was loaded",RV_TRACE_LEVEL_ERROR);
          break;
        }

      }
    }

  #if (L1_AUDIO_DRIVER)
    void audio_driver_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NOT_START:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity not started.",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_NO_MEMORY:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: not enough memory",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_BAD_PARAMETER:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: bad parameters",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_STOP_EVENT:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: stop error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_L1_STOP_EVENT:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: stop error event in Layer1",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_START_EVENT:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: start error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_L1_START_EVENT:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: start error event in Layer1",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_MODE:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: start with a wrong mode",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_FEATURE_NOT_COMPILED:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity has not been compiled",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_L1_PAUSE_EVENT:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: pause error event in Layer1",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_L1_RESUME_EVENT:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: resume error event in Layer1",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_L1_RESTART_EVENT:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: restart error event in Layer1",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_PAUSE_EVENT:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: pause error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_RESUME_EVENT:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: resume error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_RESTART_EVENT:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: restart error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_L1_INFO_EVENT:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: info error event in Layer1",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_INFO_EVENT:
          AUDIO_SEND_TRACE("AUDIO DRIVER entity: info error event",RV_TRACE_LEVEL_ERROR);
        break;


        default:
          AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER entity: unknow error", error_id, RV_TRACE_LEVEL_ERROR);
        break;
      }
    }
  #endif

  #if (L1_EXT_AUDIO_MGT==1)
    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_midi_error_trace                                   */
    /*                                                                              */
    /*    Purpose:         This function is used to report error occured during the */
    /*                     audio entity execution                                   */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        status       - Contains the error code to be reported.                */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Global Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Revision History:                                                         */
    /*       14 May 01           Create                                             */
    /*                                                                              */
    /********************************************************************************/
    void audio_midi_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NOT_START:
          AUDIO_SEND_TRACE("AUDIO MIDI not started.",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_NO_MEMORY:
          AUDIO_SEND_TRACE("AUDIO MIDI entity has not enough memory",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_BAD_PARAMETER:
          AUDIO_SEND_TRACE("AUDIO MIDI entity has bad parameters",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_STOP_EVENT:
          AUDIO_SEND_TRACE("AUDIO MIDI entity has received a stop error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_START_EVENT:
          AUDIO_SEND_TRACE("AUDIO MIDI entity has received a start error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_MODE:
          AUDIO_SEND_TRACE("AUDIO MIDI entity has received a start with a wrong mode",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_ERROR:
          AUDIO_SEND_TRACE("AUDIO MIDI entity has received wrong file name",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_NO_CLOSE:
          AUDIO_SEND_TRACE("AUDIO MIDI entity can't close a FFS file",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_FEATURE_NOT_COMPILED:
          AUDIO_SEND_TRACE("AUDIO MIDI entity: feature not compiled",RV_TRACE_LEVEL_ERROR);
        break;
        default:
          AUDIO_SEND_TRACE_PARAM("AUDIO MIDI entity: unknown error", error_id, RV_TRACE_LEVEL_ERROR);
        break;
      }
    }
   #endif // #if (L1_EXT_AUDIO_MGT==1)

   #if (L1_MP3)
    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_mp3_error_trace                                    */
    /*                                                                              */
    /*    Purpose:         This function is used to report error occured during the */
    /*                     audio entity execution                                   */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        status       - Contains the error code to be reported.                */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Global Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*                                                                              */
    /********************************************************************************/
    void audio_mp3_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NOT_START:
          AUDIO_SEND_TRACE("AUDIO MP3 not started.",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_NO_MEMORY:
          AUDIO_SEND_TRACE("AUDIO MP3 entity has not enough memory",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_BAD_PARAMETER:
          AUDIO_SEND_TRACE("AUDIO MP3 entity has bad parameters",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_STOP_EVENT:
          AUDIO_SEND_TRACE("AUDIO MP3 entity has received a stop error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_START_EVENT:
          AUDIO_SEND_TRACE("AUDIO MP3 entity has received a start error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_PAUSE_EVENT:
          AUDIO_SEND_TRACE("AUDIO MP3 entity has received a pause error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_RESUME_EVENT:
          AUDIO_SEND_TRACE("AUDIO MP3 entity has received a resume error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_INFO_EVENT:
          AUDIO_SEND_TRACE("AUDIO MP3 entity has received a info error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_MODE:
          AUDIO_SEND_TRACE("AUDIO MP3 entity has received a start with a wrong mode",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_ERROR:
          AUDIO_SEND_TRACE("AUDIO MP3 entity has received wrong file name",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_NO_CLOSE:
          AUDIO_SEND_TRACE("AUDIO MP3 entity can't close a FFS file",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_FEATURE_NOT_COMPILED:
          AUDIO_SEND_TRACE("AUDIO MP3 entity: feature not compiled",RV_TRACE_LEVEL_ERROR);
        break;
        default:
          AUDIO_SEND_TRACE_PARAM("AUDIO MP3 entity: unknown error", error_id, RV_TRACE_LEVEL_ERROR);
        break;
      }
    }
  #endif /* #if (MP3) */

   #if (L1_AAC)
    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_aac_error_trace                                    */
    /*                                                                              */
    /*    Purpose:         This function is used to report error occured during the */
    /*                     audio entity execution                                   */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        status       - Contains the error code to be reported.                */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Global Parameters:                                                        */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*                                                                              */
    /********************************************************************************/
    void audio_aac_error_trace(UINT8 error_id)
    {
      switch(error_id)
      {
        case AUDIO_ENTITY_NOT_START:
          AUDIO_SEND_TRACE("AUDIO AAC not started.",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_NO_MEMORY:
          AUDIO_SEND_TRACE("AUDIO AAC entity has not enough memory",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_BAD_PARAMETER:
          AUDIO_SEND_TRACE("AUDIO AAC entity has bad parameters",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_STOP_EVENT:
          AUDIO_SEND_TRACE("AUDIO AAC entity has received a stop error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_START_EVENT:
          AUDIO_SEND_TRACE("AUDIO AAC entity has received a start error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_PAUSE_EVENT:
          AUDIO_SEND_TRACE("AUDIO AAC entity has received a pause error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_RESUME_EVENT:
          AUDIO_SEND_TRACE("AUDIO AAC entity has received a resume error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_RESTART_EVENT:
          AUDIO_SEND_TRACE("AUDIO AAC entity has received a restart error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_INFO_EVENT:
          AUDIO_SEND_TRACE("AUDIO AAC entity has received a info error event",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ERROR_MODE:
          AUDIO_SEND_TRACE("AUDIO AAC entity has received a start with a wrong mode",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_ERROR:
          AUDIO_SEND_TRACE("AUDIO AAC entity has received wrong file name",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_ENTITY_FILE_NO_CLOSE:
          AUDIO_SEND_TRACE("AUDIO AAC entity can't close a FFS file",RV_TRACE_LEVEL_ERROR);
        break;
        case AUDIO_FEATURE_NOT_COMPILED:
          AUDIO_SEND_TRACE("AUDIO AAC entity: feature not compiled",RV_TRACE_LEVEL_ERROR);
        break;
        default:
          AUDIO_SEND_TRACE_PARAM("AUDIO AAC entity: unknown error", error_id, RV_TRACE_LEVEL_ERROR);
        break;
      }
    }
  #endif /* #if (L1_AAC) */

#endif /* #ifdef RVM_AUDIO_SWE */

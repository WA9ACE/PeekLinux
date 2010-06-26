/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_sr.c                                                  */
/*                                                                          */
/*  Purpose:  This file contains all the functions used in the different    */
/*            speech reco task                                              */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  21 Nov. 2001 Create                                                     */
/*                                                                          */
/*  Author                                                                  */
/*     Francois Mazard - Stephanie Gerthoux                                 */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/



#ifdef RVM_AUDIO_MAIN_SWE
#include "rv/rv_defined_swe.h"
  #ifndef _WINDOWS
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
  #endif

  #include "l1_confg.h"
  #if (SPEECH_RECO)
    #include "rv/rv_general.h"
    #include "rvm/rvm_gen.h"
    #include "audio/audio_features_i.h"
    #include "audio/audio_ffs_i.h"
    #include "audio/audio_api.h"
    #include "audio/audio_structs_i.h"
    #include "audio/audio_error_hdlr_i.h"
    #include "audio/audio_var_i.h"
    #include "audio/audio_messages_i.h"
    #include "rvf/rvf_target.h"
    #include "audio/audio_const_i.h"

    #include "ffs/ffs_api.h"

    #ifndef _WINDOWS
      #include "l1_types.h"
      #include "l1audio_cust.h"
      #include "l1audio_msgty.h"
      #include "l1audio_signa.h"
      #include "audio/audio_macro_i.h"
    #else
      #include "tests/rv/rv_test_filter.h"
      #if ((AUDIO_REGR == SW_COMPILED) || (AUDIO_MISC == SW_COMPILED))
        /* include the usefull L1 header */
        #define BOOL_FLAG
        #define CHAR_FLAG
        #include "l1_types.h"
        #include "l1audio_const.h"
        #include "l1audio_cust.h"
        #include "l1audio_defty.h"
        #include "l1audio_msgty.h"
        #include "l1audio_signa.h"
        #include "l1_const.h"
        #if (L1_MP3 == 1)
          #include "l1mp3_defty.h"
        #endif
        #if (L1_AAC == 1)
          #include "l1aac_defty.h"
        #endif
        #include "l1_defty.h"
        #include "l1_msgty.h"
        #include "l1_signa.h"
        #include "l1_varex.h"
        #include "audio/tests/audio_test.h"
      #endif
    #endif

#include <string.h>

  #ifndef _WINDOWS
    /**************************************/
    /* External prototypes                */
    /**************************************/
    extern void srback_CTO_algorithm       (volatile UWORD16 *RAM_address);
    extern void srback_save_model_temp     (volatile UWORD16 *RAM_address_input, UWORD16 *RAM_address_output);
    extern void Cust_srback_save_model     (UWORD8 database, UWORD8 index, volatile UWORD16 *RAM_address);
    extern void Cust_srback_save_speech    (UWORD8 database, UWORD8 index, UWORD16 *start_buffer, UWORD16 *stop_buffer, UWORD16 *start_speech, UWORD16 *stop_speech);
    extern void Cust_srback_load_model     (UWORD8 database, UWORD8 index, volatile UWORD16 *RAM_address);

  #endif

    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_sr_create_vocabulary_database                      */
    /*                                                                              */
    /*    Purpose:  Create the l1st of the model included in the specified database */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        directory of the vocabulary database                                  */
    /*                                                                              */
    /*    Output Parameters:                                                        */
    /*        pointer to the list of the model name                                 */
    /*        number of the model in the database                                   */
    /*                                                                              */
    /*    Note:                                                                     */
    /*        None.                                                                 */
    /*                                                                              */
    /*    Revision History:                                                         */
    /*        None.                                                                 */
    /*                                                                              */
    /********************************************************************************/
    INT8 audio_sr_create_vocabulary_database(char* directory, void** pp_database)
    {
      #ifndef _WINDOWS
        T_FFS_DIR       ffs_dir;
        T_FFS_SIZE      size, number_of_object;
        T_FFS_STAT      stat;
        INT8            number_of_model=0, i, j, name_size;
        char            name[AUDIO_PATH_NAME_MAX_SIZE], full_name[AUDIO_PATH_NAME_MAX_SIZE],*p_model_name, *ptr;
        T_RVF_MB_STATUS mb_status;

        /* open the path */
        number_of_object = ffs_opendir(directory, &ffs_dir);
        if (number_of_object <= 0)
        {
          return(AUDIO_ERROR);
        }

        /* allocate the buffer for the vocabulary database */
        mb_status = rvf_get_buf (p_audio_gbl_var->mb_audio_ffs,
                                 (AUDIO_SR_MAX_VOCABULARY_NAME * AUDIO_PATH_NAME_MAX_SIZE),
                                 (T_RVF_BUFFER **) (pp_database));

        /* If insufficient resources, then report a memory error and abort.               */
        if (mb_status == RVF_RED)
        {
          audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
          return (AUDIO_ERROR);
        }
        /* set the pointer used to write the table of model name */
        p_model_name = *pp_database;

        /* scan the directory and save the name of the model if it exists */
        size = 1;
        while (size > 0)
        {
          size = ffs_readdir(&ffs_dir, name,
            AUDIO_PATH_NAME_MAX_SIZE);
          if (size > 0)
          {
            /* look for the type of the object */
            /* Note the ffs_stat function needs the full path naem */
            /* therefor the directory needs to be added to the name from the ffs_readdir function */
            strcpy(full_name, directory);
            strcat(full_name,"/");
            strcat(full_name, name);
            if (ffs_stat(full_name, &stat) != EFFS_OK)
            {
              /* deallocate the memory */
              rvf_free_buf((T_RVF_BUFFER *)(*pp_database));

              return(AUDIO_ERROR);
            }
            else
            {
              if ((stat.type == OT_FILE) &&
                  (stat.size != 0) )
              /* the object is a file with data */
              {
                /* check if the name contains _sr at the end */
                name_size = strlen(name);
                ptr = name;
                ptr += (name_size - 3);

                if ( strcmp(ptr, "_sr") == 0 )
                {
                  /* increase the number of model */
                  number_of_model++;
                  /* Check if the number of vocabulary isn't too high */
                  if (number_of_model > AUDIO_SR_MAX_VOCABULARY_NAME)
                  {
                    /* deallocate the memory */
                    rvf_free_buf((T_RVF_BUFFER *)(*pp_database));
                    return(AUDIO_ERROR);
                  }
                  /* save the model name */
                  strcpy(p_model_name, full_name);
                  /* Increase the model name pointer */
                  p_model_name += AUDIO_PATH_NAME_MAX_SIZE;
                } /* strcmp(ptr, "_sr") == 0 */
              } /* stat.type == OT_FILE */
            } /* fs_stat(full_name, &stat) == EFFS_OK */
          } /* if (size>0) */
        } /* while (size>0) */

        return (number_of_model);
      #else
        #if ((AUDIO_REGR == SW_COMPILED) || (AUDIO_MISC == SW_COMPILED))
          T_RVF_MB_STATUS mb_status;

          /* allocate the buffer for the vocabulary database */
          mb_status = rvf_get_buf (p_audio_gbl_var->mb_audio_ffs,
                                  (AUDIO_SR_MAX_VOCABULARY_NAME * AUDIO_PATH_NAME_MAX_SIZE),
                                  (T_RVF_BUFFER **) (pp_database));
          /* If insufficient resources, then report a memory error and abort.               */
          if (mb_status == RVF_RED)
          {
            audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
            return (AUDIO_ERROR);
          }

          if (p_audio_test->vocabuary_size <= 0)
          {
           /* deallocate the memory */
           rvf_free_buf((T_RVF_BUFFER *)(*pp_database));
           return (AUDIO_ERROR);
          }
          else
          {
          return (p_audio_test->vocabuary_size);
          }
        #endif
      #endif
    }

    #ifndef _WINDOWS
      /********************************************************************************/
      /*                                                                              */
      /*    Function Name:   audio_sr_background_manager                              */
      /*                                                                              */
      /*    Purpose:  Execute the background task requested by the L1                 */
      /*                                                                              */
      /*    Input Parameters:                                                         */
      /*        message fomr the L1                                                   */
      /*                                                                              */
      /*    Output Parameters:                                                        */
      /*                                                                              */
      /*    Note:                                                                     */
      /*        None.                                                                 */
      /*                                                                              */
      /*    Revision History:                                                         */
      /*        None.                                                                 */
      /*                                                                              */
      /********************************************************************************/
      void audio_sr_background_manager(T_RV_HDR *p_message)
      {
        UINT16  confirm_message_id;
        DummyStruct *p_confirm_message;
        UINT8   index;

        switch (p_message->msg_id)
        {
          case L1_SRBACK_SAVE_DATA_REQ:
          {
            AUDIO_SEND_TRACE("AUDIO SR BACKGROUND: Save model", RV_TRACE_LEVEL_DEBUG_LOW);

            // Call the customer function to save the model
            Cust_srback_save_model( ((T_L1_SRBACK_SAVE_DATA_REQ *)(p_message))->database_id,
                                    ((T_L1_SRBACK_SAVE_DATA_REQ *)(p_message))->model_index,
                                    ((T_L1_SRBACK_SAVE_DATA_REQ *)(p_message))->model_RAM_address );

            if ( ((T_L1_SRBACK_SAVE_DATA_REQ *)(p_message))->speech)
            {
              AUDIO_SEND_TRACE("AUDIO SR BACKGROUND: Save speech", RV_TRACE_LEVEL_DEBUG_LOW);

              // Call the customer function to save the speech from a circular buffer to the database
              Cust_srback_save_speech( ((T_L1_SRBACK_SAVE_DATA_REQ *)(p_message))->database_id,
                                       ((T_L1_SRBACK_SAVE_DATA_REQ *)(p_message))->model_index,
                                       ((T_L1_SRBACK_SAVE_DATA_REQ *)(p_message))->start_buffer,
                                       ((T_L1_SRBACK_SAVE_DATA_REQ *)(p_message))->stop_buffer,
                                       ((T_L1_SRBACK_SAVE_DATA_REQ *)(p_message))->start_address,
                                       ((T_L1_SRBACK_SAVE_DATA_REQ *)(p_message))->stop_address);
            }

            /* set the confirm message identifier */
            confirm_message_id = L1_SRBACK_SAVE_DATA_CON;

            break;
          } // L1_SRBACK_SAVE_DATA_REQ

          case L1_SRBACK_LOAD_MODEL_REQ:
          {
            if ( ( ((T_L1_SRBACK_LOAD_MODEL_REQ *)(p_message))->CTO_enable ) == FALSE )
            {
              AUDIO_SEND_TRACE_PARAM("AUDIO SR BACKGROUND: load model without CTO",
                ((T_L1_SRBACK_LOAD_MODEL_REQ *)(p_message))->model_index,
                RV_TRACE_LEVEL_DEBUG_LOW);

              // Call the function to load a model
              Cust_srback_load_model( ((T_L1_SRBACK_LOAD_MODEL_REQ *)(p_message))->database_id,
                                      ((T_L1_SRBACK_LOAD_MODEL_REQ *)(p_message))->model_index,
                                      ((T_L1_SRBACK_LOAD_MODEL_REQ *)(p_message))->model_RAM_address );
            }
            else
            {
              // Calculate the good index
              index = ((T_L1_SRBACK_LOAD_MODEL_REQ *)(p_message))->model_index>>1;

              AUDIO_SEND_TRACE_PARAM("AUDIO SR BACKGROUND: load model with CTO",
                index,
                RV_TRACE_LEVEL_DEBUG_LOW);

              // Call the function to load a model with the good index
              Cust_srback_load_model( ((T_L1_SRBACK_LOAD_MODEL_REQ *)(p_message))->database_id,
                                      index,
                                      ((T_L1_SRBACK_LOAD_MODEL_REQ *)(p_message))->model_RAM_address );

              // The CTO algorithm is used and the model index is odd
              if ( ((T_L1_SRBACK_LOAD_MODEL_REQ *)(p_message))->model_index & 0x01 )
              {
                // Call the function to apply the CTO algorithm to the loaded model
                srback_CTO_algorithm( ((T_L1_SRBACK_LOAD_MODEL_REQ *)(p_message))->model_RAM_address );
              }
            }

            /* set the confirm message identifier */
            confirm_message_id = L1_SRBACK_LOAD_MODEL_CON;

            break;
          } // L1_SRBACK_LOAD_MODEL_REQ

          case L1_SRBACK_TEMP_SAVE_DATA_REQ:
          {
              AUDIO_SEND_TRACE("AUDIO SR BACKGROUND: save model in temporary buffer",
                RV_TRACE_LEVEL_DEBUG_LOW);

            // Call the function to save the model in a temporary buffer
            srback_save_model_temp( ((T_L1_SRBACK_TEMP_SAVE_DATA_REQ *)(p_message))->model_RAM_address_input,
                                    ((T_L1_SRBACK_TEMP_SAVE_DATA_REQ *)(p_message))->model_RAM_address_output );

            /* set the confirm message identifier */
            confirm_message_id = L1_SRBACK_TEMP_SAVE_DATA_CON;

            break;
          }
        } /* switch */

        /* send the confirmation message */
        /* allocate the buffer for the message to the L1 */
        p_confirm_message = audio_allocate_l1_message(0);
        if (p_confirm_message != NULL)
        {
          /* send the start command to the audio L1 */
          audio_send_l1_message(confirm_message_id, p_confirm_message);
        }
      } /* audio_sr_background_manager */
    #endif /* _WINDOWS */
  #endif /* SPEECH_RECO */
#endif /* RVM_AUDIO_MAIN_SWE */
void dummy_function5(void )
{
;
}


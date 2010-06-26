/*/ * @file dma_inth.c
 *
 * DMA initialise and handle interrupt functions.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  6/17/2003  ()   Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _DMA_INTH_H_
#include "dma/board/dma_inth.h"
#endif


#include "rv/rv_defined_swe.h"

#if (L1_EXT_AUDIO_MGT == 1)
#include "dma/sys_dma.h"
#endif

#undef    DMA_DEBUG

UINT8 dma_channel_number;
UINT16 dma_it_status;

extern T_DMA_CHANNEL_ARRAY dma_channel_array [DMA_MAX_NUMBER_OF_CHANNEL]; 
extern UINT8 dma_function_status [DMA_MAX_NUMBER_OF_CHANNEL];

#ifdef RVM_CAMD_SWE
#if CHIPSET != 15
extern void camd_stop_sensor_clock (void);
#else
extern void camcore_disable(void);
#endif
#endif

extern UINT8 dma_channel_number_tab[DMA_MAX_NUMBER_OF_CHANNEL];
extern UINT8 dma_it_status_tab[DMA_MAX_NUMBER_OF_CHANNEL];
extern UINT8 total_active_dma_channels;
/*-----------------------------------------------------------------------*/
/* dma_hisr()                                                            */
/*                                                                       */
/* This function is called when an DMA interrupt is received.            */
/*-----------------------------------------------------------------------*/

void dma_hisr (void)
{
    /* used to determine the status of the interrupt */
    UINT16 dma_channel_status = DMA_OK;

    /* used to send the status back to the originator */
    UINT8 dma_interrupt_status;       

    UINT8 n,dma_channel_index;

    T_DMA_TYPE_CHANNEL_PARAMETER dma_ll_channel_info ;

#ifdef DMA_DEBUG
    DMA_HISR_SEND_TRACE ("DMA HISR called", DMA_TRACE_LEVEL); 
#endif

    for(dma_channel_index=0;dma_channel_index<total_active_dma_channels;dma_channel_index++)
    {
        dma_channel_number = dma_channel_number_tab[dma_channel_index];
        /* Get the reason of the interrupt */
        dma_channel_status = dma_it_status_tab[dma_channel_index];

        if (dma_channel_number == DMA_MAX_NUMBER_OF_CHANNEL)
        {
            /* This is an impossible situation. Return immediatly */

            continue;
            /* UnMask interrupts */
        }


        /* Check if notification is required, otherwise 
           skip this part. Determine the reason now  */

        if (dma_channel_array[dma_channel_number].dma_end_notification_bool 
                == DMA_NOTIFICATION)
        {

            /* Find out the reason */
            if ((dma_channel_status & DMA_CICR_TOUT_IE_MASK) == 
                    DMA_CICR_TOUT_IE_MASK)
            {
                if ((dma_channel_status & DMA_CICR_TOUT_SRC_NDEST_IE_MASK) == 
                        DMA_CICR_TOUT_SRC_NDEST_IE_MASK)
                {
                    dma_interrupt_status = DMA_TIMEOUT_SOURCE;
                }
                else
                {
                    dma_interrupt_status = DMA_TIMEOUT_DESTINATION;
                }
            }
            else
            {
                if ((dma_channel_status & DMA_CICR_DROP_IE_MASK) == 
                        DMA_CICR_DROP_IE_MASK)
                {
                    dma_interrupt_status = DMA_MISS_EVENT;
                }
                else
                {
                    if ((dma_channel_status & DMA_CICR_HALF_BLOCK_IE_MASK) == 
                            DMA_CICR_HALF_BLOCK_IE_MASK)
                    {
#ifdef DMA_DEBUG
                        DMA_HISR_SEND_TRACE ("DMA HISR half block interupt received", DMA_TRACE_LEVEL); 
#endif
                        if (dma_channel_array[dma_channel_number].
                                double_buf_mode != DMA_NO_DOUBLE_BUF)
                        {
                            /* if the camera is working then the clock must be stopped as */
                            /* soon as possible. it is restarted after copying the momory */
#if CHIPSET != 15
                            if (dma_channel_array[dma_channel_number].channel_info.
                                    d_dma_channel_hw_synch == DMA_SYNC_DEVICE_NAND_FLASH)
                            {
#ifdef RVM_CAMD_SWE
                                extern void camd_stop_sensor_clock (void);
                                /* Stop the camera sensor */
                                camd_stop_sensor_clock ();
#endif
                            }


#endif

                            /* Decrease the amount of bytes to be transferred */
                            dma_channel_array[dma_channel_number].bytes_to_be_transferred -=
                                dma_channel_array[dma_channel_number].buffer_size;

                            dma_interrupt_status = DMA_BUF1_READY;
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else
                        if ((dma_channel_status & DMA_CICR_BLOCK_IE_MASK) == 
                                DMA_CICR_BLOCK_IE_MASK)
                        {
                            dma_interrupt_status = DMA_COMPLETED;


                            /* if the camera is working then the clock must be stopped as */
                            /* soon as possible. it is restarted after copying the momory */

#if CHIPSET != 15
                            if (dma_channel_array[dma_channel_number].channel_info.
                                    d_dma_channel_hw_synch == DMA_SYNC_DEVICE_NAND_FLASH)
                            {
#ifdef RVM_CAMD_SWE
                                extern void camd_stop_sensor_clock (void);
                                /* Stop the camera sensor */
                                camd_stop_sensor_clock ();
#endif
                            }

#endif

                            if (dma_channel_array[dma_channel_number].double_buf_mode != 
                                    DMA_NO_DOUBLE_BUF)
                            {
                                /* Check if for this channel double buffering is needed */
                                if ((dma_channel_array[dma_channel_number].double_buf_mode == 
                                            DMA_DEST_XRAM) || 
                                        (dma_channel_array[dma_channel_number].double_buf_mode == 
                                         DMA_DEST_DOUBLE_BUF))
                                {
                                    /* Change the offset in the source buffer if post incremental address mode is used */
                                    if (dma_channel_array[dma_channel_number].channel_info.
                                            d_dma_dst_channel_addr_mode == DMA_ADDR_MODE_POST_INC) 
                                    {
                                        if (dma_channel_array[dma_channel_number].int_or_ext == DMA_INTERNAL)
                                        {
                                            /* add offset to the destination adress */
                                            dma_channel_array[dma_channel_number].channel_info.d_dma_channel_src_address+=
                                                dma_channel_array[dma_channel_number].buffer_size;
                                        }
                                    }
                                }
                                else
                                {
                                    if ((dma_channel_array[dma_channel_number].double_buf_mode == DMA_SOURCE_DOUBLE_BUF) ||
                                            (dma_channel_array[dma_channel_number].double_buf_mode == DMA_SOURCE_XRAM ))
                                    {
                                        /* Change the offset in the destination buffer if post incremental address mode is used */
                                        if (dma_channel_array[dma_channel_number].channel_info.
                                                d_dma_dst_channel_addr_mode == DMA_ADDR_MODE_POST_INC) 
                                        {
                                            if (dma_channel_array[dma_channel_number].int_or_ext == DMA_EXTERNAL)
                                            {
                                                /* add offset to the destination adress */
                                                dma_channel_array[dma_channel_number].channel_info.d_dma_channel_dst_address+=
                                                    dma_channel_array[dma_channel_number].buffer_size;
                                            }
                                        }
                                    }
                                }

                                /* Decrease the amount of bytes to be transferred */
                                dma_channel_array[dma_channel_number].bytes_to_be_transferred -=
                                    dma_channel_array[dma_channel_number].buffer_size;

                                /*if (dma_channel_array[dma_channel_number].bytes_to_be_transferred>=0)*/
                                /*{*/
                                if (dma_channel_array[dma_channel_number].bytes_to_be_transferred<
                                        dma_channel_array[dma_channel_number].buffer_size)
                                {
                                    /* for the last part the frames might be modified */
                                    /* Write the number of frames to do */
                                    C_DMA_CFN_REG(dma_channel_number) = 
                                        (dma_channel_array[dma_channel_number].bytes_to_be_transferred/
                                         ((1<<dma_channel_array[dma_channel_number].channel_info.d_dma_channel_data_type)*
                                          dma_channel_array[dma_channel_number].channel_info.d_dma_channel_element_number)); 

                                    /* make sure that this is the last run */
                                    dma_channel_array[dma_channel_number].buffer_size = 
                                        dma_channel_array[dma_channel_number].bytes_to_be_transferred;
                                }

                                dma_interrupt_status = DMA_BUF2_READY;
                                /*}*/
                            }
                        }
                        else
                        {
                            /* other status are not catched yet, or 
                               an invalid status has been received.
                               in either way, nothing is done now 
                               */
                            continue;

                        }
                }
            }

            dma_send_status_message(dma_channel_number, dma_interrupt_status,
                    dma_channel_array[dma_channel_number].return_path);


            /* if finished, send DMG completed to the client */
            if (dma_channel_array[dma_channel_number].double_buf_mode != DMA_NO_DOUBLE_BUF)
            {
                if ((dma_interrupt_status == DMA_BUF1_READY) ||
                        (dma_interrupt_status == DMA_BUF2_READY))
                {
                    if (dma_channel_array[dma_channel_number].bytes_to_be_transferred==0)
                    {
                        dma_send_status_message(dma_channel_number, DMA_COMPLETED ,
                                dma_channel_array[dma_channel_number].return_path);
                        f_dma_channel_auto_init_disable(dma_channel_number);
                    }
                    else
                    {
                        /* restart if auto init is off and we are not finished yet */
                        if (dma_channel_array[dma_channel_number].channel_info.d_dma_channel_auto_init==DMA_SINGLE)
                        {
                            dma_channel_array[dma_channel_number].dma_restart = TRUE;
                        }
                    }
                }      
            }
        }

    }

    total_active_dma_channels=0;
    /* Unmask interrupts */
    F_INTH_ENABLE_ONE_IT(C_INTH_DMA_IT);

}



/*-----------------------------------------------------------------------*/
/* dma_secure_hisr()                                                     */
/*                                                                       */
/* This function is called when an secure DMA interrupt is received.     */
/*-----------------------------------------------------------------------*/
void dma_secure_hisr (void)
{
    /* used to determine the status of the interrupt */
    UINT16 dma_channel_status = DMA_OK; 

    /* used to send the status back to the originator */
    UINT16 dma_channel_number = DMA_OK;

    UINT16 dma_interrupt_status;
    UINT8 n;

    T_DMA_TYPE_CHANNEL_PARAMETER dma_ll_channel_info ;

#ifdef DMA_DEBUG
    DMA_HISR_SEND_TRACE ("DMA HISR called", DMA_TRACE_LEVEL); 
#endif

    /* Get the channel which generated an interrupt */
    dma_channel_number = DMA_ISR_REG;

    /* In case the ISR has no bits set, return immediately */
    if (dma_channel_number == 0)
    {
        /* UnMask interrupts */
        F_INTH_ENABLE_ONE_IT(C_INTH_2ND_INTH_IT)
            F_INTH_ENABLE_ONE_IT(C_INTH_SEC_DMA_IT)
            return;
    }

    /* Convert the channel mask to a number */
    for (n=0; n< DMA_MAX_NUMBER_OF_CHANNEL; n++)
    {
        if ((dma_channel_number & DMA_TRUE) == DMA_TRUE)
        {
            break;
        }
        dma_channel_number >>= 1;
    }

    dma_channel_number = n; /* In fact this is not needed, but it is easier 
                               to read. dma_channel_number now holds the actual
                               channel number
                               */

    if (dma_channel_number == DMA_MAX_NUMBER_OF_CHANNEL)
    {
        /* This is an impossible situation. Return immediatly */

        /* UnMask interrupts */
        F_INTH_ENABLE_ONE_IT(C_INTH_2ND_INTH_IT)
            F_INTH_ENABLE_ONE_IT(C_INTH_SEC_DMA_IT)
            return;
    }

    /* Get the reason of the interrupt */
    dma_channel_status = DMA_GET_CHANNEL_IT_STATUS(dma_channel_number);

    /* Check if notification is required, otherwise 
       skip this part. Determine the reason now  */

    if (dma_channel_array[dma_channel_number].dma_end_notification_bool 
            == DMA_NOTIFICATION)
    {
        /* Find out the reason */
        if ((dma_channel_status & DMA_CICR_TOUT_IE_MASK) == 
                DMA_CICR_TOUT_IE_MASK)
        {
            if ((dma_channel_status & DMA_CICR_TOUT_SRC_NDEST_IE_MASK) == 
                    DMA_CICR_TOUT_SRC_NDEST_IE_MASK)
            {
                dma_interrupt_status = DMA_TIMEOUT_SOURCE;
            }
            else
            {
                dma_interrupt_status = DMA_TIMEOUT_DESTINATION;
            }
        }
        else
        {
            if ((dma_channel_status & DMA_CICR_DROP_IE_MASK) == 
                    DMA_CICR_DROP_IE_MASK)
            {
                dma_interrupt_status = DMA_MISS_EVENT;
            }
            else
            {
                if ((dma_channel_status & DMA_CICR_HALF_BLOCK_IE_MASK) == 
                        DMA_CICR_HALF_BLOCK_IE_MASK)
                {
#ifdef DMA_DEBUG
                    DMA_HISR_SEND_TRACE ("DMA HISR half block interupt received", DMA_TRACE_LEVEL); 
#endif
                    if (dma_channel_array[dma_channel_number].double_buf_mode != 
                            DMA_NO_DOUBLE_BUF)
                    {
                        /* if the camera is working then the clock must be stopped as */
                        /* soon as possible. it is restarted after copying the momory */

#if CHIPSET != 15
                        if (dma_channel_array[dma_channel_number].channel_info.
                                d_dma_channel_hw_synch == DMA_SYNC_DEVICE_NAND_FLASH)
                        {
#if 0
                            ndef RVM_CAMD_SWE
                                extern void camd_stop_sensor_clock (void);
                            /* Stop the camera sensor */
                            camd_stop_sensor_clock ();
#endif
                        }


#endif
                        /* Decrease the amount of bytes to be transferred */
                        dma_channel_array[dma_channel_number].bytes_to_be_transferred -=
                            dma_channel_array[dma_channel_number].buffer_size;

                        dma_interrupt_status = DMA_BUF1_READY;
                    }
                    else
                    {
                        /* Unmask interrupts */
                        F_INTH_ENABLE_ONE_IT(C_INTH_2ND_INTH_IT)
                            F_INTH_ENABLE_ONE_IT(C_INTH_SEC_DMA_IT)
                            return;
                    }
                }
                else
                    if ((dma_channel_status & DMA_CICR_BLOCK_IE_MASK) == 
                            DMA_CICR_BLOCK_IE_MASK)
                    {
                        dma_interrupt_status = DMA_COMPLETED;

                        /* if the camera is working then the clock must be stopped as */
                        /* soon as possible. it is restarted after copying the momory */
#if CHIPSET != 15
                        if (dma_channel_array[dma_channel_number].channel_info.
                                d_dma_channel_hw_synch == DMA_SYNC_DEVICE_NAND_FLASH)
                        {
#ifdef RVM_CAMD_SWE
                            extern void camd_stop_sensor_clock (void);
                            /* Stop the camera sensor */
                            camd_stop_sensor_clock ();
#endif
                        }


#endif
                        if (dma_channel_array[dma_channel_number].double_buf_mode != 
                                DMA_NO_DOUBLE_BUF)
                        {
                            /* Check if for this channel double buffering is needed */
                            if ((dma_channel_array[dma_channel_number].double_buf_mode == 
                                        DMA_DEST_XRAM) || 
                                    (dma_channel_array[dma_channel_number].double_buf_mode == 
                                     DMA_DEST_DOUBLE_BUF))
                            {
                                /* Change the offset in the source buffer if post incremental address mode is used */
                                if (dma_channel_array[dma_channel_number].channel_info.
                                        d_dma_dst_channel_addr_mode == DMA_ADDR_MODE_POST_INC) 
                                {
                                    if (dma_channel_array[dma_channel_number].int_or_ext == DMA_INTERNAL)
                                    {
                                        /* add offset to the destination adress */
                                        dma_channel_array[dma_channel_number].channel_info.d_dma_channel_src_address+=
                                            dma_channel_array[dma_channel_number].buffer_size;
                                    }
                                }
                            }
                            else
                            {
                                if ((dma_channel_array[dma_channel_number].double_buf_mode == DMA_SOURCE_DOUBLE_BUF) ||
                                        (dma_channel_array[dma_channel_number].double_buf_mode == DMA_SOURCE_XRAM ))
                                {
                                    /* Change the offset in the destination buffer if post incremental address mode is used */
                                    if (dma_channel_array[dma_channel_number].channel_info.
                                            d_dma_dst_channel_addr_mode == DMA_ADDR_MODE_POST_INC) 
                                    {
                                        if (dma_channel_array[dma_channel_number].int_or_ext == DMA_EXTERNAL)
                                        {
                                            /* add offset to the destination adress */
                                            dma_channel_array[dma_channel_number].channel_info.d_dma_channel_dst_address+=
                                                dma_channel_array[dma_channel_number].buffer_size;
                                        }
                                    }
                                }
                            }

                            /* Decrease the amount of bytes to be transferred */
                            dma_channel_array[dma_channel_number].bytes_to_be_transferred -=
                                dma_channel_array[dma_channel_number].buffer_size;

                            /*if (dma_channel_array[dma_channel_number].bytes_to_be_transferred>=0)*/
                            /*{*/
                            if (dma_channel_array[dma_channel_number].bytes_to_be_transferred<
                                    dma_channel_array[dma_channel_number].buffer_size)
                            {
                                /* for the last part the frames might be modified */
                                /* Write the number of frames to do */
                                C_DMA_CFN_REG(dma_channel_number) = 
                                    (dma_channel_array[dma_channel_number].bytes_to_be_transferred/
                                     ((1<<dma_channel_array[dma_channel_number].channel_info.d_dma_channel_data_type)*
                                      dma_channel_array[dma_channel_number].channel_info.d_dma_channel_element_number)); 

                                /* make sure that this is the last run */
                                dma_channel_array[dma_channel_number].buffer_size = 
                                    dma_channel_array[dma_channel_number].bytes_to_be_transferred;
                            }

                            dma_interrupt_status = DMA_BUF2_READY;
                            /*}*/
                        }
                    }
                    else
                    {
                        /* other status are not catched yet, or 
                           an invalid status has been received.
                           in either way, nothing is done now 
                           */
                        /* Unmask interrupts */
                        F_INTH_ENABLE_ONE_IT(C_INTH_2ND_INTH_IT)
                            F_INTH_ENABLE_ONE_IT(C_INTH_SEC_DMA_IT)
                            return;
                    }
            }
        } 

        /* Send a message to the client */
        /* This is the secure handler ! */
        dma_send_status_message(dma_channel_number, dma_interrupt_status,
                dma_channel_array[dma_channel_number].return_path);


        /* if finished, send DMG completed to the client */
        if (dma_channel_array[dma_channel_number].double_buf_mode != DMA_NO_DOUBLE_BUF)
        {
            if ((dma_interrupt_status == DMA_BUF1_READY) ||
                    (dma_interrupt_status == DMA_BUF2_READY))
            {
                if (dma_channel_array[dma_channel_number].bytes_to_be_transferred==0)
                {
                    dma_send_status_message(dma_channel_number, DMA_COMPLETED ,
                            dma_channel_array[dma_channel_number].return_path);
                    f_dma_channel_auto_init_disable(dma_channel_number);
                }
                else
                {
                    /* restart if auto init is off and we are not finished yet */
                    if (dma_channel_array[dma_channel_number].channel_info.d_dma_channel_auto_init==DMA_SINGLE)
                    {
                        dma_channel_array[dma_channel_number].dma_restart = TRUE;
                    }
                }
            }      
        }
    }

    /* Unmask interrupts */
    F_INTH_ENABLE_ONE_IT(C_INTH_2ND_INTH_IT)
        F_INTH_ENABLE_ONE_IT(C_INTH_SEC_DMA_IT)
}

void hash_interupt_hisr (void)
{
    /* Unmask interrupts */
    F_INTH_ENABLE_ONE_IT(C_INTH_2ND_INTH_IT)
        F_INTH_ENABLE_ONE_IT(C_INTH_SHA1_MD5_IT)
}

/*-----------------------------------------------------------------------*/
/* Activate_DMA_HISR()                                                   */
/*                                                                       */
/* This function is called from the interrupt handler to activate        */
/* the HISR associated to the DMA Interrupt.                             */
/*                                                                       */
/*-----------------------------------------------------------------------*/
SYS_BOOL Activate_DMA_HISR(void)
{
    extern T_DMA_ENV_CTRL_BLK	*dma_env_ctrl_blk_p;

    if(NU_SUCCESS != NU_Activate_HISR(&dma_env_ctrl_blk_p->dma_hisr))
    {
        return FALSE;
    }
    return TRUE;
}


/*-----------------------------------------------------------------------*/
/* Activate_DMA_SECURE_HISR()                                            */
/*                                                                       */
/* This function is called from the interrupt handler to activate        */
/* the HISR associated to the DMA External Interrupt.                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
SYS_BOOL Activate_DMA_SECURE_HISR(void)
{
    extern T_DMA_ENV_CTRL_BLK	*dma_env_ctrl_blk_p;

    if(NU_SUCCESS != NU_Activate_HISR(&dma_env_ctrl_blk_p->dma_secure_hisr))
    {
        return FALSE;
    }
    return(TRUE);
}



/*-----------------------------------------------------------------------*/
/* dma_secure_int_handler()                                              */
/*                                                                       */
/* This function is called when an secure interrupt occurs. It will      */
/* first check if the DMA module actually exist and if it is not         */
/* uninitialised. When it exists and is initialising, the interrupt High */
/* interrupt service routine is activated that will be called in time.   */
/*-----------------------------------------------------------------------*/
void dma_secure_int_handler(void)
{

    extern T_DMA_ENV_CTRL_BLK	*dma_env_ctrl_blk_p;

    /* Mask interrupts */
    F_INTH_DISABLE_ONE_IT(C_INTH_2ND_INTH_IT)
        F_INTH_DISABLE_ONE_IT(C_INTH_SEC_DMA_IT)

        /* If dma is not started, return immediately */
        if (dma_env_ctrl_blk_p == 0)
        {
            /* re-enable the interrupts */
            F_INTH_ENABLE_ONE_IT(C_INTH_2ND_INTH_IT)
                F_INTH_ENABLE_ONE_IT(C_INTH_SEC_DMA_IT)
                return;
        }
        else
        {
            if(Activate_DMA_SECURE_HISR() == FALSE)
            {
                DMA_SEND_TRACE ("DMA SECURE HISR activation FAILED", DMA_TRACE_LEVEL);
                F_INTH_ENABLE_ONE_IT(C_INTH_2ND_INTH_IT)
                    F_INTH_ENABLE_ONE_IT(C_INTH_SEC_DMA_IT)
            }
        }
}


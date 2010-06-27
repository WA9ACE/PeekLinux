/**
 * @file	dma_inth.h
 *
 * Definitions for DMA interrupt handler.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/17/2003	 ()		Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _DMA_INTH_H_
#define _DMA_INTH_H_

#ifndef _RV_GENERAL_H_
#include "rv/rv_general.h"
#endif

#ifndef NUCLEUS
#include "nucleus.h"
#endif

#ifndef _INC_STRING
#include "string.h"
#endif

#ifndef __SYS_TYPES_H__
#include "sys_types.h"
#endif

#ifndef __DMA_INST_I_H_
#include "dma/dma_i.h"
#endif

#ifndef __SYS_INTH_H__
#include "inth/sys_inth.h"
#endif


#ifdef __cplusplus
extern "C"
{
#endif

void dma_int_handler(void);
void dma_secure_int_handler(void);

#define DMA_ISR_REG C_DMA_ISR_REG


/* Bits position in the register */
#define DMA_CICR_TOUT_IE_POS        C_DMA_CICR_TOUT_IE_POS
#define DMA_CICR_DROP_IE_POS        C_DMA_CICR_DROP_IE_POS
#define DMA_CICR_FRAME_IE_POS       C_DMA_CICR_FRAME_IE_POS
#define DMA_CICR_BLOCK_IE_POS       C_DMA_CICR_BLOCK_IE_POS
#define DMA_CICR_HALF_BLOCK_IE_POS  C_DMA_CICR_HALF_BLOCK_IE_POS

/* Mask of the field in the register */
#define DMA_CICR_TOUT_IE_MASK           (C_DMA_CSR_TOUT_MASK<<C_DMA_CSR_TOUT_POS)
#define DMA_CICR_DROP_IE_MASK           (C_DMA_CSR_DROP_MASK<<C_DMA_CSR_DROP_POS)
#define DMA_CICR_FRAME_IE_MASK          (C_DMA_CSR_FRAME_MASK<<C_DMA_CSR_FRAME_POS)
#define DMA_CICR_BLOCK_IE_MASK          (C_DMA_CSR_BLOCK_MASK<<C_DMA_CSR_BLOCK_POS)
#define DMA_CICR_HALF_BLOCK_IE_MASK     (C_DMA_CSR_HALF_BLOCK_MASK<<C_DMA_CSR_HALF_BLOCK_POS) 
#define DMA_CICR_TOUT_SRC_NDEST_IE_MASK (C_DMA_CSR_TOUT_SRC_NDST_MASK<<C_DMA_CSR_TOUT_SRC_NDST_POS) 

/* get channel interrupt status, cleared after read. 
   must be saved in a variable if re-use
 */
#define DMA_GET_CHANNEL_IT_STATUS(d_dma_channel_number)	F_DMA_GET_CHANNEL_IT_STATUS(d_dma_channel_number)	

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_DMA_INTH_H_

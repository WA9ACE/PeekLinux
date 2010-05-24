/**
 * @file	gbi_mmc.c
 *
 * Coding of the GBI MMC plugin functionality
 *
 * @author	 Patrick Klok()
 * @version 0.1
 */
/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	02/23/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */
#ifndef __GBI_MMC_H_
#define __GBI_MMC_H_

#include "rvm/rvm_gen.h"            /* Generic RVM types and functions. */
#include "mc/mc_api.h"

#include "gbi/gbi_mmc.h"
#include "gbi/gbi_message.h"
#include "gbi/gbi_mmc_states.h"

#ifndef _WINDOWS
	#define MMC_PLUGIN_USED
#endif
 
/** FIXED MEDIA AND PARTITION INFO READ FROM DANE_ELEC CARD*/
/** THIS IS ONLY TO DEMONSTRATE THE GBI -> MMC*/

/* Media 0: with 1 partitions, definitions  */
#define GBI_MMC_NR_OF_MEDIA                   0x01
#define GBI_MMC_MEDIA_NR_0                    0x00
#define GBI_MMC_MEDIA_0_NR_OF_PARTITIONS      0x01
#define GBI_MMC_MEDIA_0_MEDIA_TYPE            GBI_MMC
#define GBI_MMC_MEDIA_0_MEDIA_ID              0xE461602C
#define GBI_MMC_MEDIA_0_BYTES_PER_BLOCK       0x200    /* 512 bytes */
#define GBI_MMC_MEDIA_0_SPARE_AREA_SIZE       0x00     /* 16 bytes */
#define GBI_MMC_MEDIA_0_READ_SPEED            0x12C    /* 300 kbits/sec */
#define GBI_MMC_MEDIA_0_WRITE_SPEED           0x12C    /* 300 kbits/sec */
#define GBI_MMC_MEDIA_0_WRITEABLE             TRUE
 

/* Media 0, partition 0, definitions */
#define GBI_MMC_MEDIA_0_PARTITION_NR_0        0x00
#define GBI_MMC_MEDIA_0_PAR_0_FILESYSTEM      GBI_FAT16_B
#define GBI_MMC_MEDIA_0_PAR_0_NMB_OF_BLOCKS   0x0001EA01 /*61,25048828125 MB*/
#define GBI_MMC_MEDIA_0_PAR_0_FIRST_BLOCK     0x00
#define GBI_MMC_MEDIA_0_PAR_0_LAST_BLOCK      0x0001EA00 /**/


/**This value is used to convert from a speedindication(taac from mc/sd card) to a kb/s notation*/
/**These values have to be calculated with a benchmark from the GBI entity */
#define GBI_MMC_MEDIA_READ_SPEED_FACTOR_DIV      (1)
#define GBI_MMC_MEDIA_WRITE_SPEED_FACTOR_DIV      (1)

/** Macro for returning the C_SIZE from a CSD bit, 73:62 register */
#define GBI_CSD_GET_C_SIZE(CSD)      (((((UINT16)CSD[9]) & 0x03) << 10) | ((((UINT16)CSD[8]) & 0xFF) << 2) | ((((UINT16)CSD[7]) & 0xC0) >> 6))
/** Macro for returning the C_SIZE_MULT from a CSD, bit 49:47 register */
#define GBI_CSD_GET_C_SIZE_MULT(CSD) (((CSD[6] & 0x03) << 1) | ((CSD[5] & 0x80) >> 7))
/** Macro for returning the TAAC from a CSD, bit 119:112 register */
#define GBI_CSD_GET_TAAC(CSD)        (CSD[14] & 0xFF)
/** Macro for returning the NSAC from a CSD, bit 111:104 register */
#define GBI_CSD_GET_NSAC(CSD)        (CSD[13] & 0xFF)
/** Macro for returning the R2W_FACTOR from a CSD, bit 28:26 register */
#define GBI_CSD_GET_R2W_FACTOR(CSD) ((CSD[3] & 0x1c) >> 2)
/** Macro for returning the READ_BL_LEN from a CSD, bit 83:80 register */
#define GBI_CSD_GET_READ_BL_LEN(CSD) (CSD[10] & 0x0F)


/**macro to retreive time unit*/
#define GBI_TAAC_TIME_UNIT(taac)          (taac & 0x07)

/**values used as taac in TRAN_SPEED (CSD)*/
#define GBI_TAAC_TIME_UNIT_1_N_S          (0)
#define GBI_TAAC_TIME_UNIT_10_N_S         (1)
#define GBI_TAAC_TIME_UNIT_100_N_S        (2)
#define GBI_TAAC_TIME_UNIT_1_U_S          (3)
#define GBI_TAAC_TIME_UNIT_10_U_S         (4)
#define GBI_TAAC_TIME_UNIT_100_U_S        (5)
#define GBI_TAAC_TIME_UNIT_1_M_S          (6)
#define GBI_TAAC_TIME_UNIT_10_M_S         (7)



/**macro to retreive speedtime*/
#define GBI_TAAC_TIME_VALUE(tran_speed)   ((tran_speed >> 3) & 0x0F)

/**values used as timevalue in TRAN_SPEED (CSD)*/
#define GBI_TAAC_TIME_VALUE_1_0           (0x01)
#define GBI_TAAC_TIME_VALUE_1_2           (0x02)
#define GBI_TAAC_TIME_VALUE_1_3           (0x03)
#define GBI_TAAC_TIME_VALUE_1_5           (0x04)
#define GBI_TAAC_TIME_VALUE_2_0           (0x05)
#define GBI_TAAC_TIME_VALUE_2_5           (0x06)
#define GBI_TAAC_TIME_VALUE_3_0           (0x07)
#define GBI_TAAC_TIME_VALUE_3_5           (0x08)
#define GBI_TAAC_TIME_VALUE_4_0           (0x09)
#define GBI_TAAC_TIME_VALUE_4_5           (0x0A)
#define GBI_TAAC_TIME_VALUE_5_0           (0x0B)
#define GBI_TAAC_TIME_VALUE_5_5           (0x0C)
#define GBI_TAAC_TIME_VALUE_6_0           (0x0D)
#define GBI_TAAC_TIME_VALUE_7_0           (0x0E)
#define GBI_TAAC_TIME_VALUE_8_0           (0x0F)













extern T_RV_RET gbi_mmc_init(void);
extern T_RV_RET gbi_mmc_handle_message(T_RV_HDR *msg_p);
extern T_RV_RET gbi_mmc_hndl_read_req(T_GBI_READ_REQ_MSG *msg_p,
                                      T_GBI_BYTE_CNT media_bytes_per_blocks,
                                 T_GBI_BLOCK	 	  first_block_nmb);

extern T_RV_RET gbi_mmc_hndl_write_req(T_GBI_WRITE_REQ_MSG *msg_p,
                                 T_GBI_BYTE_CNT     media_bytes_per_blocks,
                                 T_GBI_BLOCK	 	  first_block_nmb);

extern T_RV_RET gbi_mmc_hndl_erase_req(T_GBI_ERASE_REQ_MSG *msg_p,
                                 T_GBI_BYTE_CNT     media_bytes_per_blocks,
                                 T_GBI_BLOCK	 	  first_block_nmb);

extern T_RV_RET gbi_mmc_hndl_flush_req(T_GBI_FLUSH_REQ_MSG *msg_p,
                                 T_GBI_BYTE_CNT     media_bytes_per_blocks,
                                 T_GBI_BLOCK	 	  first_block_nmb);



#endif /** __GBI_MMC_H_ */

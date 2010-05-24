/**
 * @file  mc_i.h
 *
 * Internal definitions for MC.
 *
 * @author   (P.Klok)
 * @version  0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  7/1/2003   ()   Create.
 *
 * (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __MC_INST_I_H_
#define __MC_INST_I_H_


#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#ifndef _WINDOWS
  #include "nucleus.h"
  #include "inth/sys_inth.h"

#if 1 // Added for MMC compilation in Locosto
  #define MEM_MMC_SD_IO 0xFFFFC800L
  #define    C_DMA_CHANNEL_MMC_SD_RX        16
  #define    C_DMA_CHANNEL_MMC_SD_TX        17
#endif // #if 1 // Added for MMC compilation in Locosto
  #define MEM_MC_SD_IO MEM_MMC_SD_IO
#endif

#include "mc/mc_cfg.h"
#include "mc/mc_api.h"
#include "mc/mc_message.h"

#include "mc/mc_state_i.h"

#ifndef T_FLASH_HOT_SWAP
#define T_FLASH_HOT_SWAP	//ashwin
#endif

extern void mc_send_trace_priv(char * string, UINT8 msg_length, UINT32 val, UINT8 trace_level, UINT32 swe_use_id);
/** Macro used for tracing MC messages. */
#ifdef MC_ENABLE_TRACE
#ifndef _WINDOWS
  #define MC_SEND_TRACE(string, trace_level) \
    //mc_send_trace_priv (string, (sizeof(string) - 1), NULL_PARAM, trace_level, MC_USE_ID)
    rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, MC_USE_ID)
  #define MC_SEND_TRACE_PARAM(string, param, trace_level) \
    mc_send_trace_priv (string, (sizeof(string) - 1), param, trace_level, MC_USE_ID)
#else
  #define MC_SEND_TRACE(string, trace_level) \
    rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, MC_USE_ID)
  #define MC_SEND_TRACE_PARAM(string, param, trace_level) \
    rvf_send_trace (string, (sizeof(string) - 1), param, trace_level, MC_USE_ID)
#endif
#else

/** Used Callback function*/
extern void mc_return_queue_init (T_RVF_G_ADDR_ID rq_addr_id, UINT16 rq_event,
                        T_RV_RETURN_PATH * path_to_mc_return_queue_p);



#define MC_SEND_TRACE(string, trace_level)              /* Nothing */
#define MC_SEND_TRACE_PARAM(string, param, trace_level) /* Nothing */

#endif

/** Marco for freeing messages */
#define MC_FREE_MSG(msg_p)   {if(msg_p) rvf_free_msg((T_RVF_MSG *) msg_p);(msg_p)=NULL;}

/** Marco for freeing memory */
#define MC_FREE_BUF(buf_p)   {if(buf_p) rvf_free_buf((T_RVF_BUFFER *) buf_p);(buf_p)=NULL;}

/** Marco for reading  **/
#define MC_REG(A)   (*(volatile UINT16*)(A))

/* Maximum supported cards (for now, just one supported) */
#define MC_MAX_STACK_SIZE     (1)

/* Hardware specific definitions */
/* Maximum FIFO size in bytes */
#define MAX_FIFO_SIZE         (64)

/* Maximum subscribers */
#define MC_MAX_SUBSCRIBER     (10)

/** CSD register size (in bytes) */
#define MC_CSD_SIZE          (16)

#define SD_CMD_SIZE			  (6)

#define SD_DATA_SIZE		  (512)	//Added for SD card data block size

/** CSD register size (in bytes) */
#define MC_CSR_SIZE          (8)


/** General MC timeout */
#define MC_TIME_OUT          (RVF_MS_TO_TICKS(4500))

/** defines the MCLK frequency (is 13 MHZ)*/
//#define MC_MCLK              13000000

/** Clock frequencies */
#define MC_CLK_270kHz         (0x30) /* Card acquisition */
//#define MC_CLK_1300kHz        (10)
//#define MC_CLK_1625kHz        (8)
#define MC_CLK_2166kHz        (6)    /* Safe speed */
//#define MC_CLK_2600kHz        (5)
//#define MC_CLK_3250kHz        (4)    /* Maximum without distortion */
//#define MC_CLK_13000kHz       (1)    /* Maximum possible clk speed */

/** Identification clock divider(200Khz)(400Khz is MAX):
  * 13 Mhz/0x30 */
#define MC_ID_CLK_DIV         MC_CLK_270kHz

/** Normal operating speed (1.3Mhz, safe speed for common range of cards)*/
#define MC_NORM_CLK_DIV       MC_CLK_2166kHz
//NOT USED IN LOCOSTO
#if 0
/**macro to retreive speedrate*/
#define MC_TRAN_SPEED_RATE_UNIT(tran_speed)  (tran_speed & 0x07)
/**values used as speedrate in TRAN_SPEED (CSD)*/

#define MC_TRAN_SPEED_RATE_UNIT_100_K_BIT      (0)
#define MC_TRAN_SPEED_RATE_UNIT_1_M_BIT        (1)
#define MC_TRAN_SPEED_RATE_UNIT_10_M_BIT       (2)
#define MC_TRAN_SPEED_RATE_UNIT_100_M_BIT      (3)



/**macro to retreive speedtime*/
#define MC_TRAN_SPEED_TIME_VALUE(tran_speed)   ((tran_speed >> 3) & 0x0F)

/**values used as timevalue in TRAN_SPEED (CSD)*/
#define MC_TRAN_SPEED_TIME_VALUE_1_0           (0x01)
#define MC_TRAN_SPEED_TIME_VALUE_1_2           (0x02)
#define MC_TRAN_SPEED_TIME_VALUE_1_3           (0x03)
#define MC_TRAN_SPEED_TIME_VALUE_1_5           (0x04)
#define MC_TRAN_SPEED_TIME_VALUE_2_0           (0x05)
#define MC_TRAN_SPEED_TIME_VALUE_2_5           (0x06)
#define MC_TRAN_SPEED_TIME_VALUE_3_0           (0x07)
#define MC_TRAN_SPEED_TIME_VALUE_3_5           (0x08)
#define MC_TRAN_SPEED_TIME_VALUE_4_0           (0x09)
#define MC_TRAN_SPEED_TIME_VALUE_4_5           (0x0A)
#define MC_TRAN_SPEED_TIME_VALUE_5_0           (0x0B)
#define MC_TRAN_SPEED_TIME_VALUE_5_5           (0x0C)
#define MC_TRAN_SPEED_TIME_VALUE_6_0           (0x0D)
#define MC_TRAN_SPEED_TIME_VALUE_7_0           (0x0E)
#define MC_TRAN_SPEED_TIME_VALUE_8_0           (0x0F)


/**macro to retreive time unit*/
#define MC_TAAC_TIME_UNIT(taac)          (taac & 0x07)

/**values used as taac in TRAN_SPEED (CSD)*/
#define MC_TAAC_TIME_UNIT_1_N_S          (0)
#define MC_TAAC_TIME_UNIT_10_N_S         (1)
#define MC_TAAC_TIME_UNIT_100_N_S        (2)
#define MC_TAAC_TIME_UNIT_1_U_S          (3)
#define MC_TAAC_TIME_UNIT_10_U_S         (3)
#define MC_TAAC_TIME_UNIT_100_U_S        (3)
#define MC_TAAC_TIME_UNIT_1_M_S          (3)
#define MC_TAAC_TIME_UNIT_10_M_S         (3)



/**macro to retreive speedtime*/
#define MC_TAAC_TIME_VALUE(tran_speed)   ((tran_speed >> 3) & 0x0F)

/**values used as timevalue in TRAN_SPEED (CSD)*/
#define MC_TAAC_TIME_VALUE_1_0           (0x01)
#define MC_TAAC_TIME_VALUE_1_2           (0x02)
#define MC_TAAC_TIME_VALUE_1_3           (0x03)
#define MC_TAAC_TIME_VALUE_1_5           (0x04)
#define MC_TAAC_TIME_VALUE_2_0           (0x05)
#define MC_TAAC_TIME_VALUE_2_5           (0x06)
#define MC_TAAC_TIME_VALUE_3_0           (0x07)
#define MC_TAAC_TIME_VALUE_3_5           (0x08)
#define MC_TAAC_TIME_VALUE_4_0           (0x09)
#define MC_TAAC_TIME_VALUE_4_5           (0x0A)
#define MC_TAAC_TIME_VALUE_5_0           (0x0B)
#define MC_TAAC_TIME_VALUE_5_5           (0x0C)
#define MC_TAAC_TIME_VALUE_6_0           (0x0D)
#define MC_TAAC_TIME_VALUE_7_0           (0x0E)
#define MC_TAAC_TIME_VALUE_8_0           (0x0F)

#endif


#ifndef _WINDOWS
  /* Size of the HISR stack associated to the MC interrupt */
#ifndef HISR_STACK_SHARING
  #define MC_HISR_STACK_SIZE   (512)
#endif
#endif


/*NOT REQUIRED IN LOCOSTO*/
/** Register offsets **/

//#define MC_CMD   MC_REG(MEM_MC_SD_IO + 0x00)
//#define MC_ARGL  MC_REG(MEM_MC_SD_IO + 0x02)
//#define MC_ARGH  MC_REG(MEM_MC_SD_IO + 0x04)
//#define MC_CON   MC_REG(MEM_MC_SD_IO + 0x06)
#define MC_STAT  MC_REG(MEM_MC_SD_IO + 0x08)
#define MC_IE    MC_REG(MEM_MC_SD_IO + 0x0A)
#define MC_CTO   MC_REG(MEM_MC_SD_IO + 0x0C)
#define MC_DTO   MC_REG(MEM_MC_SD_IO + 0x0E)
#define MC_DATA  MC_REG(MEM_MC_SD_IO + 0x10)
#define MC_BLEN  MC_REG(MEM_MC_SD_IO + 0x12)
#define MC_NBLK  MC_REG(MEM_MC_SD_IO + 0x14)
#define MC_BUF   MC_REG(MEM_MC_SD_IO + 0x16)
#define MC_SPI   MC_REG(MEM_MC_SD_IO + 0x18)
#define MC_SDIO  MC_REG(MEM_MC_SD_IO + 0x1A)
#define MC_SYST  MC_REG(MEM_MC_SD_IO + 0x1C)
#define MC_REV   MC_REG(MEM_MC_SD_IO + 0x1E)
#define MC_RSP0  MC_REG(MEM_MC_SD_IO + 0x20)
#define MC_RSP1  MC_REG(MEM_MC_SD_IO + 0x22)
#define MC_RSP2  MC_REG(MEM_MC_SD_IO + 0x24)
#define MC_RSP3  MC_REG(MEM_MC_SD_IO + 0x26)
#define MC_RSP4  MC_REG(MEM_MC_SD_IO + 0x28)
#define MC_RSP5  MC_REG(MEM_MC_SD_IO + 0x2A)
#define MC_RSP6  MC_REG(MEM_MC_SD_IO + 0x2C)
#define MC_RSP7  MC_REG(MEM_MC_SD_IO + 0x2E)
#define MC_IOSR  MC_REG(MEM_MC_SD_IO + 0x30)
#define MC_SYSC  MC_REG(MEM_MC_SD_IO + 0x32)
#define MC_SYSS  MC_REG(MEM_MC_SD_IO + 0x34)


/** GPIO register  mappings */
#define MC_GPIO_INPUT_LATCH     MC_REG(C_MAP_GPIO_BASE + 0x00)
#define MC_GPIO_IO_CNTL_REG     MC_REG(C_MAP_GPIO_BASE + 0x04)
#define MC_GPIO_IE_LEVEL_REG    MC_REG(C_MAP_GPIO_BASE + 0x16)
#define MC_GPIO_IE_MASK_REG     MC_REG(C_MAP_GPIO_BASE + 0x18)
#define MC_GPIO_IE_STATUS_REG   MC_REG(C_MAP_GPIO_BASE + 0x1E)
#define MC_GPIO_IE_SOFTCLEAR    MC_REG(C_MAP_GPIO_BASE + 0x22)


/** PHYSICAL USED GPIO PINS */
#define MC_GPIO_10              0x0000400
#define MC_GPIO_13              0x0002000

#ifndef _WINDOWS
#define MC_WRITE_PROTECTED    ((MC_GPIO_INPUT_LATCH & MC_GPIO_10) == MC_GPIO_10 ? 1:0)
#define MC_CARD_INSERTED      ((MC_GPIO_INPUT_LATCH & MC_GPIO_13) == MC_GPIO_13 ? 0:1)
#else
#define MC_WRITE_PROTECTED    (1)
#define MC_CARD_INSERTED      (1)
#endif /**_WINDOWS*/

/** Class 0: basic commands **/
#define GO_IDLE_STATE          0	/*SUPPORTED IN SPI MODE*/
#define SEND_OP_COND           1	/*SUPPORTED IN SPI MODE*/

#define SEND_APP_CMD		   55
#define SEND_SD_OP_COND		   41

//#define ALL_SEND_CID           2  /* NOT SUPPORTED IN SPI MODE */
//#define SET_RELATIVE_ADDR      3  /* NOT SUPPORTED IN SPI MODE */
//#define SET_DSR                4  /* NOT SUPPORTED IN SPI MODE */
//#define SEL_DESEL_CARD         7  /* NOT SUPPORTED IN SPI MODE */

#define SEND_CSD               9	/*SUPPORTED IN SPI MODE*/
#define SEND_CID              10	/*SUPPORTED IN SPI MODE*/
#define STOP_TRANSMISSION     12	/*SUPPORTED IN SPI MODE*/
#define SEND_STATUS           13	/*SUPPORTED IN SPI MODE*/


#define READ_OCR			  58	//added for LOCOSTO CMD58
#define CRC_ON_OFF			  59	//added for LOCOSTO CMD59
//#define GO_INACTIVE_STATE     15

/** Class 0: stream read commands **/
#define READ_DAT_UNTIL_STOP   11

/** Class 2: block read commands  **/
#define SET_BLOCKLEN          16	/*SUPPORTED IN SPI MODE*/
#define READ_SINGLE_BLOCK     17	/*SUPPORTED IN SPI MODE*/
#define READ_MULTIPLE_BLOCK   18	/*SUPPORTED IN SPI MODE*/
#define SET_BLOCK_COUNT       23

/** Class 3: stream read commands **/

//#define WRITE_DAT_UNTIL_STOP  20  /* NOT SUPPORTED IN SPI MODE */

/** Class 4: block write commands **/
#define WRITE_BLOCK           24	/*SUPPORTED IN SPI MODE*/
#define WRITE_MULTIPLE_BLOCK  25	/*SUPPORTED IN SPI MODE*/

//#define PROGRAM_CID           26  /* NOT SUPPORTED IN SPI MODE */

#define PROGRAM_CSD           27	/*SUPPORTED IN SPI MODE*/

/** Class 5: erase commands **/
#define ERASE_WR_BLK_START    32	/*SUPPORTED IN SPI MODE*/
#define ERASE_WR_BLK_END      33	/*SUPPORTED IN SPI MODE*/
//#define ERASE_GROUP_START     35	/* NOT SUPPORTED IN SPI MODE */
//#define ERASE_GROUP_END       36  /* NOT SUPPORTED IN SPI MODE */
#define ERASE                 38	/*SUPPORTED IN SPI MODE*/

/** Class 6: write protection commands **/
#define SET_WRITE_PROT        28	/*SUPPORTED IN SPI MODE*/
#define CLR_WRITE_PROT        29	/*SUPPORTED IN SPI MODE*/
#define SEND_WRITE_PROT       30	/*SUPPORTED IN SPI MODE*/








/** Class 8: application specific commands **/
#define APP_CMD               55	/*SUPPORTED IN SPI MODE*/
#define GEN_CMD               56	/*SUPPORTED IN SPI MODE*/

/** Application specific commands reserved by SD Memory Card only **/

//#define SET_BUS_WIDTH          6  /* NOT SUPPORTED IN SPI MODE */
#define SD_STATUS             13
//#define SEND_NUM_WR_BLOCKS    22  /* NOT SUPPORTED IN SPI MODE */
//#define SET_WR_BLK_ERASE_COUNT 23 /* NOT SUPPORTED IN SPI MODE */
#define SD_SEND_OP_COND       41  /* NOT SUPPORTED IN SPI MODE */
//#define SET_CLR_CARD_DETECT   42  /* NOT SUPPORTED IN SPI MODE */
#define SEND_SCR              51
//#define SEND_RELATIVE_ADDR     3  /* NOT SUPPORTED IN SPI MODE */
//#define LOCK_UNLOCK           42


/* IRQ flags */
#define MC_IRQ_CERR    0x4000  /* Card status error interrupt enable */
#define MC_IRQ_CIRQ    0x2000  /* Card IRQ interrupt enable */
#define MC_IRQ_OCRB    0x1000  /* OCR busy interrupt enable */
#define MC_IRQ_AE      0x0800  /* Buffer almost empty interrupt enable */
#define MC_IRQ_AF      0x0400  /* Buffer almost full interrupt enable */
#define MC_IRQ_CRW     0x0200  /* Card read wait enable */
#define MC_IRQ_CCRC    0x0100  /* Command CRC error interrupt enable */
#define MC_IRQ_CTO     0x0080  /* Command response time-out interrupt enable */
#define MC_IRQ_DCRC    0x0040  /* Data CRC error interrupt enable */
#define MC_IRQ_DTO     0x0020  /* Data response time-out interrupt enable */
#define MC_IRQ_EOFB    0x0010  /* Card exit busy state interrupt enable */
#define MC_IRQ_BRS     0x0008  /* Block received / sent interrupt enable */
#define MC_IRQ_CB      0x0004  /* Card enter busy state interrupt enable */
#define MC_IRQ_CD      0x0002  /* Card detect interrupt enable */
#define MC_IRQ_EOC     0x0001  /* End of command interrupt enable */

/*R2 response errors*/
#define MC_RESP_PARM_ERR           0x0400 /*card parameter error*/
#define MC_RESP_ADD_ERR            0x0200 /*card address error*/
#define MC_RESP_ERA_ERR            0x0100 /*card erase sequence error*/
#define MC_RESP_CRC_ERR            0x0800 /*card com crc error*/
#define MC_RESP_ICOM_ERR           0x0400 /*card illegal command*/
#define MC_RESP_ERA_RESET          0x0200 /*card erase param */
#define MC_RESP_IDLE_STATE         0x0100 /*card idle state */
#define MC_RESP_OUT_RANGE		   0x0080 /*card out of range or csd overwrite error*/
#define MC_RESP_ERA_PARM_ERR       0x0040 /* erase parameter error*/
#define MC_RESP_WP_ERR			   0x0020 /*card WP violation*/
#define MC_RESP_ECC_ERR 		   0x0010 /*card ECC error */
#define MC_RESP_CC_ERR			   0x0008 /*card controller error */
#define MC_RESP_GEN_ERR            0x0004 /*card general error*/
#define MC_RESP_WP_ERASE_ERR       0x0002 /*wp erase skip |lock/unlock cmd failed*/
#define MC_RESP_CARD_LOCKED        0x0001 /*card is locked*/
/* Masks */
#define MC_OCR_BUSY    0x80000000  /* Power-up routine (31 bit)   */
#define MC_RESET_CMPL  0x0001  /* Reset complete in MC_SYSS  */
#if 0
/* MC ocr voltage (range) masks */
#define MC_OCR_RANGE_35_36     0x0800000 /* 3.5 - 3.6 Volt*/
#define MC_OCR_RANGE_34_35     0x0400000 /* 3.4 - 3.5 Volt*/
#define MC_OCR_RANGE_33_34     0x0200000 /* 3.3 - 3.4 Volt*/
#define MC_OCR_RANGE_32_33     0x0100000 /* 3.2 - 3.3 Volt*/
#define MC_OCR_RANGE_31_32     0x0080000 /* 3.1 - 3.2 Volt*/
#define MC_OCR_RANGE_30_31     0x0040000 /* 3.0 - 3.1 Volt*/
#define MC_OCR_RANGE_29_30     0x0020000 /* 2.9 - 3.0 Volt*/
#define MC_OCR_RANGE_28_29     0x0010000 /* 2.8 - 2.9 Volt*/
#define MC_OCR_RANGE_27_28     0x0008000 /* 2.7 - 2.8 Volt*/
#define MC_OCR_RANGE_26_27     0x0004000 /* 2.6 - 2.7 Volt*/
#define MC_OCR_RANGE_25_26     0x0002000 /* 2.5 - 2.6 Volt*/
#define MC_OCR_RANGE_24_25     0x0001000 /* 2.4 - 2.5 Volt*/
#define MC_OCR_RANGE_23_24     0x0000800 /* 2.3 - 2.4 Volt*/
#define MC_OCR_RANGE_22_23     0x0000400 /* 2.2 - 2.3 Volt*/
#define MC_OCR_RANGE_21_22     0x0000200 /* 2.1 - 2.2 Volt*/
#define MC_OCR_RANGE_20_21     0x0000100 /* 2.0 - 2.1 Volt*/
#define MC_OCR_RANGE_19_20     0x0000080 /* 1.9 - 2.0 Volt*/
#define MC_OCR_RANGE_18_19     0x0000040 /* 1.8 - 1.9 Volt*/
#define MC_OCR_RANGE_17_18     0x0000020 /* 1.7 - 1.8 Volt*/
#define MC_OCR_RANGE_16_17     0x0000010 /* 1.6 - 1.7 Volt*/
#endif

/** describes the used voltage on the SD MC CARD reader, now used is the e-sample voltage*/
#define MC_OCR_USED_VOLTAGE    MC_OCR_RANGE_35_36


/* Card states */
#define MC_STATE_IDLE   0x0
#define MC_STATE_READY  0x1
#define MC_STATE_IDENT  0x2
#define MC_STATE_STDY   0x3
#define MC_STATE_TRAN   0x4
#define MC_STATE_DATA   0x5
#define MC_STATE_RCV    0x6
#define MC_STATE_PRG    0x7
#define MC_STATE_DIS    0x8


/* Card status bits*/
//#define MC_OUT_OF_RANGE        0x80000000
//#define MC_ADDRESS_ERROR       0x40000000
//#define MC_BLOCK_LEN_ERROR     0x20000000
//#define MC_ERASE_SEQ_ERROR     0x10000000
//#define MC_ERASE_PARAM         0x08000000
//#define MC_WP_VIOLATION        0x04000000
//#define MC_CARD_IS_LOCKED      0x02000000
//#define MC_LOCK_UNLOCK_FAILED  0x01000000



// Marco for setting the command register
#define SET_CMD_REG(ddir, shr, type, busy, rsp, inab, odto, indx) {\
//  MC_CMD = ( (ddir << 15) | (shr << 14) | (type << 12) | (busy << 11) |\
            (rsp << 8) | (inab << 7) | (odto << 6) | (indx) );}

// Marco for setting the configuration register
// #define SET_CON_REG(dw, mode, power_up, be, clk_div) {\
//  MC_CON = ( (dw << 15) | (mode << 12) | (power_up << 11) | (be << 10) | (clk_div) );}

// Marco for setting the buffer register
#define SET_BUF_REG(rxde, afl, txde, ael) {\
  MC_BUF = ( (rxde << 15) | (afl << 8) | (txde << 7) | (ael) );}

/** Macro for clearing status register **/
#define CLR_STATUS_REG {//MC_STAT = 0xFFFF;\
  mc_env_ctrl_blk_p->mc_host_received_events = 0;}


/** Macro's to retreive data out of a CSD register*/

/** Macro for returning the C_SIZE from a CSD bit, 73:62 register */
#define CSD_GET_C_SIZE(CSD)      (((((UINT16)CSD[9]) & 0x03) << 10) | ((((UINT16)CSD[8]) & 0xFF) << 2) | ((((UINT16)CSD[7]) & 0xC0) >> 6))
/** Macro for returning the C_SIZE_MULT from a CSD, bit 49:47 register */
#define CSD_GET_C_SIZE_MULT(CSD) (((CSD[6] & 0x03) << 1) | ((CSD[5] & 0x80) >> 7))
/** Macro for returning the READ_BL_LEN from a CSD, bit 83:80 register */
#define CSD_GET_READ_BL_LEN(CSD) (CSD[10] & 0x0F)
/** Macro for returning the READ_BL_LEN from a CSD, bit 25:22 register */
#define CSD_GET_WRITE_BL_LEN(CSD) (((CSD[3] & 0x03) << 2) | ((CSD[2] & 0xc0) >> 6))
/** Macro for returning the TAAC from a CSD, bit 119:112 register */
#define CSD_GET_TAAC(CSD)        (CSD[14] & 0xFF)
/** Macro for returning the NSAC from a CSD, bit 111:104 register */
#define CSD_GET_NSAC(CSD)        (CSD[13] & 0xFF)
/** Macro for returning the TRAN_SPEED from a CSD, bit 103:96 register */
#define CSD_GET_TRAN_SPEED(CSD) (CSD[12] & 0xFF)
/** Macro for returning the R2W_FACTOR from a CSD, bit 28:26 register */
#define CSD_GET_R2W_FACTOR(CSD) ((CSD[3] & 0x1c) >> 2)






/** Macro for calculating the card size in bytes*/
#define CSD_CARD_SIZE(CSD)    (((UINT32)(CSD_GET_C_SIZE(CSD) + 1)) <<(((UINT32)CSD_GET_C_SIZE_MULT(CSD)) + ((UINT32)2) + ((UINT32)(CSD_GET_READ_BL_LEN(CSD)))))


/** Macro for setting the OCR register */
#define SET_OCR_REG(VOLT) ( (VOLT << 7) | (0x1FF << 15) )

/** bus width defines */
//#define MC_CONF_BUS_WIDTH_1   0
#define MC_CONF_BUS_WIDTH_4   1
#if 0

/**used defines needed for DMG data transfer*/
/* use 16 bits data because of FIFO width of 16 bits*/
#define MC_DMG_DATA_WIDTH             DMG_DATA_S16

#define MC_DMG_SYNC_DEVICE_READ       DMG_PERIPHERAL_MC_SD_RX
#define MC_DMG_SYNC_DEVICE_WRITE      DMG_PERIPHERAL_MC_SD_TX

#define MC_DMG_HW_PRIORITY            DMG_HW_PRIORITY_LOW
#define MC_DMG_REPEAT                 DMG_SINGLE
#define MC_DMG_FLUSH                  DMG_FLUSH_DISABLED

#define MC_DMG_END_NOTIFICATION       DMG_NOTIFICATION
#define MC_DMG_SECURE                 DMG_NOT_SECURED


#define MC_DMG_SRC_ADDR_READ          (UINT32) &MC_DATA

#define MC_DMG_SRC_ADDR_MODE_READ     DMG_ADDR_MODE_CONSTANT
#define MC_DMG_SRC_ADDR_MODE_WRITE   DMG_ADDR_MODE_POST_INC
#define MC_DMG_SRC_PACKING            DMG_NOT_PACKED
#define MC_DMG_SRC_BURST              DMG_NO_BURST


#define MC_DMG_DEST_ADDR_WRITE        (UINT32) &MC_DATA

#define MC_DMG_DEST_ADDR_MODE_READ    DMG_ADDR_MODE_POST_INC
#define MC_DMG_DEST_ADDR_MODE_WRITE   DMG_ADDR_MODE_CONSTANT
#define MC_DMG_DEST_PACKING           DMG_NOT_PACKED
#define MC_DMG_DEST_BURST             DMG_NO_BURST

#endif


/** Host configuration type **/
typedef struct
{
  BOOL   dw;
  UINT16 mode;
  BOOL   power_up;
  BOOL   be;
  UINT16 clk_div;
  UINT16 dto;
  UINT16 cto;
} T_MC_HOST_CONF;

/** OCR register type */
typedef UINT32 T_MC_OCR;


/** DSR register type */
typedef UINT16 T_MC_DSR;


/** Card Administration structure */
typedef struct
{
  BOOL            used;       /** used flag             */
  T_MC_CARD_TYPE card_type;  /** used type of card     */
  T_MC_RCA       phys_rca;   /** Physical used RCA     */
  T_MC_OCR       ocr;        /** ocr register 128 bits */
  UINT8           cid[16];    /** cid register128 bits  */
  UINT8           csd[16];    /** csd register 128 bits*/
}T_MC_CARD_INFO;



/**
 *This structure gather general informations
 *about subscriber id
 */
typedef struct
{
  T_RV_RETURN  return_path;
  T_MC_EVENTS events;
  /**
   * Can be extended for future
   * functionality
   */
} T_SUBSCRIBER_INFO;

/**
 * The Control Block buffer of MC, which gathers all 'Global variables'
 * used by MC instance.
 *
 * A structure should gathers all the 'global variables' of MC instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_MC_ENV_CTRL_BLK buffer is allocated when creating MC instance and is
 * then always refered by MC instance when access to 'global variable'
 * is necessary.
 */
typedef struct
{


  /** Store the current state of the MC instance */
  T_MC_INTERNAL_STATE state;

  /** Set when internal initialise is complete */
  BOOL initialised;

  /** Pointer to the error function */
  T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name,
               T_RVM_RETURN error_cause,
               T_RVM_ERROR_TYPE error_type,
               T_RVM_STRING error_msg);
  /** Mem bank id. */
  T_RVF_MB_ID prim_mb_id;

  T_RVF_ADDR_ID addr_id;

  T_RV_RETURN_PATH path_to_return_queue;

  T_MC_HOST_CONF conf;

  /** Reserved subscriber id's */
  UINT32 reserved_subscriber_id;

  /* Informations for all the subscribers */
  T_SUBSCRIBER_INFO* subscriber_infos[MC_MAX_SUBSCRIBER];

  /** Reserved Card id's */
  UINT32 reserved_card_id;

  T_MC_CARD_INFO* card_infos[MC_MAX_STACK_SIZE];

  BOOL    mc_card_inserted;

  /** DMA mode */
  T_MC_DMA_MODE dma_mode;

  /** IRQ mask */
  UINT16 irq;

  UINT16 gpio_status;

  /** Host event status */
  UINT16 mc_host_received_events;

 	T_MC_EVENT_CALLBACK                     ntfcCallback;       /* Callback to notify  events to Application */
  	void									*ntfcCtx;			/* Context Information */


#ifndef _WINDOWS
  /** HISR */
  NU_HISR mc_hisr;
#ifndef HISR_STACK_SHARING
  char mc_hisr_stack[MC_HISR_STACK_SIZE];
#endif
#endif

} T_MC_ENV_CTRL_BLK;

#ifndef _WINDOWS
  extern void mc_hisr (void);
#endif

/** External ref "global variables" structure. */
extern T_MC_ENV_CTRL_BLK *mc_env_ctrl_blk_p;

void mc_return_queue_init (T_RVF_G_ADDR_ID rq_addr_id, UINT16 rq_event,
                        T_RV_RETURN_PATH * path_to_mc_return_queue_p);



#endif /* __MC_INST_I_H_ */

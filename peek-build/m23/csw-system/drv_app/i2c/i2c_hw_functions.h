/**
* @file i2c_hw_functions.h
*
* @author ICT Embedded B.V.
* @version 1.0
*/

/*
* History:
*
* Date        Author          Modification
* -------------------------------------------------------------------
* 
* (C) Copyright 2003 by ICT Embedded B.V., All Rights Reserved
*/

#ifndef __I2C_HW_FUNCTIONS_H_
#define __I2C_HW_FUNCTIONS_H_

#include "i2c/i2c_api.h"
#ifndef _WINDOWS
#include "memif/mem.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
  
  
  
/**
* @name Types and Definitions
  */
  /*@{*/
  
  /* Defines the reset state of the I2C device.*/
  typedef UINT16   T_I2C_ENABLE;
  
  /*T_I2C_ENABLE can have the following values: */
#define I2C_DISABLE   (0)
#define I2C_ENABLE    (1)
#define I2C_RESET     (2)
  
  /*Defines the events for which the client can enable the interrupt generation.*/
  typedef UINT16   T_I2C_INTERRUPT;
  
  /*T_I2C_INTERRUPT is a bit-wise logical OR of the following values:*/
#define I2C_GENERAL_CALL    (0x0020)
#define I2C_TX_READY        (0x0010)
#define I2C_RX_READY        (0x0008)
#define I2C_REG_READY       (0x0004)
#define I2C_NACK            (0x0002)
#define I2C_ARB_LOST        (0x0001)
#define I2C_NONE            (0x0000)
  
  /*Defines the status of the I2C device.*/
  typedef UINT16   T_I2C_STATUS;
  
  /*T_I2C_STATUS is a bit wise logical OR of the following values:*/
#define I2C_SINGLE_BYTE         (0x8000)
#define I2C_BUS_BUSY            (0x1000)
#define I2C_RECEIVE_OVERRUN     (0x0800)
#define I2C_TRANSMIT_UNDERFLOW  (0x0400)
#define I2C_ADDRESS_AS_SLAVE    (0x0200)
#define I2C_ADDRESS_ZERO        (0x0100)
  
  /*Defines the DMA mode for transmit and receive.*/
  typedef UINT16   T_I2C_DMA_MODE;
  
  /*T_I2C_DMA_MODE is a bit wise logical OR of the following values:*/
#define I2C_DMA_RECEIVE_ENABLE    (0x8000)
#define I2C_DMA_TRANSMIT_ENABLE   (0x0080)
#define I2C_DMA_DISABLE           (0x0000)
  
  /*Defines some general configuration details for the I2C device.*/
  typedef UINT16 T_I2C_CONFIG;
  
  /*T_I2C_CONFIG is a bit wise logical OR of the following values:*/
#define I2C_ENDIAN              (0x4000)
#define I2C_START_BYTE_MODE     (0x0800)
#define I2C_MASTER_MODE         (0x0400)
#define I2C_TRANSMIT_MODE       (0x0200)
#define I2C_EXPANDED_ADDRESS    (0x0100)
  
  
  /*@}*/
  
  /** 
  * Type definition for interrupt callback function 
  */
  typedef void (*T_I2C_INT_CALLBCK) (void);
  
  
  
  /* Macro to access registers */
#define I2C_REG(A)                     (*(volatile UINT16 *)(A))  
  
  /* I2C Register memory offset */
#define BASE_MEM_I2C     0xFFFF2800
  
  /* Register map */
#define I2C_REV_REG       I2C_REG(BASE_MEM_I2C + 0x00) /* Module Revision Register */
#define I2C_IE_REG        I2C_REG(BASE_MEM_I2C + 0x02) /* Interrupt Enable Register */
#define I2C_STAT_REG      I2C_REG(BASE_MEM_I2C + 0x04) /* I2C Status Register */
#define I2C_SYSS_REG      I2C_REG(BASE_MEM_I2C + 0x08) /* System Status Register */
#define I2C_BUF_REG       I2C_REG(BASE_MEM_I2C + 0x0A) /* Buffer Configuration Register */
#define I2C_CNT_REG       I2C_REG(BASE_MEM_I2C + 0x0C) /* Data Counter Register */
#define I2C_DATA_REG      I2C_REG(BASE_MEM_I2C + 0x0E) /* Data Access Register */
#define I2C_SYSC_REG      I2C_REG(BASE_MEM_I2C + 0x10) /* System Configuration Register */
#define I2C_CON_REG       I2C_REG(BASE_MEM_I2C + 0x12) /* I2C Configuration Register */
#define I2C_OA_REG        I2C_REG(BASE_MEM_I2C + 0x14) /* I2C Own address Register */
#define I2C_SA_REG        I2C_REG(BASE_MEM_I2C + 0x16) /* I2C Slave Address Register */
#define I2C_PSC_REG       I2C_REG(BASE_MEM_I2C + 0x18) /* I2C Clock Prescaler Register */
#define I2C_SCLL_REG      I2C_REG(BASE_MEM_I2C + 0x1A) /* I2C SCL Low Time Register */
#define I2C_SCLH_REG      I2C_REG(BASE_MEM_I2C + 0x1C) /* I2C SCL High Time Register */
#define I2C_SYSTEST_REG   I2C_REG(BASE_MEM_I2C + 0x1E) /* System Test register */
  
  
  /* I2C_CON_REG bit fields */
#define I2C_EN_BIT      BIT15
#define BE_BIT          BIT14
#define STB_BIT         BIT11
#define MST_BIT         BIT10
#define TRX_BIT         BIT9
#define XA_BIT          BIT8
#define STP_BIT         BIT2
#define STT_BIT         BIT1
  
  /* I2C_SYSC_REG bit fields */
#define SRST_BIT        BIT1
  
  /* I2C_SYSTEST_REG bit fields */
#define ST_EN_BIT       BIT15
#define FREE_BIT        BIT14
#define TMODE_BIT13     BIT13
#define TMODE_BIT12     BIT12
#define SSB_BIT         BIT11
#define SCL_I_BIT       BIT3
#define SCL_O_BIT       BIT2
#define SDA_I_BIT       BIT1
#define SDA_O_BIT       BIT0
  
  /** I2C events */
#define I2C_EVENT_NACC_RECEIVED (0x0200)  
#define I2C_EVENT_ISR_READY (0x0100)  
  
  /*
  *  For function descriptions see i2c_api.h
  */
  void i2c_hw_enable(T_I2C_ENABLE enable_state);
  void i2c_hw_set_interrupt_enable(T_I2C_INTERRUPT interrupts_state);
  void i2c_hw_get_status(T_I2C_STATUS *status_p);
  void i2c_hw_set_status(T_I2C_STATUS status);
  void i2c_hw_set_dma_mode(T_I2C_DMA_MODE dma_mode);
  void i2c_hw_set_nmb_of_bytes(UINT16 nmb_bytes);
  void i2c_hw_get_nmb_bytes_left(UINT16 *nmb_bytes_left_p);
  void i2c_hw_read_data(UINT16 *received_data_p);
  void i2c_hw_write_data( UINT16 *transmit_data_p);
  void i2c_hw_set_config_data( T_I2C_CONFIG *config_data_p);
  void i2c_hw_get_config_data(T_I2C_CONFIG *configdata_p);
  void i2c_hw_set_own_address( UINT16 own_Address);
  void i2c_hw_set_target_address(UINT16 target_Address);
  void i2c_hw_set_prescaler(UINT8 prescale_value);
  void i2c_hw_set_clock_time(UINT8 high_value, UINT8 low_value);
  UINT8 i2c_hw_get_hw_version(void);
  void i2c_hw_set_system_test(T_I2C_FREERUN   freerun_mode,
    T_I2C_INT_SET   int_status,
    T_I2C_TEST      test_mode,
    UINT8           scl_value,
    UINT8           sda_value);
  void i2c_hw_get_test_data(UINT8 *scl_value_p,
    UINT8 *sda_value_p);
  
  void i2c_hw_set_interrupt_callback(T_I2C_INT_CALLBCK  i2c_int_callbck_p);
  
  /* Interrupt handling functions */
  void i2c_hw_int_manager(void);
  void i2c_hw_initialize_i2c(void);
  void i2c_hw_start(void);
  
#ifdef __cplusplus
}
#endif


#endif /* __I2C_TASK_H_ */

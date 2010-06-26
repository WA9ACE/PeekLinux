/**
* @file i2c_message.h
*
* Data structures:
* 1) used to send messages to the I2C SWE,
* 2) I2C can receive.
*
* @author Remco Hiemstra (remco.hiemstra@ict.nl)
* @version 0.1
*/

/*
* History:
*
* Date        Author          Modification
* -------------------------------------------------------------------
* 12/30/2003  Remco Hiemstra (remco.hiemstra@ict.nl)    Create.
*
* (C) Copyright 2003 by ICT Automatisering, All Rights Reserved
*/

#ifndef __I2C_MESSAGE_H_
#define __I2C_MESSAGE_H_


#include "rv/rv_general.h"

#include "i2c/i2c_cfg.h"
#include "i2c/i2c_api.h"

#ifdef __cplusplus
extern "C"
{
#endif
  
  
/** 
* The message offset must differ for each SWE in order to have 
* unique msg_id in the system.
  */
#define I2C_MESSAGE_OFFSET   BUILD_MESSAGE_OFFSET(I2C_USE_ID)
  
  
  
  /**
  * @name I2C_SAMPLE_MESSAGE
  *
  * Short description.
  *
  * Detailled description
  */
  /*@{*/
  /** Message ID. */
  //#define I2C_SAMPLE_MESSAGE (I2C_MESSAGE_OFFSET | 0x001)
  
  /** Message structure. */
  //typedef struct 
  //{
  /** Message header. */
  //  T_RV_HDR      hdr;
  
  /** Some parameters. */
  /* ... */
  
  //} T_I2C_SAMPLE_MESSAGE;
  /*@}*/
  
  
  /**
  * @name I2C_READ_REQ_MSG
  *
  * Detailled description
  * The I2C_READ_REQ_MSG is used to read data from an I2C address.
  * the correct response to this message will be the I2C_READ_RSP_MSG
  *
  */
  /*@{*/
  /** Message ID. */
#define I2C_READ_REQ_MSG (I2C_MESSAGE_OFFSET | 0x001)
  
  /** Message structure. */
  typedef struct 
  {
    T_RV_HDR     os_hdr;
    UINT16       address;
    UINT16       *read_buffer_p;
    UINT16       nmb_of_bytes;
    T_I2C_ENDIAN endian;
    T_RV_RETURN  return_path;
  } T_I2C_READ_REQ_MSG;
  /*@}*/
  
  
  /**
  * @name I2C_READ_RSP_MSG
  *
  * Detailled description
  * The I2C_READ_RSP_MSG is message wich returns the read data from the I2C address
  * The I2C_READ_RSP_MSG is the response of the I2C_READ_REQ_MSG.
  */
  /*@{*/
  /** Message ID. */
#define I2C_READ_RSP_MSG (I2C_MESSAGE_OFFSET | 0x002)
  
  /** Message structure. */
  typedef struct 
  {
    T_RV_HDR         os_hdr;
    T_I2C_RETURN     result;
  } T_I2C_READ_RSP_MSG;
  /*@}*/
  
  
  /**
  * @name I2C_WRITE_REQ_MSG
  *
  * Detailled description
  * The I2C_WRITE_REQ_MSG is used to write data to an I2C address.
  * the correct response to this message will be the I2C_WRITE_RSP_MSG
  *
  */
  /*@{*/
  /** Message ID. */
#define I2C_WRITE_REQ_MSG (I2C_MESSAGE_OFFSET | 0x003)
  
  /** Message structure. */
  typedef struct 
  {
    
    T_RV_HDR     os_hdr;
    UINT16       address;
    UINT16      *write_buffer_p;
    UINT16       nmb_of_bytes;
    T_I2C_ENDIAN endian;
    T_RV_RETURN  return_path;
  } T_I2C_WRITE_REQ_MSG;
  /*@}*/
  
  
  /**
  * @name I2C_WRITE_RSP_MSG
  *
  * Detailled description
  * The I2C_WRITE_RSP_MSG is message wich returns if the data is correct written or not.
  * The I2C_WRITE_RSP_MSG is the response of the I2C_WRITE_REQ_MSG.
  *
  */
  /*@{*/
  /** Message ID. */
#define I2C_WRITE_RSP_MSG (I2C_MESSAGE_OFFSET | 0x004)
  
  /** Message structure. */
  typedef struct 
  {
    T_RV_HDR     os_hdr;
    T_I2C_RETURN result;
  } T_I2C_WRITE_RSP_MSG;
  /*@}*/
  
  
  /**
  * @name I2C_TRANSFER_MODE_REQ_MSG
  *
  * Detailled description
  * The I2C_TRANSFER_MODE_REQ_MSG is used to set the way the data is transfered internally
  * This can be done on polling base, on interrupt base or on dma base (not implemented)
  * If the transfer mode is correctl response will follow in the I2C_WRITE_RSP_MSG
  *
  */
  /*@{*/
  /** Message ID. */
#define I2C_TRANSFER_MODE_REQ_MSG (I2C_MESSAGE_OFFSET | 0x005)
  
  /** Message structure. */
  typedef struct 
  {
    /** Message header. */
    T_RV_HDR    os_hdr;
    T_I2C_TRANFER_MODE transfer_mode;
    T_RV_RETURN return_path;
  } T_I2C_TRANSFER_MODE_REQ_MSG;
  /*@}*/
  
  /**
  * @name I2C_TRANSFER_MODE_RSP_MSG
  *
  * Detailled description
  * The I2C_TRANSFER_MODE_RSP_MSG is message wich returns if the transfermode is correctly set or not.
  * The I2C_TRANSFER_MODE_RSP_MSG is the response of the I2C_TRANSFER_MODE_REQ_MSG.
  */
  /*@{*/
  /** Message ID. */
#define I2C_TRANSFER_MODE_RSP_MSG (I2C_MESSAGE_OFFSET | 0x006)
  
  /** Message structure. */
  typedef struct 
  {
    T_RV_HDR        os_hdr;
    T_I2C_RETURN    result;   
  } T_I2C_TRANSFER_MODE_RSP_MSG;
  /*@}*/
  
  
#ifdef __cplusplus
}
#endif

#endif /* __I2C_MESSAGE_H_ */

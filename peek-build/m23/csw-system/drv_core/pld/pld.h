/**********************************************************************************************/
/*            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION                          */
/*                                                                                            */
/*   Property of Texas Instruments -- For  Unrestricted  Internal  Use  Only                  */
/*   Unauthorized reproduction and/or distribution is strictly prohibited.  This              */
/*   product  is  protected  under  copyright  law  and  trade  secret law as an              */
/*   unpublished work.  Created 1987, (C) Copyright 1997 Texas Instruments.  All              */
/*   rights reserved.                                                                         */
/*                                                                                            */
/*                                                                                            */
/*   Filename          : pld.h                                                                */
/*                                                                                            */
/*   Description       : marcro functions to drive the Uppcosto device throught the FPGA PLD. */
/*                       The Serial Port Interface is used to connect the TI                  */
/*                       PLD (PLD).                                                           */
/*                       It is assumed that the PLD is connected as the SPI                   */
/*                       device 1 and the same config as is applied to the SPI for PLD as ABB */
/*                                                                                            */
/*   Author            : Francois Mazard                                                      */
/*                                                                                            */
/*   Version number   : 1.0                                                                   */
/*                                                                                            */
/*   Date and time    : Sep 2004                                                              */
/*                                                                                            */
/*   Previous delta   : Creation                                                              */
/*                                                                                            */
/**********************************************************************************************/

#ifndef __PLD_H__
  #define __PLD_H__

  #include "sys_types.h"

  #include "spi/spi_drv.h"

    // MACROS
  #define PLD_WRITE_REG(reg, data) { \
      SPI_WRITE_TX_LSB(0xBFFF & (reg & 0x3FFF)) \
      SPI_WRITE_TX_MSB(data) \
      SPI_START_WRITE }

  #define PLD_READ_REG(reg) { \
      SPI_WRITE_TX_LSB(0x4000 | (reg & 0x3FFF)) \
      SPI_WRITE_TX_MSB(0x0000) \
      SPI_START_READ }

  #define PLD_SPI_CONFIGURATION() { \
      T_SPI_DEV *pld \
      T_SPI_DEV init_spi_device \
      pld = &init_spi_device \
      pld->PrescVal         = SPI_CLOCK_DIV_1 \
      pld->DataTrLength   = SPI_WNB_31 \
      pld->DevAddLength  = 5 \
      pld->DevId              = SPI_DEV1 \
      pld->ClkEdge           = SPI_CLK_EDG_RISE \
      pld->TspEnLevel       = SPI_NTSPEN_NEG_LEV \
      pld->TspEnForm        = SPI_NTSPEN_LEV_TRIG \
      SPI_InitDev(pld) }

  #define PLD_CHANGE_DEVICE_PLD_TO_ABB()  \
      * (volatile SYS_UWORD16 *) SPI_REG_CTRL = (SPI_WNB_15 | SPI_DEV0)

  #define PLD_CHANGE_DEVICE_ABB_TO_PLD()  \
      * (volatile SYS_UWORD16 *) SPI_REG_CTRL = (SPI_WNB_31 | SPI_DEV1)

  #define EIGHT_CYCLES_13M_NS 616

  // Uppcosto Address
  #define PLD_UPPCOSTO_TX_BUFFER_ADDRESS  0x1E10


  /* Function Prototype Declaration */
  void PLD_Write_Uplink_Data(SYS_UWORD16* );
  void PLD_WriteRegister(SYS_UWORD16 , SYS_UWORD16 );
  SYS_UWORD16 PLD_ReadRegister(SYS_UWORD16 );
#endif  // __PLD_H__

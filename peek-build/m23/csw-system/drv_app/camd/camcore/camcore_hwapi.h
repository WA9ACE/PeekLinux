/******************************************************************************/
/*                                                                          	*/
/*  Name        camcore_hwapi.h	                                              */
/*                                                                          	*/
/*  Function    this file contains parameters related to Camera controller		*/
/* 			CC stands for CAMERA CORE.																						*/
/*                  	THIS FILE SHOULD NOT BE CHANGED FOR DIFFERENT PLATFORMS */
/*                                                                          	*/
/*  Date       Modification                                                 	*/
/*  -----------------------                                                 	*/
/*  28-Feb-2004    Create     Sumit                                           */
/*  27-Jun-2005								Venugopal Naik																	*/
/*                                                                          	*/
/******************************************************************************/


#ifndef CAMCORE_HWAPI_H
#define CAMCORE_HWAPI_H

#include "camcore_hwreg.h"

/* returns: 1 - Reset completed */
/* 		   0 - Internal Module reset is on-going */
#define CAMCORE_ISRESETDONE (CAMCORE_REG(CC_SYSSTATUS));

/* #define camcore_enableInterrupts( MASK ) {CAMCORE_REG(CC_IRQENABLE) = 0X0000001F; */
#define camcore_enableDMA (CAMCORE_REG(CC_CTRL_DMA) = ( CAMCORE_REG(CC_CTRL_DMA) | 0X00000100 ))
#define camcore_disableDMA (CAMCORE_REG(CC_CTRL_DMA) = ( CAMCORE_REG(CC_CTRL_DMA) & 0XFFFFFEFF ))

/* Actual FIFO size would be: 2^CC_FIFO_DEPTH */
#define CC_FIFO_DEPTH  (CAMCORE_REG(CC_GENPAR)&0x00000007)


/*USAGE: data = FIFOREAD; */
/* 		  FIFOWRITE(data); */
#define FIFOREAD ( CAMCORE_REG(CC_FIFODATA))
#define FIFOWRITE(DATA) ( CAMCORE_REG(CC_FIFODATA) = DATA )
#define CAMCORE_FIFO_TEST 7
#define FIFO_LEVEL ((CAMCORE_REG(CC_TEST) >> 8) & 0xFF)
#define CAMCORE_MODE(MODE) { CAMCORE_REG(CC_CTRL) &= ~CC_CTRL_PAR_MODE; \
  CAMCORE_REG(CC_CTRL) |= ((MODE << 1) & CC_CTRL_PAR_MODE); }


#endif /*  #ifndef CAMCORE_HWAPI_H */

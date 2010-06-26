/********************************************************************************/
/*                                                                          		*/
/*  Name        camcore_hwreg.h                                                 */
/*                                                                          		*/
/*  Function    this file contains parameters related to Camera controller			*/
/* 			CC stands for CAMERA CORE.																							*/
/*                  	THIS FILE SHOULD NOT BE CHANGED FOR DIFFERENT PLATFORMS   */
/*                                                                          		*/
/*  Date       Modification                                                 		*/
/*  -----------------------                                                 		*/
/*  28-Feb-2004    Create     Sumit                                             */
/*  27-Jun-2005								Venugopal Naik																		*/
/*                                                                          		*/
/********************************************************************************/ 

#ifndef CAMCORE_HWREG_H
#define CAMCORE_HWREG_H

#include "camcore_configLocostoHW.h"


#define CC_REVISION 	(0 + CC_BASE_ADDR)			/* Revision Register */
#define CC_SYSCONFIG 	(0x10 + CC_BASE_ADDR)		/* System configuration Register */
#define CC_SYSSTATUS 	(0x14 + CC_BASE_ADDR)		/* System status register */
#define CC_IRQSTATUS 	(0x18 + CC_BASE_ADDR)		/* Interrupt Status register */
#define CC_IRQENABLE 	(0x1C + CC_BASE_ADDR)		/* Interrupt Enable register */
	
#define CC_CTRL 			(0x40 + CC_BASE_ADDR)			/* Control Register */
#define CC_CTRL_DMA 	(0x44 + CC_BASE_ADDR)		/* Control DMA register */
#define CC_CTRL_XCLK 	(0x48 + CC_BASE_ADDR)		/* External Clock register */
#define CC_FIFODATA 	(0x4C + CC_BASE_ADDR)		/* FIFO data register */		
#define CC_TEST 			(0x50 + CC_BASE_ADDR)			/* Test register */		
#define CC_GENPAR 		(0x54 + CC_BASE_ADDR)			/* Generic Parameters */		

/* serial CCP registers, will not be used in this case */
#define CC_CCPFSCR 		(0x58 + CC_BASE_ADDR)		/* CCP Frame Start code register */		
#define CC_CCPFECR 		(0x5C + CC_BASE_ADDR)		/* CCP Frame End code register */
#define CC_CCPLSCR 		(0x60 + CC_BASE_ADDR)		/* CCP Line Start Code register */
#define CC_CCPLECR 		(0x64 + CC_BASE_ADDR)		/* CCP Line End Code register */
#define CC_CCPDFR 		(0x68 + CC_BASE_ADDR)			/* CCP Data Format Select register */



#define CAMCORE_REG(CC_XXX_REG) (*((volatile UINT32 *)(CC_XXX_REG)))




/* following lines gives the details for fields present in different registers */

#define CC_REVISION_VAL 				0X000000FF

#define CC_SYSCONFIG_SIDLEMODE 	0X00000018
#define CC_SYSCONFIG_SOFTRESET 	0X00000002
#define CC_SYSCONFIG_AUTOIDLE   0X00000001


/* Register Fields */

/* Interrupt Mask - CC_IRQENABLE  register value */

#define CC_IRQENABLE_OF 				0x00000002
#define CC_IRQENABLE_UF 				0x00000001

/* Camera Core CONTROL register: CC_CTRL : */
#define CC_CTRL_CCP_MODE 				1b
#define CC_CTRL_PAR_MODE 				0x0000000E
#define CC_CTRL_RESV 						0
#define CC_CTRL_NOBT_VS_POL 		0x00000100
#define CC_CTRL_NOBT_HS_POL 		0x00000200
#define CC_CTRL_PAR_CLK_POL 		0x00000400
#define CC_CTRL_PAR_ORDER_CAM 	0x00000800
#define CC_CTRL_BT_CORRECT 			0x00001000
#define CC_CTRL_NOBT_SYNCHRO 		0x00002000
#define CC_CTRL_RESV_2 					0
#define CC_CTRL_CC_EN 					0x00010000
#define CC_CTRL_CC_FRAME_TIG 		0x00020000
#define CC_CTRL_RST 						0x00040000
#define CC_CTRL_IF_SYNCHRO 			0x00080000
#define CC_CTRL_ONE_SHOT 				0x00100000
#define CC_CTRL_RESV_3 					0


#endif /*  #ifndef CAMCORE_HWREG_H */


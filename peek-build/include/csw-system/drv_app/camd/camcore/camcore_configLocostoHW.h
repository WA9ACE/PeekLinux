/********************************************************************************/
/*                                                                          		*/
/*  Name        camcore_configLocostoHW.h                                   		*/
/*                                                                          		*/
/*  Function    this file contains parameters related to Locosto and Camera 		*/
/*   						controller          																						*/
/*                                                                         			*/
/*  Date       Modification                                                			*/
/*  -----------------------                                                			*/
/*  28-Feb-2004    Create     Sumit                                        			*/
/*  27-Jun-2005								Venugopal Naik																		*/
/*                                                                         			*/
/********************************************************************************/ 


#ifndef CAMCORE_CONFIGLOCOSTOHW_H
#define CAMCORE_CONFIGLOCOSTOHW_H


#define CAMCORE_BASE_ADDR 0x09700000

#define CC_BASE_ADDR CAMCORE_BASE_ADDR


/* FIFO size is 128-32-bit word for Locosto Camera controller */
#define CAMCORE_FIFOSIZE 128 


/*#define CONF_REG_BASE 0xFFFEF100
#define	CONF_REG(CONF_XXX_REG) (*((volatile UINT16 *)(CONF_XXX_REG)))*/


/* CONF REGISTERS USED BY CAMERA MODULE */

/*#define CONF_GPIO_0	CONF_REG(CONF_REG_BASE + 0x00)
#define CONF_GPIO_19	CONF_REG(CONF_REG_BASE + 0x6A)
#define CONF_GPIO_20	CONF_REG(CONF_REG_BASE + 0x6C)
#define CONF_GPIO_21	CONF_REG(CONF_REG_BASE + 0x6E)
#define CONF_GPIO_22	CONF_REG(CONF_REG_BASE + 0x70)
#define CONF_GPIO_28	CONF_REG(CONF_REG_BASE + 0x7C)
#define CONF_GPIO_29	CONF_REG(CONF_REG_BASE + 0x7E)
#define CONF_GPIO_30	CONF_REG(CONF_REG_BASE + 0x80)
#define CONF_ND_NWP	CONF_REG(CONF_REG_BASE + 0x84)
#define CONF_GPIO_47	CONF_REG(CONF_REG_BASE + 0xB6)
#define CONF_GPIO_7	CONF_REG(CONF_REG_BASE + 0xBA)

#define CONF_GPIO_4	CONF_REG(CONF_REG_BASE + 0x1A)*/



#endif /*  #ifndef GENERAL_H */


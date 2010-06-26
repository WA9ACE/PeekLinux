/********************************************************************************/
/*                                                                         			*/
/*  Name        camcore_api.c                                                   */
/*                                                                          		*/
/*  Function    this file contains parameters related to Locosto and 						*/
/*               Camera controller									           									*/
/*                                                                          		*/
/*  Date       Modification                                                 		*/
/*  -----------------------                                                 		*/
/*  28-Feb-2004    Create     Sumit                                             */
/*  27-May-2005               Venugopa Naik																			*/
/********************************************************************************/ 
#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "pin_config.h"

#include "camcore_api.h"
#include "camcore_hwapi.h"
void f_camera_interrupt_manager(void);
T_CAMCORE_RET CameraCore_Clear_FIFO(void);
void camcore_readReg(void );
#define CAMCORE_ENABLE_TRACE
#ifdef CAMCORE_ENABLE_TRACE
#define CAMCORE_SEND_TRACE(string, trace_level) \
    rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, CAMD_USE_ID)
#define CAMCORE_SEND_TRACE_PARAM(string, param, trace_level)	\
    rvf_send_trace (string, (sizeof(string) - 1), (UINT32)(param), trace_level, CAMD_USE_ID)
#else
#define CAMCORE_SEND_TRACE(string, trace_level) /* nothing */
#define CAMCORE_SEND_TRACE_PARAM(string, param, trace_level) /* nothing */
#endif


/********************************************************************************/
/*                                                                              */
/*    Function Name:   camcore_getRevision						                          */
/*                                                                              */
/*    Purpose:     Returns software version																			*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*             None.																														*/
/*    Return :                                                                  */
/*             Software version.															                  */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/
T_CAMCORE_RET camcore_getRevision(T_CAMCORE_REVISION *revision) {

   if (!revision) return CAMCORE_INVALID_PARAMS;

   revision->major_rev = revision->minor_rev = CAMCORE_REG(CC_REVISION);
   revision->major_rev = revision->major_rev>>4;
   revision->minor_rev = (revision->minor_rev & 0xF);

   return CAMCORE_OK;
}

/********************************************************************************/
/*                                                                            	*/
/*    Function Name:   camcore_reset                                		      	*/
/*                                                                              */
/*    Purpose:     Resets camera core.												                  */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - reset_type : reset type				                             		*/
/*    Return :                                                                  */
/*             - CAMCORE_OK : success                                           */
/*             - CAMCORE_NOT_READY : CAMA task not ready                        */
/*             - CAMCORE_INTERNAL_ERR :                                */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/

T_CAMCORE_RET camcore_reset(T_CAMCORE_RESET_TYPE reset_type) {
   int i;

   if (reset_type == CAMCORE_RESET_ALL) {
      CAMCORE_REG(CC_SYSCONFIG) = (CAMCORE_REG(CC_SYSCONFIG) | CC_SYSCONFIG_SOFTRESET);
      for(i=0;i<1000;i++);

      if (!CAMCORE_REG(CC_SYSSTATUS)) 
          return CAMCORE_INTERNAL_ERR;
   }
   else if (reset_type == CAMCORE_RESET_FSM) {

      /* Disable the core-sensor interface */
      CAMCORE_REG(CC_CTRL) &= ~CC_CTRL_CC_EN;

      /* Enable the FSM reset */
      CAMCORE_REG(CC_CTRL) |= CC_CTRL_RST;

      /* Some delay for FSM to reset*/
      for(i=0;i<1000;i++);

      /* Disable the FSM reset */
      CAMCORE_REG(CC_CTRL) &= ~CC_CTRL_RST;

   }
   return CAMCORE_OK;
}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camcore_config                         		              */
/*                                                                              */
/*    Purpose:     Configures the camera core.								                  */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - params : Configuration parameters.															*/
/*    Return :                                                                  */
/*             - CAMCORE_OK : success                                           */
/*             - CAMCORE_NOT_READY : CAMA task not ready                        */
/*             - CAMCORE_INTERNAL_ERR : 					                              */
/*                                                                              */
/*    Note:                                                                     */
/*        Currently implemented for																							*/
/*				1. VSYNCH off.																												*/	
/*				2. NoBT - PARALLEL 8 bit modeNone.                                    */
/*                                                                              */
/********************************************************************************/

T_CAMCORE_RET camcore_config( const T_CAMCORE_CONFIGPARAM * params) {

   INT8 delay;
   UINT8 hwfifo;

   /* Reset the FSM of CAMERA Core */
   CAMCORE_REG(CC_CTRL) = 0;


   /*reset*/
   CAMCORE_REG(CC_CTRL) = 0x00040000;

   /* TO Check if this is required */
   for(delay=0;delay<100;delay++);

   /* SET sysconfig autoidle to 1 to enable power saving circuitry */
   CAMCORE_REG(CC_SYSCONFIG) = (CAMCORE_REG(CC_SYSCONFIG) | CC_SYSCONFIG_AUTOIDLE);


   /* configure INTERRUPTS */
   /* Enable it for: Overflow and Underflow */
   /* TODO: Make this configurable as well */
   CAMCORE_REG(CC_IRQENABLE) = (CAMCORE_REG(CC_IRQENABLE) | CC_IRQENABLE_OF 
           | CC_IRQENABLE_UF);

   /*CAMCORE_REG(CC_IRQENABLE) = 0x0;*/
   hwfifo = 1U <<(CC_FIFO_DEPTH);
   /* configure FIFO and DMA */
   if (CAMCORE_FIFOSIZE != hwfifo) 
   {
       /* Error condition, FIFO size is inconsistence */
       return CAMCORE_INVALID_PARAMS;
   };
   /* enable DMA and set FIFO threshold */

   CAMCORE_REG(CC_CTRL_DMA) = (params->fifothreshold -1)& 0x0000007F;

   /* Disable second DMA */	
   CAMCORE_REG(CC_CTRL_DMA) = 	CAMCORE_REG(CC_CTRL_DMA) | 0x00000300;
   /* Set CCP and PAR mode */
   if(params->ccpmode)
       CAMCORE_REG(CC_CTRL) = CAMCORE_REG(CC_CTRL) | ((params->ccpmode-1) <<1);
   else
       CAMCORE_REG(CC_CTRL) = CAMCORE_REG(CC_CTRL) | params->ccpmode;

   CAMCORE_REG(CC_CTRL) = CAMCORE_REG(CC_CTRL) | 0x00000800;

   /* Set clock divider */
   CAMCORE_REG(CC_CTRL_XCLK) = params->xclk_div;

   /*		
            CAMCORE_SEND_TRACE_PARAM("Revision Register",CAMCORE_REG(CC_REVISION), RV_TRACE_LEVEL_ERROR);
            CAMCORE_SEND_TRACE_PARAM("System configuration Register ",CAMCORE_REG(CC_SYSCONFIG), RV_TRACE_LEVEL_ERROR);
            CAMCORE_SEND_TRACE_PARAM("System status register",CAMCORE_REG(CC_SYSSTATUS), RV_TRACE_LEVEL_ERROR);

            CAMCORE_SEND_TRACE_PARAM("Interrupt Status register",CAMCORE_REG(CC_IRQSTATUS), RV_TRACE_LEVEL_ERROR);
            CAMCORE_SEND_TRACE_PARAM("Interrupt Enable register",CAMCORE_REG(CC_IRQENABLE), RV_TRACE_LEVEL_ERROR);
            CAMCORE_SEND_TRACE_PARAM("control register",CAMCORE_REG(CC_CTRL), RV_TRACE_LEVEL_ERROR);

            CAMCORE_SEND_TRACE_PARAM("Control DMA register",CAMCORE_REG(CC_CTRL_DMA), RV_TRACE_LEVEL_ERROR);
            CAMCORE_SEND_TRACE_PARAM("External Clock register",CAMCORE_REG(CC_CTRL_XCLK), RV_TRACE_LEVEL_ERROR);
            CAMCORE_SEND_TRACE_PARAM("Test register",CAMCORE_REG(CC_TEST), RV_TRACE_LEVEL_ERROR);

            CAMCORE_SEND_TRACE_PARAM("Generic Parameters",CAMCORE_REG(CC_GENPAR), RV_TRACE_LEVEL_ERROR);
            */		

   return CAMCORE_OK;
}

void camcore_readReg(void )
{
    CAMCORE_SEND_TRACE_PARAM("Test register",CAMCORE_REG(CC_TEST), RV_TRACE_LEVEL_ERROR);
    CAMCORE_SEND_TRACE_PARAM("FIFO value",CAMCORE_REG(CC_FIFODATA), RV_TRACE_LEVEL_ERROR);

}
/********************************************************************************/
/*                                                                              */
/*    Function Name:   camcore_enable                                       		*/
/*                                                                              */
/*    Purpose:     Enables camera core.																					*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - None.																													*/
/*    Return :                                                                  */
/*             - CAMCORE_OK : success                                           */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/

T_CAMCORE_RET camcore_enable(void){

   CAMCORE_REG(CC_CTRL) = CAMCORE_REG(CC_CTRL) | CC_CTRL_CC_EN;		

   return CAMCORE_OK;

}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   camcore_disable                                       		*/
/*                                                                              */
/*    Purpose:     Disables camera core.																				*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - None.																													*/
/*    Return :                                                                  */
/*             - CAMCORE_OK : success                                           */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/


T_CAMCORE_RET camcore_disable(void) {

   CAMCORE_REG(CC_CTRL) = CAMCORE_REG(CC_CTRL) & ~CC_CTRL_CC_EN;

   return CAMCORE_OK;
}


/********************************************************************************/
/*                                                                              */
/*    Function Name:   camcore_setmode                                       		*/
/*                                                                              */
/*    Purpose:     Sets camera core in either snapshot or viewfinder mode.			*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - mode : mode                             												*/
/*    Return :                                                                  */
/*             - CAMCORE_OK : success                                           */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/


T_CAMCORE_RET camcore_setmode(T_CAMCORE_MODE mode) {

   if (mode == CAMCORE_SNAPSHOT)
       CAMCORE_REG(CC_CTRL) = CAMCORE_REG(CC_CTRL) | CC_CTRL_ONE_SHOT | CC_CTRL_CC_EN;		
   else 
       CAMCORE_REG(CC_CTRL) = CAMCORE_REG(CC_CTRL) &  ~CC_CTRL_ONE_SHOT;

   return CAMCORE_OK;


}
/********************************************************************************/
/*                                                                              */
/*    Function Name:   camcore_setGpioPins                                      */
/*                                                            	                */
/*    Purpose:     Sets GPIO pins for camera.									                  */
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - None																														*/
/*             - rp : return path                                             	*/
/*    Return :                                                                  */
/*             - None.																													*/
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/

void camcore_setGpioPins(void) {

   /* 10 => cam_d_1 : Camera digital image data bit 1 [I] */
   CONF_GPIO_0  &=  ((~0x3) | 0x02); /* 10b */

   /* 010 => camd_hs : Camera digital horizantal sync [I] */
   CONF_GPIO_19 &=  ((~0x7) | 0x01); /* 001b */

   /* 010 => cam_d_3 : Camera digital image data bit 3 [I] */
   CONF_GPIO_20 &=  ((~0x7) | 0x02); /* 010b */

   /*01 => cam_lclk : Camera image data latch clock [I]*/
   CONF_GPIO_21 &=   ((~0x3) | 0x01); /* 01b */

   /* 001 => cam_xclk : Camera Output Clock [O] */
   CONF_GPIO_22 &=   ((~0x7) | 0x01); /* 001b */

   /* 11 => cam_d_7 : Camera digital image data bit 7 [I]*/
   CONF_GPIO_28 |=   0x03; /* 11b */

   /* 11 => cam_d_6 : Camera digital image data bit 6 [I] */
   CONF_GPIO_29 |=   0x03; /* 11b */

   /* 11 => cam_d_5 : Camera digital image data bit 5 [I] */
   CONF_GPIO_30 |=   0x03; /* 11b */

   /* 10 => cam_d_4 : Camera digital image data bit 4 [I] */
   CONF_ND_NWP  &=  ((~0x3) | 0x03); 

   /* 10 => cam_d_0 : Camera digital image data bit 0 [I]*/
   CONF_GPIO_47 &=   ((~0x3) | 0x02); /* 10b */

   /* 101 => cam_d_2 : Camera digital image data bit 2 [I] */
   CONF_GPIO_7  &=   ((~0x7) | 0x05); /* 101b */

   CONF_GPIO_4  &=  (~0x03); /* 11b */

}	


T_CAMCORE_RET CameraCore_Clear_FIFO(void) {

   UINT32 current_mode, i, remaining_words,words;
   current_mode = (CAMCORE_REG(CC_CTRL)& CC_CTRL_PAR_MODE);

   /* Set the protocol mode of camera module in test mode */
   CAMCORE_MODE(CAMCORE_FIFO_TEST);

   remaining_words = FIFO_LEVEL;
   /* Clear the FIFO by reading the remaining words */
   for (i=0; i<remaining_words; i++)
   {
       words =FIFOREAD;
   }
   /* Set the protocol mode of camera module back to its original mode */
   CAMCORE_MODE(current_mode);
   /*CAMCORE_SEND_TRACE_PARAM("FIFO cleared these many bytes",remaining_words,RV_TRACE_LEVEL_DEBUG_HIGH);*/
   return (CAMCORE_OK);  
}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   f_camera_interrupt_manager                               */
/*                                                                              */
/*    Purpose:     Handles camera core interrupts.															*/
/*                                                                              */
/*    Input Parameters:                                                         */
/*             - None.																													*/
/*    Return :                                                                  */
/*             - None.																													*/
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/********************************************************************************/

UINT8 Overflow_flag =0;
UINT8 Underflow_flag = 0;
UINT8 Overflowcount=0;


void f_camera_interrupt_manager(void) {

   UINT32 reg;

   reg = CAMCORE_REG(CC_IRQSTATUS);

   /* An underflow interrupt */
   if (CC_IRQENABLE_UF & reg) 
   {
       /* clear the interrupt */
       CAMCORE_REG(CC_IRQSTATUS) = ( CAMCORE_REG(CC_IRQSTATUS) | CC_IRQENABLE_UF);
       Underflow_flag++;
       /*camcore_reset(1);*/
   } 
   /* An overflow interrupt */
   if (CC_IRQENABLE_OF & reg) 
   {
       /* clear the interrupt */
       CAMCORE_REG(CC_IRQSTATUS) = ( CAMCORE_REG(CC_IRQSTATUS) | CC_IRQENABLE_OF);
       /*camcore_reset(1);*/

       Overflow_flag++;
       Overflowcount++;
   } 
}


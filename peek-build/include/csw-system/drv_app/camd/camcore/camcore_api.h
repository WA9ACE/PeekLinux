/********************************************************************************/
/*                                                                          		*/
/*  Name        camcore_api.h                                                   */
/*                                                                          		*/
/*  Function    this file contains parameters related to Locosto and Camera 		*/
/*							controller   																						        */
/*                                                                          		*/
/*  Date       Modification                                                 		*/
/*  -----------------------                                                 		*/
/*  28-Feb-2004    Create     Sumit                                             */
/*                                                                          		*/
/********************************************************************************/ 


#ifndef CAMCORE_API_H
#define CAMCORE_API_H

#include "general.h"

/* Camera core mode*/
typedef enum {
    CAMCORE_SNAPSHOT = 0,
    CAMCORE_VIEWFINDER
} T_CAMCORE_MODE;

/* CCP serial sensor interface or the parallel interface */
typedef enum {
		CAMCORE_CCP_EN = 0,
    CAMCORE_CCP_PARNOBT_8,
    CAMCORE_CCP_PARNOBT_10,
    CAMCORE_CCP_PARNOBT_12,
    CAMCORE_CCP_RESV_1,
    CAMCORE_CCP_PARBT_8,
    CAMCORE_CCP_PARBT_10,
    CAMCORE_CCP_RESV_2,
    CAMCORE_CCP_FIFO_TEST
    
} T_CAMCORE_CCPMODE; 

/* CAMCORE return values */
typedef enum {
	CAMCORE_OK = 0,
	CAMCORE_NOT_SUPPORTED = -2,
	CAMCORE_INTERNAL_ERR = -9,
	CAMCORE_INVALID_PARAMS = -10
} T_CAMCORE_RET;

typedef enum {
	CAMCORE_RESET_ALL = 0,
	CAMCORE_RESET_FSM
}T_CAMCORE_RESET_TYPE;

typedef UINT16 T_CAMCORE_FIFOSIZE;
typedef UINT16 T_CAMCORE_THRESHOLD; 



typedef struct
{
    T_CAMCORE_MODE mode;
    T_CAMCORE_THRESHOLD fifothreshold;
		/*if vsynch = 0, VSYNCH is not available, which is the case for Locosto */
		BOOL vsynch; 
    T_CAMCORE_CCPMODE ccpmode;	
    UINT16 xclk_div; /* CAM_XCLK = CAM_MCLK/ xclk_div */
		
} T_CAMCORE_CONFIGPARAM;


typedef struct
{
	INT8 major_rev;
	INT8 minor_rev;
}T_CAMCORE_REVISION;






/* Description: The API would configure the camera core based on the desired input
		parameters. 
  * The input parameter would typically depend on: external sensor, chip-clock 
  	requirements etc..
  * Camera needs to be enabled only after it is configured.

  * INPUT Paramerters: 
  * -----------------
  * T_CAMCORE_MODE: Camera in viewfinder or snapshot mode.
  * T_CAMCORE_FIFOSIZE: Software is not allowed to change this value. FIFO size is 
  	fixed for camera core.
  * T_CAMCORE_THRESHOLD: size of fifothreshold; software should configure this 
  	value for optimal data flow.
  * vsynch: VSYNCH is not available, which is the case for Locosto.
  * T_CAMCORE_CCPMODE: Identified the mode of the data flow. NoBT, BT656..etc..
  * xclk_div: CAM_XCLK = CAM_MCLK/ xclk_div. IF this value is -1, then sensor is 
  	supplied with its own clock.
  *

  * RETURN VALUES:
  * ----------------
  * If the configuration is InValid.
  */
T_CAMCORE_RET camcore_config(const T_CAMCORE_CONFIGPARAM * params);


/* Description: The API would enable the camera core.
  * This function needs to be called only after camera is configured.
  *
  */
T_CAMCORE_RET camcore_enable(void);


/* Description: The API would disable the camera core.
  *
  */
T_CAMCORE_RET camcore_disable(void);

T_CAMCORE_RET camcore_getRevision(T_CAMCORE_REVISION *revision);


/* Description: The API would reset the camera core depending on the 'reset_type' 
		required.
  * INPUT parameter: 
  * CAMCORE_RESET_ALL: The API would reset the whole camera core, 
  	camcore_config needs to be called after this.
  *	CAMCORE_RESET_FSM: Resets all the internal finite state machines of the camera 
  *	core module.
  * Must be applied when CC_EN = 0.
  * Configuration settings will not be altered here. <TOBE VERIFIED BY HARDWARE TEAM>
  */

T_CAMCORE_RET camcore_reset(T_CAMCORE_RESET_TYPE reset_type);

/*
	 Sets GPIO pins for camera configuration. 
*/
void camcore_setGpioPins(void);

/* 
	 Set camera in snaphot or viewfinder mode 
*/
T_CAMCORE_RET camcore_setmode(T_CAMCORE_MODE mode);
/*
	 Handles interrupts 
*/
void f_camera_interrupt_manager(void);

#endif /*  #ifndef CAMCORE_API_H */


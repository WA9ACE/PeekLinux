#define AGILENT 0
#define MICRON 1

/**
 * @file	camd_api.h
 *
 * API Definition for CAMD SWE.
 *
 * @author	raymond zandbergen
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/12/2003	raymond zandbergen 		Create.
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef __CAMD_API_H_
#define __CAMD_API_H_


#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */
#include "rvm/rvm_ext_use_Id_list.h"




/* debug */
  extern void camd_printf (const char *fmt, ...);
  extern void camd_dump_32 (UINT32 * data, int n);
  extern void camd_dump_8 (UINT8 * data, int n);
  extern void camd_dump_page (UINT16 page_num);

#define CAMD_MESSAGE_OFFSET                  BUILD_MESSAGE_OFFSET(CAMD_USE_ID)
#define CAMD_STATUS_RSP_MSG                            (CAMD_MESSAGE_OFFSET+0)
#define CAMD_SET_CONFIGPARAMS_REQ_MSG           (CAMD_MESSAGE_OFFSET+1)
#define CAMD_START_REQ_MSG					           (CAMD_MESSAGE_OFFSET+2)
#define CAMD_GET_CONFIGPARAMS_RSP_MSG           (CAMD_MESSAGE_OFFSET+3)
#define CAMD_USEBUFF_REQ_MSG                      (CAMD_MESSAGE_OFFSET+4)
#define CAMD_GET_CONFIGPARAMS_REQ_MSG         (CAMD_MESSAGE_OFFSET+5)
#define CAMD_PAUSE_REQ_MSG                    (CAMD_MESSAGE_OFFSET+6)
#define CAMD_GET_VIEWFINDER_PARAMETERS_RSP_MSG         (CAMD_MESSAGE_OFFSET+7)
    /*#define CAMD_GET_VIEWFINDER_FRAMES_REQ_MSG             (CAMD_MESSAGE_OFFSET+8)*/
#define CAMD_VIEWFINDER_DATA_RSP_MSG                   (CAMD_MESSAGE_OFFSET+9)
#define CAMD_SNAPSHOT_DATA_RSP_MSG                     (CAMD_MESSAGE_OFFSET+10)
#define CAMD_REGISTERCLIENT_REQ_MSG                     (CAMD_MESSAGE_OFFSET+11)

#define CAMD_TRANSFER_INTERNAL_DMA                      (CAMD_MESSAGE_OFFSET+12) 

#define CAMD_OK                                        RV_OK
#define CAMD_TRANSFER_COMPLETE                         RV_MEMORY_REMAINING
#define CAMD_INVALID_PARAMETER                         RV_INVALID_PARAMETER
#define CAMD_NOT_READY                                 RV_NOT_READY
#define CAMD_INTERNAL_ERR                              RV_INTERNAL_ERR
#define CAMD_MEMORY_ERR                                RV_MEMORY_ERR

  typedef enum
  {
      CAMD_QCIF,
      CAMD_VGA,
#if(CAM_SENSOR==MICRON) 
      CAMD_SXGA,
#endif     
      CAMD_NOF_RESOLUTIONS }
  T_CAMD_RESOLUTION;

 typedef T_CAMD_RESOLUTION T_CAM_RESOLUTION ; 
	typedef enum
  { CAMD_YUYV_INTERLEAVED, CAMD_RGB_888, CAMD_RGB_565}
  T_CAMD_ENCODING;

  typedef enum
  { CAMD_SINGLE_SHOT, CAMD_CONTINUOUS }
  T_CAMD_VIEWFINDER_MODE;



  typedef enum
  {
	CAMD_GAMMA_CORR_DEFAULT,
	CAMD_GAMMA_CORR_3_2
  } T_CAMD_GAMMA;

 

 

/* framework for all camd messages */


  typedef enum
  {
      ZOOM_IN,
      ZOOM_OUT,
      ZOOM_STOP
  } T_CAMD_ZOOM;
    
typedef struct {

       UINT8 count;
       UINT8 num;
       UINT8 * ptr[2];
   
    } CAM_BUF_INFO;




  typedef struct
  {
	UINT16 number_of_resolutions;
	struct
	{
	  T_CAMD_RESOLUTION resolution;
	  T_CAMD_ENCODING encoding;
	  UINT16 width;
	  UINT16 height;
	  //UINT8 R_bits;				/* number of bits in R component */
	  //UINT8 G_bits;				/* number of bits in G component */
	 // UINT8 B_bits;				/* number of bits in B component */
	 // UINT8 Y_bits;				/* number of bits in Y component */
	 // UINT8 U_bits;				/* number of bits in U component */
	 // UINT8 V_bits;				/* number of bits in V component */
	  UINT16 max_zoom;			/* 0 = no zoom supported */
	} 
#if (CAM_SENSOR==MICRON)
    resolution[3];
#else
    resolution[2];

#endif    
	
  }
  T_CAMD_CAMERA_CAPABILITIES;

  extern const T_CAMD_CAMERA_CAPABILITIES camd_camera_capabilities;
 // extern const T_CAMD_PARAMETERS
//	camd_default_viewfinder_parameters,camd_default_snapshot_parameters;
    T_RV_RET camd_stopviewfinder();

typedef UINT8 T_CAM_SENSOR_ZOOM ;

typedef UINT8 T_CAM_SENSOR_MIRRORING;

enum
{
CAM_FEATURE_ENABLE,
CAM_FEATURE_DISABLE

};

 enum
{
CAM_FEATURE_NOT_SUPPORTED,
CAM_FEATURE_SUPPORTED
} ;


 enum {
T_CAM_NO_EFFECT=0x00,	
T_CAM_NEGATIVE_EFFECT=0x02,
 T_CAM_SEPIA_EFFECT=0x04,
T_CAM_MONOCHROME_EFFECT=0x08

};
typedef UINT8 T_CAM_EFFECT; 


typedef enum
{

CAM_SENSOR_AGILENT,

CAM_SENSOR_MICRON,

CAM_NUMBER_OF_SENSORS
} T_CAM_SENSOR;


typedef UINT16 T_CAM_MAX_WIDTH;
typedef UINT16 T_CAM_MAX_HEIGHT;
typedef struct
	{

T_CAM_SENSOR_ZOOM zoom_support;
T_CAM_SENSOR_MIRRORING mirror_support;
T_CAM_SENSOR current_sensor;
T_CAM_RESOLUTION max_resolution;
T_CAM_MAX_HEIGHT max_height;
T_CAM_MAX_WIDTH max_width;
T_CAM_EFFECT effect;

} CAM_SENSOR_CAPABILITIES;



 typedef struct
  {
	BOOL capturemode;
	T_CAMD_RESOLUTION resolution;
	T_CAMD_VIEWFINDER_MODE mode; 
	T_CAMD_ENCODING encoding;
	T_CAMD_GAMMA gamma_correction;
	UINT16 imagewidth;
	UINT16 imageheight;
	//BOOL black_and_white;
	BOOL flip_x;
	BOOL flip_y;
	UINT16 rotate;
	UINT16 zoom;
	T_CAM_EFFECT effect;
	void (*start_transfer_cb) (void);
  }
  T_CAMD_PARAMETERS;

  typedef struct
  {
	T_RV_HDR os_hdr;
	T_RV_RET status;
	T_RV_RETURN rp;
	union
	{
	  T_CAMD_PARAMETERS configparams;
	  BOOL enable_sensor;
	  UINT8 *buff;
	}
	body;
  }
  T_CAMD_MSG;


  
    typedef struct {
       T_CAMD_MSG *data;
       UINT8 count;

    } CAM_BUF_Q;


T_RV_RET camd_zoom(T_CAMD_ZOOM zoom);


T_RV_RET camd_get_sensor_capabilities(CAM_SENSOR_CAPABILITIES * sensor_capabilities);
	




/**
 * @name API functions
 *
 * API functions declarations (bridge functions).
 */
/*@{*/

/**
 *
 * Returns the CAMD driver SW version according to Riviera conventions
 *
 * @param none
 *
 * @return	The software version 
 *
 */

  UINT32 camd_get_sw_version (void);

/**
 *
 * This function switches the sensor between the enabled  and disabled state. 
 * It is disabled by default. When switching to the enabled state, the driver 
 * will perform the necessary hardware initialisations. The camera must be 
 * enabled before any other API entries (except camd_get_sw_version()) may be 
 * used.
 *
 * @param enable_sensor       TRUE = enable sensor, FALSE = disable sensor
 * @param rp                  return path for the response message
 *
 * @return RV_OK						normal operation
 *         RV_NOT_READY					CAMD not started
 *         RV_MEMORY_ERR				out of memory
 *
 */

  T_RV_RET camd_registerclient (BOOL enable_sensor, T_RV_RETURN rp);

/**
 * This function sets all snapshot parameters
 *
 * @param param_p             pointer to a block of parameters to be used
 * @param rp                  return path for the response message
 *
 * @return RV_OK						normal operation
 *         RV_NOT_READY					CAMD not started
 *         RV_MEMORY_ERR				out of memory
 *
 */

  T_RV_RET camd_set_configparams (T_CAMD_PARAMETERS * param_p,
										 T_RV_RETURN rp);
/**
 * This function retrieves all snapshot parameters currently in use
 *
 * @param rp                  return path for the response message
 *
 * @return RV_OK						normal operation
 *         RV_NOT_READY					CAMD not started
 *         RV_MEMORY_ERR				out of memory
 *
 */

  T_RV_RET camd_get_configparams (T_RV_RETURN rp);

/**
 * This function sets all parameters
 *
 * @param param_p             pointer to a block of parameters to be used
 * @param rp                  return path for the response message
 *
 * @return RV_OK						normal operation
 *         RV_NOT_READY					CAMD not started
 *         RV_MEMORY_ERR				out of memory
 *
 */
    T_RV_RET camd_stopviewfinder();

T_RV_RET camd_usebuff (UINT8 *buff, T_RV_RETURN rp);

T_RV_RET camd_start_capture(T_RV_RETURN rp);

T_RV_RET camd_pause(T_RV_RETURN rp);


void camd_enqueue(T_CAMD_MSG *msg_p);

typedef union
{

UINT8 snap_buf[220*220*4];//[1280*1024*2];

struct{
UINT8 framebuf_p1  [220*220*2];
UINT8 framebuf_p2 [220*220*2];
}cam_viewfinder_buffer;
}cam_test_buffer;


#endif /*__CAMD_API_H_*/





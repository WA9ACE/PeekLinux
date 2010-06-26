/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */

#ifndef __MM_POOL_SIZE_H_
#define __MM_POOL_SIZE_H_
#include "chipset.cfg"
#include "rv/rv_defined_swe.h"
#include "rv.cfg"

#define QUE_SIZE(n) (24*(n)+8*((n)+1))

/*---------------------------------------------------------------------------
* BAE  memory requirements
----------------------------------------------------------------------------*/
  #include "bae/bae_pool_size.h"
  #define BAE_PARTITION_0_SIZE 35
  #define BAE_PARTITION_1_SIZE 45
  #define BAE_PARTITION_2_SIZE 55
  #define BAE_PARTITION_3_SIZE 65
  #define BAE_PARTITION_4_SIZE 75
  #define BAE_PARTITION_5_SIZE 85
  #define BAE_PARTITION_6_SIZE 95
  #define BAE_PARTITION_7_SIZE 115
  #define BAE_PARTITION_8_SIZE 125
  #define BAE_PARTITION_9_SIZE 145
  #define BAE_PARTITION_10_SIZE 155
  #define BAE_PARTITION_11_SIZE 285
  #define BAE_PARTITION_12_SIZE 535
  #define BAE_PARTITION_13_SIZE 635
  #define BAE_PARTITION_14_SIZE 805

  #define BAE_INT_PARTITION_0_SIZE 35
  #define BAE_INT_PARTITION_1_SIZE 45
  #define BAE_INT_PARTITION_2_SIZE 55
  #define BAE_INT_PARTITION_3_SIZE 65
  #define BAE_INT_PARTITION_4_SIZE 75
  #define BAE_INT_PARTITION_5_SIZE 85
  #define BAE_INT_PARTITION_6_SIZE 95
  #define BAE_INT_PARTITION_7_SIZE 125
  #define BAE_INT_PARTITION_8_SIZE 215
  #define BAE_INT_PARTITION_9_SIZE 835
  #define BAE_INT_PARTITION_10_SIZE 2025
  #define BAE_INT_PARTITION_11_SIZE 8025

/* number of partitions */
#ifdef RVM_BAE_SWE
  #if (LOCOSTO_LITE==1)
  #define BAE_0_PARTITIONS 0
  #define BAE_1_PARTITIONS 0
  #define BAE_2_PARTITIONS 0
  #define BAE_3_PARTITIONS 0
  #define BAE_4_PARTITIONS 0
  #define BAE_5_PARTITIONS 0
  #define BAE_6_PARTITIONS 0
  #define BAE_7_PARTITIONS 0
  #define BAE_8_PARTITIONS 0
  #define BAE_9_PARTITIONS 0
  #define BAE_10_PARTITIONS 0
  #define BAE_11_PARTITIONS 0
  #define BAE_12_PARTITIONS 0
  #define BAE_13_PARTITIONS 0
  #define BAE_14_PARTITIONS 0

  #define BAE_INT_0_PARTITIONS 0
  #define BAE_INT_1_PARTITIONS 0
  #define BAE_INT_2_PARTITIONS 0
  #define BAE_INT_3_PARTITIONS 0
  #define BAE_INT_4_PARTITIONS 0
  #define BAE_INT_5_PARTITIONS 0
  #define BAE_INT_6_PARTITIONS 0
  #define BAE_INT_7_PARTITIONS 0
  #define BAE_INT_8_PARTITIONS 0
  #define BAE_INT_9_PARTITIONS 0
  #define BAE_INT_10_PARTITIONS 0
  #define BAE_INT_11_PARTITIONS 0
  #define BAE_DATA_POOL_SIZE 18*1024
  #else
  #define BAE_0_PARTITIONS 100
  #define BAE_1_PARTITIONS 100
  #define BAE_2_PARTITIONS 100
  #define BAE_3_PARTITIONS 30
  #define BAE_4_PARTITIONS 30
  #define BAE_5_PARTITIONS 30
  #define BAE_6_PARTITIONS 30
  #define BAE_7_PARTITIONS 30
  #define BAE_8_PARTITIONS 30
  #define BAE_9_PARTITIONS 10
  #define BAE_10_PARTITIONS 10
  #define BAE_11_PARTITIONS 10
  #define BAE_12_PARTITIONS 10
  #define BAE_13_PARTITIONS 10
  #define BAE_14_PARTITIONS 10

  #define BAE_INT_0_PARTITIONS 59
  #define BAE_INT_1_PARTITIONS 8
  #define BAE_INT_2_PARTITIONS 3
  #define BAE_INT_3_PARTITIONS 3
  #define BAE_INT_4_PARTITIONS 10
  #define BAE_INT_5_PARTITIONS 3
  #define BAE_INT_6_PARTITIONS 1
  #define BAE_INT_7_PARTITIONS 1
  #define BAE_INT_8_PARTITIONS 1
  #define BAE_INT_9_PARTITIONS 1
  #define BAE_INT_10_PARTITIONS 2
  #define BAE_INT_11_PARTITIONS 1
  #define   BAE_DATA_POOL_SIZE 0
  #endif

  #define BAE_TASK_MEM (BAE_STACK_SIZE+QUE_SIZE(10))
#else
  #define BAE_PARTITION_0_SIZE 0
  #define BAE_PARTITION_1_SIZE 0
  #define BAE_PARTITION_2_SIZE 0
  #define BAE_PARTITION_3_SIZE 0
  #define BAE_PARTITION_4_SIZE 0
  #define BAE_PARTITION_5_SIZE 0
  #define BAE_PARTITION_6_SIZE 0
  #define BAE_PARTITION_7_SIZE 0
  #define BAE_PARTITION_8_SIZE 0
  #define BAE_PARTITION_9_SIZE 0
  #define BAE_PARTITION_10_SIZE 0
  #define BAE_PARTITION_11_SIZE 0
  #define BAE_PARTITION_12_SIZE 0
  #define BAE_PARTITION_13_SIZE 0
  #define BAE_PARTITION_14_SIZE 0

  #define BAE_INT_PARTITION_0_SIZE 0
  #define BAE_INT_PARTITION_1_SIZE 0
  #define BAE_INT_PARTITION_2_SIZE 0
  #define BAE_INT_PARTITION_3_SIZE 0
  #define BAE_INT_PARTITION_4_SIZE 0
  #define BAE_INT_PARTITION_5_SIZE 0
  #define BAE_INT_PARTITION_6_SIZE 0
  #define BAE_INT_PARTITION_7_SIZE 0
  #define BAE_INT_PARTITION_8_SIZE 0
  #define BAE_INT_PARTITION_9_SIZE 0
  #define BAE_INT_PARTITION_10_SIZE 0
  #define BAE_INT_PARTITION_11_SIZE 0

/* number of partitions */
  #define BAE_0_PARTITIONS 0
  #define BAE_1_PARTITIONS 0
  #define BAE_2_PARTITIONS 0
  #define BAE_3_PARTITIONS 0
  #define BAE_4_PARTITIONS 0
  #define BAE_5_PARTITIONS 0
  #define BAE_6_PARTITIONS 0
  #define BAE_7_PARTITIONS 0
  #define BAE_8_PARTITIONS 0
  #define BAE_9_PARTITIONS 0
  #define BAE_10_PARTITIONS 0
  #define BAE_11_PARTITIONS 0
  #define BAE_12_PARTITIONS 0
  #define BAE_13_PARTITIONS 0
  #define BAE_14_PARTITIONS 0

  #define BAE_INT_0_PARTITIONS 0
  #define BAE_INT_1_PARTITIONS 0
  #define BAE_INT_2_PARTITIONS 0
  #define BAE_INT_3_PARTITIONS 0
  #define BAE_INT_4_PARTITIONS 0
  #define BAE_INT_5_PARTITIONS 0
  #define BAE_INT_6_PARTITIONS 0
  #define BAE_INT_7_PARTITIONS 0
  #define BAE_INT_8_PARTITIONS 0
  #define BAE_INT_9_PARTITIONS 0
  #define BAE_INT_10_PARTITIONS 0
  #define BAE_INT_11_PARTITIONS 0

  #define   BAE_DATA_POOL_SIZE 0
  #define BAE_TASK_MEM  0
#endif
/*---------------------------------------------------------------------------
* SSL  memory requirements
----------------------------------------------------------------------------*/
/* Partition Sizes */
#include "ssl/inc/ssl_pool_size.h"
#include "msl/inc/msl_api.h"
#define SSL_PARTITION_0_SIZE    85    //Structure BSP2
#define SSL_PARTITION_1_SIZE    (DISPLAY_IMAGE_WIDTH * (DISPLAY_HEIGHT - DISPLAY_IMAGE_WIDTH)* BITSPERPIXEL_DISPLAY)/*176X44X2  : Menu : bsp32*/
#define SSL_PARTITION_2_SIZE    (DISPLAY_WIDTH * DISPLAY_WIDTH * BITSPERPIXEL_DISPLAY)/*176x176x2 : Image Viewer : bsp34*/
#define SSL_PARTITION_3_SIZE    (DISPLAY_WIDTH * DISPLAY_HEIGHT * BITSPERPIXEL_DISPLAY)/*176x220x2 : FrameBuffer, MMI : bsp 39*/

#ifdef RVM_SSL_SWE
/* Number of Partitions */
#define SSL_0_PARTITIONS        34
#define SSL_1_PARTITIONS        2
#define SSL_2_PARTITIONS        1
#define SSL_3_PARTITIONS        2

#define SSL_TASK_MEM            (SSL_STACK_SIZE+QUE_SIZE(10))

#else

/* Number of Partitions */
#define SSL_0_PARTITIONS        0
#define SSL_1_PARTITIONS        0
#define SSL_2_PARTITIONS        0
#define SSL_3_PARTITIONS        0

#define SSL_TASK_MEM            0

#endif

/*---------------------------------------------------------------------------
msl memory requirements
----------------------------------------------------------------------------*/
#define MSL_PARTITION_0_SIZE    (SNAPSHOT_WIDTH * SNAPSHOT_HEIGHT * BITSPERPIXEL_IMAGE)/*(SS image capture and color convert)*/
#define MSL_PARTITION_1_SIZE (DISPLAY_IMAGE_HEIGHT * DISPLAY_IMAGE_WIDTH * BITSPERPIXEL_IMAGE) /*(second buffer for SS mode)*/
#define MSL_PARTITION_2_SIZE    MAX_ENCODED_BUFFER_SIZE  //(encoder output buffer)
#define MSL_PARTITION_3_SIZE    95
#define MSL_PARTITION_4_SIZE    315

#ifdef RVM_MSL_SWE
/* number of partitions */
#ifdef MSL_SINGLEBUFFERMODE
#define MSL_0_PARTITIONS        1
#define MSL_1_PARTITIONS        3
#else 
#define MSL_0_PARTITIONS        2
#define MSL_1_PARTITIONS        0
#endif
#define MSL_2_PARTITIONS        MAX_BURSTCOUNT
#define MSL_3_PARTITIONS        1
#define MSL_4_PARTITIONS        6
#else

#define MSL_0_PARTITIONS        0
#define MSL_1_PARTITIONS        0
#define MSL_2_PARTITIONS        0
#define MSL_3_PARTITIONS        0
#define MSL_4_PARTITIONS        0
#endif //RVM_MSL_SWE


/*---------------------------------------------------------------------------
Img memory requirements
----------------------------------------------------------------------------*/
#include "img/inc/img_pool_size.h"
#define IMG_PARTITION_0_SIZE    45
#define IMG_PARTITION_1_SIZE    55
#define IMG_PARTITION_2_SIZE    85
#define IMG_PARTITION_3_SIZE    120

#ifdef RVM_IMG_SWE

#define IMG_0_PARTITIONS        5
#define IMG_1_PARTITIONS        3
#define IMG_2_PARTITIONS        3
#define IMG_3_PARTITIONS        10

#define IMG_TASK_MEM            (IMG_STACK_SIZE+QUE_SIZE(10))

#else

#define IMG_0_PARTITIONS        0
#define IMG_1_PARTITIONS        0
#define IMG_2_PARTITIONS        0
#define IMG_3_PARTITIONS        0

#define IMG_TASK_MEM            0
#endif

/*---------------------------------------------------------------------------
* jpeg codec  memory requirements
----------------------------------------------------------------------------*/
  #define JPEG_INT_PARTITION_0_SIZE 45
  #define JPEG_INT_PARTITION_1_SIZE 49
  #define JPEG_INT_PARTITION_2_SIZE 51
  #define JPEG_INT_PARTITION_3_SIZE 73
  #define JPEG_INT_PARTITION_4_SIZE 97
  #define JPEG_INT_PARTITION_5_SIZE 153
  #define JPEG_INT_PARTITION_6_SIZE 313
  #define JPEG_INT_PARTITION_7_SIZE 377
  #define JPEG_INT_PARTITION_8_SIZE 379
  #define JPEG_INT_PARTITION_9_SIZE 693
  #define JPEG_INT_PARTITION_10_SIZE 793
  #define JPEG_INT_PARTITION_11_SIZE 1280

#ifdef RVM_JPEGCODEC_SWE
/* jpeg codec partition counts */
  #define JPEG_INT_0_PARTITIONS 2
  #define JPEG_INT_1_PARTITIONS 1
  #define JPEG_INT_2_PARTITIONS 1
  #define JPEG_INT_3_PARTITIONS 1
  #define JPEG_INT_4_PARTITIONS 1
  #define JPEG_INT_5_PARTITIONS 1
  #define JPEG_INT_6_PARTITIONS 1
  #define JPEG_INT_7_PARTITIONS 1
  #define JPEG_INT_8_PARTITIONS 1
  #define JPEG_INT_9_PARTITIONS 1
  #define JPEG_INT_10_PARTITIONS 1
  #define JPEG_INT_11_PARTITIONS 2

#else

/* jpeg codec partition counts */
  #define JPEG_INT_0_PARTITIONS 0
  #define JPEG_INT_1_PARTITIONS 0
  #define JPEG_INT_2_PARTITIONS 0
  #define JPEG_INT_3_PARTITIONS 0
  #define JPEG_INT_4_PARTITIONS 0
  #define JPEG_INT_5_PARTITIONS 0
  #define JPEG_INT_6_PARTITIONS 0
  #define JPEG_INT_7_PARTITIONS 0
  #define JPEG_INT_8_PARTITIONS 0
  #define JPEG_INT_9_PARTITIONS 0
  #define JPEG_INT_10_PARTITIONS 0
  #define JPEG_INT_11_PARTITIONS 0
#endif //RVM_JPEGCODEC_SWE

#define MM_EXT_DATA_POOL_SIZE ((MSL_PARTITION_0_SIZE*MSL_0_PARTITIONS) +\
                                                             (MSL_PARTITION_1_SIZE*MSL_1_PARTITIONS ) +\
                                                             (MSL_PARTITION_2_SIZE*MSL_2_PARTITIONS ) +\
                                                             (MSL_PARTITION_3_SIZE*MSL_3_PARTITIONS ) +\
                                                             (MSL_PARTITION_4_SIZE*MSL_4_PARTITIONS ) +\
                                                             (SSL_PARTITION_0_SIZE*SSL_0_PARTITIONS) +\
                                                             (SSL_PARTITION_1_SIZE*SSL_1_PARTITIONS) +\
                                                             (SSL_PARTITION_2_SIZE*SSL_2_PARTITIONS) +\
                                                             (SSL_PARTITION_3_SIZE*SSL_3_PARTITIONS) +\
                                                             (IMG_PARTITION_0_SIZE*IMG_0_PARTITIONS) +\
                                                             (IMG_PARTITION_1_SIZE*IMG_1_PARTITIONS) +\
                                                             (IMG_PARTITION_2_SIZE*IMG_2_PARTITIONS) +\
                                                             (IMG_PARTITION_3_SIZE*IMG_3_PARTITIONS) +\
                                                             (BAE_PARTITION_0_SIZE*BAE_0_PARTITIONS) +\
                                                             (BAE_PARTITION_1_SIZE*BAE_1_PARTITIONS) +\
                                                             (BAE_PARTITION_2_SIZE*BAE_2_PARTITIONS) +\
                                                             (BAE_PARTITION_3_SIZE*BAE_3_PARTITIONS) +\
                                                             (BAE_PARTITION_4_SIZE*BAE_4_PARTITIONS) +\
                                                             (BAE_PARTITION_5_SIZE*BAE_5_PARTITIONS) +\
                                                             (BAE_PARTITION_6_SIZE*BAE_6_PARTITIONS) +\
                                                             (BAE_PARTITION_7_SIZE*BAE_7_PARTITIONS) +\
                                                             (BAE_PARTITION_8_SIZE*BAE_8_PARTITIONS) +\
                                                             (BAE_PARTITION_9_SIZE*BAE_9_PARTITIONS) +\
                                                             (BAE_PARTITION_10_SIZE*BAE_10_PARTITIONS) +\
                                                             (BAE_PARTITION_11_SIZE*BAE_11_PARTITIONS) +\
                                                             (BAE_PARTITION_12_SIZE*BAE_12_PARTITIONS) +\
                                                             (BAE_PARTITION_13_SIZE*BAE_13_PARTITIONS) +\
                                                             (BAE_PARTITION_14_SIZE*BAE_14_PARTITIONS))

#include "../dm_defs.h"
#define VIDEO_DECODER_INTERNAL_HEAP_SIZE             22068 
#define VIDEO_DECODER_INTERNAL_HAEP_ALLOCATIONS      1

#define AAC_OUTPUT_BUFFER_INTERNAL_HEAP_SIZE         8192
#define AAC_OUTPUT_BUFFER_INTERNAL_HEAP_ALLOCATIONS  1
#define HEAP_HEADER_FOOTER_SIZE                      2*DM_OVERHEAD

#define MM_INT_DATA_POOL_SIZE   ( HEAP_HEADER_FOOTER_SIZE + \
								  VIDEO_DECODER_INTERNAL_HEAP_SIZE + \
								  AAC_OUTPUT_BUFFER_INTERNAL_HEAP_SIZE + \
								  (VIDEO_DECODER_INTERNAL_HAEP_ALLOCATIONS+ AAC_OUTPUT_BUFFER_INTERNAL_HEAP_ALLOCATIONS) * DM_OVERHEAD )

    
                                                             
#define EXT_DATA_POOL_MM_SIZE (BAE_TASK_MEM)
#define INT_DATA_POOL_MM_SIZE (IMG_TASK_MEM+\
                                SSL_TASK_MEM) //move to internal stack pool for performance reasons. 

#endif //mm_pool_size

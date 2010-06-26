/*=============================================================================
 *    Copyright 2001-2002 Texas Instruments Inc. All rights reserved.
 */

#ifndef __RVF_POOL_SIZE_H_
#define __RVF_POOL_SIZE_H_
#include "chipset.cfg"
#include "rv/rv_defined_swe.h"
#include "rv.cfg"

#define QUE_SIZE(n) (24*(n)+8*((n)+1))
/*
 * ATP SWE
 */
#ifdef RVM_ATP_SWE
#endif

/*
 * ATP_UART SWE
 */
#ifdef RVM_ATP_UART_SWE
#endif

/*
 * Bluetooth memory partitions
 */
#ifdef BTS
  #define BTS_TASK_MEM 8000
  #define BTT_TASK_MEM 4000
  #define BTAV_TASK_MEM 4000
  #define BTU_TASK_MEM 2000
    #ifdef FMS
      #define FMS_TASK_MEM 4000
    #endif
#define BT_BTS_TASK_MEM      ((4096+8)   +       /* allocation per stack. 4096 is the stack size in pei_create */        \
                                (32*10+44+8))   /* allocation per queue. 10 is the queue size in pei_create */ 
  #define BT_BTAV_TASK_MEM      ((2996+8)   +       /* allocation per stack. 2996 is the stack size in pei_create */        \
                                 (32*10+44+8))   /* allocation per queue. 10 is the queue size in pei_create */
  #define BT_BTU_TASK_MEM       ((4096+8)   +       /* allocation per stack. 4096 is the stack size in pei_create */        \
  						(32*10+44+8)) 	/* allocation per queue. 10 is the queue size in pei_create */
  #define BT_TASK_MEM   (BT_BTS_TASK_MEM + BT_BTAV_TASK_MEM + BT_BTU_TASK_MEM)
#else
 

#define BTS_TASK_MEM 0
#define BTT_TASK_MEM 0
#define BTAV_TASK_MEM 0
#define BTU_TASK_MEM 0
#define FMS_TASK_MEM 0
#endif

/*
 * AUDIO SWE
 */
#ifdef RVM_AUDIO_MAIN_SWE
  #include "audio/audio_pool_size.h"

/* sizes of audio partitions */
  #define AUDIO_PARTITION_0_SIZE 35
  #define AUDIO_PARTITION_1_SIZE 45
  #define AUDIO_PARTITION_2_SIZE 55
  #define AUDIO_PARTITION_3_SIZE 65
  #define AUDIO_PARTITION_4_SIZE 75
  #define AUDIO_PARTITION_5_SIZE 85
  #define AUDIO_PARTITION_6_SIZE 95
  #define AUDIO_PARTITION_7_SIZE 2500
#if (L1_PCM_EXTRACTION==1)
  #define AUDIO_PARTITION_8_SIZE 40985
#endif
  #define AUDIO_INT_PARTITION_0_SIZE 45
  #define AUDIO_INT_PARTITION_1_SIZE 55
  #define AUDIO_INT_PARTITION_2_SIZE 525

/* number of partitions */
  #define AUDIO_0_PARTITIONS 5
  #define AUDIO_1_PARTITIONS 5
  #define AUDIO_2_PARTITIONS 5
  #define AUDIO_3_PARTITIONS 5
  #define AUDIO_4_PARTITIONS 5
  #define AUDIO_5_PARTITIONS 5
  #define AUDIO_6_PARTITIONS 5
  #define AUDIO_7_PARTITIONS 3
#if (L1_PCM_EXTRACTION==1)
  #define AUDIO_8_PARTITIONS 8
#endif

  #define AUDIO_INT_0_PARTITIONS (15)
  #define AUDIO_INT_1_PARTITIONS (15)
  #define AUDIO_INT_2_PARTITIONS 5

  #define AUDIO_TASK_MEM (AUDIO_STACK_SIZE+QUE_SIZE(10))
#else
  #define AUDIO_PARTITION_0_SIZE 0
  #define AUDIO_PARTITION_1_SIZE 0
  #define AUDIO_PARTITION_2_SIZE 0
  #define AUDIO_PARTITION_3_SIZE 0
  #define AUDIO_PARTITION_4_SIZE 0
  #define AUDIO_PARTITION_5_SIZE 0
  #define AUDIO_PARTITION_6_SIZE 0
  #define AUDIO_PARTITION_7_SIZE 0
#if (L1_PCM_EXTRACTION==1)
  #define AUDIO_PARTITION_8_SIZE 0
#endif

  #define AUDIO_INT_PARTITION_0_SIZE 0
  #define AUDIO_INT_PARTITION_1_SIZE 0
  #define AUDIO_INT_PARTITION_2_SIZE 0

/* number of partitions */
  #define AUDIO_0_PARTITIONS 0
  #define AUDIO_1_PARTITIONS 0
  #define AUDIO_2_PARTITIONS 0
  #define AUDIO_3_PARTITIONS 0
  #define AUDIO_4_PARTITIONS 0
  #define AUDIO_5_PARTITIONS 0
  #define AUDIO_6_PARTITIONS 0
  #define AUDIO_7_PARTITIONS 0
#if (L1_PCM_EXTRACTION==1)
  #define AUDIO_8_PARTITIONS 0
#endif

  #define AUDIO_INT_0_PARTITIONS 0
  #define AUDIO_INT_1_PARTITIONS 0
  #define AUDIO_INT_2_PARTITIONS 0

  #define AUDIO_TASK_MEM 0

#endif

/*
 * Background AUDIO SWE
 */
#ifdef RVM_AUDIO_BGD_SWE
#else
#endif

/*
 * AS SWE
 */
#ifdef RVM_AS_SWE
  #include "as/as_pool_size.h"
  #define AS_PARTITION_0_SIZE 45
  #define AS_PARTITION_1_SIZE 55
  #define AS_PARTITION_2_SIZE 105
  #define AS_PARTITION_3_SIZE 255

/* number of partitions */

  #define AS_0_PARTITIONS 1
  #define AS_1_PARTITIONS 7
  #define AS_2_PARTITIONS 1
  #define AS_3_PARTITIONS 1

  #define AS_TASK_MEM (AS_STACK_SIZE+QUE_SIZE(10))
#else
  #define AS_PARTITION_0_SIZE 0
  #define AS_PARTITION_1_SIZE 0
  #define AS_PARTITION_2_SIZE 0
  #define AS_PARTITION_3_SIZE 0

/* number of partitions */
  #define AS_0_PARTITIONS 0
  #define AS_1_PARTITIONS 0
  #define AS_2_PARTITIONS 0
  #define AS_3_PARTITIONS 0

  #define AS_TASK_MEM 0
#endif


/*
 * BPR SWE
 */

#ifdef RVM_BPR_SWE
  /*
   * Values used in bpr1_env.h
   */
#endif


/*
 * DACI SWE
 */
#ifdef RVM_DACI_SWE
#endif

/*
 * DAR SWE
 */
#ifdef RVM_DAR_SWE
  #include "dar/dar_pool_size.h"
  #define DAR_PARTITION_0_SIZE 45
  #define DAR_PARTITION_1_SIZE 65
  #define DAR_PARTITION_2_SIZE 85
  #define DAR_PARTITION_3_SIZE 125
  #define DAR_PARTITION_4_SIZE 275
  #define DAR_PARTITION_5_SIZE 525

/* number of partitions */
    #define DAR_0_PARTITIONS 5
  #define DAR_1_PARTITIONS 5
  #define DAR_2_PARTITIONS 5
  #define DAR_3_PARTITIONS 5
  #define DAR_4_PARTITIONS 5
  #define DAR_5_PARTITIONS 2

  #define DAR_TASK_MEM  (DAR_STACK_SIZE+QUE_SIZE(10))
#else
  #define DAR_PARTITION_0_SIZE 0
  #define DAR_PARTITION_1_SIZE 0
  #define DAR_PARTITION_2_SIZE 0
  #define DAR_PARTITION_3_SIZE 0
  #define DAR_PARTITION_4_SIZE 0
  #define DAR_PARTITION_5_SIZE 0

/* number of partitions */
  #define DAR_0_PARTITIONS 0
  #define DAR_1_PARTITIONS 0
  #define DAR_2_PARTITIONS 0
  #define DAR_3_PARTITIONS 0
  #define DAR_4_PARTITIONS 0
  #define DAR_5_PARTITIONS 0

  #define DAR_TASK_MEM  0

#endif

/*
 * DCM SWE
 */
#ifdef RVM_DCM_SWE
#endif

/*
 * DCFG SWE
 */
#ifdef RVM_DCFG_SWE
#endif


/*
 * DMG SWE
 */
#ifdef RVM_DMG_SWE
#endif

/*
 * CAMD SWE
 */
#ifdef RVM_CAMD_SWE
  #include "camd/camd_pool_size.h"
  #define CAMD_PARTITION_0_SIZE 55
  #define CAMD_PARTITION_1_SIZE 95
  #define CAMD_PARTITION_2_SIZE 185

  #define CAMD_INT_PARTITION_0_SIZE 65
  #define CAMD_INT_PARTITION_1_SIZE 125
  #define CAMD_INT_PARTITION_2_SIZE 525


/* number of partitions */
  #define CAMD_0_PARTITIONS 3
  #define CAMD_1_PARTITIONS 3
  #define CAMD_2_PARTITIONS 2

  #define CAMD_INT_0_PARTITIONS 10
  #define CAMD_INT_1_PARTITIONS 5
  #define CAMD_INT_2_PARTITIONS 1

  #define CAMD_TASK_MEM  (CAMD_STACK_SIZE+QUE_SIZE(10))

#else
  #define CAMD_PARTITION_0_SIZE 0
  #define CAMD_PARTITION_1_SIZE 0
  #define CAMD_PARTITION_2_SIZE 0

  #define CAMD_INT_PARTITION_0_SIZE 0
  #define CAMD_INT_PARTITION_1_SIZE 0
  #define CAMD_INT_PARTITION_2_SIZE 0


/* number of partitions */
  #define CAMD_0_PARTITIONS 0
  #define CAMD_1_PARTITIONS 0
  #define CAMD_2_PARTITIONS 0

  #define CAMD_INT_0_PARTITIONS 0
  #define CAMD_INT_1_PARTITIONS 0
  #define CAMD_INT_2_PARTITIONS 0

  #define CAMD_TASK_MEM  0

#endif

/*
 * DMA SWE
 */
#ifdef RVM_DMA_SWE
  #include "dma/dma_pool_size.h"
  #define DMA_PARTITION_0_SIZE 45
  #define DMA_PARTITION_1_SIZE 55
  #define DMA_PARTITION_2_SIZE 75
  #define DMA_PARTITION_3_SIZE 2025

/* number of partitions */
  #define DMA_0_PARTITIONS 20
  #define DMA_1_PARTITIONS 10
  #define DMA_2_PARTITIONS 10
  #define DMA_3_PARTITIONS 1

  #define DMA_TASK_MEM  (DMA_STACK_SIZE+QUE_SIZE(10))

#else
  #define DMA_PARTITION_0_SIZE 0
  #define DMA_PARTITION_1_SIZE 0
  #define DMA_PARTITION_2_SIZE 0
  #define DMA_PARTITION_3_SIZE 0

/* number of partitions */
  #define DMA_0_PARTITIONS 0
  #define DMA_1_PARTITIONS 0
  #define DMA_2_PARTITIONS 0
  #define DMA_3_PARTITIONS 0

  #define DMA_TASK_MEM  0

#endif

/*
 * ETM SWE
 */
#ifdef RVM_ETM_SWE
  #include "etm/etm_pool_size.h"
  #define ETM_PARTITION_0_SIZE 35
  #define ETM_PARTITION_1_SIZE 75
  #define ETM_PARTITION_2_SIZE 295
  #define ETM_PARTITION_3_SIZE 315
  #define ETM_PARTITION_4_SIZE 2025

/* number of partitions */

  #define ETM_0_PARTITIONS 5
  #define ETM_1_PARTITIONS 5
  #define ETM_2_PARTITIONS 3
  #define ETM_3_PARTITIONS 3
  #define ETM_4_PARTITIONS 1
  #define ETM_TASK_MEM  (ETM_STACK_SIZE+QUE_SIZE(10))

#else
 #define ETM_PARTITION_0_SIZE 0
  #define ETM_PARTITION_1_SIZE 0
  #define ETM_PARTITION_2_SIZE 0
  #define ETM_PARTITION_3_SIZE 0
  #define ETM_PARTITION_4_SIZE 0

/* number of partitions */
  #define ETM_0_PARTITIONS 0
  #define ETM_1_PARTITIONS 0
  #define ETM_2_PARTITIONS 0
  #define ETM_3_PARTITIONS 0
  #define ETM_4_PARTITIONS 0

  #define ETM_TASK_MEM  0
#endif

/*
 * FFS SWE
 */
#ifdef RVM_FFS_SWE
  #include "ffs/ffs_pool_size.h"
  #define FFS_PARTITION_0_SIZE 35
  #define FFS_PARTITION_1_SIZE 55
  #define FFS_PARTITION_2_SIZE 75
  #define FFS_PARTITION_3_SIZE 295
  #define FFS_PARTITION_4_SIZE 315
  #define FFS_PARTITION_5_SIZE 3025

/* number of partitions */
  #define FFS_0_PARTITIONS 5
  #define FFS_1_PARTITIONS 5
  #define FFS_2_PARTITIONS 5
  #define FFS_3_PARTITIONS 1
  #define FFS_4_PARTITIONS 1
  #define FFS_5_PARTITIONS 2

  #define FFS_TASK_MEM  (FFS_STACK_SIZE+QUE_SIZE(10))
#else
  #define FFS_PARTITION_0_SIZE 0
  #define FFS_PARTITION_1_SIZE 0
  #define FFS_PARTITION_2_SIZE 0
  #define FFS_PARTITION_3_SIZE 0
  #define FFS_PARTITION_4_SIZE 0
  #define FFS_PARTITION_5_SIZE 0


/* number of partitions */
  #define FFS_0_PARTITIONS 0
  #define FFS_1_PARTITIONS 0
  #define FFS_2_PARTITIONS 0
  #define FFS_3_PARTITIONS 0
  #define FFS_4_PARTITIONS 0
  #define FFS_5_PARTITIONS 0

  #define FFS_TASK_MEM  0
#endif


/*
 * HASH SWE
 */
#ifdef RVM_HASH_SWE
  #include "hash/hash_pool_size.h"
  #define HASH_PARTITION_0_SIZE 75
  #define HASH_PARTITION_1_SIZE 115

/* number of partitions */
  #define HASH_0_PARTITIONS 10
  #define HASH_1_PARTITIONS 5
  #define HASH_TASK_MEM  (HASH_STACK_SIZE+QUE_SIZE(10))

#else
  #define HASH_PARTITION_0_SIZE 0
  #define HASH_PARTITION_1_SIZE 0

/* number of partitions */
  #define HASH_0_PARTITIONS 0
  #define HASH_1_PARTITIONS 0
  #define HASH_TASK_MEM  0
#endif

/*
 * I2C SWE
 */
#ifdef RVM_I2C_SWE
#endif

/*
 * KPD SWE
 */
#ifdef RVM_KPD_SWE
#include "kpd/kpd_pool_size.h"
  #define KPD_PARTITION_0_SIZE 35
  #define KPD_PARTITION_1_SIZE 45
  #define KPD_PARTITION_2_SIZE 55
  #define KPD_PARTITION_3_SIZE 65
  #define KPD_PARTITION_4_SIZE 75
  #define KPD_PARTITION_5_SIZE 85
  #define KPD_PARTITION_6_SIZE 115

/* number of partitions */
  #define KPD_0_PARTITIONS 2
  #define KPD_1_PARTITIONS 7
  #define KPD_2_PARTITIONS 5
  #define KPD_3_PARTITIONS 1
  #define KPD_4_PARTITIONS 1
  #define KPD_5_PARTITIONS 4
  #define KPD_6_PARTITIONS 2
  #define KPD_TASK_MEM  (KPD_STACK_SIZE+QUE_SIZE(10))
#else
  #define KPD_PARTITION_0_SIZE 0
  #define KPD_PARTITION_1_SIZE 0
  #define KPD_PARTITION_2_SIZE 0
  #define KPD_PARTITION_3_SIZE 0
  #define KPD_PARTITION_4_SIZE 0
  #define KPD_PARTITION_5_SIZE 0
  #define KPD_PARTITION_6_SIZE 0

/* number of partitions */
  #define KPD_0_PARTITIONS 0
  #define KPD_1_PARTITIONS 0
  #define KPD_2_PARTITIONS 0
  #define KPD_3_PARTITIONS 0
  #define KPD_4_PARTITIONS 0
  #define KPD_5_PARTITIONS 0
  #define KPD_6_PARTITIONS 0

  #define KPD_TASK_MEM  0
#endif

/*
 * LLS SWE
 */
#ifdef RVM_LLS_SWE
#endif

/*
 * MDC SWE
 */
#ifdef RVM_MDC_SWE
  #include "mdc/mdc_pool_size.h"
#endif

/*
 * MDL SWE
 */
#ifdef RVM_MDL_SWE
#endif

/*
 * MFW SWE
 */
#ifdef RVM_MFW_SWE
#endif

/*
 * MKS SWE
 */
#ifdef RVM_MKS_SWE
  #include "mks/mks_pool_size.h"
  #define MKS_PARTITION_0_SIZE 85
  /* number of partitions */
  #define MKS_0_PARTITIONS 5

  #define MKS_TASK_MEM  (MKS_STACK_SIZE+QUE_SIZE(10))
#else
  #define MKS_PARTITION_0_SIZE 0
  /* number of partitions */
  #define MKS_0_PARTITIONS 0

  #define MKS_TASK_MEM  0
#endif


/*
 * MPM SWE
 */
#ifdef RVM_MPM_SWE
#endif


/*
 * USB SWE (USB LLD SWE)
 */
#ifdef RVM_USB_SWE
  #include "usb/usb_pool_size.h"
  #define USB_PARTITION_0_SIZE 45
  #define USB_PARTITION_1_SIZE 55
  #define USB_PARTITION_2_SIZE 75
  #define USB_PARTITION_3_SIZE 95
  #define USB_PARTITION_4_SIZE 105
  #define USB_PARTITION_5_SIZE 445

/* number of partitions */
  #define USB_0_PARTITIONS 5
  #define USB_1_PARTITIONS 3
  #define USB_2_PARTITIONS 1
  #define USB_3_PARTITIONS 4
  #define USB_4_PARTITIONS 2
  #define USB_5_PARTITIONS 1
  #define USB_TASK_MEM  (USB_STACK_SIZE+QUE_SIZE(10))
#else
 #define USB_PARTITION_0_SIZE 0
  #define USB_PARTITION_1_SIZE 0
  #define USB_PARTITION_2_SIZE 0
  #define USB_PARTITION_3_SIZE 0
  #define USB_PARTITION_4_SIZE 0
  #define USB_PARTITION_5_SIZE 0


/* number of partitions */
  #define USB_0_PARTITIONS 0
  #define USB_1_PARTITIONS 0
  #define USB_2_PARTITIONS 0
  #define USB_3_PARTITIONS 0
  #define USB_4_PARTITIONS 0
  #define USB_5_PARTITIONS 0

  #define USB_TASK_MEM  0
#endif

/*
 * USBTRC SWE (USBTRC Class SWE)
 */
#ifdef RVM_USBTRC_SWE
  #include "usbtrc/usbtrc_pool_size.h"
#define USBTRC_PARTITION_0_SIZE 95
  #define USBTRC_PARTITION_1_SIZE 125
  #define USBTRC_PARTITION_2_SIZE 155
  #define USBTRC_PARTITION_3_SIZE 225
  #define USBTRC_PARTITION_4_SIZE 1055
  #define USBTRC_PARTITION_5_SIZE 5025


/* number of partitions */
  #define USBTRC_0_PARTITIONS 10
  #define USBTRC_1_PARTITIONS 10
  #define USBTRC_2_PARTITIONS 3
  #define USBTRC_3_PARTITIONS 3
  #define USBTRC_4_PARTITIONS 1
  #define USBTRC_5_PARTITIONS 3
  #define USBTRC_TASK_MEM  (USBTRC_STACK_SIZE+QUE_SIZE(10))
#else
 #define USBTRC_PARTITION_0_SIZE 0
  #define USBTRC_PARTITION_1_SIZE 0
  #define USBTRC_PARTITION_2_SIZE 0
  #define USBTRC_PARTITION_3_SIZE 0
  #define USBTRC_PARTITION_4_SIZE 0
  #define USBTRC_PARTITION_5_SIZE 0


/* number of partitions */
  #define USBTRC_0_PARTITIONS 0
  #define USBTRC_1_PARTITIONS 0
  #define USBTRC_2_PARTITIONS 0
  #define USBTRC_3_PARTITIONS 0
  #define USBTRC_4_PARTITIONS 0
  #define USBTRC_5_PARTITIONS 0

  #define USBTRC_TASK_MEM  0
#endif

/*
 * USBMS SWE (USBMS SWE)
 */
#ifdef RVM_USBMS_SWE
  #include "usbms/usbms_pool_size.h"
  #define USBMS_PARTITION_0_SIZE 45
  #define USBMS_PARTITION_1_SIZE 65
  #define USBMS_PARTITION_2_SIZE 155
  #define USBMS_PARTITION_3_SIZE 295
  #define USBMS_PARTITION_4_SIZE 0 // added for T_USBMS_XFER_BUFFER

  #define USBMS_INT_PARTITION_0_SIZE 20024

/* number of partitions */
  #define USBMS_0_PARTITIONS 5
  #define USBMS_1_PARTITIONS 5
  #define USBMS_2_PARTITIONS 3
  #define USBMS_3_PARTITIONS 2

  #define USBMS_INT_0_PARTITIONS 1

  #define USBMS_TASK_MEM  (USBMS_STACK_SIZE+QUE_SIZE(10))
#else
#define USBMS_PARTITION_0_SIZE 0
  #define USBMS_PARTITION_1_SIZE 0
  #define USBMS_PARTITION_2_SIZE 0
  #define USBMS_PARTITION_3_SIZE 0
  #define USBMS_PARTITION_4_SIZE 0 // added for T_USBMS_XFER_BUFFER
  #define USBMS_INT_PARTITION_0_SIZE 0


/* number of partitions */
  #define USBMS_0_PARTITIONS 0
  #define USBMS_1_PARTITIONS 0
  #define USBMS_2_PARTITIONS 0
  #define USBMS_3_PARTITIONS 0
  #define USBMS_INT_0_PARTITIONS 0
  #define USBMS_TASK_MEM  0
#endif

/*
 * USBFAX SWE (USB FAX/DATA Class SWE)
 */
#ifdef RVM_USBFAX_SWE
  #include "usbfax/usbfax_pool_size.h"
 #define USBFAX_PARTITION_0_SIZE 35
  #define USBFAX_PARTITION_1_SIZE 45
  #define USBFAX_PARTITION_2_SIZE 55
  #define USBFAX_PARTITION_3_SIZE 4435

/* number of partitions */
  #define USBFAX_0_PARTITIONS 30
  #define USBFAX_1_PARTITIONS 30
  #define USBFAX_2_PARTITIONS 30
  #define USBFAX_3_PARTITIONS 1

  #define USBFAX_TASK_MEM  (USBFAX_STACK_SIZE+QUE_SIZE(10))
#else
  #define USBFAX_PARTITION_0_SIZE 0
  #define USBFAX_PARTITION_1_SIZE 0
  #define USBFAX_PARTITION_2_SIZE 0
  #define USBFAX_PARTITION_3_SIZE 0


/* number of partitions */
  #define USBFAX_0_PARTITIONS 0
  #define USBFAX_1_PARTITIONS 0
  #define USBFAX_2_PARTITIONS 0
  #define USBFAX_3_PARTITIONS 0

  #define USBFAX_TASK_MEM  0
#endif


/*
 * MC SWE
 */
#ifdef RVM_MC_SWE
#endif



/*
 * DATALIGHT SWE
 */
#ifdef RVM_DATALIGHT_SWE
  #include "datalight/datalight_pool_size.h"
#define DATALIGHT_PARTITION_0_SIZE 105
#define DATALIGHT_PARTITION_1_SIZE 1225

/* number of partitions */
  #define DATALIGHT_0_PARTITIONS 20
  #define DATALIGHT_1_PARTITIONS 2
  #define DATALIGHT_TASK_MEM  DATALIGHT_STACK_SIZE+QUE_SIZE(10)

 /* NAN partitions */
 #define NAN_PARTITION_0_SIZE 0
  #define NAN_PARTITION_1_SIZE 0
 /* number of partitions */
  #define NAN_0_PARTITIONS 0
  #define NAN_1_PARTITIONS 0
  #define NAN_TASK_MEM  0

  /* NOR paritions */
 #define NOR_BM_PARTITION_0_SIZE 0
/* number of partitions */
  #define NOR_BM_0_PARTITIONS 0
  #define NOR_BM_1_PARTITIONS 0

  #define NOR_BM_TASK_MEM  0


#else

#define DATALIGHT_PARTITION_0_SIZE 0

/* number of partitions */
  #define DATALIGHT_0_PARTITIONS 0
  #define DATALIGHT_1_PARTITIONS 0

  #define DATALIGHT_TASK_MEM  0

/*
 * NAN SWE
 */
#ifdef RVM_NAN_SWE
  #include "nan/nan_pool_size.h"
#define NAN_PARTITION_0_SIZE 105
  #define NAN_PARTITION_1_SIZE 1225

/* number of partitions */
  #define NAN_0_PARTITIONS 20
  #define NAN_1_PARTITIONS 2

  #define NAN_TASK_MEM  NAN_BM_STACK_SIZE+QUE_SIZE(10)
#else
 #define NAN_PARTITION_0_SIZE 0
  #define NAN_PARTITION_1_SIZE 0

/* number of partitions */
  #define NAN_0_PARTITIONS 0
  #define NAN_1_PARTITIONS 0

  #define NAN_TASK_MEM  0
#endif

/*
 * NOR_BM SWE
 */
#ifdef RVM_NOR_BM_SWE
  #include "nor_bm/nor_bm_pool_size.h"
#define NOR_BM_PARTITION_0_SIZE 105
#define NOR_BM_PARTITION_1_SIZE 1225

/* number of partitions */
  #define NOR_BM_0_PARTITIONS 20
  #define NOR_BM_1_PARTITIONS 2
  #define NOR_BM_TASK_MEM  NOR_BM_STACK_SIZE+QUE_SIZE(10)
#else

#define NOR_BM_PARTITION_0_SIZE 0

/* number of partitions */
  #define NOR_BM_0_PARTITIONS 0
  #define NOR_BM_1_PARTITIONS 0

  #define NOR_BM_TASK_MEM  0
#endif
#endif


/*
 * GBI SWE (Generic Block Interface)
 */
#ifdef RVM_GBI_SWE
  #include "gbi/gbi_pool_size.h"
 #define GBI_PARTITION_0_SIZE 35
  #define GBI_PARTITION_1_SIZE 45
  #define GBI_PARTITION_2_SIZE 55
  #define GBI_PARTITION_3_SIZE 65
 #define GBI_PARTITION_4_SIZE 75
  #define GBI_PARTITION_5_SIZE 85
  #define GBI_PARTITION_6_SIZE 135
  #define GBI_PARTITION_7_SIZE 155
 #define GBI_PARTITION_8_SIZE 235
  #define GBI_PARTITION_9_SIZE 545
  #define GBI_PARTITION_10_SIZE 645
  #define GBI_PARTITION_11_SIZE 3025


/* number of partitions */
  #define GBI_0_PARTITIONS 40
  #define GBI_1_PARTITIONS 40
  #define GBI_2_PARTITIONS 20
  #define GBI_3_PARTITIONS 20
  #define GBI_4_PARTITIONS 20
  #define GBI_5_PARTITIONS 20
  #define GBI_6_PARTITIONS 20
  #define GBI_7_PARTITIONS 20
  #define GBI_8_PARTITIONS 20
  #define GBI_9_PARTITIONS 10
  #define GBI_10_PARTITIONS 10
  #define GBI_11_PARTITIONS 1

  #define GBI_TASK_MEM  (GBI_STACK_SIZE+QUE_SIZE(10))
#else
  #define GBI_PARTITION_0_SIZE 0
  #define GBI_PARTITION_1_SIZE 0
  #define GBI_PARTITION_2_SIZE 0
  #define GBI_PARTITION_3_SIZE 0
  #define GBI_PARTITION_4_SIZE 0
  #define GBI_PARTITION_5_SIZE 0
  #define GBI_PARTITION_6_SIZE 0
  #define GBI_PARTITION_7_SIZE 0
  #define GBI_PARTITION_8_SIZE 0
  #define GBI_PARTITION_9_SIZE 0
  #define GBI_PARTITION_10_SIZE 0
  #define GBI_PARTITION_11_SIZE 0


/* number of partitions */
  #define GBI_0_PARTITIONS 0
  #define GBI_1_PARTITIONS 0
  #define GBI_2_PARTITIONS 0
  #define GBI_3_PARTITIONS 0
  #define GBI_4_PARTITIONS 0
  #define GBI_5_PARTITIONS 0
  #define GBI_6_PARTITIONS 0
  #define GBI_7_PARTITIONS 0
  #define GBI_8_PARTITIONS 0
  #define GBI_9_PARTITIONS 0
  #define GBI_10_PARTITIONS 0
  #define GBI_11_PARTITIONS 0

  #define GBI_TASK_MEM  0
#endif

/*
 * RFSFAT SWE (FAT16 File System Core)
 */
#ifdef RVM_RFSFAT_SWE
  #include "rfsfat/rfsfat_pool_size.h"
  #define RFSFAT_PARTITION_0_SIZE 35
  #define RFSFAT_PARTITION_1_SIZE 45
  #define RFSFAT_PARTITION_2_SIZE 55
  #define RFSFAT_PARTITION_3_SIZE 65
  #define RFSFAT_PARTITION_4_SIZE 75
  #define RFSFAT_PARTITION_5_SIZE 105
  #define RFSFAT_PARTITION_6_SIZE 155
  #define RFSFAT_PARTITION_7_SIZE 545
  #define RFSFAT_PARTITION_8_SIZE 805
  #define RFSFAT_PARTITION_9_SIZE 1024
  #define RFSFAT_PARTITION_10_SIZE 10240
/* number of partitions */
  #define RFSFAT_0_PARTITIONS 5
  #define RFSFAT_1_PARTITIONS 5
  #define RFSFAT_2_PARTITIONS 5
  #define RFSFAT_3_PARTITIONS 5
  #define RFSFAT_4_PARTITIONS 5
  #define RFSFAT_5_PARTITIONS 2
  #define RFSFAT_6_PARTITIONS 1
  #define RFSFAT_7_PARTITIONS 1
  #define RFSFAT_8_PARTITIONS 1
  #define RFSFAT_9_PARTITIONS 5
  #define RFSFAT_10_PARTITIONS 11
  #define RFSFAT_TASK_MEM  (RFSFAT_STACK_SIZE+QUE_SIZE(10))

#else
  #define RFSFAT_PARTITION_0_SIZE 0
  #define RFSFAT_PARTITION_1_SIZE 0
  #define RFSFAT_PARTITION_2_SIZE 0
  #define RFSFAT_PARTITION_3_SIZE 0
  #define RFSFAT_PARTITION_4_SIZE 0
  #define RFSFAT_PARTITION_5_SIZE 0
  #define RFSFAT_PARTITION_6_SIZE 0
  #define RFSFAT_PARTITION_7_SIZE 0
  #define RFSFAT_PARTITION_8_SIZE 0
  #define RFSFAT_PARTITION_9_SIZE 0
  #define RFSFAT_PARTITION_10_SIZE 0
/* number of partitions */
  #define RFSFAT_0_PARTITIONS 0
  #define RFSFAT_1_PARTITIONS 0
  #define RFSFAT_2_PARTITIONS 0
  #define RFSFAT_3_PARTITIONS 0
  #define RFSFAT_4_PARTITIONS 0
  #define RFSFAT_5_PARTITIONS 0
  #define RFSFAT_6_PARTITIONS 0
  #define RFSFAT_7_PARTITIONS 0
  #define RFSFAT_8_PARTITIONS 0
  #define RFSFAT_9_PARTITIONS 0
  #define RFSFAT_10_PARTITIONS 0
  #define RFSFAT_TASK_MEM  0

#endif

/*
 * RFSNAND SWE
 */
#ifdef RVM_RFSNAND_SWE
  #include "rfsnand/rfsnand_pool_size.h"
 #define RFSNAND_PARTITION_0_SIZE 45
  #define RFSNAND_PARTITION_1_SIZE 55
  #define RFSNAND_PARTITION_2_SIZE 65
  #define RFSNAND_PARTITION_3_SIZE 115
  #define RFSNAND_PARTITION_4_SIZE 395
  #define RFSNAND_PARTITION_5_SIZE 545
  #define RFSNAND_PARTITION_6_SIZE 1085

/* number of partitions */
  #define RFSNAND_0_PARTITIONS 10
  #define RFSNAND_1_PARTITIONS 10
  #define RFSNAND_2_PARTITIONS 10
  #define RFSNAND_3_PARTITIONS 10
  #define RFSNAND_4_PARTITIONS 10
  #define RFSNAND_5_PARTITIONS 10
  #define RFSNAND_6_PARTITIONS 10

  #define RFSNAND_TASK_MEM  RFSNAND_STACK_SIZE+QUE_SIZE(10)

#else
 #define RFSNAND_PARTITION_0_SIZE 0
  #define RFSNAND_PARTITION_1_SIZE 0
  #define RFSNAND_PARTITION_2_SIZE 0
  #define RFSNAND_PARTITION_3_SIZE 0
  #define RFSNAND_PARTITION_4_SIZE 0
  #define RFSNAND_PARTITION_5_SIZE 0
  #define RFSNAND_PARTITION_6_SIZE 0

/* number of partitions */
  #define RFSNAND_0_PARTITIONS 0
  #define RFSNAND_1_PARTITIONS 0
  #define RFSNAND_2_PARTITIONS 0
  #define RFSNAND_3_PARTITIONS 0
  #define RFSNAND_4_PARTITIONS 0
  #define RFSNAND_5_PARTITIONS 0
  #define RFSNAND_6_PARTITIONS 0

  #define RFSNAND_TASK_MEM  0

#endif


/*
 * RFS SWE
 */
#ifdef RVM_RFS_SWE
  #include "rfs/rfs_pool_size.h"
 #define RFS_PARTITION_0_SIZE 35
  #define RFS_PARTITION_1_SIZE 45
  #define RFS_PARTITION_2_SIZE 55
  #define RFS_PARTITION_3_SIZE 65
  #define RFS_PARTITION_4_SIZE 75
  #define RFS_PARTITION_5_SIZE 105
  #define RFS_PARTITION_6_SIZE 155
  #define RFS_PARTITION_7_SIZE 545
  #define RFS_PARTITION_8_SIZE 805
/* number of partitions */
  #define RFS_0_PARTITIONS 5
  #define RFS_1_PARTITIONS 5
  #define RFS_2_PARTITIONS 5
  #define RFS_3_PARTITIONS 5
  #define RFS_4_PARTITIONS 5
  #define RFS_5_PARTITIONS 2
  #define RFS_6_PARTITIONS 1
  #define RFS_7_PARTITIONS 1
  #define RFS_8_PARTITIONS 1
  #define RFS_TASK_MEM  (RFS_STACK_SIZE+QUE_SIZE(10))

#else
 #define RFS_PARTITION_0_SIZE 0
  #define RFS_PARTITION_1_SIZE 0
  #define RFS_PARTITION_2_SIZE 0
  #define RFS_PARTITION_3_SIZE 0
  #define RFS_PARTITION_4_SIZE 0
  #define RFS_PARTITION_5_SIZE 0
  #define RFS_PARTITION_6_SIZE 0
  #define RFS_PARTITION_7_SIZE 0
  #define RFS_PARTITION_8_SIZE 0

/* number of partitions */
  #define RFS_0_PARTITIONS 0
  #define RFS_1_PARTITIONS 0
  #define RFS_2_PARTITIONS 0
  #define RFS_3_PARTITIONS 0
  #define RFS_4_PARTITIONS 0
  #define RFS_5_PARTITIONS 0
  #define RFS_6_PARTITIONS 0
  #define RFS_7_PARTITIONS 0
  #define RFS_8_PARTITIONS 0

  #define RFS_TASK_MEM  0
#endif


/*
 * PWR SWE
 */
#ifdef RVM_PWR_SWE
  #include "pwr/pwr_pool_size.h"
#else
#endif

/*
 * LCC SWE
 */
#ifdef RVM_LCC_SWE
  #include "lcc/lcc_pool_size.h"
  #define LCC_PARTITION_0_SIZE 125
  #define LCC_PARTITION_1_SIZE 275
  #define LCC_PARTITION_2_SIZE 525

/* number of partitions */
  #define LCC_0_PARTITIONS 3
  #define LCC_1_PARTITIONS 3
  #define LCC_2_PARTITIONS 1

  #define LCC_TASK_MEM  (LCC_STACK_SIZE+QUE_SIZE(10))
#else
 #define LCC_PARTITION_0_SIZE 0
 #define LCC_PARTITION_1_SIZE 0
 #define LCC_PARTITION_2_SIZE 0


/* number of partitions */
  #define LCC_0_PARTITIONS 0
  #define LCC_1_PARTITIONS 0
  #define LCC_2_PARTITIONS 0

  #define LCC_TASK_MEM  0
#endif

/*
 * R2D SWE
 */
#ifdef RVM_R2D_SWE
  #include "r2d/r2d_pool_size.h"
 #define R2D_PARTITION_0_SIZE 45
 #define R2D_PARTITION_1_SIZE 55
 #define R2D_PARTITION_2_SIZE 65
 #define R2D_PARTITION_3_SIZE 75
 #define R2D_PARTITION_4_SIZE 135
 #define R2D_PARTITION_5_SIZE 925


/* number of partitions */
  #define R2D_0_PARTITIONS 10
  #define R2D_1_PARTITIONS 10
  #define R2D_2_PARTITIONS 10
  #define R2D_3_PARTITIONS 5
  #define R2D_4_PARTITIONS 5
  #define R2D_5_PARTITIONS 2
  #define R2D_TASK_MEM  R2D_STACK_SIZE+QUE_SIZE(2)

#else
 #define R2D_PARTITION_0_SIZE 0
 #define R2D_PARTITION_1_SIZE 0
 #define R2D_PARTITION_2_SIZE 0
 #define R2D_PARTITION_3_SIZE 0
 #define R2D_PARTITION_4_SIZE 0
 #define R2D_PARTITION_5_SIZE 0



/* number of partitions */
  #define R2D_0_PARTITIONS 0
  #define R2D_1_PARTITIONS 0
  #define R2D_2_PARTITIONS 0
  #define R2D_3_PARTITIONS 0
  #define R2D_4_PARTITIONS 0
  #define R2D_5_PARTITIONS 0

  #define R2D_TASK_MEM  0
#endif

/* lcd task memory requirment */
#ifdef RVM_LCD_SWE
#include "lcd/lcd_pool_size.h"
#define LCD_TASK_MEM  (LCD_STACK_SIZE+QUE_SIZE(2))
#else
#define LCD_TASK_MEM  0

#endif

/*
 * RGUI SWE
 */
#ifdef RVM_RGUI_SWE
#endif

/*
 * RNET SWE
 */
#ifdef RVM_RNET_SWE
#endif

/*
 * RNET_BR SWE
 */
#ifdef RVM_RNET_BR_SWE
#endif

/*
 * RNET_RT SWE
 */
#ifdef RVM_RNET_RT_SWE
#endif

/*
 * RNET_WS SWE
 */
#ifdef RVM_RNET_WS_SWE
#endif

/*
 * RTC SWE
 */
#ifdef RVM_RTC_SWE
  #include "rtc/rtc_pool_size.h"
 #define RTC_PARTITION_0_SIZE 35
 #define RTC_PARTITION_1_SIZE 45



/* number of partitions */
  #define RTC_0_PARTITIONS 5
  #define RTC_1_PARTITIONS 3

  #define RTC_TASK_MEM  (RTC_STACK_SIZE+QUE_SIZE(10))
#else
 #define RTC_PARTITION_0_SIZE 0
 #define RTC_PARTITION_1_SIZE 0


/* number of partitions */
  #define RTC_0_PARTITIONS 0
  #define RTC_1_PARTITIONS 0
  #define RTC_TASK_MEM  0
#endif


/*
 * SMBS SWE
 */
#ifdef RVM_SMBS_SWE
#endif

/*
 * SPI SWE
 */
#ifdef RVM_SPI_SWE
#endif

/*
 * TI_PRF SWE
 */
#ifdef RVM_TI_PRF_SWE
#endif

/*
 * TUT SWE
 */
#ifdef RVM_TUT_SWE
#endif

/*
 * TTY SWE
 */
#ifdef RVM_TTY_SWE
  #include "tty/tty_pool_size.h"
 #define TTY_PARTITION_0_SIZE 45
 #define TTY_PARTITION_1_SIZE 75
 #define TTY_PARTITION_2_SIZE 125


/* number of partitions */

  #define TTY_0_PARTITIONS 30
  #define TTY_1_PARTITIONS 10
  #define TTY_2_PARTITIONS 10

  #define TTY_TASK_MEM  (TTY_STACK_SIZE+QUE_SIZE(10))
#else
 #define TTY_PARTITION_0_SIZE 0
 #define TTY_PARTITION_1_SIZE 0
 #define TTY_PARTITION_2_SIZE 0

/* number of partitions */
  #define TTY_0_PARTITIONS 0
  #define TTY_1_PARTITIONS 0
  #define TTY_2_PARTITIONS 0
  #define TTY_TASK_MEM  0
#endif



/*** Bluetooth SWE (to move in a widcomm/widcomm_pool_size.h) ***/

/*
 * BTA SWE (Bluetooth SWE)
 */
#ifdef RVM_BTA_SWE
#endif

/*
 * BTH SWE (Bluetooth SWE)
 */
#ifdef RVM_BTH_SWE
#endif

/*
 * BTU SWE (Bluetooth SWE)
 */
#ifdef RVM_BTU_SWE
#endif

/*
 * BTUI SWE (Bluetooth SWE)
 */
#ifdef RVM_BTUI_SWE
 #endif

/*
 * GKI SWE (Bluetooth SWE)
 */
#ifdef RVM_GKI_SWE
 #endif

/*
 * JPEG SWE (JPEG SWE)
 */
#ifdef RVM_JPEG_SWE
//  #include "jpeg_pool_size.h"
 #else
 #endif

#ifdef RVM_CRY_SWE
  #include "cry/cry_pool_size.h"
#define CRY_PARTITION_0_SIZE 54
 #define CRY_PARTITION_1_SIZE 64

/* number of partitions */
  #define CRY_0_PARTITIONS 10
  #define CRY_1_PARTITIONS 5
  #define CRY_TASK_MEM  CRY_STACK_SIZE+QUE_SIZE(10)
 #else
 #define CRY_PARTITION_0_SIZE 0
 #define CRY_PARTITION_1_SIZE 0

/* number of partitions */
  #define CRY_0_PARTITIONS 0
  #define CRY_1_PARTITIONS 0
  #define CRY_TASK_MEM  0
 #endif

#ifdef RVM_RVT_SWE
 #include "rvt/rvt_pool_size.h"
 #define RVT_TASK_MEM  (TRACE_STACK_SIZE+QUE_SIZE(2))
   #else
#define RVT_TASK_MEM  0
 #endif

#if (TEST==1)
  #include "tests/rtest/rtest_pool_size.h"
  #define RV_TEST_TASK_MEM  RTEST_STACK_SIZE+QUE_SIZE(10)
  #define RTEST_TASK_MEM  RTEST_STACK_SIZE+QUE_SIZE(10)
#else
  #define RV_TEST_TASK_MEM  0
  #define RTEST_TASK_MEM  0
 #endif


/*
 * HCI SWE (Bluetooth SWE)
 */
#ifdef RVM_HCI_SWE
 #endif

/*
 * RPC SWE (Bluetooth SWE)
 */
#ifdef RVM_RPC_SWE
#endif

#if (LOCOSTO_LITE==1)
#define BSP_PARTITION_0_SIZE     35
#define BSP_PARTITION_0_USB_SIZE     40
#define BSP_PARTITION_1_SIZE 	45
#define BSP_PARTITION_2_SIZE 	55
#define BSP_PARTITION_3_SIZE 	65
#define BSP_PARTITION_4_SIZE 	75
#define BSP_PARTITION_5_SIZE 	85
#define BSP_PARTITION_6_SIZE 	95
#define BSP_PARTITION_7_SIZE 	105
#define BSP_PARTITION_8_SIZE 	115
#define BSP_PARTITION_9_SIZE 	125
#define BSP_PARTITION_10_SIZE 	155
#define BSP_PARTITION_11_SIZE 	185
#define BSP_PARTITION_12_SIZE 	225
#define BSP_PARTITION_13_SIZE 	235
#define BSP_PARTITION_14_SIZE 	255
#define BSP_PARTITION_15_SIZE 	275
#define BSP_PARTITION_16_SIZE 	285
#define BSP_PARTITION_17_SIZE 	295
#define BSP_PARTITION_18_SIZE 	315
#define BSP_PARTITION_19_SIZE 	400
#define BSP_PARTITION_20_SIZE 	445
#define BSP_PARTITION_21_SIZE 	525
#define BSP_PARTITION_22_SIZE 	545
#define BSP_PARTITION_23_SIZE 	645
#define BSP_PARTITION_24_SIZE 	805
#define BSP_PARTITION_25_SIZE 	925
#define BSP_PARTITION_26_SIZE 	1085
#define BSP_PARTITION_27_SIZE 	1225
#define BSP_PARTITION_28_SIZE 	2075 //2025
#define BSP_PARTITION_29_SIZE 	3025
#define BSP_PARTITION_30_SIZE 	4435
#define BSP_PARTITION_31_SIZE 	5035
#define BSP_PARTITION_31_BAE_SIZE 	8025
#define BSP_PARTITION_32_SIZE   19225
#define BSP_PARTITION_34_SIZE   65100
#define BSP_PARTITION_35_SIZE   76825
#define BSP_PARTITION_36_SIZE   153625
#define BSP_PARTITION_37_SIZE   450000
#define BSP_PARTITION_38_SIZE   620025




#ifdef  RVM_USBFAX_SWE
#define BSPPOOL_0_PARTITIONS (7+USBFAX_0_PARTITIONS)     //35
#else
#define BSPPOOL_0_PARTITIONS (7)     //35
#endif

#define BSPPOOL_0_USB_PARTITIONS 20

#ifdef  RVM_USBFAX_SWE
#define BSPPOOL_1_PARTITIONS (19+USBFAX_1_PARTITIONS) //45
#define BSPPOOL_2_PARTITIONS (3+USBFAX_2_PARTITIONS)  //55
#else
#define BSPPOOL_1_PARTITIONS (10) //45
#define BSPPOOL_2_PARTITIONS (3)  //55
#endif

#define BSPPOOL_3_PARTITIONS (3)  //65
#define BSPPOOL_4_PARTITIONS (5)  //75
#define BSPPOOL_5_PARTITIONS (4) //85
#define BSPPOOL_6_PARTITIONS (0) //95
#define BSPPOOL_7_PARTITIONS 2 //105
#define BSPPOOL_8_PARTITIONS (2) //115
#define BSPPOOL_9_PARTITIONS (10) //125
#define BSPPOOL_10_PARTITIONS (1) //155
#define BSPPOOL_11_PARTITIONS 0 //185
#define BSPPOOL_12_PARTITIONS (1) //225
#define BSPPOOL_13_PARTITIONS 19 //235
#define BSPPOOL_14_PARTITIONS 0 //255
#define BSPPOOL_15_PARTITIONS 0 //275
#define BSPPOOL_16_PARTITIONS (0) //285
#define BSPPOOL_17_PARTITIONS 2 //295
#if ((LOCOSTO_LITE == 1) && (L1_GTT == 1) && (!defined( RVM_BAE_SWE)))
#define BSPPOOL_18_PARTITIONS 4 //315
#else
#define BSPPOOL_18_PARTITIONS 1 //315
#endif
#define BSPPOOL_19_PARTITIONS 0 //400
#define BSPPOOL_20_PARTITIONS 1 //445
#define BSPPOOL_21_PARTITIONS 1 //525
#define BSPPOOL_22_PARTITIONS (0) //545
#define BSPPOOL_23_PARTITIONS (0) //645
#if ((LOCOSTO_LITE == 1) && (L1_GTT == 1) && (!defined( RVM_BAE_SWE)))
 #define BSPPOOL_24_PARTITIONS (3) //805
#else
#define BSPPOOL_24_PARTITIONS (1) //805
#endif
#define BSPPOOL_25_PARTITIONS (0) //925
#define BSPPOOL_26_PARTITIONS 1 //1085
#if ((LOCOSTO_LITE == 1) && (L1_GTT == 1) && (!defined( RVM_BAE_SWE)))
#define BSPPOOL_27_PARTITIONS 3 //1225
#else
#define BSPPOOL_27_PARTITIONS 0 //1225
#endif
#if (WCP_PROF == 1)
#define BSPPOOL_28_PARTITIONS (5) //2075
#else
#define BSPPOOL_28_PARTITIONS (4) //2075
#endif
#define BSPPOOL_29_PARTITIONS (0) //3025

#ifdef  RVM_USBFAX_SWE
#define BSPPOOL_30_PARTITIONS (0+USBFAX_3_PARTITIONS) //4435
#else
#define BSPPOOL_30_PARTITIONS 0 //4435
#endif

#define BSPPOOL_31_PARTITIONS 0 //5035
#define BSPPOOL_31_BAE_PARTITIONS (0) //8025
#define BSPPOOL_32_PARTITIONS 0 //19225
#define BSPPOOL_33_PARTITIONS 0 //51025
#define BSPPOOL_34_PARTITIONS 0 //65100
#define BSPPOOL_35_PARTITIONS 0 //76825
#define BSPPOOL_36_PARTITIONS 0 //153625
#define BSPPOOL_37_PARTITIONS 0 //450000
#define BSPPOOL_38_PARTITIONS 0//620025
#define BSPPOOL_39_PARTITIONS 0//77500

/* RVT Partition Pools COnfiguration */
/* Number of configurations */

#define BSPPOOL_RVT_0_PARTITIONS	65
#define BSPPOOL_RVT_1_PARTITIONS	40
#define BSPPOOL_RVT_2_PARTITIONS	25
#define BSPPOOL_RVT_3_PARTITIONS	0
#define BSPPOOL_RVT_4_PARTITIONS	0
#define BSPPOOL_RVT_5_PARTITIONS	0

#define BSP_PARTITION_RVT_0_SIZE   8
#define BSP_PARTITION_RVT_1_SIZE   50
#define BSP_PARTITION_RVT_2_SIZE   100
#define BSP_PARTITION_RVT_3_SIZE   200
#define BSP_PARTITION_RVT_4_SIZE   300
#define BSP_PARTITION_RVT_5_SIZE   500
/*#define BSP_PARTITION_OVERHEAD 3100 - Use this if BAE is enabled and uses Partition Pool. Not needed if BAE allocs from Dynamic pool which is the case currently*/
#ifdef RVM_USBFAX_SWE
#define BSP_PARTITION_OVERHEAD 6500
#else
#define BSP_PARTITION_OVERHEAD 2500

#endif

#if (REMU==1)
#define EXT_DATA_POOL_BSP_SIZE (	AS_TASK_MEM+\
								AUDIO_TASK_MEM+\
								CAMD_TASK_MEM+\
								CRY_TASK_MEM+\
								DAR_TASK_MEM+\
								DMA_TASK_MEM+\
								ETM_TASK_MEM+\
								FFS_TASK_MEM+\
								GBI_TASK_MEM+\
								HASH_TASK_MEM+\
								KPD_TASK_MEM+\
								LCC_TASK_MEM+\
								NAN_TASK_MEM+\
								NOR_BM_TASK_MEM+\
								DATALIGHT_TASK_MEM+\
								MKS_TASK_MEM+\
								R2D_TASK_MEM+\
								LCD_TASK_MEM+\
								RFS_TASK_MEM+\
								RFSFAT_TASK_MEM+\
								RFSNAND_TASK_MEM+\
								RTC_TASK_MEM+\
								RVT_TASK_MEM+\
								TTY_TASK_MEM+\
								USB_TASK_MEM+\
								USBFAX_TASK_MEM+\
								USBTRC_TASK_MEM+\
								USBMS_TASK_MEM+\
								RTEST_TASK_MEM+\
								RV_TEST_TASK_MEM+\
								BTAV_TASK_MEM+\
								BTS_TASK_MEM+\
								BTT_TASK_MEM+\
								BTU_TASK_MEM+\
								FMS_TASK_MEM+\
								BSP_PARTITION_OVERHEAD)

#define INT_DATA_POOL_BSP_SIZE 0
#if (TEST==1)
#define EXT_RVT_DATA_POOL_BSP_SIZE 75000
#else
#ifdef MEMORY_SUPERVISION
#define EXT_RVT_DATA_POOL_BSP_SIZE 50000
#else
#define EXT_RVT_DATA_POOL_BSP_SIZE 5000
#endif
#endif
#else
#define EXT_DATA_POOL_BSP_SIZE 0
#define EXT_DATA_POOL_BSP_SIZE 0
#define EXT_RVT_DATA_POOL_BSP_SIZE 0
#endif

#else //LOCOSTO PLUS

#define BSP_PARTITION_0_SIZE    35
#define BSP_PARTITION_1_SIZE 	45
#define BSP_PARTITION_2_SIZE 	55
#define BSP_PARTITION_3_SIZE 	65
#define BSP_PARTITION_4_SIZE 	75
#define BSP_PARTITION_5_SIZE 	85
#define BSP_PARTITION_6_SIZE 	95
#define BSP_PARTITION_7_SIZE 	105
#define BSP_PARTITION_8_SIZE 	115
#define BSP_PARTITION_9_SIZE 	125
#define BSP_PARTITION_10_SIZE 	155
#define BSP_PARTITION_11_SIZE 	185
#define BSP_PARTITION_12_SIZE 	225
#define BSP_PARTITION_13_SIZE 	235
#define BSP_PARTITION_14_SIZE 	255
#define BSP_PARTITION_15_SIZE 	275
#define BSP_PARTITION_16_SIZE 	285
#define BSP_PARTITION_17_SIZE 	295
#define BSP_PARTITION_18_SIZE 	315
#define BSP_PARTITION_19_SIZE 	400
#define BSP_PARTITION_20_SIZE 	445
#define BSP_PARTITION_21_SIZE 	525
#define BSP_PARTITION_22_SIZE 	545
#define BSP_PARTITION_23_SIZE 	645
#define BSP_PARTITION_24_SIZE 	805
#define BSP_PARTITION_25_SIZE 	925
#define BSP_PARTITION_26_SIZE 	1085
#define BSP_PARTITION_27_SIZE 	1225
#define BSP_PARTITION_28_SIZE 	2500
#define BSP_PARTITION_29_SIZE 	3025
#define BSP_PARTITION_30_SIZE 	4435
#define BSP_PARTITION_31_SIZE 	5035
#define BSP_PARTITION_32_SIZE   19225
#if (L1_PCM_EXTRACTION==1)
#define BSP_PARTITION_32_NEW_SIZE   40985
#endif
#define BSP_PARTITION_33_SIZE   32768 

/*Snapshot buffer for CAMD*/
/*  #define BSP_PARTITION_34_SIZE 1300*1024*2  */

/* Buffer for FileSystem performance Testcase */
#define BSP_PARTITION_34_SIZE 524400


#define BSPPOOL_0_PARTITIONS    	(	AUDIO_0_PARTITIONS +\
									ETM_0_PARTITIONS+\
									FFS_0_PARTITIONS+\
									KPD_0_PARTITIONS+\
									USBFAX_0_PARTITIONS+\
									GBI_0_PARTITIONS+\
									RFSFAT_0_PARTITIONS+\
									RFS_0_PARTITIONS+\
									RTC_0_PARTITIONS)

#define BSPPOOL_1_PARTITIONS    	(	AUDIO_1_PARTITIONS+\
									AS_0_PARTITIONS+\
									DAR_0_PARTITIONS+\
									DMA_0_PARTITIONS+\
									KPD_1_PARTITIONS+\
									USB_0_PARTITIONS+\
									USBMS_0_PARTITIONS+\
									USBFAX_1_PARTITIONS+\
									GBI_1_PARTITIONS+\
									RFSFAT_1_PARTITIONS+\
									RFSNAND_0_PARTITIONS+\
									RFS_1_PARTITIONS+\
									R2D_0_PARTITIONS+\
									RTC_1_PARTITIONS+\
									TTY_0_PARTITIONS)


#define BSPPOOL_2_PARTITIONS    	(	AUDIO_2_PARTITIONS+\
									AS_1_PARTITIONS+\
									CAMD_0_PARTITIONS+\
									DMA_1_PARTITIONS+\
									FFS_1_PARTITIONS+\
									KPD_2_PARTITIONS+\
									USB_1_PARTITIONS+\
									USBFAX_2_PARTITIONS+\
									GBI_2_PARTITIONS+\
									RFSFAT_2_PARTITIONS+\
									RFSNAND_1_PARTITIONS+\
									RFS_2_PARTITIONS+\
									R2D_1_PARTITIONS)

#define BSPPOOL_3_PARTITIONS    	(	AUDIO_3_PARTITIONS+\
									DAR_1_PARTITIONS+\
									KPD_3_PARTITIONS+\
									USBMS_1_PARTITIONS+\
									GBI_3_PARTITIONS+\
									RFSFAT_3_PARTITIONS+\
									RFSNAND_2_PARTITIONS+\
									RFS_3_PARTITIONS+\
									R2D_2_PARTITIONS+\
									CAMD_INT_0_PARTITIONS)


#define BSPPOOL_4_PARTITIONS    	(	AUDIO_4_PARTITIONS+\
									DMA_2_PARTITIONS+\
									ETM_1_PARTITIONS+\
									FFS_2_PARTITIONS+\
									HASH_0_PARTITIONS+\
									KPD_4_PARTITIONS+\
									USB_2_PARTITIONS+\
									GBI_4_PARTITIONS+\
									RFSFAT_4_PARTITIONS+\
									RFS_4_PARTITIONS+\
									R2D_3_PARTITIONS+\
									TTY_1_PARTITIONS)


#define BSPPOOL_5_PARTITIONS    	(	AUDIO_5_PARTITIONS+\
									DAR_2_PARTITIONS+\
									KPD_5_PARTITIONS+\
									MKS_0_PARTITIONS+\
									GBI_5_PARTITIONS)


#define BSPPOOL_6_PARTITIONS    	(	AUDIO_6_PARTITIONS+\
									CAMD_1_PARTITIONS+\
									USB_3_PARTITIONS+\
									USBTRC_0_PARTITIONS)


#define BSPPOOL_7_PARTITIONS    	(	AS_2_PARTITIONS+\
									USB_4_PARTITIONS+\
									RFSFAT_5_PARTITIONS+\
									RFS_5_PARTITIONS+\
									NAN_0_PARTITIONS+\
									DATALIGHT_0_PARTITIONS+\
									NOR_BM_0_PARTITIONS)


#define BSPPOOL_8_PARTITIONS   ( HASH_1_PARTITIONS+\
									KPD_6_PARTITIONS+\
									RFSNAND_3_PARTITIONS)


#define BSPPOOL_9_PARTITIONS   ( DAR_3_PARTITIONS+\
									CAMD_INT_1_PARTITIONS+\
									USBTRC_1_PARTITIONS+\
									LCC_0_PARTITIONS+\
									TTY_2_PARTITIONS+\
									CAMD_INT_1_PARTITIONS)


#define BSPPOOL_10_PARTITIONS   	(	GBI_6_PARTITIONS+\
									R2D_4_PARTITIONS+\
									USBTRC_2_PARTITIONS+\
									USBMS_2_PARTITIONS+\
									GBI_7_PARTITIONS+\
									RFSFAT_6_PARTITIONS+\
									RFS_6_PARTITIONS)


#define BSPPOOL_11_PARTITIONS   		(CAMD_2_PARTITIONS +8)

#define BSPPOOL_12_PARTITIONS   		(USBTRC_3_PARTITIONS)

#define BSPPOOL_13_PARTITIONS   		GBI_8_PARTITIONS

#define BSPPOOL_14_PARTITIONS   		AS_3_PARTITIONS

#define BSPPOOL_15_PARTITIONS   	(	DAR_4_PARTITIONS+\
									LCC_1_PARTITIONS)


#define BSPPOOL_16_PARTITIONS    0 //MOVED TO MM POOL

#define BSPPOOL_17_PARTITIONS   	(	ETM_2_PARTITIONS+\
									FFS_3_PARTITIONS+\
									USBMS_3_PARTITIONS+8)


#define BSPPOOL_18_PARTITIONS   	(	ETM_3_PARTITIONS+\
									FFS_4_PARTITIONS+\
									MSL_4_PARTITIONS+4)


#define BSPPOOL_19_PARTITIONS   		RFSNAND_4_PARTITIONS

#define BSPPOOL_20_PARTITIONS    		(USB_5_PARTITIONS+4)

#define BSPPOOL_21_PARTITIONS    	(	DAR_5_PARTITIONS+\
									CAMD_INT_2_PARTITIONS+\
									LCC_2_PARTITIONS)


#define BSPPOOL_22_PARTITIONS  ( GBI_9_PARTITIONS+\
									RFSFAT_7_PARTITIONS+\
									RFSNAND_5_PARTITIONS+\
									RFS_7_PARTITIONS)


#define BSPPOOL_23_PARTITIONS  ( GBI_10_PARTITIONS)


#define BSPPOOL_24_PARTITIONS  ( RFSFAT_8_PARTITIONS+\
									RFS_8_PARTITIONS)


#define BSPPOOL_25_PARTITIONS   	R2D_5_PARTITIONS

#define BSPPOOL_26_PARTITIONS   	(	USBTRC_4_PARTITIONS+\
						DATALIGHT_1_PARTITIONS+\
		                                NOR_BM_1_PARTITIONS+\
                                 RFSNAND_6_PARTITIONS+\
                                 RFSFAT_9_PARTITIONS)

#define BSPPOOL_27_PARTITIONS   		NAN_1_PARTITIONS

#define BSPPOOL_28_PARTITIONS   	(	AUDIO_7_PARTITIONS+\
									DMA_3_PARTITIONS+\
									ETM_4_PARTITIONS+\
									FFS_5_PARTITIONS)


#define BSPPOOL_29_PARTITIONS   	(GBI_11_PARTITIONS)


#define BSPPOOL_30_PARTITIONS    	USBFAX_3_PARTITIONS

#define BSPPOOL_31_PARTITIONS    	USBTRC_5_PARTITIONS


#define BSPPOOL_32_PARTITIONS  ( USBMS_INT_0_PARTITIONS+\
                                 RFSFAT_10_PARTITIONS)
#if (L1_PCM_EXTRACTION==1)
#define BSPPOOL_32_NEW_PARTITIONS   	( AUDIO_8_PARTITIONS )
#endif

#if (TEST==1)
#define BSPPOOL_33_PARTITIONS     (6)     /* For Reliance and FlashFx */ 
#else
#define BSPPOOL_33_PARTITIONS     0
#endif

#if (TEST==1)
/* For Camera test case */
/*
#ifdef RVM_CAMD_SWE
    #define BSPPOOL_34_PARTITIONS     (1)
#endif
*/
	#define BSPPOOL_34_PARTITIONS  (4)      /* For Filesystem performance test cases */
#else
    #define BSPPOOL_34_PARTITIONS   (0)
#endif





/* BSP Internla Memory requirements */

/* Partition sizes */

#define BSP_INT_PARTITION_0_SIZE    35
#define BSP_INT_PARTITION_1_SIZE    45
#define BSP_INT_PARTITION_2_SIZE    55
#define BSP_INT_PARTITION_3_SIZE    65
#define BSP_INT_PARTITION_4_SIZE    75
#define BSP_INT_PARTITION_5_SIZE    85
#define BSP_INT_PARTITION_6_SIZE    96
#define BSP_INT_PARTITION_7_SIZE    125
#define BSP_INT_PARTITION_8_SIZE    215
#define BSP_INT_PARTITION_9_SIZE    680
#define BSP_INT_PARTITION_10_SIZE    2025
#define BSP_INT_PARTITION_11_SIZE    8025



/* number of partitions */
#define BSPPOOL_INT_0_PARTITIONS        5//MOVED TO MM POOL


#define BSPPOOL_INT_1_PARTITIONS   (AUDIO_INT_0_PARTITIONS)
#define BSPPOOL_INT_2_PARTITIONS   (AUDIO_INT_1_PARTITIONS)
#define BSPPOOL_INT_3_PARTITIONS    0//MOVED TO MM POOL
#define BSPPOOL_INT_4_PARTITIONS    0//MOVED TO MM POOL
#define BSPPOOL_INT_5_PARTITIONS     0//MOVED TO MM POOL
#define BSPPOOL_INT_6_PARTITIONS     0//MOVED TO MM POOL
#define BSPPOOL_INT_7_PARTITIONS     0//MOVED TO MM POOL
#define BSPPOOL_INT_8_PARTITIONS     0//MOVED TO MM POOL
#define BSPPOOL_INT_9_PARTITIONS     2//MOVED TO MM POOL
#define SPPOOL_INT_10_PARTITIONS     0//MOVED TO MM POOL
#define BSPPOOL_INT_11_PARTITIONS     0//MOVED TO MM POOL

/* RVT Partition Pools COnfiguration */
/* RVT Partition Sizes */
#define BSP_PARTITION_RVT_0_SIZE   8
#define BSP_PARTITION_RVT_1_SIZE   50
#define BSP_PARTITION_RVT_2_SIZE   100
#define BSP_PARTITION_RVT_3_SIZE   200
#define BSP_PARTITION_RVT_4_SIZE   300
#define BSP_PARTITION_RVT_5_SIZE   500


/* Number of configurations */
#define BSPPOOL_RVT_0_PARTITIONS	310
#define BSPPOOL_RVT_1_PARTITIONS	100
#define BSPPOOL_RVT_2_PARTITIONS	100
#define BSPPOOL_RVT_3_PARTITIONS	50
#define BSPPOOL_RVT_4_PARTITIONS	30
#define BSPPOOL_RVT_5_PARTITIONS	30
#define BSP_PARTITION_OVERHEAD 6000
#define	MLUN_OVERHEAD	16*1024				// added For MLUN Support

#if (REMU==1)
#define EXT_DATA_POOL_BSP_SIZE (	AS_TASK_MEM+\
								CAMD_TASK_MEM+\
								CRY_TASK_MEM+\
								DAR_TASK_MEM+\
								DMA_TASK_MEM+\
								ETM_TASK_MEM+\
								FFS_TASK_MEM+\
								GBI_TASK_MEM+\
								HASH_TASK_MEM+\
								KPD_TASK_MEM+\
								LCC_TASK_MEM+\
								NAN_TASK_MEM+\
								MKS_TASK_MEM+\
								R2D_TASK_MEM+\
								LCD_TASK_MEM+\
								RFS_TASK_MEM+\
								RFSFAT_TASK_MEM+\
								RFSNAND_TASK_MEM+\
								RTC_TASK_MEM+\
								RVT_TASK_MEM+\
								TTY_TASK_MEM+\
								USB_TASK_MEM+\
								USBFAX_TASK_MEM+\
								USBTRC_TASK_MEM+\
								USBMS_TASK_MEM+\
								RTEST_TASK_MEM+\
								RV_TEST_TASK_MEM+\
								MLUN_OVERHEAD+\
								BTT_TASK_MEM+\
								FMS_TASK_MEM+\
								BSP_PARTITION_OVERHEAD)

#if(PSP_STANDALONE == 1)
#define INT_DATA_POOL_BSP_SIZE  AUDIO_TASK_MEM
#else
    #define INT_DATA_POOL_BSP_SIZE  (AUDIO_TASK_MEM+BT_TASK_MEM)// removed for internal memory constraints by MM +1000
#endif

#ifdef MEMORY_SUPERVISION
#define EXT_RVT_DATA_POOL_BSP_SIZE 75000
#else
#define EXT_RVT_DATA_POOL_BSP_SIZE 25000
#endif

#else
#define EXT_DATA_POOL_BSP_SIZE 0
#define EXT_DATA_POOL_BSP_SIZE 0
#define EXT_RVT_DATA_POOL_BSP_SIZE 0
#endif

#endif

#endif // __RVF_POOL_SIZE_H_



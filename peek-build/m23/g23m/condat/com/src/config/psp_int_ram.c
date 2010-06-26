/* This file contains internal memory partition pool configuration for PSP drivers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "ccdapi.h"
#include "pei.h"
#include "tok.h"
#include "os_const.h"
#include "frm_types.h"
#include "gsm_bsp_pool_size.h"

#if(PSP_STANDALONE==1)
char * str2ind_version = "&0";
#endif

#if (LOCOSTO_LITE==1)
#if (BSPPOOL_0_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool40 [ POOL_SIZE(BSPPOOL_0_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_0_SIZE)) ];
#else
extern char pool40[];
#endif
#endif


#if (BSPPOOL_0_USB_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool40_usb [ POOL_SIZE(BSPPOOL_0_USB_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_0_USB_SIZE)) ];
#else
extern char pool40_usb[];
#endif
#endif

#if (BSPPOOL_1_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool41 [ POOL_SIZE(BSPPOOL_1_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_1_SIZE)) ];
#else
extern char pool41[];
#endif
#endif

#if (BSPPOOL_2_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool42 [ POOL_SIZE(BSPPOOL_2_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_2_SIZE)) ];
#else
extern char pool42[];
#endif
#endif

#if (BSPPOOL_3_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool43 [ POOL_SIZE(BSPPOOL_3_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_3_SIZE)) ];
#else
extern char pool43[];
#endif
#endif

#if (BSPPOOL_4_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool44 [ POOL_SIZE(BSPPOOL_4_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_4_SIZE)) ];
#else
extern char pool44[];
#endif
#endif

#if (BSPPOOL_5_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool45 [ POOL_SIZE(BSPPOOL_5_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_5_SIZE)) ];
#else
extern char pool45[];
#endif
#endif

#if (BSPPOOL_6_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool46 [ POOL_SIZE(BSPPOOL_6_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_6_SIZE)) ];
#else
extern char pool46[];
#endif
#endif

#if (BSPPOOL_7_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool47 [ POOL_SIZE(BSPPOOL_7_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_7_SIZE)) ];
#else
extern char pool47[];
#endif
#endif

#if (BSPPOOL_8_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool48 [ POOL_SIZE(BSPPOOL_8_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_8_SIZE)) ];
#else
extern char pool48[];
#endif
#endif

#if (BSPPOOL_9_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool49 [ POOL_SIZE(BSPPOOL_9_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_9_SIZE)) ];
#else
extern char pool49[];
#endif
#endif

#if (BSPPOOL_10_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool50 [ POOL_SIZE(BSPPOOL_10_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_10_SIZE)) ];
#else
extern char pool50[];
#endif
#endif

#if (BSPPOOL_11_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool51 [ POOL_SIZE(BSPPOOL_11_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_11_SIZE)) ];
#else
extern char pool51[];
#endif
#endif

#if (BSPPOOL_12_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool52 [ POOL_SIZE(BSPPOOL_12_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_12_SIZE)) ];
#else
extern char pool52[];
#endif
#endif

#if (BSPPOOL_13_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool53 [ POOL_SIZE(BSPPOOL_13_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_13_SIZE)) ];
#else
extern char pool53[];
#endif
#endif

#if (BSPPOOL_14_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool54 [ POOL_SIZE(BSPPOOL_14_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_14_SIZE)) ];
#else
extern char pool54[];
#endif
#endif

#if  (BSPPOOL_15_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool55 [ POOL_SIZE(BSPPOOL_15_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_15_SIZE)) ];
#else
extern char pool55[];
#endif
 #endif

#if (BSPPOOL_16_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool56 [ POOL_SIZE(BSPPOOL_16_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_16_SIZE)) ];
 #else
extern char pool56[];
 #endif
 #endif

#if (BSPPOOL_17_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool57 [ POOL_SIZE(BSPPOOL_17_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_17_SIZE)) ];
#else
extern char pool57[];
#endif
 #endif


#if (BSPPOOL_18_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool58 [ POOL_SIZE(BSPPOOL_18_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_18_SIZE)) ];
#else
extern char pool58[];
#endif
 #endif

#if (BSPPOOL_19_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool59 [ POOL_SIZE(BSPPOOL_19_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_19_SIZE)) ];
#else
extern char pool59[];
#endif
 #endif

#if (BSPPOOL_20_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool60 [ POOL_SIZE(BSPPOOL_20_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_20_SIZE)) ];
#else
extern char pool60[];
#endif
 #endif

#if (BSPPOOL_21_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool61 [ POOL_SIZE(BSPPOOL_21_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_21_SIZE)) ];
#else
extern char pool61[];
#endif
 #endif

#if (BSPPOOL_22_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool62 [ POOL_SIZE(BSPPOOL_22_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_22_SIZE)) ];
#else
extern char pool62[];
#endif
 #endif

#if (BSPPOOL_23_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool63 [ POOL_SIZE(BSPPOOL_23_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_23_SIZE)) ];
#else
extern char pool63[];
#endif
 #endif


#if (BSPPOOL_24_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool64 [ POOL_SIZE(BSPPOOL_24_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_24_SIZE)) ];
#else
extern char pool64[];
#endif
 #endif


#if (BSPPOOL_25_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool65 [ POOL_SIZE(BSPPOOL_25_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_25_SIZE)) ];
#else
extern char pool65[];
#endif
#endif


#if (BSPPOOL_26_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool66 [ POOL_SIZE(BSPPOOL_26_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_26_SIZE)) ];
#else
extern char pool66[];
#endif
 #endif


#if (BSPPOOL_27_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool67 [ POOL_SIZE(BSPPOOL_27_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_27_SIZE)) ];
#else
extern char pool67[];
#endif
 #endif

#if (BSPPOOL_28_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool68 [ POOL_SIZE(BSPPOOL_28_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_28_SIZE)) ];
#else
extern char pool68[];
#endif
#endif

#if (BSPPOOL_29_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
 char pool69 [ POOL_SIZE(BSPPOOL_29_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_29_SIZE)) ];
#else
extern char pool69[];
#endif
#endif

#if (BSPPOOL_30_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
 char pool70 [ POOL_SIZE(BSPPOOL_30_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_30_SIZE)) ];
#else
extern char pool70[];
#endif
#endif

#if (BSPPOOL_31_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
 char pool71 [ POOL_SIZE(BSPPOOL_31_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_31_SIZE)) ];
#else
extern char pool71[];
#endif
#endif

#if (BSPPOOL_31_BAE_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
 char pool71_bae [ POOL_SIZE(BSPPOOL_31_BAE_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_31_BAE_SIZE)) ];
#else
extern char pool71_bae[];
#endif
#endif

#if (BSPPOOL_32_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
 char pool72 [ POOL_SIZE(BSPPOOL_32_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_32_SIZE)) ];
#else
extern char pool72[];
#endif
 #endif

#if (BSPPOOL_33_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool73 [ POOL_SIZE(BSPPOOL_33_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_33_SIZE)) ];
#else
extern char pool73[];
#endif
 #endif


#if (BSPPOOL_34_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool74 [ POOL_SIZE(BSPPOOL_34_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_34_SIZE)) ];
#else
extern char pool74[];
#endif
 #endif

#if (BSPPOOL_35_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool75 [ POOL_SIZE(BSPPOOL_35_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_35_SIZE)) ];
#else
extern char pool75[];
#endif
 #endif

#if (BSPPOOL_36_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool76 [ POOL_SIZE(BSPPOOL_36_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_36_SIZE)) ];
#else
extern char pool76[];
#endif
 #endif

#if (BSPPOOL_37_PARTITIONS>0) 
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool77 [ POOL_SIZE(BSPPOOL_37_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_37_SIZE)) ];
#else
extern char pool77[];
#endif
 #endif

#if (BSPPOOL_38_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
 char pool78 [ POOL_SIZE(BSPPOOL_38_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_38_SIZE)) ];
#else
extern char pool78[];
#endif
 #endif


#ifndef DATA_INT_RAM 
const T_FRM_PARTITION_POOL_CONFIG bsp_grp_config[] =
{
#if (BSPPOOL_0_PARTITIONS>0) 
  {BSPPOOL_0_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_0_SIZE), &pool40 },
#endif  
#if (BSPPOOL_0_USB_PARTITIONS>0) 
  {BSPPOOL_0_USB_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_0_USB_SIZE), &pool40_usb },
#endif  
#if (BSPPOOL_1_PARTITIONS>0)
  { BSPPOOL_1_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_1_SIZE), &pool41 },
#endif  
#if (BSPPOOL_2_PARTITIONS>0)
  { BSPPOOL_2_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_2_SIZE), &pool42 },
#endif  
#if (BSPPOOL_3_PARTITIONS>0)
  { BSPPOOL_3_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_3_SIZE), &pool43 },
#endif  
#if (BSPPOOL_4_PARTITIONS>0)
  { BSPPOOL_4_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_4_SIZE), &pool44 },
#endif  
#if (BSPPOOL_5_PARTITIONS>0)
  { BSPPOOL_5_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_5_SIZE), &pool45 },
#endif  
#if (BSPPOOL_6_PARTITIONS>0)
  { BSPPOOL_6_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_6_SIZE), &pool46 },
#endif  
#if (BSPPOOL_7_PARTITIONS>0)
  { BSPPOOL_7_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_7_SIZE), &pool47 }, 
#endif  
#if (BSPPOOL_8_PARTITIONS>0)
  { BSPPOOL_8_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_8_SIZE), &pool48 }, 
#endif  
#if (BSPPOOL_9_PARTITIONS>0)
  { BSPPOOL_9_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_9_SIZE), &pool49 }, 
#endif  
#if (BSPPOOL_10_PARTITIONS>0)
  { BSPPOOL_10_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_10_SIZE), &pool50 },
#endif  
#if (BSPPOOL_11_PARTITIONS>0)
  { BSPPOOL_11_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_11_SIZE), &pool51 },
#endif  
#if (BSPPOOL_12_PARTITIONS>0)
  { BSPPOOL_12_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_12_SIZE), &pool52 },
#endif  
#if (BSPPOOL_13_PARTITIONS>0)
  { BSPPOOL_13_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_13_SIZE), &pool53 },
#endif  
#if (BSPPOOL_14_PARTITIONS>0)
  { BSPPOOL_14_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_14_SIZE), &pool54 },
#endif  
#if (BSPPOOL_15_PARTITIONS>0)
  { BSPPOOL_15_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_15_SIZE), &pool55 },
#endif  
#if (BSPPOOL_16_PARTITIONS>0)
  { BSPPOOL_16_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_16_SIZE), &pool56 },
#endif  
#if (BSPPOOL_17_PARTITIONS>0)
  { BSPPOOL_17_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_17_SIZE), &pool57 }, 
#endif  
#if (BSPPOOL_18_PARTITIONS>0)
  { BSPPOOL_18_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_18_SIZE), &pool58 }, 
#endif  
#if (BSPPOOL_19_PARTITIONS>0)
  { BSPPOOL_19_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_19_SIZE), &pool59 }, 

#endif  
#if (BSPPOOL_20_PARTITIONS>0)
  { BSPPOOL_20_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_20_SIZE), &pool60 },
#endif  
#if (BSPPOOL_21_PARTITIONS>0)
  { BSPPOOL_21_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_21_SIZE), &pool61 },
#endif  
#if (BSPPOOL_22_PARTITIONS>0)
  { BSPPOOL_22_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_22_SIZE), &pool62 },
#endif  
#if (BSPPOOL_23_PARTITIONS>0)
  { BSPPOOL_23_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_23_SIZE), &pool63 },
#endif  
#if (BSPPOOL_24_PARTITIONS>0)
  { BSPPOOL_24_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_24_SIZE), &pool64 },
#endif  
#if (BSPPOOL_25_PARTITIONS>0)
  { BSPPOOL_25_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_25_SIZE), &pool65 },
#endif  
#if (BSPPOOL_26_PARTITIONS>0)
  { BSPPOOL_26_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_26_SIZE), &pool66 },
#endif  
#if (BSPPOOL_27_PARTITIONS>0)
  { BSPPOOL_27_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_27_SIZE), &pool67 },
#endif  
#if (BSPPOOL_28_PARTITIONS>0)
  { BSPPOOL_28_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_28_SIZE), &pool68 }, 
#endif  
#if (BSPPOOL_29_PARTITIONS>0)
  { BSPPOOL_29_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_29_SIZE), &pool69 }, 
#endif  
#if (BSPPOOL_30_PARTITIONS>0)

  { BSPPOOL_30_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_30_SIZE), &pool70 },
#endif  
#if (BSPPOOL_31_PARTITIONS>0)
  { BSPPOOL_31_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_31_SIZE), &pool71 },
#endif  
#if (BSPPOOL_31_BAE_PARTITIONS>0)
  { BSPPOOL_31_BAE_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_31_BAE_SIZE), &pool71_bae },
#endif  
#if (BSPPOOL_32_PARTITIONS>0)
  { BSPPOOL_32_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_32_SIZE), &pool72 },
#endif  
#if (BSPPOOL_33_PARTITIONS>0)
  { BSPPOOL_33_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_33_SIZE), &pool73 },
#endif  
#if (BSPPOOL_34_PARTITIONS>0)
  { BSPPOOL_34_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_34_SIZE), &pool74 },
#endif  
#if (BSPPOOL_35_PARTITIONS>0)
  { BSPPOOL_35_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_35_SIZE), &pool75 },
#endif  
#if (BSPPOOL_36_PARTITIONS>0)
  { BSPPOOL_36_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_36_SIZE), &pool76 },
#endif  
#if (BSPPOOL_37_PARTITIONS>0)
  { BSPPOOL_37_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_37_SIZE), &pool77 },
#endif  
#if (BSPPOOL_38_PARTITIONS>0)
  { BSPPOOL_38_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_38_SIZE), &pool78 },
#endif  
  { 0                    , 0                    , NULL	  }
};
#endif /* !DATA_INT_RAM */


#else

#if (BSPPOOL_INT_0_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_0_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_0_INT_RAM)
char int_pool40 [ POOL_SIZE(BSPPOOL_INT_0_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_0_SIZE)) ];
#else
extern char int_pool40 [];
#endif
#endif

#if (BSPPOOL_INT_1_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_1_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_1_INT_RAM)
char int_pool41 [ POOL_SIZE(BSPPOOL_INT_1_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_1_SIZE)) ];
#else
extern char int_pool41 [];
#endif
#endif

#if (BSPPOOL_INT_2_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_2_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_2_INT_RAM)
char int_pool42 [ POOL_SIZE(BSPPOOL_INT_2_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_2_SIZE)) ];
#else
extern char int_pool42 [];
#endif
#endif

#if (BSPPOOL_INT_3_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_3_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_3_INT_RAM)
char int_pool43 [ POOL_SIZE(BSPPOOL_INT_3_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_3_SIZE)) ];
#else
extern char int_pool43 [];
#endif
#endif

#if (BSPPOOL_INT_4_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_4_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_4_INT_RAM)
char int_pool44 [ POOL_SIZE(BSPPOOL_INT_4_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_4_SIZE)) ];
#else
extern char int_pool44 [];
#endif
#endif

#if (BSPPOOL_INT_5_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_5_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_5_INT_RAM)
char int_pool45 [ POOL_SIZE(BSPPOOL_INT_5_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_5_SIZE)) ];
#else
extern char int_pool45 [];
#endif
#endif

#if (BSPPOOL_INT_6_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_6_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_6_INT_RAM)
char int_pool46 [ POOL_SIZE(BSPPOOL_INT_6_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_6_SIZE)) ];
#else
extern char int_pool46 [];
#endif
#endif

#if (BSPPOOL_INT_7_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_7_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_7_INT_RAM)
char int_pool47 [ POOL_SIZE(BSPPOOL_INT_7_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_7_SIZE)) ];
#else
extern char int_pool47 [];
#endif
#endif

#if (BSPPOOL_INT_8_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_8_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_8_INT_RAM)
char int_pool48 [ POOL_SIZE(BSPPOOL_INT_8_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_8_SIZE)) ];
#else
extern char int_pool48 [];
#endif
#endif

#if (BSPPOOL_INT_9_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_9_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_9_INT_RAM)
char int_pool49 [ POOL_SIZE(BSPPOOL_INT_9_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_9_SIZE)) ];
#else
extern char int_pool49 [];
#endif
#endif

#if (BSPPOOL_INT_10_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_10_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_10_INT_RAM)
char int_pool50 [ POOL_SIZE(BSPPOOL_INT_10_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_10_SIZE)) ];
#else
extern char int_pool50 [];
#endif
#endif

#if (BSPPOOL_INT_11_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_11_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_11_INT_RAM)
char int_pool51 [ POOL_SIZE(BSPPOOL_INT_11_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_11_SIZE)) ];
#else
extern char int_pool51 [];
#endif
#endif

#if (BSPPOOL_INT_JPEG_2_PARTITIONS>0)
char int_pool_JPEG_2 [ POOL_SIZE(BSPPOOL_INT_JPEG_2_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_JPEG_2_SIZE)) ];
#endif

#if (BSPPOOL_INT_JPEG_3_PARTITIONS>0)
char int_pool_JPEG_3 [ POOL_SIZE(BSPPOOL_INT_JPEG_3_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_JPEG_3_SIZE)) ];
#endif

#if (BSPPOOL_INT_JPEG_4_PARTITIONS>0)
char int_pool_JPEG_4 [ POOL_SIZE(BSPPOOL_INT_JPEG_4_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_JPEG_4_SIZE)) ];
#endif

#if (BSPPOOL_INT_JPEG_5_PARTITIONS>0)
char int_pool_JPEG_5 [ POOL_SIZE(BSPPOOL_INT_JPEG_5_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_JPEG_5_SIZE)) ];
#endif

#if (BSPPOOL_INT_JPEG_6_PARTITIONS>0)
char int_pool_JPEG_6 [ POOL_SIZE(BSPPOOL_INT_JPEG_6_PARTITIONS,ALIGN_SIZE(BSP_INT_PARTITION_JPEG_6_SIZE)) ];
#endif




#ifndef DATA_INT_RAM
const T_FRM_PARTITION_POOL_CONFIG bsp_int_grp_config[] =
{
#if (BSPPOOL_INT_0_PARTITIONS>0)
  { BSPPOOL_INT_0_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_0_SIZE), &int_pool40 },
#endif
#if (BSPPOOL_INT_1_PARTITIONS>0)
  { BSPPOOL_INT_1_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_1_SIZE), &int_pool41 },
#endif
#if (BSPPOOL_INT_2_PARTITIONS>0)
  { BSPPOOL_INT_2_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_2_SIZE), &int_pool42 },
#endif
#if (BSPPOOL_INT_3_PARTITIONS>0)
  { BSPPOOL_INT_3_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_3_SIZE), &int_pool43 },
#endif
#if (BSPPOOL_INT_4_PARTITIONS>0)
  { BSPPOOL_INT_4_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_4_SIZE), &int_pool44 },
#endif
#if (BSPPOOL_INT_5_PARTITIONS>0)
  { BSPPOOL_INT_5_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_5_SIZE), &int_pool45 },
#endif
#if (BSPPOOL_INT_6_PARTITIONS>0)
  { BSPPOOL_INT_6_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_6_SIZE), &int_pool46 },
#endif

#if (BSPPOOL_INT_7_PARTITIONS>0)
  { BSPPOOL_INT_7_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_7_SIZE), &int_pool47 }, 
#endif

#if (BSPPOOL_INT_8_PARTITIONS>0)
  { BSPPOOL_INT_8_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_8_SIZE), &int_pool48 }, 
#endif
 
#if (BSPPOOL_INT_JPEG_2_PARTITIONS>0)
  { BSPPOOL_INT_JPEG_2_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_JPEG_2_SIZE), &int_pool_JPEG_2 }, 
#endif
#if (BSPPOOL_INT_JPEG_3_PARTITIONS>0)
  { BSPPOOL_INT_JPEG_3_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_JPEG_3_SIZE), &int_pool_JPEG_3 }, 
#endif
#if (BSPPOOL_INT_JPEG_4_PARTITIONS>0)
  { BSPPOOL_INT_JPEG_4_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_JPEG_4_SIZE), &int_pool_JPEG_4 }, 
#endif
#if (BSPPOOL_INT_JPEG_5_PARTITIONS>0)
  { BSPPOOL_INT_JPEG_5_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_JPEG_5_SIZE), &int_pool_JPEG_5 }, 
#endif
 
#if (BSPPOOL_INT_9_PARTITIONS>0)
  { BSPPOOL_INT_9_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_9_SIZE), &int_pool49 }, 
#endif

#if (BSPPOOL_INT_JPEG_6_PARTITIONS>0)
  { BSPPOOL_INT_JPEG_6_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_JPEG_6_SIZE), &int_pool_JPEG_6 }, 
#endif  

#if (BSPPOOL_INT_10_PARTITIONS>0)
  { BSPPOOL_INT_10_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_10_SIZE), &int_pool50 }, 
#endif
  
#if (BSPPOOL_INT_11_PARTITIONS>0)
  { BSPPOOL_INT_11_PARTITIONS, ALIGN_SIZE(BSP_INT_PARTITION_11_SIZE), &int_pool51 }, 
#endif
  { 0                    , 0                    , NULL	  }
};
#endif /* !DATA_INT_RAM */
#endif






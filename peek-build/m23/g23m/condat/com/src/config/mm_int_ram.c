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
#include "gsm_mm_pool_size.h"
#if (CHIPSET==15) && (REMU==1) && (LOCOSTO_LITE==0)
#if (MMPOOL_INT_0_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_0_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_0_INT_RAM)
char int_poolmm_0 [ POOL_SIZE(MMPOOL_INT_0_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_0_SIZE)) ];
#else
extern char int_poolmm_0 [];
#endif
#endif

#if (MMPOOL_INT_1_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_1_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_1_INT_RAM)
char int_poolmm_1 [ POOL_SIZE(MMPOOL_INT_1_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_1_SIZE)) ];
#else
extern char int_poolmm_1 [];
#endif
#endif

#if (MMPOOL_INT_2_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_2_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_2_INT_RAM)
char int_poolmm_2 [ POOL_SIZE(MMPOOL_INT_2_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_2_SIZE)) ];
#else
extern char int_poolmm_2 [];
#endif
#endif

#if (MMPOOL_INT_3_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_3_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_3_INT_RAM)
char int_poolmm_3 [ POOL_SIZE(MMPOOL_INT_3_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_3_SIZE)) ];
#else
extern char int_poolmm_3 [];
#endif
#endif

#if (MMPOOL_INT_4_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_4_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_4_INT_RAM)
char int_poolmm_4 [ POOL_SIZE(MMPOOL_INT_4_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_4_SIZE)) ];
#else
extern char int_poolmm_4 [];
#endif
#endif

#if (MMPOOL_INT_5_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_5_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_5_INT_RAM)
char int_poolmm_5 [ POOL_SIZE(MMPOOL_INT_5_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_5_SIZE)) ];
#else
extern char int_poolmm_5 [];
#endif
#endif

#if (MMPOOL_INT_6_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_6_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_6_INT_RAM)
char int_poolmm_6 [ POOL_SIZE(MMPOOL_INT_6_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_6_SIZE)) ];
#else
extern char int_poolmm_6 [];
#endif
#endif

#if (MMPOOL_INT_7_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_7_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_7_INT_RAM)
char int_poolmm_7 [ POOL_SIZE(MMPOOL_INT_7_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_7_SIZE)) ];
#else
extern char int_poolmm_7 [];
#endif
#endif

#if (MMPOOL_INT_8_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_8_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_8_INT_RAM)
char int_poolmm_8 [ POOL_SIZE(MMPOOL_INT_8_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_8_SIZE)) ];
#else
extern char int_poolmm_8 [];
#endif
#endif

#if (MMPOOL_INT_9_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_9_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_9_INT_RAM)
char int_poolmm_9 [ POOL_SIZE(MMPOOL_INT_9_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_9_SIZE)) ];
#else
extern char int_poolmm_9 [];
#endif
#endif

#if (MMPOOL_INT_10_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_10_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_10_INT_RAM)
char int_poolmm_10 [ POOL_SIZE(MMPOOL_INT_10_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_10_SIZE)) ];
#else
extern char int_poolmm_10[];
#endif
#endif

#if (MMPOOL_INT_11_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_11_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_11_INT_RAM)
char int_poolmm_11 [ POOL_SIZE(MMPOOL_INT_11_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_11_SIZE)) ];
#else
extern char int_poolmm_11[];
#endif
#endif

#if (MMPOOL_INT_12_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_12_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_12_INT_RAM)
char int_poolmm_12 [ POOL_SIZE(MMPOOL_INT_12_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_12_SIZE)) ];
#else
extern char int_poolmm_12[];
#endif
#endif

#if (MMPOOL_INT_13_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_13_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_13_INT_RAM)
char int_poolmm_13 [ POOL_SIZE(MMPOOL_INT_13_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_13_SIZE)) ];
#else
extern char int_poolmm_13[];
#endif
#endif

#if (MMPOOL_INT_14_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_14_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_14_INT_RAM)
char int_poolmm_14 [ POOL_SIZE(MMPOOL_INT_14_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_14_SIZE)) ];
#else
extern char int_poolmm_14[];
#endif
#endif

#if (MMPOOL_INT_15_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_15_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_15_INT_RAM)
char int_poolmm_15 [ POOL_SIZE(MMPOOL_INT_15_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_15_SIZE)) ];
#else
extern char int_poolmm_15[];
#endif
#endif

#if (MMPOOL_INT_16_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_16_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_16_INT_RAM)
char int_poolmm_16 [ POOL_SIZE(MMPOOL_INT_16_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_16_SIZE)) ];
#else
extern char int_poolmm_16[];
#endif
#endif

#if (MMPOOL_INT_17_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_17_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_17_INT_RAM)
char int_poolmm_17 [ POOL_SIZE(MMPOOL_INT_17_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_17_SIZE)) ];
#else
extern char int_poolmm_17[];
#endif
#endif

#if (MMPOOL_INT_18_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined MM_18_INT_RAM) || (!defined DATA_INT_RAM && !defined MM_18_INT_RAM)
char int_poolmm_18 [ POOL_SIZE(MMPOOL_INT_18_PARTITIONS,ALIGN_SIZE(MM_INT_PARTITION_18_SIZE)) ];
#else
extern char int_poolmm_18[];
#endif
#endif

const T_FRM_PARTITION_POOL_CONFIG mm_int_grp_config[] =
{
#if (MMPOOL_INT_0_PARTITIONS>0)
  { MMPOOL_INT_0_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_0_SIZE), &int_poolmm_0 },
#endif
#if (MMPOOL_INT_1_PARTITIONS>0)
  { MMPOOL_INT_1_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_1_SIZE), &int_poolmm_1 },
#endif
#if (MMPOOL_INT_2_PARTITIONS>0)
  { MMPOOL_INT_2_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_2_SIZE), &int_poolmm_2 },
#endif
#if (MMPOOL_INT_3_PARTITIONS>0)
  { MMPOOL_INT_3_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_3_SIZE), &int_poolmm_3 },
#endif
#if (MMPOOL_INT_4_PARTITIONS>0)
  { MMPOOL_INT_4_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_4_SIZE), &int_poolmm_4 },
#endif
#if (MMPOOL_INT_5_PARTITIONS>0)
  { MMPOOL_INT_5_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_5_SIZE), &int_poolmm_5 },
#endif
#if (MMPOOL_INT_6_PARTITIONS>0)
  { MMPOOL_INT_6_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_6_SIZE), &int_poolmm_6 },
#endif

#if (MMPOOL_INT_7_PARTITIONS>0)
  { MMPOOL_INT_7_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_7_SIZE), &int_poolmm_7 },
#endif

#if (MMPOOL_INT_8_PARTITIONS>0)
  { MMPOOL_INT_8_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_8_SIZE), &int_poolmm_8 },
#endif

#if (MMPOOL_INT_9_PARTITIONS>0)
  { MMPOOL_INT_9_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_9_SIZE), &int_poolmm_9 },
#endif

#if (MMPOOL_INT_10_PARTITIONS>0)
  { MMPOOL_INT_10_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_10_SIZE), &int_poolmm_10 },
#endif

#if (MMPOOL_INT_11_PARTITIONS>0)
  { MMPOOL_INT_11_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_11_SIZE), &int_poolmm_11 },
#endif

#if (MMPOOL_INT_12_PARTITIONS>0)
  { MMPOOL_INT_12_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_12_SIZE), &int_poolmm_12 },
#endif

#if (MMPOOL_INT_13_PARTITIONS>0)
  { MMPOOL_INT_13_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_13_SIZE), &int_poolmm_13 },
#endif

#if (MMPOOL_INT_14_PARTITIONS>0)
  { MMPOOL_INT_14_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_14_SIZE), &int_poolmm_14 },
#endif

#if (MMPOOL_INT_15_PARTITIONS>0)
  { MMPOOL_INT_15_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_15_SIZE), &int_poolmm_15 },
#endif

#if (MMPOOL_INT_16_PARTITIONS>0)
  { MMPOOL_INT_16_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_16_SIZE), &int_poolmm_16 },
#endif

#if (MMPOOL_INT_17_PARTITIONS>0)
  { MMPOOL_INT_17_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_17_SIZE), &int_poolmm_17 },
#endif

#if (MMPOOL_INT_18_PARTITIONS>0)
  { MMPOOL_INT_18_PARTITIONS, ALIGN_SIZE(MM_INT_PARTITION_18_SIZE), &int_poolmm_18 },
#endif
  { 0                    , 0                    , NULL    }
};

#endif //DATA_IT_RAM





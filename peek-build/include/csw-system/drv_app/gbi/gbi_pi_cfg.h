/**
 * @file	gbi_pi_cfg.h
 *
 * Configuration definitions for the GBI plugins.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	03/12/2004	 ()		Create.
 *  03/06/2006  R. de Jonge     Added NAND Block Manager
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __GBI_PI_CFG_H_
#define __GBI_PI_CFG_H_


#include "rv_swe.h"



/*default plugin values */
#define GBI_PLUGIN_0	0
#define GBI_PLUGIN_1 	0
#define GBI_PLUGIN_2	0

#ifdef RVM_NOR_BM_SWE
/* for GBI NOR plugin */
#define GBI_NOR_BM_SWE
#endif


#ifdef RVM_NAN_SWE
/* for GBI NAND plugin */
#define GBI_NAND_BM_SWE
#endif

/* Plugin get info structure definition */
typedef struct {
	UINT8					nb_linked_swe;
	T_RVM_USE_ID	linked_swe_id[RVM_MAX_NB_LINKED_SWE];
} T_GBI_PLUGIN_GET_INFO;

/*
 * The plugins provide their interfaces through a function table. This allows a more generic plugin ap-proach.
 * Each plugin must place the pointer to its function table in a specific array ('gbi_plugin_fnctbl ').
 * The sequence in this array must be according to assigned plugin number (plugin 0 first, 1 second and so on).
 */

typedef struct{
	T_GBI_RESULT  (*gbi_plugin_reg_msg) 					(T_RV_HDR *reqmsg_p);
	T_GBI_RESULT  (*gbi_plugin_reg_multiple_msg) 	(T_RV_HDR *reqmsg_p, void **data_p, void **nmb_items_p, UINT32 req_msg_id);
	T_GBI_RESULT  (*gbi_plugin_rsp_msg) 					(T_RV_HDR *rspmsg_p);
	T_GBI_RESULT  (*gbi_plugin_get_info)					(T_GBI_PLUGIN_GET_INFO *plugin_getinfo);
	T_GBI_RESULT  (*gbi_plugin_start)							(void);
	T_GBI_RESULT  (*gbi_plugin_stop)							(void);
} T_GBI_PLUGIN_FNCTBL;




#ifdef RVM_DATALIGHT_SWE
/* Data light plugin , A common plugin for both NAND and NOR */
#define GBI_DATALIGHT_SWE
#undef GBI_PLUGIN_0
#define GBI_PLUGIN_0	1   /* Only one plugin */

#else

  //enable next "USE" definespi to enable the specific plugin.
#ifdef RVM_NOR_BM_SWE
  #undef GBI_PLUGIN_0
  #define GBI_PLUGIN_0	1
  #define USE_NOR_BM
#endif

#ifdef RVM_NAN_SWE
  #undef GBI_PLUGIN_1
  #define GBI_PLUGIN_1	1
  #define USE_NAND_BM
#endif

#endif  /* RVM_DATALIGHT_SWE */


#ifdef RVM_MC_SWE
  #undef GBI_PLUGIN_2
  #define GBI_PLUGIN_2 	1
  #define USE_MC
#endif


#ifdef GBI_DATALIGHT_SWE

#define GBI_MEDIA_OFFSET_STEP       1

#define GBI_NMB_PLUGINS             (GBI_PLUGIN_0+GBI_PLUGIN_2)

#include "datalight/datalight_api.h"

#define PLUGIN_NMB_DATALIGHT         0
#define PLUGIN_NMB_NAND 			 PLUGIN_NMB_DATALIGHT

#define GBI_MC_BM_MEDIA_NR_0         (GBI_DATALIGHT_NAND_PART_ID_0+1)

#define  PLUGIN_NMB_MC              (PLUGIN_NMB_DATALIGHT+1)


#define GBI_PLUGIN_MAX_NMB_MEDIA    (GBI_NMB_PLUGINS)
#define GBI_PLUGIN_0_MEDIA_OFFSET   0

#else

#define GBI_MEDIA_OFFSET_STEP       10
#define GBI_PLUGIN_0_MEDIA_OFFSET   0
#define GBI_PLUGIN_1_MEDIA_OFFSET   (GBI_PLUGIN_0_MEDIA_OFFSET + GBI_PLUGIN_0*GBI_MEDIA_OFFSET_STEP)
#define GBI_PLUGIN_2_MEDIA_OFFSET   (GBI_PLUGIN_1_MEDIA_OFFSET + GBI_PLUGIN_1*GBI_MEDIA_OFFSET_STEP)

//This configuration consists of two plugins. The first plugin in NAND and the second
//plugin is NOR.
#define GBI_NMB_PLUGINS             (GBI_PLUGIN_0+GBI_PLUGIN_1+GBI_PLUGIN_2)

#define GBI_NOR_BM_MEDIA_NR_0     (GBI_PLUGIN_0_MEDIA_OFFSET)
#define GBI_NAND_BM_MEDIA_NR_0      (GBI_PLUGIN_1_MEDIA_OFFSET)
#define GBI_MC_BM_MEDIA_NR_0       (GBI_PLUGIN_2_MEDIA_OFFSET)

//mmc/sd plugin first media number
#define PLUGIN_NMB_NOR              (GBI_NOR_BM_MEDIA_NR_0  / GBI_MEDIA_OFFSET_STEP)
//nand plugin first media number
#define PLUGIN_NMB_NAND             (GBI_NAND_BM_MEDIA_NR_0 / GBI_MEDIA_OFFSET_STEP)
//nor plugin first media number
#define  PLUGIN_NMB_MC              (GBI_MC_BM_MEDIA_NR_0 / GBI_MEDIA_OFFSET_STEP)

#define GBI_PLUGIN_MAX_NMB_MEDIA    (GBI_NMB_PLUGINS * GBI_MEDIA_OFFSET_STEP)


#endif

extern const T_GBI_PLUGIN_FNCTBL *gbi_plugin_fnctbl[GBI_NMB_PLUGINS];


#endif /* __GBI_PI_CFG_H_ */

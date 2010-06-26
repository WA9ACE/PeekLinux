/**
 * @file	gbi_pi_cfg.c
 *
 * Internal configuration functions for GBI Plugins.
 *
 * @author	()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *  03/12/2004	  ()		    Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */


#include "gbi/gbi_i.h"
#include "gbi/gbi_pi_cfg.h"
#include "chipset.cfg"

#ifdef  GBI_NAND_BM_SWE
#include "gbi/gbi_pi_nand.h"
#include "gbi/gbi_pi_nandbm.h"
#endif

#ifdef GBI_NOR_BM_SWE
#include "gbi/gbi_pi_norbm.h"
#endif 

#ifdef USE_MC
#include "gbi_pi_cfg.h"
#include "gbi_pi_mc.h"
#endif

#ifdef GBI_DATALIGHT_SWE
#include "gbi_pi_datalight_i.h"
#endif


const T_GBI_PLUGIN_FNCTBL *gbi_plugin_fnctbl[GBI_NMB_PLUGINS] = {

#ifdef GBI_DATALIGHT_SWE
  &gbi_plugin_fnctbl_datalight
#else
	#ifdef USE_NOR_BM
	&gbi_plugin_fnctbl_nor_bm
	#endif

	#ifdef USE_NAND_BM
  		#ifdef USE_NOR_BM
  		,
  		#endif 
	&gbi_plugin_fnctbl_nand_bm
	#endif 
#endif  /* GBI_DATALIGHT_SWE */	

#ifdef USE_MC

#ifdef GBI_DATALIGHT_SWE
   ,
#else
  #ifdef USE_NAND_BM
  ,
  #else
	#ifdef USE_NOR_BM
	,
	#endif 
  #endif 
#endif

&gbi_plugin_fnctbl_mc
#endif
};



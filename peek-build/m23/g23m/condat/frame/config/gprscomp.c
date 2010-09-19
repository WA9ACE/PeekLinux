/*
+-----------------------------------------------------------------------------
|  Project :
|  Modul   :
+-----------------------------------------------------------------------------
|  Copyright 2002 Texas Instruments Berlin, AG
|                 All rights reserved.
|
|                 This file is confidential and a trade secret of Texas
|                 Instruments Berlin, AG
|                 The receipt of or possession of this file does not convey
|                 any rights to reproduce or disclose its contents or to
|                 manufacture, use, or sell anything it may describe, in
|                 whole, or in part, without the specific written consent of
|                 Texas Instruments Berlin, AG.
+-----------------------------------------------------------------------------
|  Purpose :  Component Table for GPRS
+-----------------------------------------------------------------------------
*/

#ifndef _TARGET_
#define NEW_ENTITY
#endif

/*==== INCLUDES ===================================================*/

#include "nucleus.h"
#include "typedefs.h"
#include "os.h"
#include "vsi.h"
#include "pei.h"
#include "gprsconst.h"
#include "frm_defs.h"
#include "frm_types.h"
#include "frm_glob.h"
#include "os_types.h"
#include "os_glob.h"
#include "gprsconst.h"
#include "chipset.cfg"
#include "rv/rv_defined_swe.h"
#include "../src/config/gsm_bsp_pool_size.h"
#include "../src/config/gsm_mm_pool_size.h"
#if (REMU==1)
#include "../sm_remu/inc/remu_internal.h"
#endif


/*==== CONSTANTS ==================================================*/


/*==== EXTERNALS ==================================================*/

extern SHORT tstrcv_pei_create(T_PEI_INFO const **Info);
extern SHORT tstsnd_pei_create(T_PEI_INFO const **Info);
#if(PSP_STANDALONE==0)
extern SHORT aci_pei_create   (T_PEI_INFO const **Info);
extern SHORT cst_pei_create   (T_PEI_INFO const **Info);
#ifdef FF_ESIM
extern SHORT esim_pei_create  (T_PEI_INFO const **Info); /* esim module */
#endif
//extern SHORT emo_pei_create   (T_PEI_INFO const **Info);
extern SHORT hw_pei_create	  (T_PEI_INFO const **Info);
extern SHORT trans_pei_create   (T_PEI_INFO const **Info);
//extern SHORT ems_pei_create   (T_PEI_INFO const **Info);
extern SHORT ui_pei_create   (T_PEI_INFO const **Info);
extern SHORT sim_pei_create   (T_PEI_INFO const **Info);
extern SHORT sms_pei_create   (T_PEI_INFO const **Info);
extern SHORT cc_pei_create    (T_PEI_INFO const **Info);
extern SHORT sm_pei_create    (T_PEI_INFO const **Info);
extern SHORT ss_pei_create    (T_PEI_INFO const **Info);
extern SHORT mm_pei_create    (T_PEI_INFO const **Info);
extern SHORT gmm_pei_create   (T_PEI_INFO const **Info);
extern SHORT rr_pei_create    (T_PEI_INFO const **Info);
extern SHORT grr_pei_create   (T_PEI_INFO const **Info);
extern SHORT grlc_pei_create  (T_PEI_INFO const **Info);
extern SHORT dl_pei_create    (T_PEI_INFO const **Info);
extern SHORT pl_pei_create    (T_PEI_INFO const **Info);
#ifdef FAX_AND_DATA
extern SHORT l2r_pei_create   (T_PEI_INFO const **Info);
extern SHORT rlp_pei_create   (T_PEI_INFO const **Info);
#ifdef FF_FAX
extern SHORT fad_pei_create   (T_PEI_INFO const **Info);
extern SHORT t30_pei_create   (T_PEI_INFO const **Info);
#endif
#endif /* FAX_AND_DATA */
extern SHORT llc_pei_create   (T_PEI_INFO const **Info);
extern SHORT sndcp_pei_create (T_PEI_INFO const **Info);
extern SHORT ppp_pei_create   (T_PEI_INFO const **Info);
extern SHORT uart_pei_create  (T_PEI_INFO const **Info);

#ifdef FF_MUX
extern SHORT mux_pei_create (T_PEI_INFO const **Info);
#endif /* MUX */

#ifdef FF_PKTIO
extern SHORT pktio_pei_create (T_PEI_INFO const **Info);
#endif /* #ifdef FF_PKTIO */

#ifdef FF_PSI
extern SHORT psi_pei_create (T_PEI_INFO const **Info);
#endif

#ifdef FF_EOTD
extern SHORT lc_pei_create    (T_PEI_INFO const **Info);
extern SHORT rrlp_pei_create  (T_PEI_INFO const **Info);
#endif /* FF_EOTD */

#ifdef CO_UDP_IP
extern SHORT udp_pei_create   (T_PEI_INFO const **Info);
extern SHORT ip_pei_create    (T_PEI_INFO const **Info);
#endif /* CO_UDP_IP */

#ifdef FF_WAP
extern SHORT wap_pei_create   (T_PEI_INFO const **Info);
#endif /* FF_WAP */

#ifndef _TARGET_
  extern SHORT clt_pei_create    (T_PEI_INFO const **Info);
#endif  /* !_TARGET_ */

#ifdef _TARGET_
extern SHORT l1_pei_create    (T_PEI_INFO const **Info);
#ifdef FF_TCP_IP
extern SHORT aaa_pei_create   (T_PEI_INFO const **Info);
#endif /* FF_TCP_IP */
#endif /* _TARGET_ */


extern SHORT upm_pei_create (T_PEI_INFO const **Info);

#ifdef FF_GPF_TCPIP
extern SHORT tcpip_pei_create (T_PEI_INFO const **Info);
#endif /* FF_TCP_IP */

#if defined (CO_TCPIP_TESTAPP) || defined (CO_BAT_TESTAPP)
extern SHORT app_pei_create   (T_PEI_INFO const **Info);
#endif /* CO_TCPIP_TESTAPP */
#ifndef _TARGET_
extern SHORT ra_pei_create    (T_PEI_INFO const **Info);
#endif

#ifndef FF_ATI_BAT
  #ifdef  FF_BAT
extern SHORT gdd_dio_pei_create(T_PEI_INFO const **Info);
  #endif
#endif

#ifdef BTS
/* For Bluetooth BTS */
extern SHORT bts_pei_create(T_PEI_INFO const**p_info);
#ifdef BTW
/* For Bluetooth BTW */
extern SHORT btw_pei_create(T_PEI_INFO const**p_info);
#endif /* BTW */
#ifdef BTT
/* For Bluetooth Reference Applications : BTT */
extern SHORT btt_pei_create(T_PEI_INFO const**p_info);
#endif /* BTT */
#ifdef BTAV
/* For Bluetooth  BTAV */
extern SHORT btav_pei_create(T_PEI_INFO const**p_info);
#endif /* BTAV */
#ifdef BTU
/* For Bluetooth BTU */
extern SHORT btu_pei_create(T_PEI_INFO const**p_info);
#endif /* BTU */
#ifdef FMS
extern SHORT fms_pei_create(T_PEI_INFO const* * p_info);
#endif /*FMS */
#endif /* BTS */
#endif

/*==== VARIABLES ==================================================*/

//EF For normal Test Definition Language (TDL) TAP usage set newTstHeader = FALSE
//EF For multiple entity (TCSL) Test Case Script Lang.   set newTstHeader = TRUE
#ifndef _TARGET_

#ifdef TDL_TAP
BOOL newTstHeader = FALSE;
#else
BOOL newTstHeader = TRUE;
#endif

#endif

#ifndef DATA_EXT_RAM

const T_COMPONENT_ADDRESS tstrcv_list[] =
{
  { tstrcv_pei_create,   NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS tstsnd_list[] =
{
  { tstsnd_pei_create,   NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

#if(PSP_STANDALONE==0)
const T_COMPONENT_ADDRESS mmi_list[] =
{
  { aci_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

#ifdef FF_ESIM
const T_COMPONENT_ADDRESS esim_list[] =
{
  { esim_pei_create,     NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

const T_COMPONENT_ADDRESS cst_list[] =
{
  { cst_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS sim_list[] =
{
  { sim_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS cm_list[] =
{
  { sms_pei_create,      NULL,   ASSIGNED_BY_TI },
  { cc_pei_create,       NULL,   ASSIGNED_BY_TI },
  { sm_pei_create,       NULL,   ASSIGNED_BY_TI },
  { ss_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   (int)"CM" }
};

const T_COMPONENT_ADDRESS mmgmm_list[] =
{
  { mm_pei_create,       NULL,   ASSIGNED_BY_TI },
  { gmm_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   (int)"MMGMM" }
};

const T_COMPONENT_ADDRESS rr_list[] =
{
  { rr_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS grr_list[] =
{
  { grr_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS grlc_list[] =
{
  { grlc_pei_create,     NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS dl_list[] =
{
  { dl_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS pl_list[] =
{
  { pl_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

#ifdef FAX_AND_DATA
const T_COMPONENT_ADDRESS l2rt30_list[] =
{
  { l2r_pei_create,      NULL,   ASSIGNED_BY_TI },
#ifdef FF_FAX
  { t30_pei_create,      NULL,   ASSIGNED_BY_TI },
#endif
  { NULL,                NULL,   (int)"L2RT30" }
};

const T_COMPONENT_ADDRESS rlpfad_list[] =
{
  { rlp_pei_create,      NULL,   ASSIGNED_BY_TI },
#ifdef FF_FAX
  { fad_pei_create,      NULL,   ASSIGNED_BY_TI },
#endif
  { NULL,                NULL,   (int)"RLPFAD" }
};
#endif /* FAX_AND_DATA */


const T_COMPONENT_ADDRESS llc_list[] =
{
  { llc_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS sndcp_list[] =
{
  { sndcp_pei_create,    NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS ppp_list[] =
{
  { ppp_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS uart_list[] =
{
  { uart_pei_create,     NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

#ifdef FF_MUX
const T_COMPONENT_ADDRESS mux_list[] =
{
  { mux_pei_create,    NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /* MUX */

#ifdef FF_PKTIO
const T_COMPONENT_ADDRESS pktio_list[] =
{
  { pktio_pei_create,    NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /* #ifdef FF_PKTIO */

#ifdef FF_PSI
const T_COMPONENT_ADDRESS psi_list[] =
{
  { psi_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /* #ifdef FF_PSI */

//#ifdef FF_EGPRS
const T_COMPONENT_ADDRESS upm_list[] =
{
  { upm_pei_create,    NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
//#endif /* #ifdef FF_UPM */

#ifndef _TARGET_
const T_COMPONENT_ADDRESS clt_list[] =
{
  { clt_pei_create,  NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /* !_TARGET_ */


#ifdef FF_EOTD
const T_COMPONENT_ADDRESS eotd_list[] =
{
  { lc_pei_create,       NULL,   ASSIGNED_BY_TI },
  { rrlp_pei_create,     NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   (int)"EOTD" }
};
#endif /* FF_EOTD */

#ifdef FF_WAP
const T_COMPONENT_ADDRESS wap_list[] =
{
  { wap_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /* FF_WAP */

#ifdef CO_UDP_IP
const T_COMPONENT_ADDRESS udp_list[] =
{
  { udp_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS ip_list[] =
{
  { ip_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /* CO_UDP_IP */

#ifndef _TARGET_
const T_COMPONENT_ADDRESS ra_list[] =
{
  { ra_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /* !_TARGET_ */

#ifdef FF_TCP_IP
const T_COMPONENT_ADDRESS aaa_list[] =
{
#ifdef _TARGET_
  { aaa_pei_create,      NULL,   ASSIGNED_BY_TI },
#else /* _TARGET_ */
  { NULL,               "AAA",   ASSIGNED_BY_TI },
#endif /* else _TARGET_ */
  { NULL,                NULL,   0 }
};
#endif /* FF_TCP_IP */

#ifdef FF_GPF_TCPIP
const T_COMPONENT_ADDRESS tcpip_list[] =
{
  { tcpip_pei_create,    NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#if defined (CO_TCPIP_TESTAPP) || defined (CO_BAT_TESTAPP)
const T_COMPONENT_ADDRESS app_list[] =
{
  { app_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /* CO_TCPIP_TESTAPP */

const T_COMPONENT_ADDRESS l1_list[] =
{
#ifdef _TARGET_
  { l1_pei_create,       NULL,   ASSIGNED_BY_TI },
#else
  { NULL,                "L1",   ASSIGNED_BY_TI },
#endif
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS hw_list[] =
{
  { hw_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

/*
const T_COMPONENT_ADDRESS emo_list[] =
{
  { emo_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
*/

const T_COMPONENT_ADDRESS trans_list[] =
{
  { trans_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};

const T_COMPONENT_ADDRESS ui_list[] =
{
  { ui_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
/*
const T_COMPONENT_ADDRESS ems_list[] =
{
  { ems_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
*/
#ifndef FF_ATI_BAT
  #ifdef  FF_BAT
const T_COMPONENT_ADDRESS gdd_dio_list[] =
{
  { gdd_dio_pei_create,    NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
  #endif
#endif

#ifdef BTS
/* Bluetooth sub-system */
const T_COMPONENT_ADDRESS bts_list[] =
{
  { bts_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#ifdef BTW
/* Bluetooth Worker */
const T_COMPONENT_ADDRESS btw_list[] =
{
  { btw_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /* BTW */
#ifdef BTT
/* Bluetooth Reference Applications */
const T_COMPONENT_ADDRESS btt_list[] =
{
  { btt_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /* BTT */
#ifdef BTAV
/* Bluetooth AV task */
const T_COMPONENT_ADDRESS btav_list[] =
{
  { btav_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /* BTAV */
#ifdef BTU
/* Bluetooth UART */
const T_COMPONENT_ADDRESS btu_list[] =
{
  { btu_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /* BTU */

#ifdef FMS
const T_COMPONENT_ADDRESS fms_list[] =
{
  { fms_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif /*FMS */
#endif /* BTS */
#endif

#if (CHIPSET==15) && (REMU==1) && defined _TARGET_

#ifdef RVM_KPD_SWE
extern SHORT kpd_pei_create   (T_PEI_INFO const **Info);
const T_COMPONENT_ADDRESS kpd_list[] =
{
  { kpd_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#if (TEST == 1)
extern SHORT rv_test_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS rv_test_list[] =
{
  { rv_test_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
extern SHORT rtest_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS rtest_list[] =
{
  { rtest_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_DMA_SWE
extern SHORT dma_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS dma_list[] =
{
  { dma_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_R2D_SWE
extern SHORT r2d_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS r2d_list[] =
{
  { r2d_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_RVT_SWE
extern SHORT rvt_pei_create   (T_PEI_INFO const **Info);
const T_COMPONENT_ADDRESS rvt_list[] =
{
  { rvt_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_USB_SWE
extern SHORT usb_pei_create   (T_PEI_INFO const **Info);
const T_COMPONENT_ADDRESS usb_list[] =
{
  { usb_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_USBFAX_SWE
extern SHORT usbfax_pei_create   (T_PEI_INFO const **Info);
const T_COMPONENT_ADDRESS usbfax_list[] =
{
  { usbfax_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_USBMS_SWE
extern SHORT usbms_pei_create   (T_PEI_INFO const **Info);
const T_COMPONENT_ADDRESS usbms_list[] =
{
  { usbms_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif


#ifdef RVM_USBTRC_SWE
extern SHORT usbtrc_pei_create   (T_PEI_INFO const **Info);
const T_COMPONENT_ADDRESS usbtrc_list[] =
{
  { usbtrc_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif


#ifdef RVM_RTC_SWE
extern SHORT rtc_pei_create   (T_PEI_INFO const **Info);
const T_COMPONENT_ADDRESS rtc_list[] =
{
  { rtc_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_ETM_SWE
extern SHORT etm_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS etm_list[] =
{
  { etm_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_FFS_SWE
extern SHORT ffs_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS ffs_list[] =
{
  { ffs_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_DAR_SWE
extern SHORT dar_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS dar_list[] =
{
  { dar_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif


#ifdef RVM_GBI_SWE
extern SHORT gbi_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS gbi_list[] =
{
  { gbi_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_DATALIGHT_SWE
extern SHORT datalight_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS datalight_list[] =
{
  { datalight_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#else
#ifdef RVM_NAN_SWE
extern SHORT nan_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS nan_list[] =
{
  { nan_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_NOR_BM_SWE
extern SHORT nor_bm_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS nor_bm_list[] =
{
  { nor_bm_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif
#endif

#ifdef RVM_CAMA_SWE
extern SHORT cama_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS cama_list[] =
{
  { cama_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_CAMD_SWE
extern SHORT camd_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS camd_list[] =
{
  { camd_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_AUDIO_MAIN_SWE
extern SHORT audio_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS audio_list[] =
{
  { audio_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_BAE_SWE
extern SHORT bae_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS bae_list[] =
{
  { bae_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_AS_SWE
extern SHORT as_pei_create   (T_PEI_INFO const **Info);

const T_COMPONENT_ADDRESS as_list[] =
{
  { as_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_IMG_SWE
extern SHORT img_pei_create   (T_PEI_INFO const **Info);
 const T_COMPONENT_ADDRESS img_list[] =
{
  { img_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_RFS_SWE
extern SHORT rfs_pei_create   (T_PEI_INFO const **Info);
 const T_COMPONENT_ADDRESS rfs_list[] =
{
  { rfs_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_RFSNAND_SWE
extern SHORT rfsnand_pei_create   (T_PEI_INFO const **Info);

 const T_COMPONENT_ADDRESS rfsnand_list[] =
{
  { rfsnand_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_RFSFAT_SWE
extern SHORT rfsfat_pei_create   (T_PEI_INFO const **Info);
 const T_COMPONENT_ADDRESS rfsfat_list[] =
{
  { rfsfat_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_MKS_SWE
extern SHORT mks_pei_create   (T_PEI_INFO const **Info);
 const T_COMPONENT_ADDRESS mks_list[] =
{
  { mks_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_HASH_SWE
extern SHORT hash_pei_create   (T_PEI_INFO const **Info);
 const T_COMPONENT_ADDRESS hash_list[] =
{
  { hash_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_CRY_SWE
extern SHORT cry_pei_create   (T_PEI_INFO const **Info);
 const T_COMPONENT_ADDRESS cry_list[] =
{
  { cry_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_TTY_SWE
extern SHORT tty_pei_create   (T_PEI_INFO const **Info);
 const T_COMPONENT_ADDRESS tty_list[] =
{
  { tty_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_LCD_SWE
extern SHORT lcd_pei_create   (T_PEI_INFO const **Info);
const T_COMPONENT_ADDRESS lcd_list[] =
{
  { lcd_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#if(PSP_STANDALONE==0)
#ifdef RVM_SSL_SWE
extern SHORT ssl_pei_create   (T_PEI_INFO const **Info);
const T_COMPONENT_ADDRESS ssl_list[] =
{
  { ssl_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif
#endif

#ifdef RVM_LCC_SWE
extern SHORT lcc_pei_create   (T_PEI_INFO const **Info);
const T_COMPONENT_ADDRESS lcc_list[] =
{
  { lcc_pei_create,      NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#ifdef RVM_MC_SWE
extern SHORT mc_pei_create   (T_PEI_INFO const **Info);
const T_COMPONENT_ADDRESS mc_list[] =
{
  { mc_pei_create,       NULL,   ASSIGNED_BY_TI },
  { NULL,                NULL,   0 }
};
#endif

#endif //if CHIPSET == 15 and REMU == 1


const T_COMPONENT_ADDRESS *ComponentTables[]=
{
  tstrcv_list,
  tstsnd_list,
#if (CHIPSET==15) && (REMU==1) && defined _TARGET_ /* PSP Driver Entities. DO NOT ADD ENTIRIS IN PSP ENTITIES 
														SECTION THIS WILL LEAD TO SYTEM CRASH */
#ifdef RVM_KPD_SWE
 kpd_list,
#endif

#if (TEST == 1)
 rv_test_list,
 rtest_list,
#endif

#ifdef RVM_DMA_SWE
 dma_list,
#endif

#ifdef RVM_R2D_SWE
 r2d_list,
#endif

#ifdef RVM_RVT_SWE
 rvt_list,
#endif

#ifdef RVM_USB_SWE
 usb_list,
#endif

#ifdef RVM_USBFAX_SWE
 usbfax_list,
#endif

#ifdef RVM_USBMS_SWE
 usbms_list,
#endif

#ifdef RVM_USBTRC_SWE
 usbtrc_list,
#endif

#ifdef RVM_RTC_SWE
 rtc_list,
#endif

#ifdef RVM_ETM_SWE
 etm_list,
#endif

#ifdef RVM_FFS_SWE
 ffs_list,
#endif
#ifdef RVM_DAR_SWE
 dar_list,
#endif

#ifdef RVM_MC_SWE
mc_list,
#endif

#ifdef RVM_GBI_SWE
  gbi_list,
#endif

#ifdef RVM_DATALIGHT_SWE
 datalight_list,
#else
  
#ifdef RVM_NAN_SWE
 nan_list,
#endif

#ifdef RVM_NOR_BM_SWE
 nor_bm_list,
#endif 
#endif   /* RVM_DATALIGHT_SWE */

#ifdef RVM_CAMA_SWE
 cama_list,
#endif

#ifdef RVM_CAMD_SWE
 camd_list,
#endif

#ifdef RVM_RFS_SWE
  rfs_list,
#endif

#ifdef RVM_RFSFAT_SWE
  rfsfat_list,
#endif
#ifdef RVM_AUDIO_MAIN_SWE
 audio_list,
#endif
#ifdef RVM_IMG_SWE
  img_list,
#endif

#ifdef RVM_RFSNAND_SWE
  rfsnand_list,
#endif
#ifdef RVM_BAE_SWE
 bae_list,
#endif

#ifdef RVM_AS_SWE
 as_list,
#endif
#ifdef RVM_MKS_SWE
 mks_list,
#endif

#ifdef RVM_HASH_SWE
hash_list,
#endif

#ifdef RVM_CRY_SWE
 cry_list,
#endif
#ifdef RVM_TTY_SWE
#if (L1_GTT == 1)
tty_list,
#endif
#endif

#ifdef RVM_LCD_SWE
 lcd_list,
#endif

#ifdef RVM_LCC_SWE
 lcc_list,
#endif

#if(PSP_STANDALONE==0) 
#ifdef RVM_SSL_SWE
 ssl_list,
#endif
#endif

#endif /* end (CHIPSET==15) */
#if(PSP_STANDALONE==0) 
  mmi_list,
  cst_list,
  sim_list,
#ifdef BTS
  bts_list,	/* Bluetooth sub-system */
#ifdef BTW
  btw_list,	/* Bluetooth worker test */
#endif /* BTW */
#ifdef BTT
  btt_list,	/* Bluetooth Reference Applications */
#endif /* BTT */
#ifdef BTAV
  btav_list,	/*Bluetooth AV task */
#endif /* BTAV */
#ifdef BTU
  btu_list,	/* Bluetooth UART */
#endif /* BTU */
#ifdef FMS
 fms_list,
#endif /*FMS */
#endif /* BTS */

  cm_list,
  mmgmm_list,
  rr_list,
  grr_list,
  grlc_list,
  dl_list,
  pl_list,
#ifdef FAX_AND_DATA
  l2rt30_list,
  rlpfad_list,
#endif /* FAX_AND_DATA */
  llc_list,
  sndcp_list,
  ppp_list,
  uart_list,
#ifdef FF_MUX
  mux_list,
#endif
#ifdef FF_PKTIO
  pktio_list,
#endif
#ifdef FF_PSI
  psi_list,
#endif
upm_list,
#ifndef _TARGET_
  clt_list,
#endif /* !_TARGET_ */
#ifndef FF_ATI_BAT
  #ifdef  FF_BAT
  gdd_dio_list,
  #endif /* FF_BAT */
#endif
#ifdef FF_EOTD
  eotd_list,
#endif
#ifdef FF_WAP
  wap_list,
#endif

#ifdef CO_UDP_IP
  udp_list,
  ip_list,
#endif
#if defined _SIMULATION_ && defined FF_FAX
  ra_list,
#endif
#ifdef FF_TCP_IP
  aaa_list,
#endif /* FF_TCP_IP */

#ifdef FF_GPF_TCPIP
  tcpip_list,
#endif

#if defined (CO_TCPIP_TESTAPP) || defined (CO_BAT_TESTAPP)
  app_list,
#endif /* CO_TCPIP_TESTAPP */

#ifdef FF_ESIM
  esim_list, /* needed for esim module */
#endif
  l1_list,
#endif  
  //emo_list,
  ui_list,
  hw_list,
  trans_list,
  //ems_list,
  NULL
};

/*==== VERSIONS ===================================================*/
#ifndef CTRACE
  char * str2ind_version = "&0";
#endif

#endif /* DATA_EXT_RAM */

/*==== MEMORY CONFIGURATION =======================================*/

/*
 * Partitions pool configuration for primitive communication
 */

/*
 * Memory extensions for multiplexer
 */
#ifdef FF_MUX
#define PRIMPOOL_0_MUX_ADDITION 30
#define PRIMPOOL_2_MUX_ADDITION 10
#else /* FF_MUX */
#define PRIMPOOL_0_MUX_ADDITION  0
#define PRIMPOOL_2_MUX_ADDITION  0
#endif /* else FF_MUX */

/*
 * Memory extensions for multiple PDP contexts
 */
#ifdef FF_PKTIO
#define PRIMPOOL_2_MPDP_ADDITION 30
#else /* FF_PKTIO */
#define PRIMPOOL_2_MPDP_ADDITION  0
#endif /* else FF_PKTIO */
#ifdef WIN32
/*
 * Required for testing LLC acknowledged mode.
 */
#define PRIMPOOL_0_PARTITIONS		200
#define PRIMPOOL_1_PARTITIONS		100
#define PRIMPOOL_2_PARTITIONS		 20
#define PRIMPOOL_3_PARTITIONS		 20

#else /*WIN32*/

#define PRIMPOOL_0_PARTITIONS   (190 + PRIMPOOL_0_MUX_ADDITION + 20)
#define PRIMPOOL_1_PARTITIONS		 110
#define PRIMPOOL_2_PARTITIONS   ( 50 + PRIMPOOL_2_MPDP_ADDITION + PRIMPOOL_2_MUX_ADDITION + 5)
#if (DRP_FW_EXT==1)
#define PRIMPOOL_3_PARTITIONS		  8
#else
#define PRIMPOOL_3_PARTITIONS		   7
#endif
#endif /*WIN32*/

#define PRIM_PARTITION_0_SIZE	    60
#define PRIM_PARTITION_1_SIZE	   128
#define PRIM_PARTITION_2_SIZE	   632

#if (CHIPSET == 15)
#define PRIM_PARTITION_3_SIZE	  1764
#else
#define PRIM_PARTITION_3_SIZE	  1600
#endif 

#ifndef DATA_INT_RAM
unsigned int MaxPrimPartSize = PRIM_PARTITION_3_SIZE;
#endif /* !DATA_INT_RAM */

#if (!defined DATA_EXT_RAM && defined PRIM_0_INT_RAM) || (!defined DATA_INT_RAM && !defined PRIM_0_INT_RAM)
char pool10 [ POOL_SIZE(PRIMPOOL_0_PARTITIONS,ALIGN_SIZE(PRIM_PARTITION_0_SIZE)) ];
#else
extern char pool10 [];
#endif

#if (!defined DATA_EXT_RAM && defined PRIM_1_INT_RAM) || (!defined DATA_INT_RAM && !defined PRIM_1_INT_RAM)
char pool11 [ POOL_SIZE(PRIMPOOL_1_PARTITIONS,ALIGN_SIZE(PRIM_PARTITION_1_SIZE)) ];
#else
extern char pool11 [];
#endif

#if (!defined DATA_EXT_RAM && defined PRIM_2_INT_RAM) || (!defined DATA_INT_RAM && !defined PRIM_2_INT_RAM)
char pool12 [ POOL_SIZE(PRIMPOOL_2_PARTITIONS,ALIGN_SIZE(PRIM_PARTITION_2_SIZE)) ];
#else
extern char pool12 [];
#endif

#if (!defined DATA_EXT_RAM && defined PRIM_3_INT_RAM) || (!defined DATA_INT_RAM && !defined PRIM_3_INT_RAM)
char pool13 [ POOL_SIZE(PRIMPOOL_3_PARTITIONS,ALIGN_SIZE(PRIM_PARTITION_3_SIZE)) ];
#else
extern char pool13 [];
#endif

#ifndef DATA_INT_RAM
const T_FRM_PARTITION_POOL_CONFIG prim_grp_config[] =
{
  { PRIMPOOL_0_PARTITIONS, ALIGN_SIZE(PRIM_PARTITION_0_SIZE), &pool10 },
  { PRIMPOOL_1_PARTITIONS, ALIGN_SIZE(PRIM_PARTITION_1_SIZE), &pool11 },
  { PRIMPOOL_2_PARTITIONS, ALIGN_SIZE(PRIM_PARTITION_2_SIZE), &pool12 },
  { PRIMPOOL_3_PARTITIONS, ALIGN_SIZE(PRIM_PARTITION_3_SIZE), &pool13 },
  { 0                    , 0                    , NULL	  }
};
#endif /* !DATA_INT_RAM */

/*
 * Partitions pool configuration for test interface communication
 */
#define TESTPOOL_0_PARTITIONS		 10
#define TESTPOOL_1_PARTITIONS		200
#define TESTPOOL_2_PARTITIONS		  2

#define TSTSND_QUEUE_ENTRIES         (TESTPOOL_0_PARTITIONS+TESTPOOL_1_PARTITIONS+TESTPOOL_2_PARTITIONS)
#define TSTRCV_QUEUE_ENTRIES         50

#define TEST_PARTITION_0_SIZE	    80
#ifdef _TARGET_
 #define TEST_PARTITION_1_SIZE   160
#else
 #define TEST_PARTITION_1_SIZE   260
#endif
#define TEST_PARTITION_2_SIZE	  1600

#ifndef DATA_INT_RAM
const USHORT TST_SndQueueEntries    = TSTSND_QUEUE_ENTRIES;
const USHORT TST_RcvQueueEntries    = TSTRCV_QUEUE_ENTRIES;
const USHORT TextTracePartitionSize = TEST_PARTITION_1_SIZE;
#endif /* !DATA_INT_RAM */

#if (!defined DATA_EXT_RAM && defined TEST_0_INT_RAM) || (!defined DATA_INT_RAM && !defined TEST_0_INT_RAM)
char pool20 [ POOL_SIZE(TESTPOOL_0_PARTITIONS,ALIGN_SIZE(TEST_PARTITION_0_SIZE)) ];
#else
extern char pool20 [];
#endif

#if (!defined DATA_EXT_RAM && defined TEST_1_INT_RAM) || (!defined DATA_INT_RAM && !defined TEST_1_INT_RAM)
char pool21 [ POOL_SIZE(TESTPOOL_1_PARTITIONS,ALIGN_SIZE(TEST_PARTITION_1_SIZE)) ];
#else
extern char pool21 [];
#endif

#if (!defined DATA_EXT_RAM && defined TEST_2_INT_RAM) || (!defined DATA_INT_RAM && !defined TEST_2_INT_RAM)
char pool22 [ POOL_SIZE(TESTPOOL_2_PARTITIONS,ALIGN_SIZE(TEST_PARTITION_2_SIZE)) ];
#else
extern char pool22 [];
#endif

#ifndef DATA_INT_RAM
const T_FRM_PARTITION_POOL_CONFIG test_grp_config[] =
{
  { TESTPOOL_0_PARTITIONS, ALIGN_SIZE(TEST_PARTITION_0_SIZE), &pool20 },
  { TESTPOOL_1_PARTITIONS, ALIGN_SIZE(TEST_PARTITION_1_SIZE), &pool21 },
  { TESTPOOL_2_PARTITIONS, ALIGN_SIZE(TEST_PARTITION_2_SIZE), &pool22 },
  { 0                    , 0                    , NULL	  }
};
#endif /* !DATA_INT_RAM */

/*
 * Partitions pool configuration for general purpose allocation
 */

#define DMEMPOOL_0_PARTITIONS		70
#define DMEMPOOL_1_PARTITIONS		 2

#define DMEM_PARTITION_0_SIZE		16
#ifdef _TARGET_
#define DMEM_PARTITION_1_SIZE		1600 /* for non tracing ccd arm7 */
#else
#define DMEM_PARTITION_1_SIZE		2800 /* for non tracing ccd pc */
#endif

#if (!defined DATA_EXT_RAM && defined DMEM_0_INT_RAM) || (!defined DATA_INT_RAM && !defined DMEM_0_INT_RAM)
char pool30 [ POOL_SIZE(DMEMPOOL_0_PARTITIONS,ALIGN_SIZE(DMEM_PARTITION_0_SIZE)) ];
#else
extern char pool30 [];
#endif

#if (!defined DATA_EXT_RAM && defined DMEM_1_INT_RAM) || (!defined DATA_INT_RAM && !defined DMEM_1_INT_RAM)
char pool31 [ POOL_SIZE(DMEMPOOL_1_PARTITIONS,ALIGN_SIZE(DMEM_PARTITION_1_SIZE)) ];
#else
extern char pool31 [];
#endif

#ifndef DATA_INT_RAM
const T_FRM_PARTITION_POOL_CONFIG dmem_grp_config[] =
{
  { DMEMPOOL_0_PARTITIONS, ALIGN_SIZE(DMEM_PARTITION_0_SIZE), &pool30 },
  { DMEMPOOL_1_PARTITIONS, ALIGN_SIZE(DMEM_PARTITION_1_SIZE), &pool31 },
  { 0                    , 0                    , NULL	  }
};
#endif /* !DATA_INT_RAM */

/*
 * Partitions pool configuration for board support package (based on REMU)
 */
#if (CHIPSET==15) && (REMU==1) && (LOCOSTO_LITE==0)

#if (BSPPOOL_0_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
char pool40 [ POOL_SIZE(BSPPOOL_0_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_0_SIZE)) ];
#else
extern char pool40[];
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

#if (BSPPOOL_32_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
 char pool72 [ POOL_SIZE(BSPPOOL_32_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_32_SIZE)) ];
#else
extern char pool72[];
#endif
 #endif

#if (L1_PCM_EXTRACTION==1)
#if (BSPPOOL_32_NEW_PARTITIONS>0)
#if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
 char pool72_new [ POOL_SIZE(BSPPOOL_32_NEW_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_32_NEW_SIZE)) ];
#else
extern char pool72_new[];
#endif
 #endif
#endif

#if (BSPPOOL_33_PARTITIONS>0)
 #if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
  char pool73_new [ POOL_SIZE(BSPPOOL_33_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_33_SIZE)) ];
 #else
 extern char pool73_new[];
 #endif
#endif

/*Added for camd snapshot in RTEST*/ 
#if (BSPPOOL_34_PARTITIONS>0)
 #if (!defined DATA_EXT_RAM && defined BSP_26_INT_RAM) || (!defined DATA_INT_RAM && !defined BSP_26_INT_RAM)
  char pool74_new [ POOL_SIZE(BSPPOOL_34_PARTITIONS,ALIGN_SIZE(BSP_PARTITION_34_SIZE)) ];
 #else
 extern char pool74_new[];
 #endif
#endif

#ifndef DATA_INT_RAM
const T_FRM_PARTITION_POOL_CONFIG bsp_grp_config[] =
{
#if (BSPPOOL_0_PARTITIONS>0)
  {BSPPOOL_0_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_0_SIZE), &pool40 },
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
#if (BSPPOOL_32_PARTITIONS>0)
  { BSPPOOL_32_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_32_SIZE), &pool72 },
#endif
#if (L1_PCM_EXTRACTION==1)
#if (BSPPOOL_32_NEW_PARTITIONS>0)
  { BSPPOOL_32_NEW_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_32_NEW_SIZE), &pool72_new },
#endif
#endif
#if (BSPPOOL_33_PARTITIONS>0)
  { BSPPOOL_33_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_33_SIZE), &pool73_new },
#endif
#if (BSPPOOL_34_PARTITIONS>0)
  { BSPPOOL_34_PARTITIONS, ALIGN_SIZE(BSP_PARTITION_34_SIZE), &pool74_new },
#endif

  { 0                    , 0                    , NULL	  }
};
/* Moved the defn of GSPTaskIdTable from REMU library to here so that size of this table will vary depending 
   on LITE or PLUS build. Adding a buffer of 5 to ensure that remu task id will not exceed size of GSPTaskIdTable. 
   Note that all the REMU entities should be immediately after tst snd and tst rcv task in ComponentTables list. 
   Otherwise GSPTaskIdTable will overflow which will lead to system crash */
T_GSP_RT_ADDR_ID_DATA*  GSPTaskIdTable[MAX_REMU_ENTITIES+5]; 

#endif /* !DATA_INT_RAM */
#endif /* end CHIPSET==15*/



/*
 * Partitions group list
 */

extern T_HANDLE PrimGroupHandle;
extern T_HANDLE DmemGroupHandle;
extern T_HANDLE TestGroupHandle;
#if (CHIPSET==15) && (REMU==1) && defined _TARGET_
#if (LOCOSTO_LITE==0)
extern T_HANDLE BspGroupHandle;
extern T_HANDLE BspIntGroupHandle;
extern T_FRM_PARTITION_POOL_CONFIG bsp_int_grp_config[];
#endif
extern T_HANDLE BspGroupHandle;
extern T_HANDLE BspRvtGroupHandle;
#endif

#ifndef DATA_INT_RAM
const T_FRM_PARTITION_GROUP_CONFIG partition_grp_config[MAX_POOL_GROUPS+1] =
{
  { "PRIM", &prim_grp_config[0] },
  { "TEST", &test_grp_config[0] },
  { "DMEM", &dmem_grp_config[0] },
#if (CHIPSET==15) && (REMU==1) && defined _TARGET_
  { "BEXT",  &bsp_grp_config[0] },
#if (LOCOSTO_LITE==0)
  { "BINT",  & bsp_int_grp_config[0] },
#endif
#endif
  { NULL,   NULL                }
};

T_HANDLE *PoolGroupHandle[MAX_POOL_GROUPS+1] =
{
  &PrimGroupHandle,
  &TestGroupHandle,
  &DmemGroupHandle,
#if (CHIPSET==15) && (REMU==1) && defined _TARGET_
  &BspGroupHandle,
#if (LOCOSTO_LITE==0)
  &BspIntGroupHandle,
#endif
#endif
    NULL
};
#endif /* !DATA_INT_RAM */

/*
 * Dynamic Memory Pool Configuration
 */

#ifdef _TARGET_
#ifdef FF_ESIM
 #define EXT_DATA_POOL_PS_BASE_SIZE 45000
#else
#define EXT_DATA_POOL_PS_BASE_SIZE 52000 /* L23 task stacks movement to external RAM: OMAPS00122070 */
#endif
#define INT_DATA_POOL_PS_BASE_SIZE 8012
#else /* _TARGET_ */
#define EXT_DATA_POOL_TCPIP_ADDTIION 120000
#define EXT_DATA_POOL_PS_BASE_SIZE  80000 + EXT_DATA_POOL_TCPIP_ADDTIION
#define INT_DATA_POOL_PS_BASE_SIZE  1000
#endif /* _TARGET_ */

#ifdef MEMORY_SUPERVISION
 #define EXT_DATA_POOL_PPS_ADDITION ((EXT_DATA_POOL_PS_BASE_SIZE>>3)+25000)
 #define INT_DATA_POOL_PPS_ADDITION ((INT_DATA_POOL_PS_BASE_SIZE>>3))
#else  /* MEMORY_SUPERVISION */
 #define EXT_DATA_POOL_PPS_ADDITION 0
 #define INT_DATA_POOL_PPS_ADDITION 0
#endif /* MEMORY_SUPERVISION */

#if defined (FF_WAP) || defined (FF_SAT_E)
 #define EXT_DATA_POOL_WAP_ADDITION 15000
#else
 #define EXT_DATA_POOL_WAP_ADDITION 0
#endif /* FF_WAP OR SAT E */

#ifdef GRR_PPC_IF_PRIM
 #define INT_DATA_POOL_GRR_PPC_IF_PRIM_ADDITION  3000
#else  /* #ifdef GRR_PPC_IF_PRIM */
 #define INT_DATA_POOL_GRR_PPC_IF_PRIM_ADDITION  0
#endif /* #ifdef GRR_PPC_IF_PRIM */

#ifdef BTU 
  #define EXT_DATA_POOL_BTU_ADDITION 2000
#else  
  #define EXT_DATA_POOL_BTU_ADDITION 0
#endif
  
#define EXT_DATA_POOL_PS_SIZE   (EXT_DATA_POOL_PS_BASE_SIZE + EXT_DATA_POOL_WAP_ADDITION             + EXT_DATA_POOL_PPS_ADDITION+EXT_DATA_POOL_BTU_ADDITION)
#define INT_DATA_POOL_PS_SIZE   (INT_DATA_POOL_PS_BASE_SIZE + INT_DATA_POOL_GRR_PPC_IF_PRIM_ADDITION + INT_DATA_POOL_PPS_ADDITION)

#define EXT_DATA_POOL_GPF_SIZE  (2048 + OS_QUEUE_ENTRY_SIZE(TSTSND_QUEUE_ENTRIES) + OS_QUEUE_ENTRY_SIZE(TSTRCV_QUEUE_ENTRIES))

#define EMO_APP					0x0 //0x3000
#define EMO_UI					0x0 // 0xc800 /* 50k for UI/Emo tasks */
#define EXT_DATA_POOL_SIZE      (EXT_DATA_POOL_PS_SIZE + EXT_DATA_POOL_GPF_SIZE + EXT_DATA_POOL_BSP_SIZE + EXT_DATA_POOL_MM_SIZE + EMO_UI + EMO_APP)
#define INT_DATA_POOL_SIZE      (INT_DATA_POOL_PS_SIZE+INT_DATA_POOL_BSP_SIZE+INT_DATA_POOL_MM_SIZE+1000)

#if(PSP_STANDALONE == 1)
#define MM_EXT_DATA_POOL_SIZE     (1)
#define MM_INT_DATA_POOL_SIZE     (1)
#endif

#ifndef DATA_INT_RAM
char ext_data_pool              [ EXT_DATA_POOL_SIZE ];
char mm_ext_data_pool          [MM_EXT_DATA_POOL_SIZE];

#if (REMU==1)
#ifdef _TARGET_
	char rvt_data_pool              [ EXT_RVT_DATA_POOL_BSP_SIZE ];
#endif
#endif
GLOBAL T_HANDLE mm_ext_data_pool_handle;
GLOBAL T_HANDLE mm_int_data_pool_handle;
#endif /*DATA_INT_RAM*/

#ifndef DATA_EXT_RAM
char int_data_pool              [ INT_DATA_POOL_SIZE ];
char mm_int_data_pool          [MM_INT_DATA_POOL_SIZE];
#else
extern char int_data_pool       [ ];
extern char mm_int_data_pool          [];
#endif
#ifndef DATA_INT_RAM

const T_MEMORY_POOL_CONFIG memory_pool_config[MAX_MEMORY_POOLS+1] =
{
  { "INTPOOL", INT_DATA_POOL_SIZE, &int_data_pool[0] },
  { "EXTPOOL", EXT_DATA_POOL_SIZE, &ext_data_pool[0] },
  { "MMEPOOL", MM_EXT_DATA_POOL_SIZE, &mm_ext_data_pool[0] },
  { "MMIPOOL", MM_INT_DATA_POOL_SIZE, &mm_int_data_pool[0] },
#if (REMU==1)
#ifdef _TARGET_
  { "RVTPOOL", EXT_RVT_DATA_POOL_BSP_SIZE, &rvt_data_pool[0] },
#endif
#endif
  {  NULL }
};

extern T_HANDLE ext_data_pool_handle;
extern T_HANDLE int_data_pool_handle;
extern T_HANDLE rvt_data_pool_handle;

T_HANDLE *MemoryPoolHandle[MAX_MEMORY_POOLS+1] =
{
  &int_data_pool_handle,
  &ext_data_pool_handle,
  &mm_ext_data_pool_handle,
  &mm_int_data_pool_handle,
#if (REMU==1)
#ifdef _TARGET_
  &rvt_data_pool_handle,
#endif  
#endif
  NULL
};

#endif /* !DATA_INT_RAM */



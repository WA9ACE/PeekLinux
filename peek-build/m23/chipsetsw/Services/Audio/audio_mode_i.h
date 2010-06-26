/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_mode_i.h                                              */
/*                                                                          */
/*  Purpose:  This file contains symbolic constant used for the audio mode  */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date         Modification                                               */
/*  ------------------------------------                                    */
/*  18 Jan 2002  Create                                                     */
/*                                                                          */
/*  Author       Francois Mazard                                            */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"

#ifndef _WINDOWS
   #include "chipset.cfg"
   #include "l1sw.cfg"
#endif

#ifdef RVM_AUDIO_MAIN_SWE

  #ifndef __AUDIO_MODE_I_H_
    #define __AUDIO_MODE_I_H_

    #ifdef __cplusplus
      extern "C"
        {
    #endif

    #if (ANLG_FAM == 1)
      /* Register mapping for OMEGA, NAUSICA */

      /* VBCR register */
      #define AUDIO_VBCR_VFBYP        (0x0200)
      #define AUDIO_VBCR_VBDFAUXG     (0x0100)
      #define AUDIO_VBCR_VSYNC        (0x0080)
      #define AUDIO_VBCR_VCLKMODE     (0x0040)
      #define AUDIO_VBCR_VALOOP       (0x0020)
      #define AUDIO_VBCR_MICBIAS      (0x0010)
      #define AUDIO_VBCR_VULSWITCH    (0x0008)
      #define AUDIO_VBCR_VBUZ         (0x0004)
      #define AUDIO_VBCR_VDLEAR       (0x0002)
      #define AUDIO_VBCR_VDLAUX       (0x0001)

      /* VBUR */
      #define AUDIO_VBUR_DXEN         (0x0200)
      #define AUDIO_VBUR_VDLST        (0x000F)
      #define AUDIO_VBUR_VULPG        (0x001F)

      /* VBDR */
      #define AUDIO_VBDR_VDLPG        (0x000F)
      #define AUDIO_VBDR_VOLCTL       (0x000F)
    #endif
    #if (ANLG_FAM == 2)
      /* Register mapping for IOTA */

      /* VBCR register */
      #define AUDIO_VBCR_VFBYP        (0x0200)
      #define AUDIO_VBCR_VBDFAUXG     (0x0100)
      #define AUDIO_VBCR_VSYNC        (0x0080)
      #define AUDIO_VBCR_VCLKMODE     (0x0040)
      #define AUDIO_VBCR_VALOOP       (0x0020)
      #define AUDIO_VBCR_MICBIAS      (0x0010)
      #define AUDIO_VBCR_VULSWITCH    (0x0008)
      #define AUDIO_VBCR_VBUZ         (0x0004)
      #define AUDIO_VBCR_VDLEAR       (0x0002)
      #define AUDIO_VBCR_VDLAUX       (0x0001)

      /* VBCR2 */
      #define AUDIO_VBCR2_MICBIASEL   (0x0001)
      #define AUDIO_VBCR2_VDLHSO      (0x0002)
      #define AUDIO_VBCR2_MICNAUX     (0x0004)

      /* VBUR */
      #define AUDIO_VBUR_DXEN         (0x0200)
      #define AUDIO_VBUR_VDLST        (0x000F)
      #define AUDIO_VBUR_VULPG        (0x001F)

      /* VBDR */
      #define AUDIO_VBDR_VDLPG        (0x000F)
      #define AUDIO_VBDR_VOLCTL       (0x000F)
    #endif
    #if ((ANLG_FAM == 3)||(ANLG_FAM == 4))
      /** For reg. VBCR **/
      #define AUDIO_VBCR_VULSWITCH  (0x008)
      #define AUDIO_VBCR_MICBIAS    (0x010)
      #define AUDIO_VBCR_VALOOP     (0x020)
      #define AUDIO_VBCR_VCLKMODE   (0x040)
      #define AUDIO_VBCR_VSYNC      (0x080)
      #define AUDIO_VBCR_VBDFAUXG   (0x100)
      #define AUDIO_VBCR_VFBYP      (0x200)

      /** For reg. VBCR2 **/
      #define AUDIO_VBCR2_HSMICSEL  (0x001)
      #define AUDIO_VBCR2_MICBIASEL (0x004)
      #define AUDIO_VBCR2_SPKG      (0x008)
      #define AUDIO_VBCR2_HSOVMID   (0x010)
      #define AUDIO_VBCR2_HSDIF     (0x020)
      #define AUDIO_VBCR2_WBA       (0x040)
      #define AUDIO_VBCR2_VMIDFBYP  (0x080)

      /** For reg. VBUR **/
      #define AUDIO_VBUR_VDLST      (0x000F)
      #define AUDIO_VBUR_VULPG      (0x001F)
      #define AUDIO_VBUR_DXEN       (0x200)

      /* VBDR */
      #define AUDIO_VBDR_VDLPG      (0x000F)
      #define AUDIO_VBDR_VOLCTL     (0x000F)

      /** For reg. VPOP **/
      #define AUDIO_VBPOP_HSODIS    (0x001)
      #define AUDIO_VBPOP_HSOCHG    (0x002)
      #define AUDIO_VBPOP_HSOAUTO   (0x004)
      #define AUDIO_VBPOP_EARDIS    (0x008)
      #define AUDIO_VBPOP_EARCHG    (0x010)
      #define AUDIO_VBPOP_EARAUTO   (0x020)
      #define AUDIO_VBPOP_AUXDIS    (0x040)
      #define AUDIO_VBPOP_AUXCHG    (0x080)
      #define AUDIO_VBPOP_AUXAUTO   (0x100)
      #define AUDIO_VBPOP_AUXFBYP   (0x400)

      /** For reg. VAUOCTRL **/
      #define AUDIO_VAUOCTRL_HSOR_VOICE (0x001)
      #define AUDIO_VAUOCTRL_HSOR_AUDIO (0x002)
      #define AUDIO_VAUOCTRL_HSOL_VOICE (0x004)
      #define AUDIO_VAUOCTRL_HSOL_AUDIO (0x008)
      #define AUDIO_VAUOCTRL_SPK_VOICE  (0x010)
      #define AUDIO_VAUOCTRL_SPK_AUDIO  (0x020)
      #define AUDIO_VAUOCTRL_AUX_VOICE  (0x040)
      #define AUDIO_VAUOCTRL_AUX_AUDIO  (0x080)
      #define AUDIO_VAUOCTRL_EAR_VOICE  (0x100)
      #define AUDIO_VAUOCTRL_EAR_AUDIO  (0x200)

      /** For reg. VAUDCTRL **/
      #define AUDIO_VAUDCTRL_MONOR      (0x002)
      #define AUDIO_VAUDCTRL_MONOL      (0x004)
      #define AUDIO_VAUDCTRL_HPFBYP     (0x008)
      #define AUDIO_VAUDCTRL_VULBST     (0x010)
      #define AUDIO_VAUDCTRL_SRW0       (0x020)
      #define AUDIO_VAUDCTRL_SRW1       (0x040)
      #define AUDIO_VAUDCTRL_SRW2       (0x080)
      #define AUDIO_VAUDCTRL_AUGA       (0x100)
      #define AUDIO_VAUDCTRL_VSPCK      (0x200)

      #define AUDIO_VAUDCTRL_SRW_48KHZ     (0)
      #define AUDIO_VAUDCTRL_SRW_44_1KHZ   (AUDIO_VAUDCTRL_SRW1)
      #define AUDIO_VAUDCTRL_SRW_32KHZ     (AUDIO_VAUDCTRL_SRW1|AUDIO_VAUDCTRL_SRW0)
      #define AUDIO_VAUDCTRL_SRW_22_05KHZ  (AUDIO_VAUDCTRL_SRW2)
      #define AUDIO_VAUDCTRL_SRW_16KHZ     (AUDIO_VAUDCTRL_SRW2|AUDIO_VAUDCTRL_SRW0)
      #define AUDIO_VAUDCTRL_SRW_11_025KHZ (AUDIO_VAUDCTRL_SRW2|AUDIO_VAUDCTRL_SRW1)
      #define AUDIO_VAUDCTRL_SRW_8KHZ      (AUDIO_VAUDCTRL_SRW2|AUDIO_VAUDCTRL_SRW1|AUDIO_VAUDCTRL_SRW0)

      /** For reg. VAUDCTRL **/
      #define AUDIO_VAUSCTRL_AULGA0     (0x001)
      #define AUDIO_VAUSCTRL_AULGA1     (0x002)
      #define AUDIO_VAUSCTRL_AULGA2     (0x004)
      #define AUDIO_VAUSCTRL_AULGA3     (0x008)
      #define AUDIO_VAUSCTRL_AULGA4     (0x010)
      #define AUDIO_VAUSCTRL_AURGA0     (0x020)
      #define AUDIO_VAUSCTRL_AURGA1     (0x040)
      #define AUDIO_VAUSCTRL_AURGA2     (0x080)
      #define AUDIO_VAUSCTRL_AURGA3     (0x100)
      #define AUDIO_VAUSCTRL_AURGA4     (0x200)
    #endif

    #ifdef __cplusplus
      }
    #endif

  #endif /* __AUDIO_MODE_I_H_ */
#endif /* #ifdef RVM_AUDIO_MAIN_SWE */




#----------------------------------------------------------------------------- 
#  Project :  
#  Modul   :  ccd_inc.mk
#----------------------------------------------------------------------------- 
#  Copyright 2002 Texas Instruments Berlin, AG 
#                 All rights reserved. 
# 
#                 This file is confidential and a trade secret of Texas 
#                 Instruments Berlin, AG 
#                 The receipt of or possession of this file does not convey 
#                 any rights to reproduce or disclose its contents or to 
#                 manufacture, use, or sell anything it may describe, in 
#                 whole, or in part, without the specific written consent of 
#                 Texas Instruments Berlin, AG. 
#----------------------------------------------------------------------------- 
# Purpose : Some general definition also used in CCD\util.
#----------------------------------------------------------------------------- 

#-------------------------Suffixes for the name of the output file
#---------e.g. ccd_tr_rt_po_db.lib

ifdef TOOL_CHOICE
  TSDIR:=/ts$(TOOL_CHOICE)
  tsTail  :=_ts$(TOOL_CHOICE)
else
  TSDIR:=
endif

ifeq ($(TRACE), 1)
   trTail :=_tr
   TRACEDIR=/trace
else 
   trTail:=
   TRACEDIR=
endif

ifeq ($(MEMSUPER), 1)
  PSDIR=/ps
  pstail :=_ps
else
  PSDIR=
  pstail:=
endif

ifeq ($(DEBUG), 1)
   DEBUGDIR=/debug
   dbTail :=_db
else 
   DEBUGDIR=
   dbTail:=
endif

SECTION=
secTail=
ifeq ($(RUN_FLASH), 1)
  SECTION=/fl
  secTail:=_fl
endif
ifeq ($(RUN_INT_RAM), 1)
  SECTION=/ir
  secTail:=_ir
endif

ifeq ($(TARGET), nuc)
   tTail :=_na7
   ifeq ($(PLATFORM), arm7)
      tTail :=_na7
      ifeq ($(DYNARR), 1)
        DADIR=/da
        datail :=_da
      else
        DADIR=
        datail:=
      endif
   else
      ifeq ($(PLATFORM), arm9)
         tTail :=_na9
      else
        ifeq ($(PLATFORM), pc)
           tTail :=_npc
        endif #pc
      endif #arm9
   endif #arm7
else
   ifeq ($(TARGET), win32)
     tTail :=_wn
   else
     ifeq ($(TARGET), linuxpc)
      tTail :=_lpc
      OSENV=sus
     else
       ifeq ($(TARGET), rtlpc)
        tTail :=_rlp
        OSENV=sus
        RTL_VERSION=2.0
        RTL_DIR=/opt/rtldk-$(RTL_VERSION)/rtlinuxpro
        include $(RTL_DIR)/rtl.mk
       else
         ifeq ($(TARGET), linuxarm)
          tTail :=_la9
          OSENV=sus
          else
           ifeq ($(TARGET), solsparc)
            tTail :=_ssp
            OSENV=sus
            BYTESEX=motorola
           endif #solsparc
         endif #linuxarm
       endif #rtlpc
     endif #linuxpc
   endif #win32  
endif #nuc

XXX:=$(tTail)$(trTail)$(dbTail)$(pstail)$(datail)$(secTail)$(tsTail)

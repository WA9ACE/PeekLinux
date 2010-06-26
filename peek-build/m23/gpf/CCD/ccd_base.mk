#----------------------------------------------------------------------------- 
#  Project :  
#  Modul   :  ccd_base.mk
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
#  Purpose : Basic definitions for building ccddata.
#----------------------------------------------------------------------------- 
ifdef PROJECT
  PROST=$(PROJECT)
endif
ifndef PROJECT
  PROJECT=$(PROST)
endif

GPF:=$(subst \,/,$(GPF))
TESTROOT:=$(subst \,/,$(TESTROOT))

CP=cp
RM=rm -f

GPFINCDIR=$(GPF)/inc
GPFLIBDIR=$(GPF)/LIB/$(TARGET)$(DEBUGDIR)
GPFBINDIR=$(GPF)/BIN$(DEBUGDIR)
GPFPRJDIR=$(GPF)/PROJECT/$(PROJECT)/lib/$(TARGET)

VPATH=$(OBJDIR)

CCDDIR=.

# toolset
ifdef TOOL_CHOICE
TSDIR:=/ts$(TOOL_CHOICE)
else
TSDIR:=
endif

ifdef PATH_CC_1_22E 
 Path:=$(PATH_CC_1_22E);$(Path)
 C_DIR:=$(PATH_CC_1_22E)
endif

ifeq ($(TOOL_CHOICE), 3)
 CFLAGS= --align_structs=4 -pden -q -o 
 ifndef PATH_CC_2_54 
   PATH_CC_2_54=C:/Dvpt_tools/Codegen/TMS4701x_2.54
 endif
 Path:=$(PATH_CC_2_54)/bin;$(Path)
 INCLUDES += -I"$(PATH_CC_2_54)/include"
endif

ifeq ($(TOOL_CHOICE), 5)
 CFLAGS= --align_structs=4 -pden -q -o
 ifndef PATH_CC_2_55 
   PATH_CC_2_55=C:\Dvpt_tools\Codegen\TMS4701x_2.55
 endif
 Path:=$(PATH_CC_2_55)/bin;$(Path)
 INCLUDES += -I"$(PATH_CC_2_55)/include"
endif

ifeq ($(TOOL_CHOICE), 6)
  # --code_state=16 => -mt
  # --endian=little => -me
  CFLAGS= -mv=5e --abi=ti_arm9_abi
  CFLAGS+= -pden -pdv -mf -ms -qq -disable_branch_chaining -pi
  CFLAGS+= -O=2
  ifndef PATH_CC_4_11 
    PATH_CC_4_11=C:\Dvpt_tools\Codegen\TMS4701x_4.11
  endif
  Path:=$(PATH_CC_4_11)/bin;$(Path)
  INCLUDES += -I"$(PATH_CC_4_11)/include"
endif

# Suffixes for the name of the output file  - e.g. ccd_cdg_tr_po_db.lib
ifeq ($(TRACE), 1)
   trTail :=_tr
   DEFINES+=-DDEBUG_CCD
else 
   trTail:=
endif

ifeq ($(TARGET), arm7)
   tTail :=_na
else
   ifeq ($(TARGET), nucwin)
      tTail :=_nw
   else
      ifeq ($(TARGET), win32)
         tTail := _wn
      else
        ifeq ($(TARGET), psos)
           tTail :=_po
        endif
      endif
   endif   
endif

DEBUGDIR=
ifeq ($(DEBUG), 1)
   ifeq ($(TARGET),win32)
     DEBUGDIR=/debug
   endif
   dbTail :=_db
else 
   dbTail:=
endif

OBJDIR=obj/ccddata/$(PROST)/$(TARGET)$(TSDIR)$(DEBUGDIR)

ifeq ($(TARGET), linuxpc)
  OSENV=sus
endif

ifeq ($(TARGET), rtlpc)
  OSENV=sus
  RTL_VERSION=2.0
  RTL_DIR=/opt/rtldk-$(RTL_VERSION)/rtlinuxpro
  include $(RTL_DIR)/rtl.mk
endif

ifeq ($(TARGET), linuxarm)
  OSENV=sus
endif

ifeq ($(TARGET), solsparc)
  OSENV=sus
endif

ifeq ($(OSENV), sus)
  MKDIR = mkdir
else
  MKDIR = $(GPF)/tools/bin/mkdir
  SHELL=$(GPF)/tools/bin/sh.exe
endif

# sus environments
ifeq ($(OSENV), sus)
  INCLUDES+=-I. -I$(GPFINCDIR) -I$(CCDDIR)
  DEFINES+=$(PROSTCFLAGS) -DNEW_ENTITY -DNEW_FRAME -DCCDDATA_STATIC_DATA
  ifeq ($(CDEMSTR), 0)
    DEFINES += -DCCDDATA_NO_CDEMSTR
  endif
  ifeq ($(CCDENT), 0)
    DEFINES += -DCCDDATA_NO_CCDENT
  endif
  ifeq ($(CCDALIAS), 0)
    DEFINES += -DCCDDATA_NO_ALIAS
  endif
  ifeq ($(CCDDATA_U32), 1)
    DEFINES += -DCCDDATA_U32
  endif
   ifeq ($(TARGET), rtlpc)
     COPTSNF = $(CFLAGS)
     MKLIB= $(AR) ruv $@
     DEFINES += -D_RT_LINUX_
   else
     ifeq ($(TARGET), linuxarm)
       CC=arm-linux-gcc
       MKLIB= arm-linux-ar ruv $@
       RANLIB= arm-linux-ranlib
       DEFINES += -DGSP_TARGET
     else
       ifeq ($(TARGET), solsparc)
         CC=gcc
         MKLIB= ar ruv $@
         RANLIB= ranlib
       else
         CC=gcc
         MKLIB= ar ruv $@
         RANLIB= ranlib
       endif
     endif
   endif
  CFLAGS+=-c $(DEFINES) $(INCLUDES) -o $(OBJDIR)/$@
  # Debug
  ifeq ($(DEBUG), 1)
    CFLAGS += -g
  endif
endif

# win32
ifeq ($(TARGET),win32)
  CC=cl
  INCLUDES+=-I"." -I"$(GPFINCDIR)" -I"$(CCDDIR)"
  DEFINES+=$(PROSTCFLAGS) -DWIN32 -DNEW_ENTITY -DNEW_FRAME -D_TOOLS_
  ifeq ($(CDEMSTR), 0)
    DEFINES += -DCCDDATA_NO_CDEMSTR
  endif
  ifeq ($(CCDENT), 0)
    DEFINES += -DCCDDATA_NO_CCDENT
  endif
  ifeq ($(CCDALIAS), 0)
    DEFINES += -DCCDDATA_NO_ALIAS
  endif
  ifeq ($(CCDDATA_U32), 1)
    DEFINES += -DCCDDATA_U32
  endif

  CFLAGS=/nologo /c /W3 /Zp1 $(DEFINES) $(INCLUDES) /Fo$(OBJDIR)/
  LINK=link.exe
  MKLIB=lib.exe
  # Debug
  ifeq ($(DEBUG), 1)
    CFLAGS += /Fr$(OBJDIR)/ /MDd /Zi /Fd$(OBJDIR)/
    LDFLAGS_DLL += /DEBUG /incremental:no /pdb:none
  else
    CFLAGS += /MD
  endif

endif

# nucwin
ifeq ($(TARGET),nucwin)
  CC=cl
  INCLUDES+=-I"." -I"$(GPFINCDIR)" -I"$(CCDDIR)"
  DEFINES+=$(PROSTCFLAGS) -DWIN32 -DNEW_ENTITY -DNEW_FRAME -DCCDDATA_STATIC_DATA
  ifeq ($(CDEMSTR), 0)
    DEFINES += -DCCDDATA_NO_CDEMSTR
  endif
  ifeq ($(CCDENT), 0)
    DEFINES += -DCCDDATA_NO_CCDENT
  endif
  ifeq ($(CCDALIAS), 0)
    DEFINES += -DCCDDATA_NO_ALIAS
  endif
  ifeq ($(CCDDATA_U32), 1)
    DEFINES += -DCCDDATA_U32
  endif
  CFLAGS=/nologo /c /W3 /Zp1 $(DEFINES) $(INCLUDES) /Fo$(OBJDIR)/
  MKLIB=lib.exe
  LDFLAGS+=/nologo $(PROSTLDFLAGS)
  # Debug
  ifeq ($(DEBUG), 1)
  CFLAGS += /Fr$(OBJDIR)/ /MTd /Zi /Fd$(OBJDIR)/
  else
  CFLAGS += /MT
  endif
endif # target == nucwin

# arm7
ifeq ($(TARGET), arm7)
   CC=cl470
   CFLAGS+=-me -mt
   ifeq ($(TOOL_CHOICE), )
    CFLAGS+= -mw -x -pw2 -o -q
   endif
   ifeq ($(DEBUG), 1)
     ifeq ($(TOOL_CHOICE), )
        CFLAGS += -g
      else
        CFLAGS += -gt
      endif
      CFLAGS += -mn
   endif
   CFLAGS+=-fr $(OBJDIR) -c 
   DEFINES+=-d_TMS470 -dNEW_FRAME -dNEW_ENTITY -dCCDDATA_STATIC_DATA
   ifeq ($(CDEMSTR), 0)
     DEFINES += -dCCDDATA_NO_CDEMSTR
   endif
   ifeq ($(CCDENT), 0)
     DEFINES += -dCCDDATA_NO_CCDENT
   endif
   ifeq ($(CCDALIAS), 0)
     DEFINES += -dCCDDATA_NO_ALIAS
   endif
   ifeq ($(CCDDATA_U32), 1)
     DEFINES += -dCCDDATA_U32
   endif
   INCLUDES+=-I"." -I"$(GPFINCDIR)" -I"$(CCDDIR)"
   CFLAGS+=$(DEFINES) $(INCLUDES)
   MKLIB=ar470
endif # target == arm7

ifeq ($(TARGET), arm9)

   CC=cl470
   CFLAGS+=-me -mt -o
   ifeq ($(DEBUG), 1)
     ifeq ($(TOOL_CHOICE), 6)
       CFLAGS += --symdebug:coff
     else
       # toolsets 3 and 5
       CFLAGS += -gt
     endif
     CFLAGS += -mn 
   endif

   CFLAGS+=-fr $(OBJDIR) -c 
   DEFINES+=-d_TMS470 -dNEW_FRAME -dNEW_ENTITY -dCCDDATA_STATIC_DATA
   DEFINES += -dCCDDATA_U32
   ifeq ($(CDEMSTR), 0)
     DEFINES += -dCCDDATA_NO_CDEMSTR
   endif
   ifeq ($(CCDENT), 0)
     DEFINES += -dCCDDATA_NO_CCDENT
   endif
   ifeq ($(CCDALIAS), 0)
     DEFINES += -dCCDDATA_NO_ALIAS
   endif
   INCLUDES+=-I"." -I"$(GPFINCDIR)" -I"$(CCDDIR)"
   CFLAGS+=$(DEFINES) $(INCLUDES)
   MKLIB=ar470
endif # target == arm9

# arm_b16
ifeq ($(TARGET), arm_b16)
   CC=armcc
   CFLAGS+=-Epl -Wx -O0
   ifeq ($(DEBUG), 1)
     CFLAGS+=-g
   endif
   CFLAGS+=-o $(OBJDIR)/$@ -c
   DEFINES+=-D_TMS470 -DNEW_FRAME -DNEW_ENTITY -DCCDDATA_STATIC_DATA
   INCLUDES+=-I"." -I"$(GPFINCDIR)" -I"$(CCDDIR)"
   CFLAGS+=$(DEFINES) $(INCLUDES)
   MKLIB=armar
endif # target == arm_b16

# arm_b32
ifeq ($(TARGET), arm_b32)
   CC=armcc
   CFLAGS+=-Epl -Wx -O0
   ifeq ($(DEBUG), 1)
     CFLAGS+=-g
   endif
   CFLAGS+=-o $(OBJDIR)/$@ -c
   DEFINES+=-D_TMS470 -DNEW_FRAME -DNEW_ENTITY -DCCDDATA_STATIC_DATA
   INCLUDES+=-I"." -I"$(GPFINCDIR)" -I"$(CCDDIR)"
   CFLAGS+=$(DEFINES) $(INCLUDES)
   MKLIB=armar
endif # target == arm_b32

# psos
ifeq ($(TARGET), psos)
   CC=cc386
   COPTS_FILE:=c.opt
   -include $(PSS_BSP)/bsp.mk# board support settings e.g. MODEL, FPU
   ifeq ( FPU,H )
      FPUOPT = -VNDP
   else 
      FPUOPT=
   endif
   _CFLAGS+=-c -VANSI -VLONGNAME -VSTRICT -VSPROTO -VROM -VBSS -V$(MODEL)
   _CFLAGS+=$(FPUOPT) -VNOSIGNEDCHAR
   ifeq ($(DEBUG), 1)
      _CFLAGS+=-VXDB
   endif
   DEFINES+=-DMODEL=$(MODEL) -DPSOS=1 -DNEW_FRAME -DNEW_ENTITY
   DEFINES+=-DCCDDATA_STATIC_DATA
   ifeq ($(CDEMSTR), 0)
     DEFINES += -DCCDDATA_NO_CDEMSTR
   endif
   ifeq ($(CCDENT), 0)
     DEFINES += -DCCDDATA_NO_CCDENT
   endif
   ifeq ($(CCDALIAS), 0)
     DEFINES += -DCCDDATA_NO_ALIAS
   endif
   ifeq ($(CCDDATA_U32), 1)
     DEFINES += -DCCDDATA_U32
   endif
   INCLUDES+=-I. -I$(PSS_ROOT)/include -I$(PSS_BSP) -I$(PSS_ROOT)/bsps/devices
   CFLAGS=@c.opt -o $(OBJDIR)/$@
   MKLIB=lib386
endif # target == psos

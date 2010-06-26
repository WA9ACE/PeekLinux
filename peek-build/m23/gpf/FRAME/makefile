#######################  Makefile for GNU make    #####################
#
#           makefile with rules for all different variations of Frame
#
#           ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#           ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#           targets:
#
#           os_layer      (objects)
#           frameobjs     Frame + OS-Layer + TST (objects)
#           ../lib/frame.lib or ../lib/frame.dll
#           ../lib/ofe.lib or ../lib/ofe.dll Frame + OS_LAYER + TST + OFE
#           clean         delete object files
#
#######################################################################


VPATH=$(OS_LAYER_SRC) $(OBJDIR)
OWN_MAKELEVEL=0


GPF:=$(subst \,/,$(GPF))

ifdef TOOL_CHOICE
  TS:=/ts$(TOOL_CHOICE)
  tsTail  :=_ts$(TOOL_CHOICE)
else
  TS:=
endif

ifdef PATH_CC_1_22E 
 PATH:=$(PATH_CC_1_22E);$(PATH)
 C_DIR:=$(PATH_CC_1_22E)
endif

ifeq ($(TOOL_CHOICE), 3)
 COPTS= --align_structs=4 -pden -q
 ifndef PATH_CC_2_54 
   PATH_CC_2_54=C:/Dvpt_tools/Codegen/TMS4701x_2.54
   C_DIR=PATH_CC_2_54
 endif
 Path:=$(PATH_CC_2_54)/bin;$(Path)
 INCLUDES += -I"$(PATH_CC_2_54)/include"
endif

ifeq ($(TOOL_CHOICE), 5)
 COPTS= --align_structs=4 -pden -q
 ifndef PATH_CC_2_55 
   PATH_CC_2_55=C:\Dvpt_tools\Codegen\TMS4701x_2.55
#  C_DIR=PATH_CC_2_55
 endif
 Path:=$(PATH_CC_2_55)/bin;$(Path)
 INCLUDES += -I"$(PATH_CC_2_55)/include"
endif

ifeq ($(TOOL_CHOICE), 6)
  # --code_state=16 => -mt
  # --endian=little => -me
  # what is -ml?
  COPTS= -mv=5e --abi=ti_arm9_abi
  COPTS+= -pden -pdv -mf -ms -qq -disable_branch_chaining -pi
  ifndef PATH_CC_4_11 
    PATH_CC_4_11=C:\Dvpt_tools\Codegen\TMS4701x_4.11
#   C_DIR=PATH_CC_4_11
  endif
  Path:=$(PATH_CC_4_11)/bin;$(Path)
  INCLUDES += -I"$(PATH_CC_4_11)/include"
endif

#*==== PARAMETERS-ANALYSIS =========================================*#
ifeq ($(FLOAT), 1)
REPLACE		= -replace
endif

ifeq ($(OS_ONLY), 1)
  OSAL:=/os
else
  OSAL:=
endif

ifeq ($(TARGET), nuc)
  ifeq ($(RUN_FLASH), 1)
    SECTION:=/fl
  endif

  ifeq ($(RUN_INT_RAM), 1)
    SECTION:=/ir
  endif
else
  SECTION:=
endif

ifeq ($(TARGET), linuxpc)
  ESF=1
endif
ifeq ($(TARGET), linuxarm)
  ESF=1
endif
ifeq ($(TARGET), solsparc)
  ESF=1
endif

ifeq ($(ESF), 1)
  ES:=/esf
else
  ES:=
endif

ifeq ($(NO_LINT), 1)
  USE_LINT:=1
else
  USE_LINT:=0
endif

ifeq "$(CLEARCASE_PRIMARY_GROUP)" ""
  CLRCAS=0
else
  ifeq "$(IN_VOB)" "FALSE"
    CLRCAS=0
  else
    CLRCAS=1
  endif
endif

ifeq ($(CLRCAS), 1)
  CHECKOUT = -cleartool co -nc
  CHECKIN = -cleartool ci -nc
else
  CHECKOUT = attrib -r
  CHECKIN = attrib +r
endif

# for created object files
ifeq ($(TARGET), nuc)
  OBJDIR=obj/$(TARGET)/$(PLATFORM)$(DB)$(OSAL)$(PS)$(ES)$(SECTION)$(OPT)$(TS)
  NUC_OBJDIR=e:/nucleus/v1.1/os/asm
  TIF_OBJDIR=../tst/$(OBJDIR)
  MISC_OBJDIR=../misc/$(OBJDIR)
else
  OBJDIR=obj/$(TARGET)$(DB)$(OSAL)$(PS)$(ES)
endif

# for stack calculation
GAWK=../tools/bin/gawk.exe
AWK_PROG=../BIN/cg$(tTail).awk

# Lint
LINT=lint-nt.exe
LINT_INC=-i/gpf/template/lint/
LINT_OPT_FILE=/gpf/template/lint/options.lnt

# general directories
GPFDIR=..
INCDIR=../inc
ifndef TI_DIR
  TI_DIR=/chipsetsw/
endif
override TI_DIR:=$(subst \,/,$(TI_DIR))

# for created libraries
ifeq ($(TARGET), win32)
  LIBDIR=../LIB/WIN32$(DEBUGDIR)
  BINDIR=../BIN$(DEBUGDIR)
else
  LIBDIR:=../LIB
endif


OS_LAYER_SRC=./$(TARGET)
FRAME_SRC=./



#-------------------------general exported include files
INC_FILES=Vsi.h os.h typedefs.h drvconf.h STDDEFS.H gdi.h comp.h Header.h \
          ofe.h PEI.H poolconf.h TOK.H $(LIBDIR)/FrmConst.h

#-------------------------Suffixes for the name of the output file
#---------e.g. frame_tr_rt_po_db.lib

ifeq ($(ESF), 1)
   esfTail :=_esf
else
   esfTail :=
endif

ifeq ($(RUN_FLASH), 1)
   flTail :=_fl
else
   flTail:=
endif

ifeq ($(RUN_INT_RAM), 1)
   irTail :=_ir
else
   irTail:=
endif

ifeq ($(TARGET), nuc)
  ifeq ($(PLATFORM), arm7)
    tTail :=_na7
  else
    ifeq ($(PLATFORM), arm9)
      tTail :=_na9
    else
      ifeq ($(PLATFORM), pc)
        tTail :=_npc
      endif
    endif
  endif
else
  ifeq ($(TARGET), nuc_ARM)
    tTail :=_ar
  else
    ifeq ($(TARGET), win32)
      tTail :=
    else
      ifeq ($(TARGET), psos)
        tTail :=_po
      else
   	ifeq ($(TARGET), vxworks)
	  tTail :=_vx
    	  ifeq ($(TOOL), gnu)
	    tlTail :=_gn
          else
	    tlTail :=_di
	  endif
    	  ifeq ($(CPU), PPC603)
	    cpTail :=_pp
	  endif
        else
   	  ifeq ($(TARGET), linuxpc)
	    tTail :=_lpc
            OSENV=sus
            NO_LINT=1
          else
            ifeq ($(TARGET), rtlpc)
              tTail :=_rlp
              OSENV=sus
              NO_LINT=1
              RTL_VERSION=2.0
              RTL_DIR=/opt/rtldk-$(RTL_VERSION)/rtlinuxpro
              include $(RTL_DIR)/rtl.mk
            else
              ifeq ($(TARGET), linuxarm)
                tTail :=_la9
                OSENV=sus
                NO_LINT=1
              else
                ifeq ($(TARGET), solsparc)
                  tTail :=_ssp
                  OSENV=sus
                  NO_LINT=1
                endif
              endif
	    endif
	  endif
	endif
      endif
    endif
   endif
endif


ifeq ($(DEBUG), 1)
   DB=/db
   ifeq ($(TARGET), win32)
     DEBUGDIR=/debug
     dbTail :=
   else
     dbTail :=_db
   endif
else
   dbTail:=
   DB=
endif

ifeq ($(OS_ONLY), 1)
   LIBNAME = os
else
   LIBNAME = frame
endif

ifeq ($(NO_OPT), 1)
  DISABLE_OPT:=1
  OPT:=/no
   noTail:=_no
else
  DISABLE_OPT:=0
  OPT:=
  noTail:=
endif

#---------- shell calls
RM      		= -rm -rf
ifeq ($(OSENV), sus)
  MKDIR   		= mkdir
else
  MKDIR   		= gmkdir
endif
COPY    		= cp
MOVE      	    = mv
COPTS_FILE	    =
CLEARTOOL		= cleartool
ECHO			= @echo

#-------------------------Suffixes for the name of the output files
#--------frame.lib suffixes

ifeq ($(MEMSUPER), 2)
   msTail :=_ps
   PS=/ps
endif
ifeq ($(MEMSUPER), 1)
   msTail :=_pm
endif
ifeq ($(MEMSUPER), 0)
   msTail:=
   PS=
endif

XXX:=$(tTail)$(tlTail)$(cpTail)$(dbTail)$(msTail)$(esfTail)$(irTail)$(flTail)$(noTail)$(tsTail)

#-------------------------Suffixes for the name of the output files

_OS_LAYER= os_pro.$(OBJTAIL) os_com.$(OBJTAIL) os_tim.$(OBJTAIL) os_sem.$(OBJTAIL) os_mis.$(OBJTAIL) os_mem.$(OBJTAIL) $(OS_ISR) $(OS_PART) $(OSDRV)

ifeq ($(OS_ONLY), 1)
_FRAMEOBJS =  $(ESF_FUNC)
else
_FRAMEOBJS =  frame.$(OBJTAIL) vsi_sem.$(OBJTAIL) vsi_com.$(OBJTAIL) vsi_mem.$(OBJTAIL) vsi_tim.$(OBJTAIL)\
              vsi_mis.$(OBJTAIL) vsi_drv.$(OBJTAIL) vsi_trc.$(OBJTAIL) vsi_pro.$(OBJTAIL) xalert.$(OBJTAIL)\
              route.$(OBJTAIL) prf_func.$(OBJTAIL) frm_ext.$(OBJTAIL) $(ESF_FUNC)
endif

_TIFOBJS = $(TIF_OBJDIR)/tst_pei.$(OBJTAIL) $(TIF_OBJDIR)/tstdriver.$(OBJTAIL) $(TIF_OBJDIR)/tif.$(OBJTAIL)\
          $(TIF_OBJDIR)/tr.$(OBJTAIL) $(TIF_OBJDIR)/titrc.$(OBJTAIL)

_MISCOBJS = $(MISC_OBJDIR)/tok.$(OBJTAIL) $(MISC_OBJDIR)/tools.$(OBJTAIL)


_NUCOBJS = $(NUC_OBJDIR)/inc.$(OBJTAIL) $(NUC_OBJDIR)/dmi.$(OBJTAIL) $(NUC_OBJDIR)/evi.$(OBJTAIL) $(NUC_OBJDIR)/pmi.$(OBJTAIL) $(NUC_OBJDIR)/mbi.$(OBJTAIL) $(NUC_OBJDIR)/ioi.$(OBJTAIL) $(NUC_OBJDIR)/tmi.$(OBJTAIL) \
           $(NUC_OBJDIR)/qui.$(OBJTAIL) $(NUC_OBJDIR)/hii.$(OBJTAIL) $(NUC_OBJDIR)/eri.$(OBJTAIL) $(NUC_OBJDIR)/pii.$(OBJTAIL) $(NUC_OBJDIR)/smi.$(OBJTAIL) $(NUC_OBJDIR)/tci.$(OBJTAIL) $(NUC_OBJDIR)/erc.$(OBJTAIL) \
           $(NUC_OBJDIR)/mbd.$(OBJTAIL) $(NUC_OBJDIR)/iod.$(OBJTAIL) $(NUC_OBJDIR)/hid.$(OBJTAIL) $(NUC_OBJDIR)/pid.$(OBJTAIL) $(NUC_OBJDIR)/qud.$(OBJTAIL) $(NUC_OBJDIR)/erd.$(OBJTAIL) $(NUC_OBJDIR)/lic.$(OBJTAIL) \
           $(NUC_OBJDIR)/rld.$(OBJTAIL) $(NUC_OBJDIR)/lid.$(OBJTAIL) $(NUC_OBJDIR)/ioc.$(OBJTAIL) $(NUC_OBJDIR)/ioce.$(OBJTAIL) $(NUC_OBJDIR)/iof.$(OBJTAIL) $(NUC_OBJDIR)/quc.$(OBJTAIL) $(NUC_OBJDIR)/quce.$(OBJTAIL) \
           $(NUC_OBJDIR)/quf.$(OBJTAIL) $(NUC_OBJDIR)/qus.$(OBJTAIL) $(NUC_OBJDIR)/quse.$(OBJTAIL) $(NUC_OBJDIR)/pic.$(OBJTAIL) $(NUC_OBJDIR)/pice.$(OBJTAIL) $(NUC_OBJDIR)/pif.$(OBJTAIL) $(NUC_OBJDIR)/pis.$(OBJTAIL) \
           $(NUC_OBJDIR)/pise.$(OBJTAIL) $(NUC_OBJDIR)/mbc.$(OBJTAIL) $(NUC_OBJDIR)/mbce.$(OBJTAIL) $(NUC_OBJDIR)/mbf.$(OBJTAIL) $(NUC_OBJDIR)/mbs.$(OBJTAIL) $(NUC_OBJDIR)/mbse.$(OBJTAIL) $(NUC_OBJDIR)/sms.$(OBJTAIL) \
           $(NUC_OBJDIR)/smse.$(OBJTAIL) $(NUC_OBJDIR)/tcs.$(OBJTAIL) $(NUC_OBJDIR)/tcse.$(OBJTAIL) $(NUC_OBJDIR)/evf.$(OBJTAIL) $(NUC_OBJDIR)/hic.$(OBJTAIL) $(NUC_OBJDIR)/dmc.$(OBJTAIL) $(NUC_OBJDIR)/dmce.$(OBJTAIL) \
           $(NUC_OBJDIR)/dmd.$(OBJTAIL) $(NUC_OBJDIR)/dmf.$(OBJTAIL) $(NUC_OBJDIR)/evc.$(OBJTAIL) $(NUC_OBJDIR)/evce.$(OBJTAIL) $(NUC_OBJDIR)/evd.$(OBJTAIL) $(NUC_OBJDIR)/pmf.$(OBJTAIL) $(NUC_OBJDIR)/smf.$(OBJTAIL) \
           $(NUC_OBJDIR)/tcf.$(OBJTAIL) $(NUC_OBJDIR)/tmf.$(OBJTAIL) $(NUC_OBJDIR)/rlc.$(OBJTAIL) $(NUC_OBJDIR)/csc.$(OBJTAIL) $(NUC_OBJDIR)/tmc.$(OBJTAIL) $(NUC_OBJDIR)/pmc.$(OBJTAIL) $(NUC_OBJDIR)/pmce.$(OBJTAIL) \
           $(NUC_OBJDIR)/pmd.$(OBJTAIL) $(NUC_OBJDIR)/smc.$(OBJTAIL) $(NUC_OBJDIR)/smce.$(OBJTAIL) $(NUC_OBJDIR)/smd.$(OBJTAIL) $(NUC_OBJDIR)/tcc.$(OBJTAIL) $(NUC_OBJDIR)/tcce.$(OBJTAIL) $(NUC_OBJDIR)/tcd.$(OBJTAIL) \
           $(NUC_OBJDIR)/tms.$(OBJTAIL) $(NUC_OBJDIR)/tmse.$(OBJTAIL) $(NUC_OBJDIR)/tmd.$(OBJTAIL)


ifeq ($(MEMSUPER), 2)
   _FRAMEOBJS +=  vsi_ppm.$(OBJTAIL)
endif

_FRAMEOBJS +=  frame_version.$(OBJTAIL)

#----------------------------------- Special settings for each target
#----------Nucleus
ifeq ($(TARGET), nuc)
  ifeq ($(PLATFORM), arm7)

    OSDRV = os_drv.$(OBJTAIL)
    OS_ISR = os_isr.$(OBJTAIL)
    OS_LAYER_SRC:=./nuc/
    _OS_LAYER += os_evt.$(OBJTAIL)

   ifeq ($(TOOL_CHOICE), )
    COPTS+= -mw -x -pw2 -q -o
   endif

   CC=cl470
   COPTS+= -me -mt

   ifeq ($(DEBUG), 1)
     ifeq ($(TOOL_CHOICE), )
       COPTS += -g
     else
       COPTS += -gt
     endif
     COPTS += -mn
     DEFINES += -dNU_DEBUG
   endif

    ifeq ($(ASM_ONLY), 1)
      ASMOBJS = $(_NUCOBJS) $(_TIFOBJS) $(_MISCOBJS)
      COPTS+= -q -n -x0 -fs $(OBJDIR)
      MKLIB = $(GAWK) -f $(AWK_PROG) > $(FRAME_LIB)
      OBJTAIL = asm
      FRAME_LIB = $(OBJDIR)/frame$(XXX).pa
    else
      LIBEXT = lib
      OBJTAIL = obj
      FRAME_LIB=$(LIBDIR)/$(LIBNAME)$(XXX).$(LIBEXT)
      LINT_OPT_FILE += nucleus.lnt
      LINT_OPTS = $(LINT_INC) -I"$(subst \,/,$(C_DIR))" -u  $(LINT_OPT_FILE)
      MKLIB = ar470  $(LFLAGS)
    endif

	ifeq ($(OS_ONLY), 1)
	  DEFINES += -dFF_OS_ONLY
    else
	  DEFINES += -d_FF_RV_EXIST_
      COPTS+= -o3
	endif

    ifeq ($(POOL_SPLIT), 1)
      DEFINES += -dFF_POOL_SPLIT
    endif

    ifeq ($(RUN_INT_RAM), 1)
      COPTS += -dRUN_INT_RAM
    endif

    ifeq ($(RUN_FLASH), 1)
      COPTS += -dRUN_FLASH
    endif

    COPTS += -fr $(OBJDIR) -c

    ifeq ($(MEMSUPER), 2)
      DEFINES +=  -dMEMORY_SUPERVISION -dOPTIMIZE_POOL
    endif
    ifeq ($(MEMSUPER), 1)
      DEFINES += -dMEMORY_SUPERVISION
    endif

    INCLUDES+=-I"." -I"..\FRAME\NUC" -I"..\INC\NUC" -I"..\INC\NUC\ARM7" -I"..\INC" \
             -I"..\TST" -I"$(TI_DIR)\Riviera" -I"$(TI_DIR)\Riviera\rv" -I"$(TI_DIR)\Services\dar" \
             -I"$(TI_DIR)\Riviera\rvm" -I"$(TI_DIR)\System" -I"$(TI_DIR)\Services" \
             -I"$(TI_DIR)\Drivers\drv_app"  -I"$(TI_DIR)\Drivers\drv_app\ffs" \
             -I"$(TI_DIR)\System\deps_objs\gprs_dsample_amr_nw" -I"$(TI_DIR)\System\deps_objs\gprs_dsample_amr_nw\config"


    DEFINES += -d_TARGET_ -d_NUCLEUS_
    COPTSNF = $(COPTS) $(DEFINES) $(INCLUDES)
    INCLUDELIST=$(INCLUDES)
    DEFINELIST=$(subst -d,-D,$(DEFINES))
    COPTS_FILE=
    COMMLFLAGS = -rq $@             # only one command:a
    LFLAGS=$(COMMLFLAGS)
    LIBEXT=lib

    clean:
		@$(RM) obj/$(TARGET)/$(PLATFORM)$(DB)$(OSAL)$(PS)$(SECTION)$(TS)/*.$(OBJTAIL)
		@$(RM) $(FRAME_LIB)

  endif
endif         # nucleus & arm7

#----------Nucleus
ifeq ($(TARGET), nuc)
  ifeq ($(PLATFORM), arm9)

    OSDRV = os_drv.$(OBJTAIL)
    OS_ISR = os_isr.$(OBJTAIL)
    OS_LAYER_SRC:=./nuc/
    _OS_LAYER += os_evt.$(OBJTAIL)

    CC=cl470
    COPTS+= -me -mt

    ifneq ($(DISABLE_OPT), 1)
      ifeq ($(TOOL_CHOICE), 6)
        COPTS+= -O=2
      else
        COPTS+= -o
      endif
    endif

    ifeq ($(DEBUG), 1)
      ifeq ($(TOOL_CHOICE), 6)
        COPTS += --symdebug:coff
      else
      # toolset 3 and 5
        COPTS += -gt
      endif
      COPTS += -mn
      DEFINES += -dNU_DEBUG
    endif

    LINT_OPT_FILE += nucleus.lnt
    LINT_OPTS = $(LINT_INC) -I"$(subst \,/,$(C_DIR))" -u  $(LINT_OPT_FILE)

	ifeq ($(OS_ONLY), 1)
	  DEFINES += -dFF_OS_ONLY
	endif

	ifeq ($(ESF), 1)
	  ESF_FUNC = esf_func.$(OBJTAIL)
	  DEFINES +=  -d_ESF_SUPPORT_
	endif

    ifeq ($(RUN_INT_RAM), 1)
      COPTS += -dRUN_INT_RAM
    endif

    ifeq ($(RUN_FLASH), 1)
      COPTS += -dRUN_FLASH
    endif

    COPTS += -fr $(OBJDIR) -c

    ifeq ($(MEMSUPER), 2)
      DEFINES +=  -dMEMORY_SUPERVISION -dOPTIMIZE_POOL
    endif
    ifeq ($(MEMSUPER), 1)
      DEFINES += -dMEMORY_SUPERVISION
    endif

    INCLUDES+=-I"." -I"..\FRAME\NUC" -I"..\INC\NUC" -I"..\INC" -I"..\TST"
    MKLIB=ar470 $(LFLAGS)

    INCLUDES += -I"..\inc\NUC\ARM9"

    DEFINES += -d_TARGET_ -d_NUCLEUS_
    COPTSNF = $(COPTS) $(DEFINES) $(INCLUDES)
    INCLUDELIST=$(INCLUDES)
    DEFINELIST=$(subst -d,-D,$(DEFINES))
    COPTS_FILE=
    COMMLFLAGS = -rq $@             # only one command:a
    LFLAGS=$(COMMLFLAGS)
    LIBEXT=lib
    OBJTAIL=obj
    FRAME_LIB=$(LIBDIR)/$(LIBNAME)$(XXX).$(LIBEXT)

    clean:
		@$(RM) obj/$(TARGET)/$(PLATFORM)$(DB)$(OSAL)$(PS)$(ES)$(SECTION)$(OPT)$(TS)/*.$(OBJTAIL)
		@$(RM) $(FRAME_LIB)

  endif
endif         # nucleus & arm9


#----------------------------------- Special settings for each target
#----------Nucleus_ARM
ifeq ($(TARGET), nuc_ARM)

   OSDRV = os_drv.$(OBJTAIL)
   OS_LAYER_SRC:=./nuc/
   _OS_LAYER += os_evt.$(OBJTAIL)

   CC=perl "\G23M\Condat\INT\BIN\gti_compile.pl"
   COPTS= -me -mt -pw2 -q  -x -mw -o  # from condat_env.mak and os.mak for nucleus.lib

   ifeq ($(DEBUG), 1)
      COPTS += -g -mn
      DEFINES += -dNU_DEBUG
   endif
   COPTS += -fr $(OBJDIR) -fo $(OBJDIR)/$@ -c

   ifeq ($(MEMSUPER), 2)
     DEFINES +=  -dMEMORY_SUPERVISION -dOPTIMIZE_POOL
   endif
   ifeq ($(MEMSUPER), 1)
     DEFINES += -dMEMORY_SUPERVISION
   endif

   INCLUDES+=-I"..\FRAME\NUC" -I"..\inc\NUC" -I"..\inc\NUC\ARM7" -I"..\inc"  -I"..\TST"
   DEFINES += -d_TARGET_ -d_NUCLEUS_
   COPTSNF = $(COPTS) $(DEFINES) $(INCLUDES)
   INCLUDELIST=$(INCLUDES)
   DEFINELIST=$(subst -d,-D,$(DEFINES))
   COPTS_FILE=
   COMMLFLAGS = -rq $@             # only one command:a
   LFLAGS=$(COMMLFLAGS)
   MKLIB=perl "\G23M\Condat\INT\BIN\gti_archive.pl" $(LFLAGS)
   LIBEXT=lib
   OBJTAIL=obj
   FRAME_LIB=$(LIBDIR)/$(LIBNAME)$(XXX).$(LIBEXT)

   clean:
		@$(RM) obj/$(TARGET)$(DB)$(PS)/*.$(OBJTAIL)
		@$(RM) $(FRAME_LIB)

endif         # target: Nucleus_ARM


#----------------------------------- Special settings for each target
#----------NucleusWin
ifeq ($(TARGET), nuc)
  ifeq ($(PLATFORM), pc)

    OS_ISR = os_isr.$(OBJTAIL)
    _OS_LAYER += os_ctb.obj
    _OS_LAYER += os_evt.$(OBJTAIL)
    OS_LAYER_SRC:=./nuc/
    CC=cl
    COPTS = /c /nologo /W3 /GX /Zp1 /GZ
    INCLUDES += -I"." -I"..\inc\NUC" -I"..\inc\NUC\PC" -I"..\inc" -I"..\TST"
    LINT_OPT_FILE += co-msc50.lnt
    LINT_OPTS = $(LINT_INC) -D_WIN32 -D_M_IX86 -I"NUC" -u $(LINT_OPT_FILE)

    ifeq ($(DEBUG), 1)
      COPTS += /MD /DEBUG /Z7
      DEFINES += -D"_DEBUG" -D"NU_DEBUG"
    else
      COPTS += /MT
    endif

    COPTS += /FR"..\temp\nucwin"  /Fd"..\temp\nucwin" /Fo./$(OBJDIR)/

    ifeq ($(OS_ONLY), 1)
	  DEFINES += -D"FF_OS_ONLY"
    endif

    ifeq ($(MEMSUPER), 2)
      DEFINES +=  -D"MEMORY_SUPERVISION"  -D"OPTIMIZE_POOL"
    endif
    ifeq ($(MEMSUPER), 1)
      DEFINES += -D"MEMORY_SUPERVISION"
    endif

    ifeq ($(POOL_SPLIT), 1)
      DEFINES += -D"FF_POOL_SPLIT"
    endif

    DEFINES += -D"_NUCLEUS_"
    COPTSNF = $(COPTS) $(DEFINES) $(INCLUDES)
    COPTS_FILE =
    INCLUDELIST=$(subst /I ,-I,$(INCLUDES))
    DEFINELIST=$(subst /D ,-D,$(DEFINES))

    COMMLFLAGS =  /nologo  /subsystem:console  /machine:I386
    LFLAGS=$(COMMLFLAGS) #/debug??
    MKLIB=LIB /out:$@ $(LFLAGS)
    LIBEXT=lib
    OBJTAIL=obj
    FRAME_LIB=$(LIBDIR)/$(LIBNAME)$(XXX).$(LIBEXT)

    clean:
		@$(RM) obj/$(TARGET)/$(PLATFORM)$(DB)$(OSAL)$(PS)/*.$(OBJTAIL)
		@$(RM) $(FRAME_LIB)

  endif
endif         # nucleus && pc


ifeq ($(OSENV), sus)

    #_OS_LAYER += os_ctb.o
    OS_ISR = os_isr.$(OBJTAIL)
    ESF_FUNC = esf_func.$(OBJTAIL)
    OS_LAYER_SRC:=./$(OSENV)/
    COPTS = -c -o$(OBJDIR)/$@
    INCLUDES += -I. -I../inc/$(OSENV) -I../inc -I../TST -I$(OS_LAYER_SRC)
    ifeq ($(TARGET), linuxtool)
      DEFINES += -D_TOOLS_
    endif

    DEFINES += -DPURE_USER_MODE

    ifeq ($(OSL_LOG), 1)
      DEFINES += -DOSL_OSL_LOG
    endif
    ifeq ($(OS_ONLY), 1)
      DEFINES += -DFF_OS_ONLY
    endif

    ifeq ($(DEBUG), 1)
      COPTS += -g
      DEFINES += -DOSL_DEBUG
    endif

    ifeq ($(MEMSUPER), 2)
      DEFINES +=  -DMEMORY_SUPERVISION  -DOPTIMIZE_POOL
    endif
    ifeq ($(MEMSUPER), 1)
      DEFINES += -DMEMORY_SUPERVISION
    endif

   ifeq ($(TARGET), rtlpc)
     COPTSNF = $(CFLAGS)
     MKLIB= i386-linux-ar ruv $@
     RANLIB= i386-linux-ranlib
     DEFINES += -D_RT_LINUX_
   else
     ifeq ($(TARGET), linuxarm)
       CC=arm-linux-gcc
       MKLIB= arm-linux-ar ruv $@
       RANLIB= arm-linux-ranlib
       DEFINES += -D_TARGET -D_LINUX_ -D_ESF_SUPPORT_
     else
       ifeq ($(TARGET), solsparc)
         CC=gcc
         MKLIB= ar ruv $@
         RANLIB= ranlib
         DEFINES += -D_SOLARIS_ -D_LINUX_ -D_ESF_SUPPORT_
       else
         CC=gcc
         MKLIB= ar ruv $@
         RANLIB= ranlib
         DEFINES += -D_LINUX_ -D_ESF_SUPPORT_
       endif
     endif
   endif
    COPTSNF += $(COPTS) $(DEFINES) $(INCLUDES)
    COPTS_FILE =
    INCLUDELIST=$(subst /I ,-I,$(INCLUDES))
    DEFINELIST=$(subst /D ,-D,$(DEFINES))

    MKLIB=ar ruv $@
    LIBEXT=a
    OBJTAIL=o
    FRAME_LIB=$(LIBDIR)/lib$(LIBNAME)$(XXX).$(LIBEXT)

    clean:
		@$(RM) $(OBJDIR)/*.$(OBJTAIL)
		@$(RM) $(FRAME_LIB)

#_TESTOBJS=os_pro.o os_mem.o os_com.o os_sem.o os_tim.o os_mis.o
#TESTOBJS=$(patsubst %.$(OBJTAIL),$(OBJDIR)/%.$(OBJTAIL),$(_TESTOBJS))
   os_test: os_test$(tTail)
	@echo os_test$(tTail) built

   os_test$(tTail): $(_TESTOBJS) sus/os_test.c $(FRAME_LIB)
	$(CC) -g -o $@ $(INCLUDES) $(DEFINES)  sus/os_test.c $(FRAME_LIB) -lpthread -lrt

endif # osenv = sus

#----------------------------------- Special settings for each target
#----------Win32
ifeq ($(TARGET), win32)
   MISCXXX:=$(tTail)$(dbTail)
   TIFXXX:=$(tTail)$(dbTail)
   TOOLS_LIBDIR=../tools/lib
   OS_LAYER_SRC:=./vcms/
   CC=cl
   COPTS += /c /nologo /GX /W3 /Zp1 /GZ
   INCLUDES += -I"." -I$(OS_LAYER_SRC) -I"..\inc\vcms" -I"..\inc\win32" -I"..\inc\vcms\win32" -I"..\inc"
   LINT_OPT_FILE += co-msc50.lnt
   LINT_OPTS = $(LINT_INC) -D_WIN32 -D_M_IX86 -D_MSC_VER -D"WIN32" -u $(LINT_OPT_FILE)

   INCLUDES += -I"..\lib\win32"
   _OS_LAYER += frame_dllmain.$(OBJTAIL)

   COPTS += /FR"..\temp\win32"  /Fo./$(OBJDIR)/

   DEFINES += -D"CRASH_TOL"
   DEFINES += -D"_TOOLS_"

   DEFINES += -D"FRAME_DLL"
   DLLFLAGS= /dll\
             $(EXPFILE) $(LIBDIR)/misc$(MISCXXX).lib \
             $(LIBDIR)/tif$(TIFXXX).lib \
             $(LIBDIR)/ntutils.lib $(TOOLS_LIBDIR)/psapi.lib \
             user32.lib

   COMMLFLAGS = $(DLLFLAGS)
   COMMLFLAGS += /nologo
   ifeq ($(DEBUG), 1)
     COPTS += -DEBUG /MDd /Z7
     DEFINES += -D"_DEBUG"
     COMMLFLAGS += /DEBUG /incremental:no /pdb:none\
                   ../os_lib/win32/debug/cms.lib \
                   /nodefaultlib:"libcmtd"
   else  #debug
     COPTS += /MD
     COMMLFLAGS += ../os_lib/win32/cms.lib \
                   /nodefaultlib:"libcmt"
   endif #debug
   COPTSNF = $(COPTS) $(DEFINES) $(INCLUDES)
   INCLUDELIST=$(subst /I ,-I,$(INCLUDES))
   DEFINELIST=$(subst /D ,-D,$(DEFINES))

   LFLAGS=$(COMMLFLAGS)
   LFLAGS += /SECTION:FRAME_SHARED,RWS
   LIBEXT = dll
   MKLIB=link.exe /out:$@ $(LFLAGS)
   OBJTAIL=obj
   FRAME_LIB=$(BINDIR)/$(LIBNAME)$(XXX).$(LIBEXT)

   COPTS_FILE=
   EXPFILE=$(LIBDIR)/frame$(XXX).exp
   IMPLIB=$(LIBDIR)/frame$(XXX).lib

   INC_FILES+=vcms/os_const.h

   implib: $(IMPLIB)

   $(IMPLIB) $(EXPFILE): $(OBJDIR) $(_OS_LAYER) $(_FRAMEOBJS)
#  OWN_MAKELEVEL == 0 if makefile called directly, == 1 if nested
   ifneq ($(OWN_MAKELEVEL), 0)
	@echo checking out and touching using $(MAKE) for two possible targets ($@), make level $(OWN_MAKELEVEL)
		$(CHECKOUT) $(IMPLIB)
		touch -c $(IMPLIB)
   else
		gnumake MAKE=gnumake OWN_MAKELEVEL=1 $(subst \,/,$@ $<)
	@echo proceeding using $(MAKE) for two possible targets ($@), make level $(OWN_MAKELEVEL)
		lib.exe /nologo /MACHINE:IX86 /DEF:frame$(msTail).def $(FRAMEOBJS)\
            $(OS_LAYER) /OUT:$(IMPLIB) /NAME:frame$(XXX).dll
   endif

   clean:
	    $(RM) $(OBJDIR)/*.$(OBJTAIL)
	    $(RM) $(IMPLIB) $(EXPFILE) $(FRAME_LIB)

endif         # target: win32



#----------------------------------- Special settings for each target
#----------pSOS+
ifeq ($(TARGET), psos)

   OS_LAYER_SRC:=./psos/
   CC=cc386
   COPTS_FILE:=c.opt
   _OS_LAYER += drv_conf.$(OBJTAIL)
   SPEC_F_OBJS=

   -include $(PSS_BSP)/bsp.mk# board support settings e.g. MODEL, FPU

   ifeq ( FPU,H )
      FPUOPT = -VNDP
   else
      FPUOPT=
   endif

   COPTS1  =       -c -V$(MODEL) -VROM -VBSS -VANSI -VLONGNAME -VSPROTO -VQUIET -VDIAGNOSTICS
   COPTS2  =       $(FPUOPT)  -I$(COMFSRC)/configps/
   COPTS3  =       -I. -I$(PSS_ROOT)/include -I$(PSS_BSP) -I$(PSS_ROOT)/bsps/devices
   COPTS4  =        -I./ -I../inc/ -I../inc/psos/ -I./psos/ -I../../g23net/dev/config  -I../TST
   COPTS5  =       -DMODEL=$(MODEL) -VNOSIGNEDCHAR -D__CADUL__=1 -DPSOS=1 -D_PSOS_

   ifeq ($(MEMSUPER), 2)
      COPTS5 += -DMEMORY_SUPERVISION -DOPTIMIZE_POOL
   endif
   ifeq ($(MEMSUPER), 1)
      COPTS5 += -DMEMORY_SUPERVISION
   endif

   ifeq ($(DEBUG), 1)
      COPTS5 += -VXDB
   endif

   COPTS5 += -DTEST_TARGET_IP_ADDRESS

   COPTS   =       @c.opt

#$(COPTS1)  $(COPTS2)  $(COPTS3)  $(COPTS4)  $(COPTS5) ich wollte c.opt umgehen. erstmal keinen Erfolg.

   COPTS += -o $(OBJDIR)/$@
   COPTSNF = $(COPTS)
   LIBEXT = lib
   MKLIB = lib386 $@         # psos related libs will not be included here
   OBJTAIL=obj
   FRAME_LIB=$(LIBDIR)/$(LIBNAME)$(XXX).$(LIBEXT)

c.opt:  makefile
	@echo $(COPTS1) >  c.opt
	@echo $(COPTS2) >> c.opt
	@echo $(COPTS3) >> c.opt
	@echo $(COPTS4) >> c.opt
	@echo $(COPTS5) >> c.opt

#psos.abs: os.abs wie? wozu??

#~~~~~~
   clean:
		@$(RM) obj/$(TARGET)$(DB)$(PS)/*.$(OBJTAIL)
		@$(RM) $(FRAME_LIB)
		@$(RM) c.opt
endif         # target: psos

#----------------------------------- Special settings for each target
#---------- vxworks
ifeq ($(TARGET), vxworks)

   OS_PART = os_part.$(OBJTAIL)

   CC = ccppc
   OS_LAYER_SRC  = ./$(TARGET)
   SPEC_OSL_INC  =
   SPEC_OSL_OBJS =
   SPEC_F_OBJS   =

  ifeq ($(DEBUG), 1)
    COPTSNF  = -g
  else
    COPTSNF  = -O2
  endif
    COPTSNF += -c -mstrict-align -ansi -nostdinc -DRW_MULTI_THREAD -D_REENTRANT \
               -fvolatile -fno-builtin -fno-for-scope -DCPU=$(CPU) \
               -I$(WIND_BASE)/target/h -I. -I../../g23net/dev/config -I../inc/$(TARGET) \
               -I../inc -D_VXWORKS_  -I../TST

   ifeq ($(MEMSUPER), 2)
      COPTSNF +=  -DMEMORY_SUPERVISION -DOPTIMIZE_POOL
   endif
   ifeq ($(MEMSUPER), 1)
      COPTSNF += -DMEMORY_SUPERVISION
   endif

   COPTSNF += -o $(OBJDIR)/$@
   LIBEXT   = a
   MKLIB     = arppc crus $@
   OBJTAIL=obj

   FRAME_LIB= $(LIBDIR)/$(LIBNAME)$(XXX).$(LIBEXT)

clean:
	@$(RM) obj/$(TARGET)$(DB)$(PS)/*.o*
	@$(RM) $(FRAME_LIB)

endif
# target: vxworks

######################### echo for control ##################################

#echo:
#	@echo $(_OS_LAYER)
#	@echo $(OS_LAYER)
#	@echo $(_FRAMEOBJS)
#	@echo $(FRAMEOBJS)
#	@echo $(SRCFILES)
#	@echo $(DEFINES)
#	@echo $(DEFINELIST)

######################### OS layer ##################################

OS_LAYER=$(patsubst %.$(OBJTAIL),$(OBJDIR)/%.$(OBJTAIL),$(_OS_LAYER))

os_layer: $(COPTS_FILE) $(_OS_LAYER)

#########################  Frame    ##################################
.PHONY:	all label help

asm: $(OBJDIR) $(FRAME_LIB)

FRAMEOBJS=$(patsubst %.$(OBJTAIL),$(OBJDIR)/%.$(OBJTAIL),$(_FRAMEOBJS))

frameobjs: $(COPTS_FILE) $(_OS_LAYER) $(_FRAMEOBJS)

frame.$(LIBEXT): $(FRAME_LIB)

$(OBJDIR) :
	$(MKDIR) -p $(OBJDIR)

$(FRAME_LIB) : $(OBJDIR) $(COPTS_FILE) $(_OS_LAYER) $(_FRAMEOBJS) makefile $(EXPFILE)
#  OWN_MAKELEVEL == 0 if makefile called directly, == 1 if nested
ifneq ($(OWN_MAKELEVEL), 0)
	@echo checking out and touching using $(MAKE)
	$(CHECKOUT) $@ $(IMPLIB)
	touch -c $@ $(IMPLIB)
else
	gnumake MAKE=gnumake OWN_MAKELEVEL=1 $(subst \,/,$@ $<)
	@echo proceeding using $(MAKE)
  ifneq ($(OSENV), sus)
		$(RM) $@
  endif
		$(MKLIB) $(FRAMEOBJS) $(OS_LAYER) $(ASMOBJS)
  ifeq ($(OSENV), sus)
		$(RANLIB) $@
  endif
endif

$(_OS_LAYER) $(_FRAMEOBJS): %.$(OBJTAIL) :	%.c
	$(RM) $(OBJDIR)/frame_version.$(OBJTAIL)
ifneq ($(NO_LINT), 1)
	$(LINT) $(LINT_OPTS) $(DEFINES) $(INCLUDES) $<
endif
	$(CC) $(COPTSNF) $<

SRCFILES=$(patsubst %.$(OBJTAIL),%.c,$(_FRAMEOBJS) $(_OS_LAYER))

FRAME_DEP=dep_frame_$(PROJECT)$(XXX).mk

help:
	$(ECHO) usage...
	$(ECHO) building:
	$(ECHO) "gnumake [DEBUG={1|0}] [ROUTING={1|0}] [MEMSUPER={1|0}] ..."
	$(ECHO) cleaning:
	$(ECHO) "gnumake clean [DEBUG={1|0}] [ROUTING={1|0}] [MEMSUPER={1|0}] ..."
	$(ECHO) ClearCase:
	$(ECHO) "gnumake checkin - not implemeted yet -"
	$(ECHO) "gnumake label LABEL=<label type> [FLOAT={1|0}] [DEBUG={1|0}] [ROUTING={1|0}] "
	$(ECHO) "        [MEMSUPER={1|0}] ..."
	$(ECHO) help:
	$(ECHO) "gnumake help"

DEPEND_DIRS=$(GPFDIR) $(LIBDIR) $(BINDIR) $(INCDIR)
EXPORT_FILES=$(IMPLIB) $(FRAME_LIB)
INCLUDE_FILES=$(patsubst %,../inc/%,$(INC_FILES))

label:
	-$(CLEARTOOL) mklabel -recurse $(REPLACE) $(LABEL) . 2>> error.log
	-$(CLEARTOOL) mklabel $(REPLACE) $(LABEL) $(DEPEND_DIRS) \
	                      $(EXPORT_FILES) $(INCLUDE_FILES) 2>> error.log

depend: $(SRCFILES)
	@touch $(FRAME_DEP)
	makedepend -b -f$(FRAME_DEP) $^ -Y -o.$(OBJTAIL) $(INCLUDELIST) $(DEFINELIST)
	@$(RM) $(FRAME_DEP)

-include $(FRAME_DEP)


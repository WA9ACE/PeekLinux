#----------------------------------------------------------------------------- 
#  Project :  
#  Modul   :  makefile
#----------------------------------------------------------------------------- 
#  Copyright 2002 Texas Instruments Deutschland GmbH
#                 All rights reserved. 
# 
#                 This file is confidential and a trade secret of Texas 
#                 Instruments Deutschland GmbH
#                 The receipt of or possession of this file does not convey 
#                 any rights to reproduce or disclose its contents or to 
#                 manufacture, use, or sell anything it may describe, in 
#                 whole, or in part, without the specific written consent of 
#                 Texas Instruments Deutschland GmbH. 
#----------------------------------------------------------------------------- 
#  Purpose:   (gnu-) makefile to build libraries of the Condat Coder Decoder
#             targets:  ccd.dll, ccd.lib and clean
#             Hint: To achieve variations of ccd.lib you need to set some 
#             environment variables, e.g. DEBUG and TRACE. To ease the building 
#             process you can also use the batch file "m.bat".
#-----------------------------------------------------------------------------

include ccd_inc.mk

VPATH=$(OBJDIR)

#---------- Directories
GPF:=$(subst \,/,$(GPF))

# force to use sh.exe in gpf
ifneq ($(OSENV), sus)
SHELL=$(GPF)/tools/bin/sh.exe
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

ifdef PATH_CC_1_22E 
 Path:=$(PATH_CC_1_22E);$(Path)
 PATH:=$(PATH_CC_1_22E);$(PATH)
 C_DIR:=$(PATH_CC_1_22E)
endif

ifeq ($(TOOL_CHOICE), 3)
 COPTS= --align_structs=4 -pden 
 ifndef PATH_CC_2_54 
   PATH_CC_2_54=C:/Dvpt_tools/Codegen/TMS4701x_2.54
 endif
 Path:=$(PATH_CC_2_54)/bin;$(Path)
 INCLUDES += -I"$(PATH_CC_2_54)/include"
endif

ifeq ($(TOOL_CHOICE), 5)
 COPTS= --align_structs=4 -pden 
 ifndef PATH_CC_2_55 
   PATH_CC_2_55=C:/Dvpt_tools/Codegen/TMS4701x_2.55
 endif
 Path:=$(PATH_CC_2_55)/bin;$(Path)
 INCLUDES += -I"$(PATH_CC_2_55)/include"
endif

ifeq ($(TOOL_CHOICE), 6)
  # --code_state=16 => -mt
  # --endian=little => -me
  COPTS= -mv=5e --abi=ti_arm9_abi
  COPTS+= -pden -pdv -mf -ms -qq -disable_branch_chaining -pi
  COPTS+= -O=2
  ifndef PATH_CC_4_11 
    PATH_CC_4_11=C:/Dvpt_tools/Codegen/TMS4701x_4.11
  endif
  Path:=$(PATH_CC_4_11)/bin;$(Path)
  INCLUDES += -I"$(PATH_CC_4_11)/include"
endif

# for created object files
ifeq ($(PLATFORM), )
OBJDIR=obj/ccd/$(TARGET)$(TRACEDIR)$(DEBUGDIR)$(PSDIR)
else
OBJDIR=obj/ccd/$(TARGET)/$(PLATFORM)$(TSDIR)$(TRACEDIR)$(DEBUGDIR)$(PSDIR)$(DADIR)$(SECTION)
endif

# guarantee / as path seperator even if OBJDIR is set with the make call
override OBJDIR:=$(subst \,/,$(OBJDIR))

# for stack calculation
GAWK=$(GPF)/tools/bin/gawk.exe
AWK_PROG=$(GPF)/BIN/cg$(tTail).awk

# for created libraries
LIBDIR=../LIB
BINDIR=../BIN$(DEBUGDIR)

FRAMELIB_DIR	=../lib/win32$(DEBUGDIR)

# for CCD source files
CCD_SRC=./


#---------- shell calls
RM      =       rm -rf
ECHO	= 	@echo
ifeq ($(OSENV), sus)
  MKDIR =       mkdir
else
  MKDIR =       $(GPF)/tools/bin/mkdir
endif

ifeq ($(CLRCAS), 1)
  CHECKOUT = -cleartool co -nda -nc
  CHECKIN = -cleartool ci -nc
else
  CHECKOUT = attrib -r
  CHECKIN = attrib +r
endif

CO_LIBS = $(CCD_LIB)

_CCD_OBJS = ccd.$(OBJTAIL) bitfun.$(OBJTAIL) cdc_std.$(OBJTAIL) \
            cdc_com.$(OBJTAIL) ccd_err.$(OBJTAIL) ccd_store.$(OBJTAIL)\
        gsm1_tv.$(OBJTAIL) gsm1_v.$(OBJTAIL) gsm2_t.$(OBJTAIL) gsm3_tv.$(OBJTAIL) gsm3_v.$(OBJTAIL) gsm4_lv.$(OBJTAIL) \
        gsm4_tlv.$(OBJTAIL) gsm5_tlv.$(OBJTAIL) gsm5_tv.$(OBJTAIL) gsm5_v.$(OBJTAIL) gsm6_tlv.$(OBJTAIL) gsm7_lv.$(OBJTAIL) \
        bcd_mnc.$(OBJTAIL) bcd_nofill.$(OBJTAIL) bcdeven.$(OBJTAIL) bcdodd.$(OBJTAIL) \
        gsm1_asn.$(OBJTAIL) \
        csn1_s1.$(OBJTAIL) csn1_sh.$(OBJTAIL) s_padding.$(OBJTAIL) t30_ident.$(OBJTAIL) \
                asn1_bitstr.$(OBJTAIL) asn1_octet.$(OBJTAIL) asn1_integ.$(OBJTAIL) \
                asn1_seq.$(OBJTAIL) asn1_choice.$(OBJTAIL) no_code.$(OBJTAIL)\
                asn1_choice_ext.$(OBJTAIL) asn1_seq_ext.$(OBJTAIL) asn1_integ_ext.$(OBJTAIL) \
                asn1_opentype.$(OBJTAIL) asn1_objid.$(OBJTAIL)\
                noncritical_ext.$(OBJTAIL) critical_ext.$(OBJTAIL)\
                ccd_elem.$(OBJTAIL) s_padding_0.$(OBJTAIL) csn1_s0.$(OBJTAIL) hl_flag.$(OBJTAIL) \
                freq_list.$(OBJTAIL) fdd_ci.$(OBJTAIL) tdd_ci.$(OBJTAIL) freq_list_com.$(OBJTAIL) \
                csn1_concat.$(OBJTAIL) break_cond.$(OBJTAIL) csn1_sx.$(OBJTAIL) \
                csn1_choice_1.$(OBJTAIL) csn1_choice_2.$(OBJTAIL) csn1_choice_x.$(OBJTAIL) \
                csn1_sh_opt.$(OBJTAIL) csn1_s1_opt.$(OBJTAIL) csn1_s0_opt.$(OBJTAIL)

ifeq ($(TARGET), win32)
_CCD_OBJS += ccd_config.obj ccd_patch.obj
endif

#----------------------------------- Special settings for each target 
#----------Nucleus
ifeq ($(TARGET), nuc)
ifeq ($(PLATFORM), arm7)

   ifeq ($(TOOL_CHOICE), )
    COPTS+= -mw -x -pw2
   endif

   CC=cl470
   COPTS+= -me -mt -q
   ifeq ($(ASM_ONLY), 1)
     COPTS+= -q -n -x0 -fs $(OBJDIR)
     MKLIB = $(GAWK) -f $(AWK_PROG) > $(CCD_LIB)
     OBJTAIL = asm
     CCD_LIB = $(OBJDIR)/ccd$(XXX).pa
   else
     COPTS+= -o 
     MKLIB = ar470  $(LFLAGS)
     LIBEXT = lib
     OBJTAIL = obj
     CCD_LIB = $(LIBDIR)/ccd$(XXX).$(LIBEXT)
   endif

   ifeq ($(DEBUG), 1)
      ifeq ($(TOOL_CHOICE), )
        COPTS += -g
      else
        COPTS += -gt
      endif
      COPTS += -mn   
   endif
   COPTS += -fr $(OBJDIR) -c 

   ifeq ($(TRACE), 1)
     DEFINES += -dDEBUG_CCD -dERR_TRC_STK_CCD
   endif

   ifeq ($(RUN_INT_RAM), 1)
     DEFINES += -dRUN_INT_RAM
   endif

   ifeq ($(RUN_FLASH), 1)
     DEFINES += -dRUN_FLASH
   endif

   ifeq ($(MEMSUPER), 1)
     DEFINES += -dMEMORY_SUPERVISION
   endif

   INCLUDES += -I"." -I"../inc" -I"../inc/nuc"
   ifeq ($(DYNARR), 1)
     DEFINES += -dDYNAMIC_ARRAYS
   endif
   DEFINES += -dM_INTEL -dSHARED_VSI -dSHARED_CCD
   DEFINES += -dCCDDATA_STATIC_DATA
   DEFINES += -d_TMS470 -dNEW_ENTITY -dERR_TRACE_CCD
   COPTSNF  = $(COPTS) $(DEFINES) $(INCLUDES)
   INCLUDELIST = $(INCLUDES)
   DEFINELIST = $(subst -d,-D,$(DEFINES))
   COMMLFLAGS = -rq $@             
   LFLAGS = $(COMMLFLAGS)

endif       # platform: arm7, TI compiler
endif       # target: nucleus OS

#----------Nucleus & arm9
ifeq ($(TARGET), nuc)
ifeq ($(PLATFORM), arm9)

   CC=cl470
   COPTS+= -me -mt -q

   ifeq ($(ASM_ONLY), 1)
     COPTS+= -n -pn -pi -fs $(OBJDIR) 
     MKLIB = $(GAWK) -f $(AWK_PROG) > $(CCD_LIB)
     OBJTAIL = asm
     CCD_LIB = $(OBJDIR)/ccd$(XXX).pa
   else
     COPTS+= -c -o  
     MKLIB=ar470  $(LFLAGS)
     LIBEXT=lib
     OBJTAIL=obj
     CCD_LIB=$(LIBDIR)/ccd$(XXX).$(LIBEXT)
     ifeq ($(DEBUG), 1)
        COPTS += -mn   
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
   endif
   COPTS += -fr $(OBJDIR)

   ifeq ($(TRACE), 1)
     DEFINES += -dDEBUG_CCD -dERR_TRC_STK_CCD
   endif
   ifeq ($(MEMSUPER), 1)
     DEFINES += -dMEMORY_SUPERVISION
   endif

   INCLUDES += -I"." -I"../inc" -I"../inc/nuc" 

   DEFINES += -dM_INTEL -dSHARED_VSI -dCCD_PLUGIN_GSM -dSHARED_CCD
   DEFINES += -dCCDDATA_STATIC_DATA -dDYNAMIC_ARRAYS
   DEFINES += -d_TMS470 -dNEW_ENTITY -dERR_TRACE_CCD
   DEFINES += -dCCDDATA_U32
   ifeq ($(CCD_SYMBOLS), 1)
     DEFINES += -dCCD_SYMBOLS
   endif

   COPTSNF  = $(COPTS) $(DEFINES) $(INCLUDES)
   COMMLFLAGS = -rq $@             
   LFLAGS=$(COMMLFLAGS)

endif       # platform: arm9, TI compiler
endif       # target: nucleus OS

#----------NucleusWin
ifeq ($(TARGET), nuc)
ifeq ($(PLATFORM), pc)

   CC=cl
   COPTS = /c /nologo /W3 /GX /Zp1
   COPTS += /Fo$(OBJDIR)/
   ifeq ($(DEBUG), 1)
     COPTS += /MTd /Z7 
   else
     COPTS += /MT
   endif

   ifeq ($(GLOBS), 1)
     _CCD_OBJS = pc_globs.obj
     CCD_LIB = $(OBJDIR)/pc_globs.exe
     MKLIB = link.exe /out:$@ $(LFLAGS) /INCREMENTAL:NO /PDB:NONE
   else
     CCD_LIB = $(LIBDIR)/ccd$(XXX).$(LIBEXT)
     MKLIB = link.exe -lib /out:$@ $(LFLAGS)
   endif

   ifeq ($(TRACE), 1)
     DEFINES += -DDEBUG_CCD -DERR_TRC_STK_CCD
   endif
   ifeq ($(MEMSUPER), 1)
     DEFINES += -DMEMORY_SUPERVISION
   endif

   INCLUDES = -I"." -I"../inc" -I"../inc/nuc" 
   DEFINES += -D"M_INTEL" -D"SHARED_VSI" -D"SHARED_CCD"\
              -D"CCDDATA_STATIC_DATA" -D"DYNAMIC_ARRAYS"\
              -D"CCD_SYMBOLS" -D"NEW_ENTITY" -D"WIN32"\
              -D"ERR_TRACE_CCD"
   COPTSNF = $(COPTS) $(DEFINES) $(INCLUDES)
   INCLUDELIST = $(subst /I ,-I,$(INCLUDES))
   DEFINELIST = $(subst /D ,-D,$(DEFINES))
   COMMLFLAGS =  /nologo  #/subsystem:console  /machine:I386
   LFLAGS = $(COMMLFLAGS)
   LIBEXT = lib
   OBJTAIL = obj

endif       # platform: PC
endif       # target: nucleus OS

#----------------------------------- Special settings for each target 
#----------Win32
ifeq ($(TARGET), win32)
   LIBDIR=../LIB/WIN32$(DEBUGDIR)
   CCD_IMPORTLIB=$(LIBDIR)/ccd.lib

   CC=cl
   COPTS += /c /nologo /W3 /GX /Zp1

   ifeq ($(DEBUG), 1)
     COPTS += /MDd /Z7 
   else
     COPTS += /MD
   endif

   COPTS += /Fo./$(OBJDIR)/
   INCLUDES = -I"." -I"../inc" -I"../inc/vcms" -I"../inc/vcms/win32"
   DEFINES += -D"M_INTEL" -D"_TOOLS_" -D"CCDDATA_LOAD"\
            -D"WIN32" -D"TARGET_WIN32" -D"CCD_SYMBOLS" -D"CCD_MK"

   DEFINES += -D"SHARED_VSI" -D"SHARED_CCD"\
              -D"NEW_ENTITY" -D"DYNAMIC_ARRAYS" -D"ERR_TRACE_CCD"
   DEFINES += -D"ERR_TRC_STK_CCD" -D"DEBUG_CCD"

   COPTSNF = $(COPTS) $(DEFINES) $(INCLUDES)
   INCLUDELIST=$(INCLUDES)
   DEFINELIST=$(DEFINES)
   COMMLFLAGS += /nologo /subsystem:console /machine:I386

   LFLAGS = $(COMMLFLAGS)
   MKLIB = lib.exe /out:$@ $(LFLAGS)
   LIBEXT = lib
   OBJTAIL = obj
   CCD_LIB = $(LIBDIR)/ccd.$(LIBEXT)

endif         # target: win32

ifeq ($(OSENV), sus)
   COPTS += -c -o $(OBJDIR)/$@
   ifeq ($(DEBUG), 1)
     COPTS += -g
   endif
   ifeq ($(TRACE), 1)
        DEFINES += -DDEBUG_CCD -DERR_TRC_STK_CCD
   endif
   ifeq ($(MEMSUPER), 1)
        DEFINES += -DMEMORY_SUPERVISION
   endif

   INCLUDES = -I. -I../inc -I../inc/$(OSENV)
   DEFINES += -DSHARED_VSI -DSHARED_CCD\
              -DCCDDATA_STATIC_DATA -DDYNAMIC_ARRAYS\
              -DNEW_ENTITY \
              -DERR_TRACE_CCD
   ifeq ($(BYTESEX), motorola)
     DEFINES += -DM_MOTOROLA 
   else
     DEFINES += -DM_INTEL 
   endif
   ifeq ($(TARGET), rtlpc)
     COPTSNF = $(CFLAGS)
     MKLIB= i386-linux-ar ruv $@
     RANLIB= i386-linux-ranlib
     DEFINES += -D_RT_LINUX_ -DCCD_SYMBOLS 
   else
     ifeq ($(TARGET), linuxarm)
       CC=arm-linux-gcc
       MKLIB= arm-linux-ar ruv $@
       RANLIB= arm-linux-ranlib
       DEFINES += -DGSP_TARGET -DCCD_SYMBOLS 
     else
       ifeq ($(TARGET), solsparc)
         CC=gcc
         MKLIB= ar ruv $@
         RANLIB= ranlib
         DEFINES += -DCCD_SYMBOLS 
       else
         CC=gcc
         MKLIB= ar ruv $@
         RANLIB= ranlib
         DEFINES += -DCCD_SYMBOLS 
       endif
     endif
   endif
   COPTSNF += $(COPTS) $(DEFINES) $(INCLUDES)

   LIBEXT = a
   OBJTAIL = o
   CCD_LIB = $(LIBDIR)/libccd$(XXX).$(LIBEXT)
endif

#########################   Rules and targets   ##########################
.PHONY:	all clean

all: $(OBJDIR) $(CCD_LIB)

CCD_OBJS=$(patsubst %.$(OBJTAIL),$(OBJDIR)/%.$(OBJTAIL),$(_CCD_OBJS))
LNTFILES=$(patsubst %.$(OBJTAIL),%.lto,$(_CCD_OBJS))

objdir $(OBJDIR):
	@echo Creating object directory "$(OBJDIR)"
	$(MKDIR) -p $(OBJDIR)

$(_CCD_OBJS) : %.$(OBJTAIL) : $(CCD_SRC)/%.c
	$(CC) $(COPTSNF) $<

$(CCD_LIB) : $(_CCD_OBJS)  makefile $(CCD_DEF)
	$(MKLIB) $(CCD_OBJS)
ifeq ($(OSENV), sus)
	$(RANLIB) $@
endif

colibs:
	$(CHECKOUT) $(CO_LIBS)

cilibs:
	$(CHECKIN) $(CO_LIBS)

help:
	$(ECHO) usage...
	$(ECHO) building:
	$(ECHO) "gnumake TARGET={nuc|win32|vxworks|psos}\
	         [PLATFORM={arm7|arm|pc}] [CPU=PPC603] [DEBUG={1|0}]\
	         [TRACE={1|0}] [THUMB_MODE=1] [MEMSUPER={0|1} ..."
	$(ECHO) cleaning:
	$(ECHO) "gnumake clean TARGET={nuc|win32|vxworks|psos}\
	         [PLATFORM={arm7|arm|pc}] [CPU=PPC603] [DEBUG={1|0}]\
	         [TRACE={1|0}] [THUMB_MODE=1] [MEMSUPER={0|1} ..."

SRCFILES=$(patsubst %.$(OBJTAIL),%.c,$(_CCD_OBJS))
CCD_DEP=dep_ccd$(XXX).mk

clean:
	$(RM) $(CCD_OBJS) $(CCD_LIB)

depend: $(SRCFILES)
	@touch $(CCD_DEP)
	makedepend -b -f$(CCD_DEP) $^ -Y -o.$(OBJTAIL) $(INCLUDELIST)\
	    $(DEFINELIST)
	$(RM) $(CCD_DEP).bak

include ccd_lnt.mk
-include $(CCD_DEP)

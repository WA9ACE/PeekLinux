#----------------------------------------------------------------------------- 
#  Project :  
#  Modul   :  ccddata.mk
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
#  Purpose : Building Ccddata Dll/Lib. 
#----------------------------------------------------------------------------- 

ifndef TARGET
  TARGET=win32
endif

ifeq ($(TARGET), nuc)
  override TARGET=arm7
endif
PLATFORM=$(TARGET)

COFILES=

include ccd_base.mk

ifeq ($(PROST), g23net)
  CDGINCDIR=$(TESTROOT)/Cdginc
  PROSTCFLAGS= #??? -DOPTION_MULTITHREADED -DSHARED_CCD
endif

ifeq ($(PROST), g23m)
  CDGINCDIR=$(TESTROOT)/ms/CDGINC
endif

ifeq ($(PROST), gsm)
  CDGINCDIR=$(TESTROOT)/ms/CDGINC
endif

ifeq ($(PROST), gprs)
  ifeq ($(TARGET), arm7)
    CDGINCDIR=$(TESTROOT)/cdginc/cdginc_vo
  else
    CDGINCDIR=$(TESTROOT)/ms/CDGINC
  endif
endif

ifeq ($(PROST), generic)
  ifndef CDGINCDIR
    CDGINCDIR=$(GPF)/util/teststack/inc/cdginc/
  endif
endif

ifndef CCDDATADIR
  CCDDATADIR=$(GPFPRJDIR)
endif
override CCDDATADIR:=$(subst \,/,$(CCDDATADIR))

CCDDATA_OBJECTS=ccddata_ccd.obj ccddata_mconst.obj ccddata_ccdmtab.obj\
                ccddata_cdemval.obj ccddata_ccdent.obj 
CCDDATA_OBJFILES=$(patsubst %.obj,$(OBJDIR)/%.obj,$(CCDDATA_OBJECTS))

CCDDATA_EXE_OBJECTS=cdt.obj
CCDDATA_EXE_OBJFILES=$(patsubst %.obj,$(OBJDIR)/%.obj,$(CCDDATA_EXE_OBJECTS))

LNTFILES=$(patsubst %.obj,%.lto,$(CCDDATA_OBJECTS))

# win32
ifeq ($(TARGET),win32)

include ccddata_version.h

ifeq ($(LOAD),1)
  PART=load
  DLL_PREFIX=
  CCDDATA_OBJECTS = ccddata_load.obj
  COFILES+=$(CCDDATA_LIB)
  LDFLAGS_DLL+=/IMPLIB:$(CCDDATA_IMPLIB)
  DEFINES+=-DCCDDATA_LOAD
else
  PART=dll
  DLL_PREFIX=cddl_
  LDFLAGS_DLL+=/COMMENT:CCDDATA_$(CCDDATA_VERSION)
  CCDDATA_OBJECTS += ccddata_pconst.obj ccddata_ccdptab.obj \
                   ccddata_cdemstr.obj ccddata_cdepval.obj \
                   ccddata_tap_priv.obj ccddata_alias.obj \
                   ccddata_version.obj ccddata_eg.obj ccddata_pdi.obj
endif

  include ccddata_exp.mk

  CCDDATA_LIB=$(GPFBINDIR)/ccddata_$(PART).dll
  CCDDATA_IMPLIB=$(subst win32,WIN32,$(GPFLIBDIR)/ccddata_$(PART).lib)
  CCDDATA_IMPLIB_LOCAL=$(OBJDIR)/ccddata_$(PART).lib
  COFILES+=$(CCDDATA_IMPLIB)

  CCDEDIT_LIB=$(GPFLIBDIR)/ccdedit.lib
  CCD_LIB=$(GPFLIBDIR)/ccd.lib
  PCON_LIB=$(GPFLIBDIR)/pcon.lib
  FRAME_LIB=$(GPFLIBDIR)/frame.lib

  CCDDATA_EXE=$(GPFBINDIR)/cdt.exe

  INCLUDES+=-I"$(CDGINCDIR)"
  DEFINES+=-DCCDDATA_DABSTR
  LDFLAGS_DLL+=/nologo /dll $(PROSTLDFLAGS) $(CCDEDIT_LIB)
  ifneq ($(LOAD),1)
    LDFLAGS_DLL+=/IMPLIB:$(CCDDATA_IMPLIB_LOCAL)
  else
    LINK_EXPORTS+=/EXPORT:ccddata_init /EXPORT:ccddata_exit\
                  /EXPORT:ccddata_dllname /SECTION:.shared,RWS
  endif
  LDFLAGS_DLL+=/OUT:"$@" $(LINK_EXPORTS) 
  LDFLAGS_DLL+= $(PCON_LIB) $(CCD_LIB) $(FRAME_LIB) 

  LDFLAGS_EXE=/nologo /OUT:"$@" $(PROSTLDFLAGS)

  ifeq ($(DEBUG), 1)
    LDFLAGS_EXE += /DEBUG /incremental:no /pdb:none
  endif

  ccddata: all

  $(CCDDATA_LIB): $(CCDDATA_OBJECTS) 
	$(LINK) $(LDFLAGS_DLL) $(CCDDATA_OBJFILES)

  cdt: $(CCDDATA_EXE) 

  $(CCDDATA_EXE): $(CCDDATA_EXE_OBJECTS) 
	-cleartool co -nda -nc $@
	$(LINK) $(LDFLAGS_EXE) $(CCDDATA_EXE_OBJFILES) $(CCDDATA_IMPLIB)

else
  CCDDATA_OBJECTS += ccd_config.obj
endif # target == win32

# nucwin
ifeq ($(TARGET),nucwin)
  CCDDATA_LIB=$(CCDDATADIR)/ccddata.lib
  INCLUDES+=-I"$(CDGINCDIR)"
  LDFLAGS+=/OUT:"$(CCDDATA_LIB)"
  # currently for all nucwin: put the p* info for PCON in the lib
  CCDDATA_OBJECTS += ccddata_pconst.obj ccddata_ccdptab.obj 
  # and for the concept of synchronous frame with the tap inside
  # of the protocol stack include also most of the rest of ccddata; it
  # should not matter even without the tap
  CCDDATA_OBJECTS += ccddata_cdemstr.obj ccddata_cdepval.obj \
                   ccddata_tap_priv.obj ccddata_alias.obj

  $(CCDDATA_LIB): $(CCDDATA_OBJECTS) 
	$(MKLIB) $(LDFLAGS) $(CCDDATA_OBJFILES)

endif # target == nucwin

# nuc
ifeq ($(TARGET),arm7)
  CCDDATA_OBJECTS += ccddata_pconst.obj ccddata_ccdptab.obj
  ifeq ($(CCD_SYMBOLS), 1)
    CCDDATA_OBJECTS += ccddata_alias.obj 
  endif
  CCDDATA_LIB=$(CCDDATADIR)/ccddata.lib
  override CCDDATA_LIB:=$(subst \,/,$(CCDDATA_LIB))
  INCLUDES+=-I"$(CDGINCDIR)"
  LDFLAGS+=-rq $@  
  $(CCDDATA_LIB): $(CCDDATA_OBJECTS) 
	$(MKLIB) $(LDFLAGS) $(CCDDATA_OBJFILES)
endif # target == arm7

# arm9
ifeq ($(TARGET),arm9)
    CCDDATA_OBJECTS += ccddata_pconst.obj ccddata_ccdptab.obj
    ifeq ($(CCD_SYMBOLS), 1)
      CCDDATA_OBJECTS += ccddata_alias.obj 
    endif
    CCDDATA_LIB=$(CCDDATADIR)/ccddata.lib
    INCLUDES+=-I"$(CDGINCDIR)"
    LDFLAGS+=-rq $@
    $(CCDDATA_LIB): $(CCDDATA_OBJECTS) 
	$(MKLIB) $(LDFLAGS) $(CCDDATA_OBJFILES)
endif # target = arm9

# arm_b16
ifeq ($(TARGET),arm_b16)
  CCDDATA_LIB=$(CCDDATADIR)/ccddata.lib
  INCLUDES+=-I"$(CDGINCDIR)"
  LDFLAGS+=-rv $@
  $(CCDDATA_LIB): $(CCDDATA_OBJECTS) 
	$(MKLIB) $(LDFLAGS) $(CCDDATA_OBJFILES)
endif # target == arm_b16

# arm_b32
ifeq ($(TARGET),arm_b32)
  CCDDATA_LIB=$(CCDDATADIR)/ccddata.lib
  INCLUDES+=-I"$(CDGINCDIR)"
  LDFLAGS+=-rv $@
  $(CCDDATA_LIB): $(CCDDATA_OBJECTS) 
	$(MKLIB) $(LDFLAGS) $(CCDDATA_OBJFILES)
endif # target == arm_b32

# psos
ifeq ($(TARGET),psos)
  CCDDATA_LIB=$(CCDDATADIR)/ccddata.lib
  INCLUDES+=-I"$(CDGINCDIR)"
  LDFLAGS+=$@
  $(CCDDATA_LIB): $(COPTS_FILE) $(CCDDATA_OBJECTS) 
	$(MKLIB) $(LDFLAGS) $(CCDDATA_OBJFILES)
	$(RM) $(COPTS_FILE)

$(COPTS_FILE):
	@echo $(_CFLAGS) >  c.opt
	@echo $(INCLUDES) >> c.opt
	@echo $(DEFINES) >> c.opt

endif # target == psos

# sus environments
ifeq ($(OSENV), sus)
  CCDDATA_LIB=$(CCDDATADIR)/libccddata.a
  INCLUDES+=-I$(CDGINCDIR)
  # currently for all emulations: put the p* info for PCON in the lib
  CCDDATA_OBJECTS += ccddata_pconst.obj ccddata_ccdptab.obj 
  # and for the concept of synchronous frame with the tap inside
  # of the protocol stack include also most of the rest of ccddata; it
  # should not matter even without the tap
  CCDDATA_OBJECTS += ccddata_cdemstr.obj ccddata_cdepval.obj \
                   ccddata_tap_priv.obj ccddata_alias.obj

  $(CCDDATA_LIB): $(CCDDATA_OBJECTS) 
	$(MKLIB) $(CCDDATA_LIB) $(CCDDATA_OBJFILES)
	$(RANLIB) $(CCDDATA_LIB)

endif # sus

all:  $(CCDDATA_LIB)

exe: $(CCDDATA_EXE)

$(CCDDATA_OBJECTS) $(CCDDATA_EXE_OBJECTS):\
	            %.obj:	%.c $(OBJDIR)
	$(CC) $(CFLAGS) "$<"

CCDDATA_LIBDIR=$(subst \,/,$(dir $(CCDDATA_LIB))).

$(CCDDATA_LIB): $(CCDDATA_LIBDIR) $(CCDEDIT_LIB)

$(OBJDIR) $(CCDDATA_LIBDIR):
	@$(MKDIR) -p $@

clean:
	$(RM) $(OBJDIR)/ccddata* $(OBJDIR)/ccd_config* $(OBJDIR)/cdt.obj \
	  $(CCDDATA_LIB)

help:
	@echo Usage: gnumake -f ccddata.mk [ DEBUG=1 ] TARGET=target
	@echo with target: arm7, arm9, arm_b16, arm_b32, nucwin, win32, or psos
	@echo further options:
	@echo CCDENT=0 when the file ccdent.cdg does not exist in the\
	               cdginc directory
	@echo CCDALIAS=0 when the files malias.cdg and palias.cdg do not exist\
	                 in the cdginc directory
	@echo CDEMSTR=0 when the file cdemstr.cdg.cdg does not exist\
	                 in the cdginc directory
	@echo CDGINCDIR=path with the path to the cdginc directory
	@echo CCDDATA_LIB=path with the path to the ccddata lib to be built

ci:
	-cleartool ci -nc $(COFILES)

co:
	-cleartool co -nda -nc $(COFILES)

-include ccd_lnt.mk
include ccddata_dep.mk

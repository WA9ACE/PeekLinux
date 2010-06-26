#----------------------------------------------------------------------------- 
#  Project :  
#  Modul   :  gpf_makefile_template.mk
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
#|  Purpose :  Template makefile
#----------------------------------------------------------------------------- 


#*==== PARAMETERS ==================================================*#

# for command line parameters call "gnumake help"

TARGET=win32
GUI=0
EXPORT=1
FLOAT=0

#*==== PARAMETERS-ANALYSIS =========================================*#
ifeq ($(DEBUG), 1)
DEBUGDIR	=/debug
endif

ifeq ($(TARGET), win32)
OBJ			=obj
else
OBJ			=o
endif

ifeq ($(EXPORT), 1)
TARGET_BINDIR	= $(GPF_BINDIR)
else
TARGET_BINDIR	= $(BINDIR)
endif

ifeq ($(FLOAT), 1)
REPLACE		= -replace
endif

#*==== DIRECTORIES =================================================*#
GPFDIR		=../..
OBJDIR		=obj$(DEBUGDIR)
BINDIR		=bin$(DEBUGDIR)
GPF_BINDIR	=$(GPFDIR)/BIN$(DEBUGDIR)
GPF_ASSISTDIR	=$(GPFDIR)/assist
LIBDIR		=$(GPFDIR)/lib
DOCDIR		=doc
SRCDIR		=src
INCDIR		=inc
RESDIR		=res
MSDEV_PROJDIR 	=MSDev

DEPEND_DIRS	=$(GPFDIR) $(GPF_BINDIR) $(GPF_ASSISTDIR)

#*==== PROGRAMS ====================================================*#
CC			=cl.exe
LINK			=link.exe
RSC			=rc.exe
RM			=-rm -f
CP			=cp
CLEARTOOL		=cleartool
ECHO			=@echo
REM_READONLY   =-chmod +w
MAKEDEPEND	=makedepend
FIND			=gfind

#*==== FILES ========================================================*#
CPP			={c|cpp}

OBJS			=<source files>
SRCFILES		=$(patsubst %,$(SRCDIR)/%.$(CPP),$(OBJS))
OBJFILES		=$(patsubst %,$(OBJDIR)/%.$(OBJ),$(OBJS))

ifeq ($(TARGET), win32)
ifeq ($(GUI), 1)
RCS			=<windows rc-files>
RCFILES		=$(patsubst %,$(SRCDIR)/%.rc,$(RCS))
RESFILES		=$(patsubst %,$(OBJDIR)/%.res,$(RCS))
endif
endif

BATS			=<batch files>
BATFILES		=$(patsubst %,$(SRCDIR)/%.bat,$(BATS))

INIS			=<ini-files>
INIFILES		=$(patsubst %,$(SRCDIR)/%.ini,$(INIS))

DOCUS 		=<doc-files> 
DOCFILES		=$(patsubst %,$(DOCDIR)/%.doc,$(DOCUS))

INCS			=<include files>
INCFILES		=$(patsubst %,$(INCDIR)/%.h,$(GEN_INCS)) \

RESS			=<resources like xxx.ico>
RESOURCES		=$(patsubst %,$(RESDIR)/%,$(RESS))

MSDEVFILES	=$(MSDEV_PROJDIR)/<project>.dsw \
	$(MSDEV_PROJDIR)/<proj-exe>/<proj-exe>.dsp \
	$(MSDEV_PROJDIR)/<proj-exe>/<proj-exe>.dsw

MAKEFILE		=makefile

CHECKOUTS		=$(shell $(CLEARTOOL) lsco -cview -recurse -short)
COS			=$(patsubst .\\%,%,$(CHECKOUTS))
CO_FILES		=$(subst \,/,$(COS)) 

#*==== TARGETS =====================================================*#
# Executables
ifeq ($(GUI), 1)
EXE			=<gui-exe>
else
EXE			=<nongui-exe>
endif

# Batch files 
BATTARGETS	=$(patsubst %,$(TARGET_BINDIR)/%.bat,$(BATS))

# INI files 
INITARGETS	=$(patsubst %,$(TARGET_BINDIR)/%.ini,$(INIS))


# exports
BATEXPORTS	=$(patsubst %,$(GPF_BINDIR)/%.bat,$(BATS))
INIEXPORTS	=$(patsubst %,$(GPF_BINDIR)/%.ini,$(INIS))

EXPORTS		=$(BATEXPORTS) $(INIEXPORTS) \
                $(GPF_BINDIR)/$(EXE)

# assist files
ASSISTS		=$(GPF_ASSISTDIR)/files_xxx.bat

#*==== GENERAL DEFINES FOR COMPILER ETC. ===========================*#

ifndef CICMT
CICMT		=auto checkin
endif

GEN_INC		=-I "$(GPFDIR)/INC"
INCLUDES		=-I $(INCDIR) $(GEN_INC)
ifeq ($(TARGET), win32)
INCLUDES		+=-I $(GPFDIR)/INC/WIN32
endif

DEFINES		=
ifeq ($(TARGET), win32)
DEFINES		=-DWIN32 -D"_WINDOWS"
endif
ifeq ($(GUI), 1)
else
DEFINES		+=-D_CONSOLE -D_MBCS
endif
ifeq ($(DEBUG), 1)
DEFINES		+=-D"_DEBUG" 
else
DEFINES		+=-D"NDEBUG"
endif

CFLAGS		=/nologo /c /W3 /Zp1 /GX
ifeq ($(DEBUG),1)
CFLAGS		+=/MTd /Gm /Zi /Od
else
CFLAGS		+=/MT /O2
endif 
ifeq ($(TARGET), win32)
ifeq ($(GUI), 1)
CFLAGS		+= /YX"stdafx.h" 
endif #GUI
endif #TARGET
CFLAGS		+=$(DEFINES) $(INCLUDES)

LDFLAGS		=/nologo /incremental:no /pdb:none
ifeq ($(DEBUG), 1)
LDFLAGS		+=/debug 
else
LDFLAGS		+=/IGNORE:4089
#ignore warning: 
#The linker discarded all packaged functions that referenced 
#exports in <dynamic-link library>. As a result, dynamic-link 
#library and its import library are unneeded.
endif
ifeq ($(TARGET), win32)
ifeq ($(GUI), 1)
LDFLAGS		+=/subsystem:windows
else
LDFLAGS		+=/subsystem:console 
endif #GUI
endif #TARGET

GEN_LIBRARIES	=
ifeq ($(TARGET), win32)
WIN32_LIBS	=
GEN_LIBRARIES	+=$(WIN32_LIBS)
endif

LIBRARIES		=$(GEN_LIBRARIES) 

RSC_FLAGS		=/l 0x407 
ifeq ($(DEBUG), 1)
RSC_FLAGS		+=/d _DEBUG 
endif


#*==== Rules and targets =============================================*#
.PHONY:	all exe help bat ini clean checkin

all: exe bat ini
ifeq ($(EXPORT), 1)
ifeq ($(DEBUG), 1)
	$(ECHO) "To debug <proj> now goto '/GPF/BIN/debug' and call <proj-exe>."
else #DEBUG
	$(ECHO) "To run <proj> now just call <proj-exe>."
endif #DEBUG
else #EXPORT
ifeq ($(DEBUG), 1)
	$(ECHO) "To debug <proj> now goto 'bin/debug' and call <proj-exe>."
else #DEBUG
	$(ECHO) "To run <proj> now goto 'bin' and call <proj-exe>."
endif #DEBUG
endif #EXPORT

exe: $(TARGET_BINDIR)/$(EXE)

bat: $(BATTARGETS)

$(BINDIR)/%.bat: $(SRCDIR)/%.bat
	$(CP) $< $@ 
	$(REM_READONLY) $@

ini: $(INITARGETS)

$(BINDIR)/%.ini: $(SRCDIR)/%.ini
	$(CP) $< $@ 
	$(REM_READONLY) $@

$(BINDIR)/$(EXE): $(OBJFILES) $(RESFILES) 
	$(LINK) /OUT:$@ $(LDFLAGS) $^ $(LIBRARIES)

$(OBJDIR)/%.$(OBJ): $(SRCDIR)/%.$(CPP) $(INC_FILES)
	$(CC) $(CFLAGS) $< /Fo$@

$(OBJDIR)/%.res: $(SRCDIR)/%.rc
	$(RSC) $(RSC_FLAGS) /Fo$@ $<


$(EXPORTS): $(GPF_BINDIR)%: $(BINDIR)%
	-$(CLEARTOOL) co -nc $@ 2>> error.log
	$(CP) $< $@

checkin: 
	-$(CLEARTOOL) ci -identical -c "$(CICMT)" $(CO_FILES) \
                                               $(EXPORTS) $(ASSISTS) \
	                                          $(DEPEND_DIRS) 2>> error.log
label:
	-$(CLEARTOOL) mklabel -recurse $(REPLACE) $(LABEL) . 2>> error.log
	-$(CLEARTOOL) mklabel $(REPLACE) $(LABEL) $(EXPORTS) $(ASSISTS) \
	                      $(DEPEND_DIRS) 2>> error.log


help:
	$(ECHO) usage...
	$(ECHO) building:
	$(ECHO) "gnumake depend"
	$(ECHO) "gnumake [exe] [DEBUG={1|0}] [GUI={1|0}] [EXPORT={1|0}]"
	$(ECHO) cleaning:
	$(ECHO) "gnumake clean [DEBUG={1|0}]"
	$(ECHO) ClearCase:
	$(ECHO) "gnumake checkin [CICMT=<checkin comment>] [DEBUG={1|0}] [GUI={1|0}]"
	$(ECHO) "gnumake label LABEL=<label type> [FLOAT={1|0}] [DEBUG={1|0}] [GUI={1|0}]"
	$(ECHO) help:	
	$(ECHO) "gnumake help"

clean: 
	$(RM) $(OBJDIR)/*.*

depend: $(SRCFILES)
	$(MAKEDEPEND) $^ $(INCLUDES)

# DO NOT DELETE



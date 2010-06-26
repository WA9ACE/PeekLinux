#----------------------------------------------------------------------------- 
#  Project :  
#  Modul   :  ccdedit.mk
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
#  Purpose : Build ccdedit.lib.
#----------------------------------------------------------------------------- 
GPFINCDIR=$(GPF)/inc
GPFLIBDIR=$(GPF)/LIB

VPATH=$(OBJDIR)

CCDDIR=.
CCDEDIT_LIBNAME=ccdedit.lib
CCDEDIT_LOCAL_LIB=$(OBJDIR)/$(CCDEDIT_LIBNAME)
CCDEDIT_LIB=$(subst \,/,$(GPFLIBDIR)/$(TARGET)$(DEBUGDIR)/$(CCDEDIT_LIBNAME))
CC=cl.exe
INCLUDES=-I "$(CCDDIR)" -I "$(GPFINCDIR)"
DEFINES=$(PROSTCFLAGS) -DWIN32 -DCCD_SYMBOLS -DCCDDATA_LOAD -D_TOOLS_
CFLAGS=/nologo /c /W3 /Zp1 /Zl $(DEFINES) $(INCLUDES)
LINK=link.exe
MKLIB=lib.exe
LDFLAGS_LIB=/nologo /OUT:"$(CCDEDIT_LOCAL_LIB)"
OBJECTS = ccdedit.obj pdi.obj
OBJFILES=$(patsubst %.obj,$(OBJDIR)/%.obj,$(OBJECTS))
TARGET=WIN32
# Debug
ifeq ($(DEBUG), 1)
  DEBUGDIR=/debug
  CFLAGS += /Fr$(OBJDIR)/ /Z7 -D _MT -D _DLL -D _DEBUG
else
  DEBUGDIR=
  CFLAGS += -D _MT -D _DLL
endif
OBJDIR=obj/ccdedit$(DEBUGDIR)

all: $(CCDEDIT_LOCAL_LIB)

export: $(CCDEDIT_LIB)

colib:
	-cleartool co -nda -nc $(CCDEDIT_LIB)

cilib:
	-cleartool ci -nc $(CCDEDIT_LIB)

$(CCDEDIT_LIB): $(CCDEDIT_LOCAL_LIB)
	cp "$<" $@

$(CCDEDIT_LOCAL_LIB): $(OBJDIR) $(OBJECTS)
	$(MKLIB) $(LDFLAGS_LIB) $(OBJFILES)

$(OBJDIR):
	@gmkdir -p $(OBJDIR)

$(OBJECTS): %.obj:	%.c
	$(CC) $(CFLAGS) "$<" /Fo$(OBJDIR)/$@

clean:
	rm -f $(OBJDIR)/ccdedit.* $(OBJDIR)/pdi.*

ccdedit.obj: ccdedit.c $(GPFINCDIR)/typedefs.h $(CCDDIR)/ccdtable.h \
  $(CCDDIR)/ccddata.h $(GPFINCDIR)/ccdedit.h

pdi.obj: pdi.c $(GPFINCDIR)/typedefs.h $(CCDDIR)/ccdtable.h \
  $(CCDDIR)/ccddata.h $(GPFINCDIR)/pdi.h $(GPFINCDIR)/CCDAPI.H

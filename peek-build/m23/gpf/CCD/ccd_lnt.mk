#----------------------------------------------------------------------------- 
#  Project :  
#  Modul   :  ccd_lnt.mk
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
# Purpose : Some general definition for lint.
#----------------------------------------------------------------------------- 
LINT_EXE=lint-nt.exe
LINT_OPT+=-i/gpf/TEMPLATE/lint -u options.lnt -v -b
LINT_OPT+=-emacro(\*,va_start,va_arg,va_end)

ifeq ($(CC),cl)
  LINT_OPT+=-D_WIN32 -D_M_IX86 -D_MSC_VER=1100
endif
ifeq ($(CC),cl470)
  INCLUDE=$(C_DIR)
  ifeq ($(PLATFORM),arm9)
    LINT_OPT+=-D__TMS470__
  endif
endif

lint: $(LNTFILES)

$(LNTFILES): %.lto:	%.c
	-$(LINT_EXE) $(LINT_OPT) $(DEFINES) $(INCLUDES) "$<"

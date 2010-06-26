#----------------------------------------------------------------------------- 
#  Project :  
#  Modul   :  ccddata_dep.mk
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
#  Purpose : Dependencies for Ccddata.
#----------------------------------------------------------------------------- 
ccddata_pconst.obj: ccddata_pconst.c $(CDGINCDIR)/pconst.cdg
ccddata_mconst.obj: ccddata_mconst.c $(CDGINCDIR)/mconst.cdg
ccddata_ccdmtab.obj: ccddata_ccdmtab.c $(CDGINCDIR)/mconst.cdg \
  $(GPFINCDIR)/typedefs.h ccdtable.h
ccddata_ccdptab.obj: ccddata_ccdptab.c $(CDGINCDIR)/pconst.cdg \
  $(GPFINCDIR)/typedefs.h ccdtable.h
ccddata_cdemval.obj: ccddata_cdemval.c \
  $(GPFINCDIR)/typedefs.h ccdtable.h
ccddata_cdepval.obj: ccddata_cdepval.c \
  $(GPFINCDIR)/typedefs.h ccdtable.h
ccddata_tap_priv.obj: ccddata_tap_priv.c ccddata_tap_priv.h \
  $(GPFINCDIR)/typedefs.h $(CDGINCDIR)/mconst.cdg
ccd_config.obj: ccd_config.c ccd.h ccd_globs.h $(GPFINCDIR)/ccd_codingtypes.h \
  $(GPFINCDIR)/typedefs.h $(GPFINCDIR)/header.h $(CDGINCDIR)/mconst.cdg
ccddata_ccdent.obj: ccddata_ccdent.c ccd.h ccddata.h $(CDGINCDIR)/mconst.cdg \
  $(GPFINCDIR)/typedefs.h
ccddata_cdemstr.obj: ccddata_cdemstr.c $(CDGINCDIR)/mconst.cdg \
  $(GPFINCDIR)/typedefs.h ccdtable.h
ccddata_alias.obj: ccddata_alias.c $(CDGINCDIR)/malias.cdg \
  $(CDGINCDIR)/palias.cdg $(GPFINCDIR)/typedefs.h ccdtable.h
cdt.obj: cdt.c ccddata.h ccddata_version.h $(GPFINCDIR)/typedefs.h
ccddata_eg.obj: ccddata_eg.c
ccddata_load.obj: ccddata_load.c $(GPFINCDIR)/typedefs.h $(GPFINCDIR)/vsi.h\
  ccdtable.h $(GPFINCDIR)/Ccdedit.h $(GPFINCDIR)/pdi.h $(GPFINCDIR)/pcon.h\
  ccddata.h ccddata_tap_priv.h

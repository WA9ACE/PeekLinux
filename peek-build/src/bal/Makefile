
# Include the Common Makefile
# ---------------------------

include ../makefile_hdr

# Project Specific Modifications to global variables
# --------------------------------------------------

CC_SW_ZAT = 

# Global Include Paths for Compiler
# ---------------------------------

CC_INCS += -I$(BASE_PATH)/hwd -I$(BASE_PATH)/ai/isotel/aie -I$(BASE_PATH)/cust/esw/fsm/inc -I$(BASE_PATH)/cust/esw/val

# Additional Paths to Source files
VPATH += lsm
#*******************************
#* File Based Compile Switches *
#*******************************

$(BUILD_DIR)/%.o            : CC_SW_EXTRA = 
$(BUILD_DIR)/%.o            : CC_SW_ANSI = 


# Object Files Required for the Library Build
# -------------------------------------------
OBJS_LOCAL =    valkeypad.o         \
                valaccapi.o         \
                valbattapi.o        \
                vallightcore.o      \
                valfsiapi.o         \
                valdb.o             \
                valphbapi.o         \
                valphbflash.o       \
                valschedulerapi.o   \
                valrecentcallapi.o  \
                valrecentcallflash.o \
                valschedulerflash.o \
                vallsmapp.o	   \
              	vallsmtask.o       \
              	valsmartmessageflash.o \
              	valfmapi.o \
  
                  

OBJS_LOCAL  +=  valamps.o           \
                valhlwapi.o         \
                valhlwctrl.o        \
                valmisc.o           \
                valotasp.o          \
                valsmsctrl.o        \
                valsmsapi.o         \
                valsmsflash.o       \
                valsmsrecstorage.o  \
                valsmsdupdetect.o   \
                valsmsfmtconv.o     \
                valsmsprocmsg.o     \
                valsmssendmsg.o     \
                valtask.o           \
                valhal.o            \
                valstorage.o        \
                valvmemo.o          \
                valvrec.o           \
                valuserinfo.o       \
                valutils.o          \
                valdispfont.o       \
                valdisppaint.o      \
                valdisppaint_iram.o \
                valdispcontroller.o \
                valdns.o            \
                valmailbox.o        \
                valsndapi.o         \
                vallbs.o         \
                valuimapi.o         \
                valutkapi.o         \
                valpswapi.o	\
                ValSndTest.o
#                valdisptest.o  \
#//dazhong

ifeq "$(USE_ENHANCEDAT)" "TRUE"
    OBJS_LOCAL += ValATCmd.o
endif

ifeq "$(CUST_ERRORFAULT)" "TRUE"
    OBJS_LOCAL += btc_faultlog.o
endif

#for language package --void.wang
   OBJS_LOCAL += vallangpackage.o

# Name of Unit to Make
# --------------------

UNIT_NAME = val

#******************
#* Ending Include *
#******************

include ../makefile_end

################################################################################
#------------------------------------------------------------------------------
#  when        who    what, where, why
# 12/24/07  wby     Added for supproting Crash Dump.
################################################################################
#  $Log: Makefile $
#  Revision 1.2  2007/11/19 17:25:33  ywang
#  Add enhanced AT commands support.
#  Revision 1.1  2007/10/29 12:01:26  binye
#  Initial revision
#  Revision 1.1  2007/10/12 13:16:44  lwang
#  Initial revision
#  Revision 1.1  2007/09/24 14:01:04  binye
#  Initial revision
#  Revision 1.10  2006/12/25 11:12:15  yjin
#  for sms cust
#  Revision 1.9  2006/12/22 16:43:28  yjin
#  for cust,del phb uim
#  Revision 1.8  2006/12/22 15:08:52  xyliu
#  Move the file valdispfontdata.c to the cust directory.
#  Revision 1.7  2006/12/19 17:06:28  yjin
#  move to cust val
#  Revision 1.6  2006/12/19 14:57:51  dsu
#  move customer file to cust directory
#  Revision 1.5  2006/12/13 15:20:54  rli
#  add valstorage.o,valhal.o
#  Revision 1.4  2006/12/13 10:23:47  gzhu
#  Add valrecentcall
#  Revision 1.3  2006/12/04 20:31:40  xuhua
#  merge EswPlatform_move2sw_baseline
#  Revision 1.2  2006/11/27 21:23:28  shitong
#  remove CORE_OBJS
#  Revision 1.1  2006/11/26 22:14:02  yliu
#  Initial revision
#  Revision 1.13  2006/11/24 17:09:51  rli
#  rename valphbcustomer.o to valphbcust.o
#  Revision 1.12  2006/11/24 15:00:12  stan
#  added valbattapi.c to core lib.
#  Revision 1.11  2006/11/23 10:19:59  yjin
#  add fsi,db,phb to val core
#  Revision 1.10  2006/11/22 15:15:11  zzhang
#  Add valaccapi.o into the core lib.
#  Revision 1.9  2006/11/15 16:34:52  zzhang
#  Remove the battery management code.
#  Revision 1.8  2006/11/10 15:52:29  rli
#  add valphbcustomer.o,valphbflash.o,valphbuim.o
#  Revision 1.7  2006/11/09 19:42:48  hzhang
#  new VAL DISPLAY APIs
#  Revision 1.6  2006/11/06 16:47:45  dsu
#  add battery management module
#  Revision 1.4  2006/10/31 14:38:00  dsu
#  merge keypad and light code
#  Revision 1.3  2006/10/30 16:40:11  yjin
#  add valscheduler
#  Revision 1.2  2006/10/26 09:48:54  yjin
#  add valdb
#  Revision 1.1  2006/10/24 15:04:35  binye
#  Initial revision
#  Revision 1.2  2006/01/07 12:54:06  wavis
#  Merging in VAL.
#  Revision 1.1.1.2  2005/12/16 10:28:48  wavis
#  Renamed valfsitst.o to valfsimsg.o.
#  Revision 1.1.1.1  2005/11/08 13:14:34  wavis
#  Duplicate revision
#  Revision 1.1  2005/11/08 13:14:34  vnarayana
#  Initial revision
#  Revision 1.1  2005/11/07 15:32:57  wavis
#  Initial revision
#  Revision 1.1.1.2  2005/10/12 15:36:14  dorloff
#  new VAL\UI\FSM merge
#  Revision 1.3  2004/06/08 14:28:53  cmastro
#  added USB files
#  Revision 1.2  2004/02/18 14:35:04  vxnguyen
#  Removed ___ver.c from project.
#  Revision 1.1  2004/01/22 10:43:09  fpeng
#  Initial revision
#
#
#  Revision 1.6  2002/09/21 08:28:14  mshaver
#  Added iopgpsimagew.o, iopgpsw.o, and iopisrgpsw.o to the build
################################################################################

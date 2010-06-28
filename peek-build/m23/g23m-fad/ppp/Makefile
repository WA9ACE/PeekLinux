DEFINE += RNET_CFG_REAL_TRANSPORT
DEFINE += RVM_RNET_RT_SWE

UNDEFINE += RELEASE99
UNDEFINE += L1
UNDEFINE += PPP_INT_RAM
UNDEFINE += PPP_FLASH_ROM

VARIANT_INCLUDE=$(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core
INCLUDE += $(M23DIR)/g23m-fad/ppp
INCLUDE += $(M23DIR)/g23m-aci/bti


SRC  = ppp_arbf.c
SRC += ppp_arbp.c
SRC += ppp_arbs.c
SRC += ppp_frxf.c
SRC += ppp_frxp.c
SRC += ppp_frxs.c
SRC += ppp_ftxf.c
SRC += ppp_ftxs.c
SRC += ppp_lcpf.c
SRC += ppp_lcpp.c
SRC += ppp_lcps.c
SRC += ppp_onaf.c
SRC += ppp_onap.c
SRC += ppp_onas.c
SRC += ppp_ncpf.c
SRC += ppp_ncpp.c
SRC += ppp_ncps.c
SRC += ppp_papf.c
SRC += ppp_papp.c
SRC += ppp_paps.c
SRC += ppp_capf.c
SRC += ppp_capp.c
SRC += ppp_caps.c
SRC += ppp_pei.c
SRC += ppp_prxf.c
SRC += ppp_prxp.c
SRC += ppp_prxs.c
SRC += ppp_ptxf.c
SRC += ppp_ptxp.c
SRC += ppp_ptxs.c
SRC += ppp_rtf.c
SRC += ppp_rtp.c
SRC += ppp_rts.c
SRC += ppp_tbls.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/ppp.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET)
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules

DEFINE += 
UNDEFINE += RELEASE99

VARIANT_INCLUDE=$(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/g23m-gprs/sndcp
INCLUDE += $(M23DIR)/g23m-aci/dti
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core

SRC  = sndcp_f.c
SRC += sndcp_mgf.c
SRC += sndcp_mgp.c
SRC += sndcp_mgs.c
SRC += sndcp_ndf.c
SRC += sndcp_ndp.c
SRC += sndcp_nds.c
SRC += sndcp_nuf.c
SRC += sndcp_nup.c
SRC += sndcp_nus.c
SRC += sndcp_pdaf.c
SRC += sndcp_pdap.c
SRC += sndcp_pdas.c
SRC += sndcp_pdf.c
SRC += sndcp_pdp.c
SRC += sndcp_pds.c
SRC += sndcp_pei.c
SRC += sndcp_sdaf.c
SRC += sndcp_sdap.c
SRC += sndcp_sdas.c
SRC += sndcp_sdf.c
SRC += sndcp_sdp.c
SRC += sndcp_sds.c
SRC += sndcp_suaf.c
SRC += sndcp_suap.c
SRC += sndcp_suas.c
SRC += sndcp_suf.c
SRC += sndcp_sup.c
SRC += sndcp_ciap.c
SRC += sndcp_ciaf.c
SRC += sndcp_cias.c
SRC += sndcp_sus.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/sndcp.a

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

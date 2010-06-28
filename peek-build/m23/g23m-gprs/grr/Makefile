DEFINE += 
UNDEFINE += RELEASE99

VARIANT_INCLUDE=$(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/g23m-gprs/grr
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core

SRC  = grr_cpapf.c
SRC += grr_cpapp.c
SRC += grr_cpaps.c
SRC += grr_csf.c
SRC += grr_csp.c
SRC += grr_css.c
SRC += grr_ctrlf.c
SRC += grr_ctrlp.c
SRC += grr_ctrls.c
SRC += grr_em.c
SRC += grr_f.c
SRC += grr_gfpf.c
SRC += grr_gfpp.c
SRC += grr_meass.c
SRC += grr_measp.c
SRC += grr_measf.c
SRC += grr_pei.c
SRC += grr_pgf.c
SRC += grr_pgp.c
SRC += grr_pgs.c
SRC += grr_psif.c
SRC += grr_psip.c
SRC += grr_psis.c
SRC += grr_tcf.c
SRC += grr_tcp.c
SRC += grr_tcs.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/grr.a

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

DEFINE += 
UNDEFINE += RELEASE99

INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/g23m-gprs/llc
INCLUDE += $(M23DIR)/g23m-gprs/cci
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core

SRC  = llc_f.c
SRC += llc_irxf.c
SRC += llc_irxl.c
SRC += llc_irxp.c
SRC += llc_irxs.c
SRC += llc_itxf.c
SRC += llc_itxp.c
SRC += llc_itxs.c
SRC += llc_itxt.c
SRC += llc_llmef.c
SRC += llc_llmep.c
SRC += llc_llmes.c
SRC += llc_pei.c
SRC += llc_rxf.c
SRC += llc_rxp.c
SRC += llc_rxs.c
SRC += llc_t200f.c
SRC += llc_t200p.c
SRC += llc_t200s.c
SRC += llc_txf.c
SRC += llc_txl.c
SRC += llc_txp.c
SRC += llc_txs.c
SRC += llc_uf.c
SRC += llc_uirxf.c
SRC += llc_uirxp.c
SRC += llc_uirxs.c
SRC += llc_uitxf.c
SRC += llc_uitxp.c
SRC += llc_uitxs.c
SRC += llc_up.c
SRC += llc_us.c
SRC += llc_ul.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/llc.a

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

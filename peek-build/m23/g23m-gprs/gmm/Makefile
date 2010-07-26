DEFINE += 
UNDEFINE += RELEASE99

INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/g23m-gprs/gmm
INCLUDE += $(M23DIR)/g23m-aci/aci
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core

SRC  = gmm_f.c
SRC += gmm_pei.c
SRC += gmm_kernf.c
SRC += gmm_kernl.c
SRC += gmm_kernp.c
SRC += gmm_kerns.c
SRC += gmm_txf.c
SRC += gmm_txp.c
SRC += gmm_txs.c
SRC += gmm_rxf.c
SRC += gmm_rxp.c
SRC += gmm_rxs.c
SRC += gmm_rdyf.c
SRC += gmm_rdyp.c
SRC += gmm_rdys.c
SRC += gmm_syncf.c
SRC += gmm_syncp.c
SRC += gmm_syncs.c
SRC += gmm_em.c              

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/gmm.a

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

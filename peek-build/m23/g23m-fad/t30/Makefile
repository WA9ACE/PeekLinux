DEFINE += RNET_CFG_REAL_TRANSPORT
DEFINE += RVM_RNET_RT_SWE

UNDEFINE += RELEASE99
UNDEFINE += L1
UNDEFINE += MUX

VARIANT_INCLUDE=$(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core
INCLUDE += $(M23DIR)/g23m-fad/t30
INCLUDE += $(M23DIR)/g23m-fad/bti

SRC  = t30_bcsf.c
SRC += t30_version.c
SRC += t30_bcss.c
SRC += t30_kerf.c
SRC += t30_kerp.c
SRC += t30_kers.c
SRC += t30_msgs.c
SRC += t30_muxp.c
SRC += t30_muxs.c
SRC += t30_pei.c
SRC += t30_tim.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/t30.a

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

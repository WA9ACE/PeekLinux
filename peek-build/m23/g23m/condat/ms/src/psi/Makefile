DEFINE += MFW

UNDEFINE += L1
UNDEFINE += RELEASE99

VARIANT_INCLUDE=$(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core
INCLUDE += $(M23DIR)/g23m/condat/ms/src/psi


SRC  = psi_drxf.c
SRC += psi_drxs.c
SRC += psi_dtxf.c
SRC += psi_dtxs.c
SRC += psi_kerf.c
SRC += psi_kerp.c
SRC += psi_kers.c
SRC += psi_pei.c
SRC += psi_rxf.c
SRC += psi_rxp.c
SRC += psi_rxs.c
SRC += psi_txf.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/psi.a

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

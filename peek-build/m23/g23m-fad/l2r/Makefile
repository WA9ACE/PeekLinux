DEFINE += RNET_CFG_REAL_TRANSPORT
DEFINE += RVM_RNET_RT_SWE

UNDEFINE += RELEASE99
UNDEFINE += L1
UNDEFINE += DTI

VARIANT_INCLUDE=$(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core
INCLUDE += $(M23DIR)/g23m-fad/l2r


SRC  = l2r_dnf.c
SRC += l2r_dnp.c
SRC += l2r_dns.c
SRC += l2r_mgtf.c
SRC += l2r_mgtp.c
SRC += l2r_mgts.c
SRC += l2r_pei.c
SRC += l2r_ups.c
SRC += l2r_tim.c
SRC += l2r_upf.c
SRC += l2r_upp.c
SRC += tra_mgts.c
SRC += tra_mgtf.c
SRC += tra_mgtp.c
SRC += tra_dns.c
SRC += tra_dnf.c
SRC += tra_dnp.c
SRC += tra_ups.c
SRC += tra_upf.c
SRC += tra_upp.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/l2r.a

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

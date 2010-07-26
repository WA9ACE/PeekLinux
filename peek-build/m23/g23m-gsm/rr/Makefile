DEFINE += 
UNDEFINE += RELEASE99

INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/g23m-gsm/rr
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core

SRC  = rr_attf.c
SRC += rr_version.c
SRC += rr_attp.c
SRC += rr_atts.c
SRC += rr_datf.c
SRC += rr_datp.c
SRC += rr_dats.c
SRC += rr_forf.c
SRC += rr_forp.c
SRC += rr_fors.c
SRC += rr_csf.c
SRC += rr_pei.c
SRC += rr_srv.c
SRC += rr_tim.c
SRC += rr_cs.c
SRC += rr_gprs.c
SRC += rr_datg.c
SRC += rr_attg.c
SRC += rr_em.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/rr.a

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

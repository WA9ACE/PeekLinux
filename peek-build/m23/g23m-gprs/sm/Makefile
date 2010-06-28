DEFINE += ENTITY_SM

UNDEFINE += RELEASE99

VARIANT_INCLUDE=$(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/g23m-gprs/sm
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core

SRC  = sm.c
SRC += sm_aci_output_handler.c
SRC += sm_context_control.c
SRC += sm_context_deactivate_control.c
SRC += sm_debug.c
SRC += sm_dispatch_message.c
SRC += sm_dispatcher.c
SRC += sm_memory_handler.c
SRC += sm_mm_output_handler.c
SRC += sm_network_control.c
SRC += sm_pei.c
SRC += sm_qos.c
SRC += sm_sequencer.c
SRC += sm_tft.c
SRC += sm_timer_handler.c
SRC += sm_upm_output_handler.c
SRC += sm_user_plane_control.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/sm.a

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

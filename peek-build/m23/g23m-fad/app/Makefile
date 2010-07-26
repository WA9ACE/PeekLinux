DEFINE += RNET_CFG_REAL_TRANSPORT
DEFINE += RVM_RNET_RT_SWE

UNDEFINE += RELEASE99
UNDEFINE += L1

DEFINE += EMO_DBG
DEFINE +=TRACE_FUNCTION=emo_printf
DEFINE +=TRACE_ERROR=emo_printf
DEFINE +=TRACE_EVENT=emo_printf
DEFINE +=TRACE_EVENT_P1=emo_printf
DEFINE += TRACE_EVENT_P2=emo_printf
DEFINE += TRACE_EVENT_P3=emo_printf
DEFINE += TRACE_EVENT_P4=emo_printf
DEFINE += TRACE_EVENT_P5=emo_printf
DEFINE += TRACE_EVENT_P6=emo_printf
DEFINE += TRACE_EVENT_P7=emo_printf
DEFINE += TRACE_FUNCTION_P1=emo_printf
DEFINE += TRACE_FUNCTION_P2=emo_printf


INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/csw-system/drv_app
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/csw-system/drv_core
INCLUDE += $(M23DIR)/g23m-aci/bat
INCLUDE += $(M23DIR)/g23m-fad/tcpip
INCLUDE += $(M23DIR)/g23m-fad/tcpip/include
INCLUDE += $(M23DIR)/g23m-fad/app
INCLUDE += $(M23DIR)/g23m-aci/bat
INCLUDE += $(M23DIR)/g23m-aci/gdd_dio
INCLUDE += $(M23DIR)/g23m-aci/l2p


SRC  = app_cmds.c
SRC += app_core.c
SRC += app_pei.c
SRC += app_bat_atcmds.c
SRC += app_util.c
SRC += app_gdd.c
SRC += app_bat.c
SRC += bat_cfg.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/app.a

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

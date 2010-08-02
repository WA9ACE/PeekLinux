DEFINE += 
UNDEFINE += FF_MIDI_LOAD_FROM_MEM
UNDEFINE += FF_MIDI_LOAD_FROM_LFS

DEFINE +=
DEFINE += EMO_DBG
DEFINE +=TRACE_FUNCTION=emo_printf
DEFINE +=TRACE_ERROR=emo_printf
DEFINE +=TRACE_EVENT=emo_printf
DEFINE +=TRACE_EVENT_P1=emo_printf
DEFINE += TRACE_EVENT_P2=emo_printf
DEFINE += TRACE_EVENT_P3=emo_printf
DEFINE += TRACE_EVENT_P4=emo_printf
DEFINE += TRACE_EVENT_P5=emo_printf
DEFINE += TRACE_EVENT_P7=emo_printf
DEFINE += TRACE_FUNCTION_P1=emo_printf
DEFINE += TRACE_FUNCTION_P2=emo_printf

INCLUDE += $(VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/csw-system/drv_app/lcd
INCLUDE += $(M23DIR)/csw-system/drv_app/kpd
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_core/inc
INCLUDE += $(M23DIR)/chipsetsw/riviera/rvf
INCLUDE += $(M23DIR)/chipsetsw/riviera/rvm

SRC  = audio.c
SRC += display.c
SRC += font_bitmaps.c
SRC += power.c
SRC += keypad.c
SRC += rtc.c
SRC += rx.c
SRC += temp_stub.c

OBJO += light.o

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/gdi.a

all: $(TARGET)

$(TARGET): $(OBJ) $(OBJO)
	$(AR) $(ARFLAGS) $@ $?

clean:
	$(RM) $(OBJ) 
	$(RM) $(TARGET) 
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules

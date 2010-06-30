DEFINE += 
UNDEFINE += FF_MIDI_LOAD_FROM_MEM
UNDEFINE += FF_MIDI_LOAD_FROM_LFS

VARIANT_INCLUDE = 
INCLUDE += $(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/csw-system/drv_app/lcd/
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_core/inc

SRC  = audio.c
SRC += display.c
SRC += font_bitmaps.c
SRC += power.c
SRC += rtc.c
SRC += rx.c

OBJO = keypad.o
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

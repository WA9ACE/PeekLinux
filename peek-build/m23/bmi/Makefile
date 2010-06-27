DEFINE += 
UNDEFINE += FF_MIDI_LOAD_FROM_MEM
UNDEFINE += FF_MIDI_LOAD_FROM_LFS

VARIANT_INCLUDE = 
INCLUDE += $(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_core/inc

SRC  = gdi/audio.c
SRC += gdi/display.c
SRC += gdi/font_bitmaps.c
SRC += gdi/keypad.c
SRC += gdi/light.c
SRC += gdi/power.c
SRC += gdi/rtc.c
SRC += gdi/rx.c	 

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/bmi.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $?

clean:
	$(RM) $(OBJ) 
	$(RM) $(TARGET) 
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules

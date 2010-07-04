DEFINE += RV_CJK
DEFINE += TI_PS_16BIT_CPY

UNDEFINE += REMOVE_SERIAL_SWITCH_FOR_N12N5x
UNDEFINE += LITE_14_FM_CARKIT_REMOVAL
UNDEFINE += LCD_STATE

VARIANT_INCLUDE = 
INCLUDE += $(M23_VARIANT_INCLUDE)

SRC  = lcd_manager.c
SRC += lcd_transport.c
SRC += lcd_interface.c
SRC += lcd_pei.c
SRC += lcd_pwr.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/lcd_drv_flash.a

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

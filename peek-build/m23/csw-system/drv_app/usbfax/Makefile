DEFINE += 
UNDEFINE += 

VARIANT_INCLUDE = 
INCLUDE += $(M23_VARIANT_INCLUDE)

SRC  = usbfax_pei.c
SRC += usbfax_cfg.c
SRC += usbfax_task.c
SRC += usbfax_export.c
SRC += usbfax_handle_message.c
SRC += usbfax_handle_timer.c
SRC += usbfax_api.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/usbfax.a

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

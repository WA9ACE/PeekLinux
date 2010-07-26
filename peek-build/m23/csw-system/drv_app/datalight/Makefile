DEFINE += 
UNDEFINE += 

INCLUDE += $(VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/CS_ThirdParty/Datalight/FlashFX/os/rtos/include

SRC  = datalight_pei.c
SRC += datalight_api.c
SRC += datalight_handle_message.c
SRC += datalight_interface.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/datalight.a

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

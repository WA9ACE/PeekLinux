DEFINE += 
UNDEFINE += 

VARIANT_INCLUDE = 
INCLUDE += $(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/CS_ThirdParty/Datalight/FlashFX/os/rtos/include

SRC  = gbi_pei.c
SRC += gbi_api.c
SRC += gbi_handle_message.c
SRC += gbi_handle_timer.c
SRC += gbi_operations.c
SRC += gbi_pi_cfg.c
SRC += gbi_pi_datalight_env.c
SRC += gbi_pi_datalight_handle_message.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/gbi.a

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

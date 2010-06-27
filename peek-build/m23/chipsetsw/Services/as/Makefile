DEFINE += 

UNDEFINE += AS_OPTION_AAC

VARIANT_INCLUDE = 
INCLUDE += $(M23_VARIANT_INCLUDE)

SRC  = as_api.c
SRC += as_audio.c
SRC += as_buzzer.c
SRC += as_pei.c
SRC += as_handle_message.c
SRC += as_handle_timer.c
SRC += as_processing.c
SRC += as_utils.c
SRC += as_aac.c
SRC += as_E1_Melody.c
SRC += as_bt.c 
SRC += as_midi.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/as.a

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

DEFINE += NWARN
DEFINE += NCONFIG

UNDEFINE += 

VARIANT_INCLUDE = 
INCLUDE += $(M23_VARIANT_INCLUDE)

SRC  = lcc_api.c
SRC += lcc_pei.c
SRC += lcc_handle_message.c
SRC += lcc_modulate.c
SRC += lcc_task.c
SRC += lcc_trace.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/lcc.a

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

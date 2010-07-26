DEFINE += OP_L1_STANDALONE=0 
UNDEFINE += 

INCLUDE += $(VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/L1/layer1/aac_include
INCLUDE += $(M23DIR)/csw-system
INCLUDE += $(M23DIR)/L1/layer1/gtt_include
INCLUDE += $(M23DIR)/L1/layer1/dyn_dwl_include
INCLUDE += $(M23DIR)/L1/layer1/mp3_include

SRC  = main.c
SRC += dynamic_clock.c
SRC += init.c
SRC += ../init_app/create_RVtasks.c
SRC += ../init_app/sys_dummy.c
SRC += mem_load.c
SRC += int_loc_1_15.s

CCSTRICT=
CCTHUMB=

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/init.a

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

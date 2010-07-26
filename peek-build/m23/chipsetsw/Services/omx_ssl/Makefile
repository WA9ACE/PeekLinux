DEFINE += 
UNDEFINE += CONFIG_MODULE
UNDEFINE += DATA_EXT_RAM
UNDEFINE += DATA_INT_RAM

INCLUDE += $(M23DIR)/chipsetsw/Services/omx_core/inc
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_ssl/inc
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_ssl/src

INCLUDE += $(VARIANT_INCLUDE)

SRC  = src/omx_sslcomponent.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/omx_ssl.a

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

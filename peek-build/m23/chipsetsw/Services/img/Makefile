DEFINE += 
UNDEFINE += CONFIG_MODULE
UNDEFINE += DATA_EXT_RAM

VARIANT_INCLUDE = 
INCLUDE += $(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_core/inc
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_ssl/inc
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_cam/inc
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_img/inc
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_img/src
INCLUDE += $(M23DIR)/chipsetsw/Services/img/src
INCLUDE += $(M23DIR)/chipsetsw/Services/img/inc
INCLUDE += $(M23DIR)/CS_ThirdParty/Emuzed/imaging/SourceCode/Export_hdr
INCLUDE += $(M23DIR)/CS_ThirdParty/Emuzed/imaging/SourceCode/Code/Inc/Encoder
INCLUDE += $(M23DIR)/CS_ThirdParty/Emuzed/imaging/SourceCode/Code/Inc/Decoder
INCLUDE += $(M23DIR)/g23m/condat/frame/config

SRC  = src/img_pei.c
SRC += src/img_core.c
SRC += src/img_buffer_queue.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/img.a

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

DEFINE += 
UNDEFINE += 

VARIANT_INCLUDE = 
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_core/inc
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_ssl/inc
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_ssl/src
INCLUDE += $(M23DIR)/chipsetsw/Services/ssl/inc
INCLUDE += $(M23DIR)/chipsetsw/Services/ssl/src
INCLUDE += $(M23DIR)/csw-system/drv_app/lcd
INCLUDE += $(M23DIR)/CS_ThirdParty/Emuzed/imaging/SourceCode/Code/Inc/Decoder
INCLUDE += $(M23DIR)/CS_ThirdParty/Emuzed/imaging/SourceCode/Export_hdr

INCLUDE += $(M23_VARIANT_INCLUDE)

SRC  = src/ssl_pei.c


OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/ssl.a

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

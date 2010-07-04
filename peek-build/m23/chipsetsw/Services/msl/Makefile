DEFINE += 

UNDEFINE += CONFIG_MODULE

VARIANT_INCLUDE = 
INCLUDE += $(M23_VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/chipsetsw/Services/msl/inc 
INCLUDE += $(M23DIR)/chipsetsw/Services/msl/src 
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_ssl/inc 
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_cam/inc 
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_img/inc 
INCLUDE += $(M23DIR)/chipsetsw/Services/omx_core/inc 

SRC  = src/msl_imgthmb.c
SRC += src/msl_imgview.c
SRC += src/msl_api.c

CCSTRICT=

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/msl.a

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

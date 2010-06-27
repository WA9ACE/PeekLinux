DEFINE += 
UNDEFINE += 

VARIANT_INCLUDE = 
INCLUDE += $(M23_VARIANT_INCLUDE)

SRC  = usb_api.c
SRC += usb_pei.c
SRC += usb_cfg_dat.c
SRC += usb_handle_message.c
SRC += usb_cfg.c
SRC += usb_test_cfg.c
SRC += board/usb_devstat.c
SRC += usb_pwr.c
SRC += board/usb_task.c
SRC += board/usb_inth.c
SRC += board/usb_dma.c
SRC += otg/bspUsblh_Otg_Conf.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/usb.a

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

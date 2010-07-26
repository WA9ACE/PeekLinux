DEFINE += 
UNDEFINE += 

INCLUDE += $(VARIANT_INCLUDE)

SRC  = kpd_api.c
SRC += kpd_functions.c
SRC += kpd_power_api.c
SRC += kpd_process_internal_msg.c
SRC += kpd_virtual_key_table_mgt.c
SRC += kpd_pei.c
SRC += kpd_handle_message.c
SRC += kpd_handle_timer.c
SRC += kpd_task.c
SRC += board/kpd_scan_functions.c


OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/kpd.a

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

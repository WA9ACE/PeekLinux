DEFINE += 
UNDEFINE += 

INCLUDE += $(VARIANT_INCLUDE)

SRC  = rfs_api.c
SRC += rfs_pei.c
SRC += rfs_handle_message.c
SRC += rfs_process_internal_msg.c
SRC += rfs_handle_timer.c
SRC += rfs_fm_requests.c
SRC += rfs_fm_responses.c
SRC += rfs_fm.c
SRC += rfs_media_manager.c


OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/rfs.a

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

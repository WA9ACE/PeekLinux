DEFINE += 
UNDEFINE += 

INCLUDE += $(VARIANT_INCLUDE)

SRC  = rfsfat_api.c
SRC += rfsfat_blk.c
SRC += rfsfat_clm.c
SRC += rfsfat_dir.c
SRC += rfsfat_pei.c
SRC += rfsfat_fam.c
SRC += rfsfat_ffs_disk.c
SRC += rfsfat_ffs_dman.c
SRC += rfsfat_ffs_file.c
SRC += rfsfat_ffs_fman.c
SRC += rfsfat_handle_message.c
SRC += rfsfat_handle_timer.c
SRC += rfsfat_mem_operations.c
SRC += rfsfat_mma.c
SRC += rfsfat_posconf.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/rfsfat.a

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

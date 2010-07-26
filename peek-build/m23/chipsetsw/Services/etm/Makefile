DEFINE += ETM=0
UNDEFINE += 

INCLUDE += $(VARIANT_INCLUDE)

SRC  = etm_task.c
SRC += etm_pei.c
SRC += etm_api.c
SRC += etm_trace.c
SRC += etm_audio.c
SRC += etm_tmpwr.c
#SRC += etm_at.c
SRC += etm_tmcore.c
SRC += etm_tmcore_misc.c
SRC += etm_tm3core.c


OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/etm.a

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

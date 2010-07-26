DEFINE +=
UNDEFINE += 

INCLUDE += $(VARIANT_INCLUDE)
INCLUDE += $(M23DIR)/gpf


CCOPTIMIZE0     =
CCALIGNSTRUCTS  =

SRC  = rvt_api.c
SRC += rvt_pei.c
SRC += rvt_task.c
SRC += rvt_trace_adapt.c
SRC += ti_profiler/ti_profiler.c
SRC += ti_profiler/ti_profiler_env.c


OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.s,%.o,$(SRC)))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/rvt.a

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

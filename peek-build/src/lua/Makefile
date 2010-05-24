VARIANT_DEFINE :=

DEFINE += alloca=p_malloc
DEFINE += malloc=p_malloc
DEFINE += calloc=p_calloc
DEFINE += realloc=p_realloc
DEFINE += free=p_free
DEFINE += printf=emo_printf

INCLUDE += $(SRCDIR)/include

SRC = lapi.c
SRC += lcode.c
SRC += ldebug.c
SRC += ldo.c
SRC += ldump.c
SRC += lfunc.c
SRC += lgc.c
SRC += llex.c
SRC += lmem.c
SRC += lobject.c
SRC += lopcodes.c
SRC += lparser.c
SRC += lstate.c
SRC += lstring.c
SRC += ltable.c
SRC += ltm.c
SRC += lundump.c
SRC += lvm.c
SRC += lzio.c
SRC += lauxlib.c
SRC += lbaselib.c
SRC += ldblib.c
SRC += liolib.c
SRC += lmathlib.c
SRC += loslib.c
SRC += ltablib.c
SRC += lstrlib.c
SRC += loadlib.c
SRC += linit.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/lua.a

all: $(TARGET)
OBJ  = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

$(TARGET): $(OBJ)
	$(subst /cygdrive/c,C:,$(AR) $(ARFLAGS) $@ $?)

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET)
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))

include $(BASEDIR)/Makefile.rules


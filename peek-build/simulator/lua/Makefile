DEFINE += printf=script_emo_printf
DEFINE += SIMULATOR

LUADIR = $(SRCDIR)/lua

INCLUDE = $(LUADIR)

SRC  = lapi.c
SRC += lauxlib.c
SRC += lbaselib.c
SRC += lcode.c
SRC += ldblib.c
SRC += ldebug.c
SRC += ldo.c
SRC += ldump.c
SRC += lfunc.c
SRC += lgc.c
SRC += linit.c
SRC += liolib.c
SRC += llex.c
SRC += lmathlib.c
SRC += lmem.c
SRC += loadlib.c
SRC += lobject.c
SRC += lopcodes.c
SRC += loslib.c
SRC += lparser.c
SRC += lstate.c
SRC += lstring.c
SRC += lstrlib.c
SRC += ltable.c
SRC += ltablib.c
SRC += ltm.c
SRC += lundump.c
SRC += lvm.c
SRC += lzio.c

OBJ = $(addprefix $(LUADIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/lua.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $?

clean:
	$(RM) $(OBJ) 
	$(RM) $(TARGET) 
	$(RM) $(DEPS)

include $(BASEDIR)/Makefile.rules

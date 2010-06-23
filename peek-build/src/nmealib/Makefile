VARIANT_DEFINE :=

DEFINE += alloca=p_malloc
DEFINE += malloc=p_malloc
DEFINE += calloc=p_calloc
DEFINE += realloc=p_realloc
DEFINE += free=p_free
DEFINE += printf=emo_printf

INCLUDE += $(SRCDIR)/include
INCLUDE += ./include/

SRC = src/context.c
SRC += src/generate.c
SRC += src/generator.c
SRC += src/gmath.c
SRC += src/info.c
SRC += src/parse.c
SRC += src/parser.c
SRC += src/sentence.c
SRC += src/time.c
SRC += src/tok.c

OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/nmealib.a

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


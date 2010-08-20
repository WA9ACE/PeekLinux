VARIANT_DEFINE :=

DEFINE += alloca=p_malloc
DEFINE += malloc=p_malloc
DEFINE += calloc=p_calloc
DEFINE += realloc=p_realloc
DEFINE += free=p_free
DEFINE += printf=emo_printf

DEFINE += ENABLE_RELOCATABLE=1
DEFINE += set_relocation_prefix=libiconv_set_relocation_prefix
#DEFINE += BUILDING_LIBICONV
DEFINE += IN_LIBRARY
DEFINE += NO_XMALLOC

INCLUDE += $(SRCDIR)/include
INCLUDE += $(SRCDIR)/libiconv/include

SRC = lib/iconv.c
SRC += lib/relocateable.c
SRC += libcharset/lib/localcharset.c


OBJ = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/iconv.a

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


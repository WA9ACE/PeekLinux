VARIANT_DEFINE :=

DEFINE += alloca=p_malloc
DEFINE += malloc=p_malloc
DEFINE += calloc=p_calloc
DEFINE += realloc=p_realloc
DEFINE += free=p_free
DEFINE += printf=emo_printf

INCLUDE += $(SRCDIR)/include
INCLUDE += $(SRCDIR)/netdepends/libparserutils/src
INCLUDE += $(SRCDIR)/netdepends/libparserutils/include/parserutils
INCLUDE += $(SRCDIR)/netdepends/libparserutils/include
INCLUDE += $(SRCDIR)/netsurf/emoport

SRC  = src/input/filter.c
SRC += src/input/inputstream.c
SRC += src/charset/codec.c
SRC += src/charset/encodings/utf8.c
SRC += src/charset/encodings/utf16.c
SRC += src/charset/charset.c
SRC += src/charset/codecs/codec_utf16.c
SRC += src/charset/codecs/codec_utf8.c
SRC += src/charset/codecs/codec_ascii.c
SRC += src/charset/codecs/codec_ext8.c
SRC += src/charset/codecs/codec_8859.c
SRC += src/charset/aliases.c
SRC += src/parserutils.c
SRC += src/utils/buffer.c
SRC += src/utils/stack.c
SRC += src/utils/errors.c
SRC += src/utils/vector.c

DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libparserutils.a

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


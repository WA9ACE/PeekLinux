VARIANT_DEFINE :=

DEFINE += alloca=p_malloc
DEFINE += malloc=p_malloc
DEFINE += calloc=p_calloc
DEFINE += realloc=p_realloc
DEFINE += free=p_free
DEFINE += printf=emo_printf

INCLUDE += $(SRCDIR)/include
INCLUDE += $(SRCDIR)/netdepends/libwapcaplet/include
INCLUDE += $(SRCDIR)/netdepends/libcss/src
INCLUDE += $(SRCDIR)/netdepends/libcss/include
INCLUDE += $(SRCDIR)/netdepends/libparserutils/include/parserutils
INCLUDE += $(SRCDIR)/netdepends/libparserutils/include
INCLUDE += $(SRCDIR)/netsurf/emoport


SRC = src/select/computed.c
SRC += src/select/select.c
SRC += src/select/hash.c
SRC += src/select/properties.c
SRC += src/select/dispatch.c
SRC += src/stylesheet.c
SRC += src/lex/lex.c
SRC += src/parse/important.c
SRC += src/parse/propstrings.c
SRC += src/parse/language.c
SRC += src/parse/parse.c
SRC += src/parse/properties/positioning.c
SRC += src/parse/properties/border_outline.c
SRC += src/parse/properties/generated_list.c
SRC += src/parse/properties/ui.c
SRC += src/parse/properties/font.c
SRC += src/parse/properties/text.c
SRC += src/parse/properties/background.c
SRC += src/parse/properties/page.c
SRC += src/parse/properties/aural.c
SRC += src/parse/properties/utils.c
SRC += src/parse/properties/margin.c
SRC += src/parse/properties/effects_stacking.c
SRC += src/parse/properties/padding.c
SRC += src/parse/properties/display_sizing.c
SRC += src/parse/properties/parseproperties.c
SRC += src/parse/properties/table.c
SRC += src/libcss.c
SRC += src/charset/detect.c
SRC += src/utils/utils.c
SRC += src/utils/errors.c

DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libcss.a

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


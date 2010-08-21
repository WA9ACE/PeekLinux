VARIANT_DEFINE :=

DEFINE =

INCLUDE  = $(SRCDIR)/netsurf/emoport
INCLUDE += $(SRCDIR)/netdepends/libparserutils/include
INCLUDE += $(SRCDIR)/netdepends/hubbub-0.0.2/include
INCLUDE += $(SRCDIR)/netdepends/hubbub-0.0.2/src

SRC += src/hubbub.c
SRC += src/tokeniser/entities.c
SRC += src/tokeniser/tokeniser.c
SRC += src/hubbubparser.c
SRC += src/treebuilder/initial.c
SRC += src/treebuilder/in_foreign_content.c
SRC += src/treebuilder/in_body.c
SRC += src/treebuilder/in_head_noscript.c
SRC += src/treebuilder/in_head.c
SRC += src/treebuilder/in_table_body.c
SRC += src/treebuilder/before_html.c
SRC += src/treebuilder/after_after_body.c
SRC += src/treebuilder/in_table.c
SRC += src/treebuilder/after_body.c
SRC += src/treebuilder/after_after_frameset.c
SRC += src/treebuilder/treebuilder.c
SRC += src/treebuilder/in_frameset.c
SRC += src/treebuilder/after_frameset.c
SRC += src/treebuilder/before_head.c
SRC += src/treebuilder/after_head.c
SRC += src/treebuilder/in_cell.c
SRC += src/treebuilder/in_row.c
SRC += src/treebuilder/generic_rcdata.c
SRC += src/treebuilder/in_caption.c
SRC += src/treebuilder/in_select.c
SRC += src/treebuilder/in_select_in_table.c
SRC += src/treebuilder/in_column_group.c
SRC += src/charset/hubbubdetect.c
SRC += src/utils/hubbubstring.c
SRC += src/utils/hubbubdict.c
SRC += src/utils/hubbuberrors.c

DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libhubbub.a

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


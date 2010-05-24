INCLUDE += ./
INCLUDE += $(SRCDIR)/include
INCLUDE += $(SRCDIR)/bal
INCLUDE += $(SRCDIR)/bal/socket
INCLUDE += $(SRCDIR)/bal/ata
INCLUDE += $(SRCDIR)/bal/gprs
INCLUDE += $(SRCDIR)/bui
INCLUDE += $(SRCDIR)/bui/uiex
INCLUDE += $(SRCDIR)/res
INCLUDE += $(SRCDIR)/baf

SRC_O = buidcc.o buistring.o
OBJ_O = $(addprefix $(CURDIR)/,$(SRC_O))

TARGET  = $(CURDIR)/bui.a

all: $(TARGET)

$(TARGET): $(OBJ_O)
	$(subst /cygdrive/c,C:,$(AR) $(ARFLAGS) $@ $?)

clean:
	$(RM) $(TARGET)

include $(BASEDIR)/Makefile.rules

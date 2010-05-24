DEFINE += LOCOSTO_PLUS
DEFINE += __NO_SYS_TYPES_H_HACK__

INCLUDE += ./
INCLUDE += $(INCDIR)/g23m-aci/aci
INCLUDE += $(INCDIR)/g23m/condat/ms/src/aci
INCLUDE += $(INCDIR)/g23m/condat/ms/src/aci_dti_mng
INCLUDE += $(INCDIR)/csw-system/drv_core
INCLUDE += $(INCDIR)/g23m/condat/com/include
INCLUDE += $(INCDIR)/csw-system/drv_core
INCLUDE += $(SRCDIR)/include
INCLUDE += $(SRCDIR)/bal
INCLUDE += $(SRCDIR)/bal/socket
INCLUDE += $(SRCDIR)/bal/ata
INCLUDE += $(SRCDIR)/bal/gprs
INCLUDE += $(SRCDIR)/bui
INCLUDE += $(SRCDIR)/bui/uiex
INCLUDE += $(SRCDIR)/res
INCLUDE += $(SRCDIR)/baf
INCLUDE += $(SRCDIR)/freetype2/include

SRC_O = aslfileio.o
OBJ_O = $(addprefix $(CURDIR)/,$(SRC_O))

TARGET  = $(CURDIR)/baf.a

all: $(TARGET)

$(TARGET): $(OBJ_O)
	$(subst /cygdrive/c,C:,$(AR) $(ARFLAGS) $@ $?)

clean:
	$(RM) $(TARGET)

include $(BASEDIR)/Makefile.rules

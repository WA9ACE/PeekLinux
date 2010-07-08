DEFINE += LOCOSTO_PLUS
DEFINE += BAL_IP_PUSH

INCLUDE += $(INCDIR)/g23m-aci/aci
INCLUDE += $(INCDIR)/g23m/condat/ms/src/aci
INCLUDE += $(INCDIR)/g23m/condat/ms/src/aci_dti_mng
INCLUDE += $(INCDIR)/g23m/condat/com/inc
INCLUDE += $(INCDIR)/csw-system/drv_core
INCLUDE += $(INCDIR)/g23m/condat/com/include
INCLUDE += $(SRCDIR)/include
INCLUDE += $(SRCDIR)/bal/ata
INCLUDE += $(SRCDIR)/bal/gprs
INCLUDE += $(SRCDIR)/bal/socket
INCLUDE += $(SRCDIR)/bal
INCLUDE += $(SRCDIR)/res
INCLUDE += $(SRCDIR)/baf
INCLUDE += $(SRCDIR)/bui
INCLUDE += $(SRCDIR)/bui/uiex
INCLUDE += $(SRCDIR)/bam
INCLUDE += $(SRCDIR)/emobiix
INCLUDE += $(SRCDIR)/nmealib/include

SRC  = amdb.cpp
SRC += amdb_task.cpp
SRC += emailstask.cpp
SRC += ems.cpp
SRC += markup.cpp

TARGET = $(CURDIR)/bam.a

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

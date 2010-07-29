DEFINE +=

INCLUDE += $(M23DIR)/g23m-aci/aci
INCLUDE += $(M23DIR)/csw-system/os/nucleus/1_15/plus
INCLUDE += $(M23DIR)/csw-system/os/nucleus/1_15
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/g23m/condat/ms/src/aci
INCLUDE += $(M23DIR)/g23m/condat/ms/src/aci_dti_mng
INCLUDE += $(M23DIR)/g23m/condat/com/inc
INCLUDE += $(M23DIR)/csw-system/drv_core
INCLUDE += $(M23DIR)/g23m/condat/com/include
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

SRC = emo_pei.c
#SRC += ems_pei.c
#SRC += trans_pei.c
SRC += ui_pei.c

TARGET = $(CURDIR)/peitasks.a

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

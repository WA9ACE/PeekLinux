INCLUDE += ./
INCLUDE += $(M23DIR)/g23m-aci/aci
INCLUDE += $(M23DIR)/g23m/condat/ms/src/aci
INCLUDE += $(M23DIR)/g23m/condat/ms/src/aci_dti_mng
INCLUDE += $(M23DIR)/csw-system/os/nucleus/1_15/plus
INCLUDE += $(M23DIR)/csw-system/os/nucleus/1_15
INCLUDE += $(M23DIR)/gpf/inc
INCLUDE += $(M23DIR)/chipsetsw/riviera
INCLUDE += $(M23DIR)/config
INCLUDE += $(M23DIR)/cdginc
INCLUDE += $(M23DIR)/csw-system/drv_core
INCLUDE += $(M23DIR)/csw-system/drv_core/timer
INCLUDE += $(M23DIR)/csw-system/drv_core/abb
INCLUDE += $(M23DIR)/csw-system/drv_core/i2c
INCLUDE += $(M23DIR)/csw-system/drv_core/util
INCLUDE += $(M23DIR)/chipsetsw/riviera/rv
INCLUDE += $(M23DIR)/chipsetsw/riviera/rvm
INCLUDE += $(M23DIR)/bmi/gdi
INCLUDE += $(M23DIR)/bmi/Condat/MS/SRC/mfw
INCLUDE += $(M23DIR)/bmi/Condat/MS/SRC/bmi
INCLUDE += $(M23DIR)/bmi/Condat/MS/SRC/atb
INCLUDE += $(M23DIR)/g23m-aci/ksd
INCLUDE += $(M23DIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/chipsetsw/riviera/rvf
INCLUDE += $(M23DIR)/csw-system/drv_app/ffs
INCLUDE += $(M23DIR)/csw-system/drv_app/lcd
INCLUDE += $(M23DIR)/csw-system/drv_core
INCLUDE += $(SRCDIR)/include
INCLUDE += $(SRCDIR)/emobiix/thirdparty
INCLUDE += $(SRCDIR)/lua
INCLUDE += $(SRCDIR)/freetype2/include
INCLUDE += $(SRCDIR)/libpng
INCLUDE += $(SRCDIR)/zlib
INCLUDE += $(SRCDIR)/emobiix/protocol
INCLUDE += $(SRCDIR)/emobiix/peek
INCLUDE += $(SRCDIR)/netsurf/emoport

SRC  = peek/KeyHandler.c
SRC += peek/PeekEmobiix.c
SRC += Application.c
SRC += Array.c
SRC += BalTCPTransport.c
SRC += BitmapCache.c
SRC += Balmem.c
SRC += Cache.c
SRC += ConnectionContext.c
SRC += ConnectionManager.c
SRC += DataObject.c
SRC += DataObjectMap.c
SRC += emobiix_font.c
SRC += emobiix_font.c
SRC += Font.c
SRC += Gprs.c
SRC += Gradient.c
SRC += HashTable.c
SRC += lgui.c
SRC += List.c
SRC += Map.c
SRC += p_malloc.c
SRC += Point.c
SRC += Protocol.c
SRC += Rectangle.c
#SRC += Sounds.c
SRC += StaticList.c
SRC += EmoTask.c
#SRC += emailstask.c
SRC += Style.c
SRC += Subscription.c
SRC += Task.c
SRC += Transport.c
SRC += UITask.c
SRC += URL.c
SRC += Widget.c
SRC += WidgetRenderer.c
SRC += EntryWidget.c
SRC += Script.c
SRC += ProtocolUtils.c
SRC += peek/Debug.c
SRC += peek/Imei.c
SRC += Mime.c
#SRC += ffs.c
SRC += FFSFile.c
#SRC += system_battery.c
#SRC += system_misc.c
SRC += peek/KeyMappings.c
SRC += ApplicationManager.c
SRC += BootApplication.c
SRC += RootApplication.c
SRC += RootStyle.c
SRC += ArrayWidget.c
SRC += SetWidget.c
SRC += ImageWidget.c
SRC += TextWidget.c
SRC += LabelWidget.c
SRC += StaticApplications.c
SRC += CheckBoxWidget.c
SRC += RenderManager.c
SRC += Cache.c
SRC += StackWidget.c
SRC += ScrollBarWidget.c
SRC += ScrolledWidget.c

ifneq (,$(findstring EMO_SIM,$(VARIANT_DEFINE)))
SRC += SimDriver.c
endif

TARGET = $(CURDIR)/emobiix.a

.PHONY: protocol

OBJ   = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.c,%.o,$(SRC))))
DEPS  = $(patsubst %.o,%.d,$(OBJ))

all: $(TARGET)

$(TARGET): protocol $(OBJ) $(OBJ_O)
	$(AR) $(ARFLAGS) $@ $(OBJ)

protocol:
	$(MAKE) -C $@ prepare
	$(MAKE) -C $@

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET)
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))
	$(MAKE) -C protocol clean

include $(BASEDIR)/Makefile.rules

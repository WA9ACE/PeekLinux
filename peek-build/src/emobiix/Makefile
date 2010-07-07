INCLUDE += ./
INCLUDE += $(INCDIR)/g23m-aci/aci
INCLUDE += $(INCDIR)/g23m/condat/ms/src/aci
INCLUDE += $(INCDIR)/g23m/condat/ms/src/aci_dti_mng
INCLUDE += $(INCDIR)/csw-system/drv_core
INCLUDE += $(M23DIR)/bmi/gdi
INCLUDE += $(INCDIR)/g23m/condat/com/include
INCLUDE += $(M23DIR)/chipsetsw/riviera/rvf
INCLUDE += $(INCDIR)/csw-system/drv_core
INCLUDE += $(SRCDIR)/include
INCLUDE += $(SRCDIR)/bal
INCLUDE += $(SRCDIR)/bal/ata
INCLUDE += $(SRCDIR)/bal/socket
INCLUDE += $(SRCDIR)/emobiix/thirdparty
INCLUDE += $(SRCDIR)/lua
INCLUDE += $(SRCDIR)/bui
INCLUDE += $(SRCDIR)/freetype2/include
INCLUDE += $(SRCDIR)/libpng
INCLUDE += $(SRCDIR)/zlib
INCLUDE += $(SRCDIR)/emobiix/protocol

SRC  = Application.c
SRC += Array.c
SRC += BalTCPTransport.c
SRC += BitmapCache.c
SRC += Boot.c
SRC += Cache.c
SRC += Exeapi.c
SRC += ConnectionContext.c
SRC += ConnectionManager.c
SRC += DataObject.c
SRC += DataObjectMap.c
SRC += emobiix_font.c
SRC += emobiix_font.cpp
SRC += Font.c
SRC += Gprs.c
SRC += Gradient.c
SRC += HashTable.c
SRC += KeyHandler.cpp
SRC += LcdControl.c
SRC += lgui.c
SRC += List.c
SRC += Map.c
SRC += p_malloc.c
SRC += Point.c
SRC += Protocol.c
SRC += Rectangle.c
SRC += sms.cpp
SRC += Sounds.c
SRC += StaticList.c
SRC += stub.cpp
SRC += Style.c
SRC += Subscription.c
SRC += Task.c
SRC += Transport.c
#SRC += tweet.cpp
SRC += UIEntryStub.cpp
SRC += UITask.cpp
SRC += URL.c
SRC += Widget.c
SRC += WidgetRenderer.c
SRC += EntryWidget.c
SRC += Script.c
SRC += ProtocolUtils.c
#SRC += Debug.c
SRC += Mime.c
SRC += system_battery.c
SRC += ApplicationManager.cpp
SRC += BootApplication.c
SRC += RootApplication.c
SRC += RootStyle.c
SRC += ArrayWidget.c

TARGET = $(CURDIR)/emobiix.a
SOCKET_SIM = $(CURDIR)/socket_sim.a

.PHONY: protocol

SIM_OBJ = SimDriver.o
OBJ   = $(addprefix $(CURDIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS  = $(patsubst %.o,%.d,$(OBJ))

all: $(TARGET) $(SOCKET_SIM)

$(TARGET): protocol $(OBJ) $(OBJ_O)
	$(AR) $(ARFLAGS) $@ $(OBJ)

$(SOCKET_SIM): $(SIM_OBJ)
	$(AR) $(ARFLAGS) $@ $(SIM_OBJ)

protocol:
	$(MAKE) -C $@ prepare
	$(MAKE) -C $@

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET)
	$(RM) $(SOCKET_SIM)
	$(RM) $(SIM_OBJ)
	$(RM) $(DEPS)
	$(RM) $(patsubst %.o,%.pp,$(OBJ) $(SIM_OBJ))
	$(RM) $(patsubst %.o,%.pp_,$(OBJ))
	$(MAKE) -C protocol clean

include $(BASEDIR)/Makefile.rules

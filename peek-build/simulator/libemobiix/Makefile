DEFINE += SIMULATOR
DEFINE += NOGDI
DEFINE += LINUX

EMODIR = $(SRCDIR)/emobiix

INCLUDE  = $(SRCDIR)/simulator
INCLUDE += $(SRCDIR)/freetype2/include
INCLUDE += $(SRCDIR)/lua
INCLUDE += $(SRCDIR)/zlib
INCLUDE += $(SRCDIR)/libpng
INCLUDE += $(SRCDIR)/emobiix
INCLUDE += $(SRCDIR)/emobiix/simulator
INCLUDE += $(SRCDIR)/emobiix/protocol
INCLUDE += $(SRCDIR)/emobiix/thirdparty

SRC  = Application.c
SRC += ApplicationManager.c
SRC += Array.c
SRC += ArrayWidget.c
SRC += BitmapCache.c
SRC += BootApplication.c
SRC += Cache.c
SRC += CheckBoxWidget.c
SRC += ConnectionContext.c
SRC += ConnectionManager.c
SRC += DataObject.c
SRC += EntryWidget.c
SRC += Font.c
SRC += Gradient.c
SRC += HashTable.c
SRC += ImageWidget.c
SRC += LabelWidget.c
SRC += List.c
SRC += Map.c
SRC += Mime.c
SRC += Point.c
SRC += Protocol.c
SRC += ProtocolUtils.c
SRC += Rectangle.c
SRC += RenderManager.c
SRC += RootApplication.c
SRC += RootStyle.c
SRC += Script.c
SRC += ScrollBarWidget.c
SRC += ScrolledWidget.c
SRC += SetWidget.c
SRC += StackWidget.c
SRC += StaticApplications.c
SRC += StaticList.c
SRC += Style.c
SRC += Subscription.c
SRC += TCPTransport.c
SRC += Task.c
SRC += TextWidget.c
SRC += Transport.c
SRC += URL.c
SRC += Widget.c
SRC += WidgetRenderer.c
SRC += emobiix_font.c
SRC += lgui.c
SRC += peek/KeyHandler.c
SRC += simulator/KeyMappings.c
SRC += simulator/NetworkTask.c
SRC += Gprs.c

OBJ = $(addprefix $(EMODIR)/,$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRC))))
DEPS = $(patsubst %.o,%.d,$(OBJ))

TARGET = $(CURDIR)/libemobiix.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $(ARFLAGS) $@ $?

clean:
	$(RM) $(OBJ) 
	$(RM) $(TARGET) 
	$(RM) $(DEPS)

include $(BASEDIR)/Makefile.rules
